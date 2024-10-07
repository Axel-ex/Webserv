#include "../includes/Cluster.hpp"
#include "../includes/Log.hpp"
#include "../includes/Parser.hpp"
#include <cstddef>
#include <stdio.h>
#include <cerrno>
#include <stdexcept>



Cluster::Cluster(std::string config_file)
{
    Parser parser;

    parser.parse(config_file, *this);
    Log::setLogLevel(DEBUG);
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
}

void Cluster::start(void)
{
    while (!stopFlag)
    {
        int activity;

        // Loop to handle EINTR (syscall interruptions while polling) error
        do
        {
            activity = poll(_poll_fds.data(), _poll_fds.size(), 1000);
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
