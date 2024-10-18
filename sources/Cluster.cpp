#include "../includes/Cluster.hpp"
#include "../includes/Log.hpp"
#include "../includes/Parser.hpp"
#include <cerrno>
#include <cstddef>
#include <stdexcept>
#include <stdio.h>
#include <algorithm>

Cluster::Cluster(std::string config_file)
{
    Parser parser;

    Log::setLogLevel(DEBUG);
    parser.parse(config_file, *this);
    Log::clearScreen();
}

Cluster::~Cluster(void) {}

/**
 * @brief collect pollfd from all the servers, bind virtual servers with their
 * associated server instance.
 */
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
    _bindVirtualServers();
    for (size_t i = 0; i < _virtual_servers.size(); i++)
        _virtual_servers[i].announce(true);
}

/**
 * @brief main event loop. poll the vector of poll_fda. If an event is detected,
 * match a server instance with the fd receiving the event. This operation
 * creates a new client fd which is stored in the right server instance.
 * Then for each server check timeout processes, store the finish processes in a
 * structure for further processing and serve clients.
 */
void Cluster::start(void)
{
    while (!stopFlag)
    {
        int activity;

        // Loop to handle EINTR (syscall interruptions while polling) error
        do
        {
            activity = poll(_poll_fds.data(), _poll_fds.size(), 350);
        } while (activity < 0 && errno == EINTR);

        if (activity < 0 && !stopFlag)
        {
            perror("poll");
            throw std::runtime_error("Error in poll");
        }

        // check if any of the fds detected an event and call
        // acceptIncomingConnections on the appropriated server (match a server
        // pointer with its pollfd)
        for (size_t i = 0; i < _poll_fds.size(); ++i)
            if (_poll_fds[i].revents & POLLIN)
                _server_ptr[i]->acceptIncomingConnections(_poll_fds[i]);

        // Check timeout and serve_clients
        for (size_t i = 0; i < _servers.size(); i++)
        {
            // _servers[i].checkTimeouts();
            _servers[i].checkFinishedProcesses();
            _servers[i].serveClients();
        }
    }
}

/**
 * @brief look in the virtual server vector (server with repeated port number)
 * and associate a _virtual_server with its associated Server instance
 */
void Cluster::_bindVirtualServers(void)
{
    for (std::vector<Server>::iterator it_virtual_servers =
             _virtual_servers.begin();
         it_virtual_servers != _virtual_servers.end(); ++it_virtual_servers)
    {
        std::vector<int> virtual_ports =
            it_virtual_servers->getConfig().getPorts();
        if (virtual_ports.size() > 1)
            Log::log(
                WARNING,
                "The config contains a virtual server with more then one port");
        for (size_t i = 0; i < virtual_ports.size(); ++i)
        {
            int port = virtual_ports[i];

            // Look for a server that shares the port
            std::vector<Server>::iterator it_server = std::find_if(
                _servers.begin(), _servers.end(), ServerTools::MatchPort(port));

            // bind it with the appropriate Server instance
            if (it_server != _servers.end())
                it_server->addVirtualServer(&(*it_virtual_servers));
        }
    }
}

std::vector<Server>& Cluster::getServers(void) { return _servers; }

void Cluster::addVirtualServer(Server& server)
{
    _virtual_servers.push_back(server);
}
