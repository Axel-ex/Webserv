#include "../includes/Cluster.hpp"
#include "../includes/Log.hpp"
#include "../includes/Parser.hpp"
#include <cerrno>
#include <cstddef>
#include <stdexcept>
#include <stdio.h>

Cluster::Cluster(std::string config_file)
{
    Parser parser;

    Log::setLogLevel(DEBUG);
    parser.parse(config_file, *this);
    Log::clearScreen();
}

Cluster::~Cluster(void) {}

void Cluster::init(void)
{
    std::vector<t_pollfd> poll_fds;

    // Store the fds to poll into the cluster object
    for (size_t i = 0; i < _servers.size(); i++)
    {
        poll_fds = _servers[i].init();
        for (size_t j = 0; j < poll_fds.size(); j++)
        {
            _poll_fds.push_back(poll_fds[j]);
            _server_ptr.push_back(&_servers[i]);
        }
    }
	_bindTwinServers();
    for (size_t i = 0; i < _twin_servers.size(); i++)
        _twin_servers[i].announce();
}

void Cluster::start(void)
{
    while (!stopFlag)
    {
        int activity;

        // Loop to handle EINTR (syscall interruptions while polling) error
        do
        {
            activity = poll(_poll_fds.data(), _poll_fds.size(), 500);
        } while (activity < 0 && errno == EINTR);

        if (activity < 0 && !stopFlag)
        {
            perror("poll");
            throw std::runtime_error("Error in poll");
        }

        // check if any of the fds detected an event and call
        // acceptIncomingConnections on the appropriated server
        for (size_t i = 0; i < _poll_fds.size(); ++i)
            if (_poll_fds[i].revents & POLLIN)
                _server_ptr[i]->acceptIncomingConnections(_poll_fds[i]);

        // Check timeout and serve_clients
        for (size_t i = 0; i < _servers.size(); i++)
        {
            _servers[i].checkTimeouts();
            _servers[i].checkFinishedProcesses();
            _servers[i].serveClients();
        }
    }
}

std::vector<Server>& Cluster::getServers(void) { return _servers; }

void Cluster::addTwinServer(Server& server) { _twin_servers.push_back(server); }


void Cluster::_bindTwinServers(void)
{
    for (std::vector<Server>::iterator it_twin_servers = _twin_servers.begin();
         it_twin_servers != _twin_servers.end();
         ++it_twin_servers)
    {
        std::vector<int> twin_ports = it_twin_servers->getConfig().getPorts();
        for (size_t i = 0; i < twin_ports.size(); ++i)
        {
            int port = twin_ports[i];

            // Look for a server that shares the port
            std::vector<Server>::iterator it_server = std::find_if(
                _servers.begin(), _servers.end(), ServerTools::MatchPort(port));

            // bind it with the appropriate Server instance
            if (it_server != _servers.end())
				it_server->addTwinServer(&(*it_twin_servers));
        }
    }
}

