#include "bbp/sonata/edges.h"
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>
#include <highfive/H5DataSet.hpp>

#include <chrono>
#include <iostream>
#include <random>

using namespace bbp::sonata;
using rtClock = std::chrono::high_resolution_clock;

NodeID findNumNodes(const std::string& edgeFile,
                    const std::string& populationName)
{
    // There's no way we can do this with the current API, accessing the
    // H5 file directly
    HighFive::File file(edgeFile);
    auto dataset = file.getGroup(
        "edges/" + populationName + "/indices/target_to_source").
        getDataSet("node_id_to_ranges");
    auto dims = dataset.getSpace().getDimensions();
    return dims[0];
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " edge_file [num_nodes=1000]"
                  << std::endl;
        return -1;
    }

    int numNodes = 1000;
    if (argc == 3)
    {
        char *end = argv[2];
        if ((numNodes = std::strtol(argv[2], &end, 10)) <= 0 ||
            *end != '\0')
        {
            std::cerr << "Invalid number of nodes, defaulting to 1000"
                      << std::endl;
            numNodes = 1000;
        }
    }

    auto fileName = argv[1];
    EdgeStorage edges(fileName);
    auto names = edges.populationNames();
    if (names.empty())
    {
        std::cerr << "No edge population found" << std::endl;
    }

    for (const auto& name : names)
    {
        std::cout << "Benchmarking population " << name << std::endl;

        auto population = edges.openPopulation(name);
        const NodeID maxNodes = findNumNodes(fileName, name);

        //std::random_device device;
        auto seed = 3119003351;//device();
        std::cout << "Seed " << seed << std::endl;
        std::mt19937 generator(seed);
        std::uniform_int_distribution<> distribution(0, maxNodes);

        // Random sampling of numNodes Node ids
        std::vector<NodeID> ids;
        for (int i = 0; i != numNodes; ++i)
            ids.push_back(distribution(generator));

        std::cout << "Querying afferent edges for " << numNodes
                  << " random nodes" << std::endl;
        std::cout << "One query per node" << std::endl;
        size_t count = 0;
        auto ref = rtClock::now();
        for (const auto id : ids)
        {
            const auto sources =
                population->sourceNodeIDs(population->afferentEdges({id}));
            count += sources.size();
        }
        double elapsed =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                rtClock::now() - ref).count() / 1e9;
        std::cout << elapsed << " sec "
                  << (ids.size() / elapsed) << " queries/sec "
                  << count << " edges" << std::endl;

        std::cout << "All at once" << std::endl;
        ref = rtClock::now();
        count = population->sourceNodeIDs(population->afferentEdges(ids)).size();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
            rtClock::now() - ref).count() / 1e9;
        std::cout << elapsed << " sec "
                  << count << " edges" << std::endl;

        std::cout << "Querying efferent edges for " << numNodes
                  << " random nodes" << std::endl;
        std::cout << "One query per node" << std::endl;
        count = 0;
        ref = rtClock::now();
        for (const auto id : ids)
        {
            const auto targets =
                population->targetNodeIDs(population->efferentEdges({id}));
            count += targets.size();
        }
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
            rtClock::now() - ref).count() / 1e9;
        std::cout << elapsed << " sec "
                  << (ids.size() / elapsed) << " queries/sec "
                  << count << " edges" << std::endl;

        std::cout << "All at once" << std::endl;
        ref = rtClock::now();
        count = population->targetNodeIDs(population->efferentEdges(ids)).size();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
            rtClock::now() - ref).count() / 1e9;
        std::cout << elapsed << " sec "
                  << count << " edges" << std::endl;
    }
}
