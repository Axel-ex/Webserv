#ifndef INCLUDE_INCLUDES_CLUSTER_HPP_
#define INCLUDE_INCLUDES_CLUSTER_HPP_

#include "../includes/Server.hpp"
#include <vector>

class Cluster
{
    public:
        Cluster(std::string config_file);
        ~Cluster();

        void init();
        void start();

        std::vector<Server>& getServers(void);
		void addTwinServer(Server &server);

    private:
        std::vector<Server> _servers;
		std::vector<Server> _twin_servers;
		std::vector<Server*> _server_ptr;
        std::vector<t_pollfd> _poll_fds;

		void _bindTwinServers(void);
};

#endif // INCLUDE_INCLUDES_CLUSTER_HPP_
