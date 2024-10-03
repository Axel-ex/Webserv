#ifndef INCLUDE_INCLUDES_CLUSTER_HPP_
#define INCLUDE_INCLUDES_CLUSTER_HPP_

#include <vector>
#include "Server.hpp"

class Cluster {
public:
	Cluster(std::string config_file);
	~Cluster();

	void init();
	void start();

private:
	std::vector<Server> _servers;
};

#endif  // INCLUDE_INCLUDES_CLUSTER_HPP_
