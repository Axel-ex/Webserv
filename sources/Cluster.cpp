#include "../includes/Cluster.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Log.hpp"

Cluster::Cluster(std::string config_file) {
	Parser parser;

		//TODO: pass the cluster as argument so it can be populated by the parser
		parser.parse(config_file);
		Log::setLogLevel(DEBUG);
		Log::clearScreen();

}

Cluster::~Cluster(void) {}

void Cluster::init(void) {
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].init();
}

void Cluster::start(void) {
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].start();
}
