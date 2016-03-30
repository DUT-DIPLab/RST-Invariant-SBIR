#pragma once

#include <vector>

class FlowMatching
{
public:
    FlowMatching();
    virtual ~FlowMatching();

    double getMatchingScore(std::vector<std::vector<double>> dists);

private:
    struct Edge {
        int v, flow;
        double cost;
    };
    std::vector<std::vector<Edge>> edges;
    std::vector<int> prev;
    std::vector<double> dist;

    bool findPath(int src, int dst, int nodeNum);
};

