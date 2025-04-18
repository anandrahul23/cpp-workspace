#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <limits>
#include <functional>

using namespace std;

struct GraphNode
{
    int node;
    int weight;
    GraphNode(int nn, int ww) : node(nn), weight(ww) {}

    bool operator==(const GraphNode &other) const
    {
        return node == other.node && weight == other.weight;
    }
};

struct GraphNodeHash
{
    size_t operator()(const GraphNode &g) const
    {
        return hash<int>()(g.node) ^ (hash<int>()(g.weight) << 1);
    }
};

using GraphType = unordered_map<int, unordered_set<GraphNode, GraphNodeHash>>;
using visitedtype = unordered_set<int>;

struct PQCompare
{
    bool operator()(const pair<int, int> &l, const pair<int, int> &r)
    {
        return l.second > r.second;
    }
};

using PQ = priority_queue<pair<int, int>, vector<pair<int, int>>, PQCompare>;

class Solution
{
public:
    int solve(int A, vector<vector<int>> &B, int C, int D)
    {
        if (C == D)
        {
            return 0;
        }

        GraphType graph;
        PQ minDistanceQueue;
        vector<int> dist(A, numeric_limits<int>::max());

        cout << "size of dist: " << dist.size() << endl;
        visitedtype visited;

        for (const auto &vec : B)
        {
            graph[vec[0]].insert(GraphNode(vec[1], vec[2]));
            graph[vec[1]].insert(GraphNode(vec[0], vec[2]));
        }

        minDistanceQueue.push(make_pair(C, 0));
        dist[C] = 0;

        while (!minDistanceQueue.empty())
        {
            const auto tempPqNode = minDistanceQueue.top();
            minDistanceQueue.pop();

            cout << "Processing node: " << tempPqNode.first << " with current distance: " << tempPqNode.second << endl;

            const auto it = graph.find(tempPqNode.first);
            const auto itVisited = visited.find(tempPqNode.first);

            if (it != graph.end() && itVisited == visited.end())
            {
                const auto &neighbours = it->second;

                for (const auto &nn : neighbours)
                {
                    int currentNodeDistance = dist[tempPqNode.first];
                    std::cout << "Current node:" << tempPqNode.first << " distance: " << currentNodeDistance << std::endl;
                    cout << "Checking neighbor: " << nn.node << " with edge weight: " << nn.weight << endl;
                    if (dist[nn.node] > currentNodeDistance + nn.weight)
                    {
                        dist[nn.node] = currentNodeDistance + nn.weight;
                        minDistanceQueue.push(make_pair(nn.node, dist[nn.node]));
                        cout << "Updated distance for node: " << nn.node << " to: " << dist[nn.node] << endl;
                    }
                }
                visited.insert(it->first);
                cout << "Marked node " << it->first << " as visited" << endl;
            }
        }

        if (dist[D] == numeric_limits<int>::max())
        {
            return -1;
        }

        return dist[D];
    }
};

int main()
{
    Solution solution;
    vector<vector<int>> B = {{0, 2, 1}, {0, 4, 2}, {1, 3, 1}, {1, 4, 1}, {0, 1, 1}, {2, 4, 1}, {3, 4, 2}, {1, 2, 1}};
    int C = 1;
    int D = 3;
    int result = solution.solve(5, B, C, D);
    cout << "Result: " << result << endl;
    return 0;
}
