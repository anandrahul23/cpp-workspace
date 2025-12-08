#pragma once 
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue> //  minheap 

using namespace std; 
namespace prim // prim is only applicapble to non negative weights , why ?? 
{
    struct Edge
    {
        size_t toVertex; 
        size_t weight; 
    };
    using GraphType = unordered_map<size_t, vector<Edge>>; 

    class PrimGraph 
    {
        public: 
        void addEdge(GraphType& graph, size_t u, size_t v, size_t w)
        {
            graph[u].emplace_back(Edge{v,w});
            graph[v].emplace_back(Edge{u,w});
        }
        void addEdge(GraphType &graph, size_t u) //standalone node 
        {
            graph[u] = {};
        }
        GraphType getMinSpanTree(const GraphType& graph)
        {
            //start with any node , so will start with first node 
            unordered_set<size_t> visited; 

            auto cIt = graph.cbegin();
            auto sourceNode = cIt->first;
            visited.insert(sourceNode);
            //using SourceWithWeight = std::pair<size_t, size_t> ;
            using SourceToEdge = std::pair<size_t, Edge>;
            auto SourceToEdgeCmp = [](const SourceToEdge &sEdge1, const SourceToEdge &sEdge2)
            {
                if (sEdge1.second.weight == sEdge2.second.weight)
                {
                    return sEdge1.second.toVertex > sEdge2.second.toVertex; 
                }
                return sEdge1.second.weight > sEdge2.second.weight;
            };
            priority_queue<SourceToEdge, 
                                vector<SourceToEdge>, 
                                decltype(SourceToEdgeCmp)> minEdgeHeap(SourceToEdgeCmp);

            
            for (auto n : cIt->second) // neighobours
            {
                minEdgeHeap.emplace(sourceNode, n); 
            }

            //create the min span tree now 

            GraphType mst;

            while (!minEdgeHeap.empty()) // V-1 edges
            {
                auto topEl = minEdgeHeap.top(); 
                minEdgeHeap.pop(); 
                if(visited.size() == graph.size())
                {
                    break;
                }
                if(auto itVisited = visited.find(topEl.second.toVertex); itVisited == visited.end())
                {//then only create the nee MST 
                    addEdge(mst, topEl.first, topEl.second.toVertex, topEl.second.weight);
                    visited.insert(topEl.second.toVertex);
                    const auto &neighbours = graph.at(topEl.second.toVertex);
                    for (const auto &ed : neighbours)
                    {
                        // push to minHeap
                        minEdgeHeap.emplace(topEl.second.toVertex, ed);
                    }
                }
               

            }

            return mst;
        }
        private: 
        //GraphType graph; 
    }; 
    
}
