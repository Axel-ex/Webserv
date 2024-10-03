#include "../includes/Cluster.hpp"
#include "../includes/Log.hpp"
#include "../includes/Parser.hpp"

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
    for (size_t i = 0; i < _servers.size(); i++)
        _servers[i].init();
}

void Cluster::start(void)
{
    for (size_t i = 0; i < _servers.size(); i++)
        _servers[i].start();
}

std::vector<Server>& Cluster::getServers(void) { return _servers; }
