#include "path_finder.h"

std::vector<std::pair<char, char>> find_min_path(Weighted &graph, char start, char finish)
{
    auto verts = graph.GetVertices();
    auto edges = graph.GetEdges();
    auto weights = graph.GetWeights();
    std::vector<std::pair<char, char>> path;
    std::vector<char> visited;
    std::map<char, int> path_length;
    for (auto v: verts) {
        if (v == start) {
            path_length[v] = 0;
        } else {
            path_length[v] = INF;
        }
    }
    visited.push_back(start);
    char cur = start;
    while (visited.size() != verts.size()) {
        int min_path = INF;
        char candidate = cur;
        auto len = edges.size();
        for (int i = 0; i < len; ++i) {
            if (edges[i].first == cur) {
                auto place = std::find(visited.begin(), visited.end(), edges[i].second);
                if (place == visited.end()) {
                    if (path_length[cur] + weights[i] < path_length[edges[i].second]) {
                        path_length[edges[i].second] = path_length[cur] + weights[i];
                    }
                    int cur_path = weights[i];
                    if (cur_path < min_path) {
                        candidate = edges[i].second;
                        min_path = cur_path;
                    }
                }
            }
            if (edges[i].second == cur) {
                auto place = std::find(visited.begin(), visited.end(), edges[i].first);
                if (place == visited.end()) {
                    if (path_length[cur] + weights[i] < path_length[edges[i].first]) {
                        path_length[edges[i].first] = path_length[cur] + weights[i];
                    }
                    int cur_path = weights[i];
                    if (cur_path < min_path) {
                        candidate = edges[i].first;
                        min_path = cur_path;
                    } 
                }
            }
        }
        if (candidate == cur && visited.size() != verts.size()) {
            throw std::logic_error("Not a connected graph!");
        }
        visited.push_back(candidate);
        cur = candidate;
    }
    auto cur_length = path_length[finish];
    char next;
    cur = finish;
    while (cur != start) {
        auto len = edges.size();
        for (int i = 0; i < len; ++i) {
            if (edges[i].first == cur) {
                if (path_length[cur] - weights[i] == path_length[edges[i].second]) {
                    next = edges[i].second;
                    break;
                }
            }
            if (edges[i].second == cur) {
                if (path_length[cur] - weights[i] == path_length[edges[i].first]) {
                    next = edges[i].first;
                    break;
                }
            }
        }
        path.push_back(std::make_pair(next, cur));
        cur = next;
    }
    std::reverse(path.begin(), path.end());
    return path;
}