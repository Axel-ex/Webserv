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

void Cluster::_createSocketFds(void)
{
    std::vector<int> already_in_use_ports;

    for (size_t i = 0; i < _servers.size(); i++)
    {
        std::vector<int> server_ports = _servers[i].getConfig().getPorts();
        for (size_t j = 0; j < server_ports.size(); j++)
        {
            // TODO: check if the port is already in use. We will later match
            // the sockfd with the server ports and create pollfds for servers
            // that might share the same port
            if (std::find(already_in_use_ports.begin(),
                          already_in_use_ports.end(),
                          server_ports[j]) != already_in_use_ports.end())
                continue;

            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0)
                throw std::runtime_error("Fail creating socket");
            int optval = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                       sizeof(optval));

            t_sockaddr_in address;
            std::memset(&address, 0, sizeof(address));
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(server_ports[j]);

            if (bind(sockfd, (t_sockaddr*)&address, sizeof(address)) < 0)
                throw std::runtime_error("Fail binding the socket");

            if (listen(sockfd, MAX_CLIENT) < 0)
                throw std::runtime_error("fail listening for connection");

            already_in_use_ports.push_back(server_ports[j]);
            _socket_fds[server_ports[j]] = sockfd;
        }
    }
}

void Cluster::init(void)
{
    std::vector<t_pollfd> poll_fds;

    _createSocketFds();
    // Store the fds to poll into the cluster object
    for (size_t i = 0; i < _servers.size(); i++)
    {
        poll_fds = _servers[i].init(_socket_fds);
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
