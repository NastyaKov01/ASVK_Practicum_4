#include "graph_types.h"

BipartParams::BipartParams(std::vector<char> p1, std::vector<char> p2)
{
    for (auto v: p1) {
        part1.push_back(v);
    }
    for (auto v: p2) {
        part2.push_back(v);
    }
}

SimpleParams::SimpleParams(std::vector<std::string> eds)
{
    for (auto e: eds) {
        edges.push_back(e);
    }
}

CompleteParams::CompleteParams(std::vector<char> verts)
{
    for (auto v: verts) {
        vertices.push_back(v);
    }
}

WeightParams::WeightParams(std::vector<std::string> eds, std::vector<int> w)
{
    for (auto e: eds) {
        edges.push_back(e);
    }
    for (auto weight: w) {
        weights.push_back(weight);
    }
}

Weighted::Weighted(std::unique_ptr<WeightParams> && params)
{
    auto eds = params->edges;
    auto w = params->weights;
    for (auto edge: eds) {
        edges.push_back(std::make_pair(edge[0], edge[1]));
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
    for (auto weight: w) {
        weights.push_back(weight);
    }
}

Weighted::Weighted(std::vector<std::string> eds, std::vector<int> w)
{
    for (auto edge: eds) {
        edges.push_back(std::make_pair(edge[0], edge[1]));
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
    for (auto weight: w) {
        weights.push_back(weight);
    }
}

const std::vector<char> Weighted::GetVertices()
{
    return vertices;
}

const std::vector<std::pair<char, char>> Weighted::GetEdges()
{
    return edges;
}

const std::string Weighted::ToString()
{
    std::stringstream ss;
    ss << "WeightedGraph {";
    auto len = edges.size();
    for (int i = 0; i < len; ++i) {
        ss << edges[i].first << edges[i].second;
        ss << ":";
        ss << weights[i];
        if (i != len - 1) {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

Bipartite::Bipartite(std::unique_ptr<BipartParams> && params)
{
    auto p1 = params->part1;
    auto p2 = params->part2;
    for (auto v: p1) {
        part1.push_back(v);
    }
    for (auto v: p2) {
        part2.push_back(v);
    }
}

Bipartite::Bipartite(std::vector<char> p1, std::vector<char> p2)
{
    for (auto v: p1) {
        part1.push_back(v);
    }
    for (auto v: p2) {
        part2.push_back(v);
    }
}

const std::vector<char> Bipartite::GetVertices()
{
    std::vector<char>res;
    for (auto v: part1) {
        res.push_back(v);
    }
    for (auto v: part2) {
        res.push_back(v);
    }
    return res;
}

const std::vector<std::pair<char, char>> Bipartite::GetEdges()
{
    std::vector<std::pair<char, char>> edges;
    for (auto v1: part1) {
        for (auto v2: part2) {
            edges.push_back(std::make_pair(v1, v2));
        }
    }
    return edges;
}

const std::string Bipartite::ToString()
{
    std::stringstream ss;
    ss << "BipartiteGraph {{";
    for (auto &v1: part1) {
        ss << v1;
        if (v1 != *(part1.end()-1)) {
            ss << ", ";
        }
    }
    ss << "}, {";
    for (auto &v2: part2) {
        ss << v2;
        if (v2 != *(part2.end()-1)) {
            ss << ", ";
        }
    }
    ss << "}}";
    return ss.str();
}

Weighted Bipartite::AsWeighted(int default_weight)
{
    std::vector<std::string> edges;
    std::vector<int> weights;
    for (auto v1: part1) {
        for (auto v2: part2) {
            std::stringstream ss;
            ss << v1 << v2;
            edges.push_back(ss.str());
            weights.push_back(default_weight);
        }
    }
    return Weighted(edges, weights);
}

Complete::Complete(std::unique_ptr<CompleteParams> && params) 
{
    auto verts = params->vertices;
    for (auto iter = verts.begin(); iter != verts.end(); iter++) {
        vertices.push_back(*iter);
    }
}

Complete::Complete(std::vector<char> verts) 
{
    for (auto iter = verts.begin(); iter != verts.end(); iter++) {
        vertices.push_back(*iter);
    }
}

const std::vector<char> Complete::GetVertices()
{
    return vertices;
}

const std::vector<std::pair<char, char>> Complete::GetEdges()
{
    std::vector<std::pair<char, char>> edges;
    for (auto iter = vertices.begin(); iter != vertices.end(); iter++) {
        for (auto another = iter + 1; another != vertices.end(); another++) {
            edges.push_back(std::make_pair(*iter, *another));
        }
    }
    return edges;
}

const std::string Complete::ToString()
{
    std::stringstream ss;
    ss << "CompleteGraph {";
    for (auto &v: vertices) {
        ss << v;
        if (v != *(vertices.end() - 1))
        {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

Weighted Complete::AsWeighted(int default_weight)
{
    std::vector<std::string> edges;
    std::vector<int> weights;
    for (auto iter = vertices.begin(); iter != vertices.end(); iter++) {
        for (auto another = iter + 1; another != vertices.end(); another++) {
            std::stringstream ss;
            ss << *iter << *another;
            edges.push_back(ss.str());
            weights.push_back(default_weight);
        }
    }
    return Weighted(edges, weights);
}

Simple::Simple(std::unique_ptr<SimpleParams> && params)
{
    auto eds = params->edges;
    for (auto edge: eds) {
        edges.push_back(std::make_pair(edge[0], edge[1]));
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
}

Simple::Simple(std::vector<std::string> eds)
{
    for (auto edge: eds) {
        edges.push_back(std::make_pair(edge[0], edge[1]));
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
}

const std::vector<char> Simple::GetVertices()
{
    return vertices;
}

const std::vector<std::pair<char, char>> Simple::GetEdges()
{
    return edges;
}

const std::string Simple::ToString()
{
    std::stringstream ss;
    ss << "SimpleGraph {";
    for (auto &edg: edges) {
        ss << edg.first << edg.second;
        if (edg != *(edges.end()-1)) {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

Weighted Simple::AsWeighted(int default_weight)
{
    std::vector<std::string> eds;
    std::vector<int> weights;
    for (auto e: edges) {
        std::stringstream ss;
        ss << e.first << e.second;
        eds.push_back(ss.str());
        weights.push_back(default_weight);
    }
    return Weighted(eds, weights);
}