#include <queue>
#include "FlowMatching.h"
using namespace std;

FlowMatching::FlowMatching()
{
}


FlowMatching::~FlowMatching()
{
}

double FlowMatching::getMatchingScore(vector<vector<double>> dists) {
    int parNum = dists.size();
    int nodeNum = parNum * 2 + 2;
    edges = vector<vector<Edge>>(nodeNum);
    prev = vector<int>(nodeNum);
    for (int i = 0; i < parNum; ++i) {
        edges[0].push_back({ i + 1, 1, 0.0 });
        edges[i + 1].push_back({ 0, 0, 0.0 });
        edges[1 + parNum + i].push_back({ nodeNum - 1, 1, 0.0 });
        edges[nodeNum - 1].push_back({ 1 + parNum + i, 0, 0.0 });
        for (int j = 0; j < parNum; ++j) {
            edges[1 + i].push_back({ 1 + parNum + j, 1, dists[i][j] });
            edges[1 + parNum + j].push_back({ 1 + i, 0, 0.0 });
        }
    }
    double totalCost = 0.0;
    while (findPath(0, nodeNum - 1, nodeNum)) {
        totalCost += dist[nodeNum - 1];
        int v = nodeNum - 1;
        while (v) {
            int u = prev[v];
            for (int i = 0; i < (int)edges[u].size(); ++i) {
                if (edges[u][i].v == v) {
                    edges[u][i].flow -= 1;
                    break;
                }
            }
            for (int i = 0; i < (int)edges[v].size(); ++i) {
                if (edges[v][i].v == u) {
                    edges[v][i].flow += 1;
                    break;
                }
            }
            v = u;
        }
    }
    return totalCost;
}

bool FlowMatching::findPath(int src, int dst, int nodeNum) {
    queue<int> q;
    vector<bool> visit(nodeNum, false);
    dist = vector<double>(nodeNum, 1e100);
    dist[src] = 0.0;
    visit[src] = true;
    q.push(src);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto edge : edges[u]) {
            int v = edge.v;
            int flow = edge.flow;
            double cost = edge.cost;
            if (flow > 0 && dist[v] > dist[u] + cost) {
                dist[v] = dist[u] + cost;
                prev[v] = u;
                if (!visit[v]) {
                    visit[v] = true;
                    q.push(v);
                }
            }
        }
        visit[u] = false;
    }
    return dist[dst] < 1e90;
}