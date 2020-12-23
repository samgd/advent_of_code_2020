#pragma once

#include <list>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// undirected Edge
//
// equality (operator==) and hash are invariant to ordering of the vertices
struct Edge {
    int first;
    int second;

    friend bool operator==(const Edge& e1, const Edge& e2) {
        return (
            (e1.first == e2.first && e1.second == e2.second) ||
            (e1.first == e2.second && e1.second == e2.first)
        );
    }
};

namespace std {
    template<>
    struct hash<Edge> {
        std::size_t operator()(const Edge& e) const
        {
            return hash<int>()(e.first) ^ hash<int>()(e.second);
        }
    };
}

class Graph {
public:
    using Matching = std::unordered_set<Edge>;
    using AugmentPath = std::vector<Edge>;

    Graph(int num_edges_u, int num_edges_v)
        : adj(num_edges_u + num_edges_v)
    {
    }

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    bool is_free(int i, const Matching& matching) const
    {
        for (auto j : adj[i]) {
            if (matching.find({i, j}) != matching.end()) {
                return false;
            }
        }
        return true;
    }

    AugmentPath find_augmenting_path(const Matching& matching) const
    {
        for (int i = 0; i < static_cast<int>(adj.size()); i++) {
            // attempt to find an augmenting path starting at any position
            if (!is_free(i, matching)) {
                continue;
            }

            // i is a free vertex, do DFS to find augmenting path if one exists
            std::vector<bool> visited(adj.size());

            std::stack<int> to_visit;
            std::unordered_map<int, int> depth;
            std::unordered_map<int, int> parent;
            to_visit.push(i);
            depth[i] = 0;

            while (!to_visit.empty()) {
                int vertex = to_visit.top();
                to_visit.pop();
                int vertex_depth = depth.at(vertex);

                if (vertex_depth % 2 == 1 && is_free(vertex, matching)) {
                    // found augmenting path
                    AugmentPath augment_path;
                    int i = parent.at(vertex);   // guaranteed as depth % 2 == 1
                    int j = vertex;
                    augment_path.push_back({i, j});
                    // path length always odd, insert pairs until no more
                    while (parent.find(i) != parent.end()) {
                        j = parent.at(i);
                        augment_path.push_back({i, j});
                        i = parent.at(j);
                        augment_path.push_back({i, j});
                    }
                    return augment_path;
                }

                if (visited[vertex]) {
                    continue;
                }
                visited[vertex] = true;
                for (auto v : adj[vertex]) {
                    if (visited[v]) {
                        continue;
                    }
                    if (vertex_depth % 2 == 0) {
                        if (matching.find({vertex, v}) != matching.end()) {
                            continue;
                        }
                    } else {
                        if (matching.find({vertex, v}) == matching.end()) {
                            continue;
                        }
                    }
                    to_visit.push(v);
                    depth[v] = vertex_depth + 1;
                    parent[v] = vertex;
                }
            }
        }
        return {};
    }

    Matching matching() const
    {
        Matching matching;

        while (true) {
            auto augmenting_path = find_augmenting_path(matching);
            if (augmenting_path.size() == 0) {
                break;
            }
            for (AugmentPath::size_type i = 0; i < augmenting_path.size(); ++i) {
                auto edge = augmenting_path[i];
                if (i % 2 == 0) {
                    matching.insert(edge);
                } else {
                    matching.erase(edge);
                }
            }
        }
        return matching;
    }

private:
    std::vector<std::list<int>> adj;
};
