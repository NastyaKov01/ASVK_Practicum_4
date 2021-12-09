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

Weighted::Weighted(const Weighted &other)
{
    std::vector<std::string> eds;
    for (auto e: other.GetEdges()) {
        std::stringstream ss;
        ss << e.first << e.second;
        eds.push_back(ss.str());
    }
    *this = Weighted(eds, other.weights);
}

const std::vector<char> Weighted::GetVertices() const
{
    return vertices;
}

const std::vector<std::pair<char, char>> Weighted::GetEdges() const
{
    return edges;
}

const std::vector<int> Weighted::GetWeights() const
{
    return weights;
}

const std::string Weighted::ToString() const
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

std::unique_ptr<TGraph> Weighted::AsWeighted(int default_weight) const
{
    std::vector<std::string> eds;
    for (auto ed: this->edges) {
        std::stringstream ss;
        ss << ed.first << ed.second;
        eds.push_back(ss.str());
    }
    return std::make_unique<Weighted> (eds, weights);
}

// std::unique_ptr<TGraph> Weighted::operator+=(TGraph &other)
// {
//     std::vector<std::string> eds;
//     for (auto ed: this->edges) {
//         std::stringstream ss;
//         ss << ed.first << ed.second;
//         eds.push_back(ss.str());
//     }
//     return std::make_unique<Weighted> (eds, weights);
// }

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

Bipartite::Bipartite(const Bipartite &other)
{
    *this = Bipartite(other.part1, other.part2);
}

const std::vector<char> Bipartite::GetVertices() const
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

const std::vector<char> Bipartite::GetPart1() const
{
    return part1;
}

const std::vector<char> Bipartite::GetPart2() const
{
    return part2;
}

const std::vector<std::pair<char, char>> Bipartite::GetEdges() const
{
    std::vector<std::pair<char, char>> edges;
    for (auto v1: part1) {
        for (auto v2: part2) {
            edges.push_back(std::make_pair(v1, v2));
        }
    }
    return edges;
}

const std::string Bipartite::ToString() const
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

std::unique_ptr<TGraph> Bipartite::AsWeighted(int default_weight) const
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
    return std::make_unique<Weighted>(std::make_unique<WeightParams> (edges, weights));
}


Bipartite operator+=(Bipartite &first, Bipartite &second)
{
    auto second_p1 = second.GetPart1();
    auto second_p2 = second.GetPart2();
    for (auto v: second_p1) {
        if (std::find(first.part1.begin(), first.part1.end(), v) == first.part1.end()) {
            first.part1.push_back(v);
        }
    }
    for (auto v: second_p2) {
        if (std::find(first.part2.begin(), first.part2.end(), v) == first.part2.end()) {
            first.part2.push_back(v);
        }
    }
    return first;
}

Bipartite operator+(Bipartite &first, Bipartite &second)
{
    Bipartite sum = Bipartite(first);
    return sum+= second;
}

Bipartite operator+(Bipartite &first, Weighted &second)
{
    throw std::logic_error("Can not sum");
}

Simple operator+(Bipartite &first, TGraph &second)
{
    auto first_eds = first.GetEdges();
    auto second_eds = second.GetEdges();
    std::vector<std::string> edges;
    for (auto e: first_eds) {
        std::stringstream ss;
        std::string straight, reverse;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> straight >> reverse;
        if (std::find(edges.begin(), edges.end(), straight) == edges.end() &&
            std::find(edges.begin(), edges.end(), reverse) == edges.end()) {
            edges.push_back(straight);
        }
    }
    for (auto e: second_eds) {
        std::stringstream ss;
        std::string straight, reverse;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> straight >> reverse;
        if (std::find(edges.begin(), edges.end(), straight) == edges.end() &&
            std::find(edges.begin(), edges.end(), reverse) == edges.end()) {
            edges.push_back(straight);
        }
    }
    return Simple(edges);
}

Simple operator+=(Simple &first, TGraph &second)
{
    auto first_eds = first.GetEdges();
    auto second_eds = second.GetEdges();
    std::vector<std::string> edges;
    // for (auto e: first_eds) {
    //     std::stringstream ss;
    //     std::string straight, reverse;
    //     ss << e.first << e.second << " " << e.second << e.first;
    //     ss >> straight >> reverse;
    //     if (std::find(edges.begin(), edges.end(), straight) == edges.end() &&
    //         std::find(edges.begin(), edges.end(), reverse) == edges.end()) {
    //         edges.push_back(straight);
    //     }
    // }
    auto len = second_eds.size();
    for (int i = 0; i < len; ++i) {
        if (std::find(first.edges.begin(), first.edges.end(), second_eds[i]) == first.edges.end() &&
            std::find(first.edges.begin(), first.edges.end(), 
            std::make_pair(second_eds[i].second, second_eds[i].first)) == first.edges.end()) {
            first.edges.push_back(second_eds[i]);
        }
    }
    return first;
}

Simple operator+(Simple &first, TGraph &second)
{
    Simple another = Simple(first);
    return another += second;
}

Simple operator+(Simple &first, Weighted &second)
{
    throw std::logic_error("Can not sum");
}

Complete operator+=(Complete &first, Complete &second)
{
    // std::vector<char> verts;
    // for (auto v: first.GetVertices()) {
    //     verts.push_back(v);
    // }
    for (auto v: second.GetVertices()) {
        if (std::find(first.vertices.begin(), first.vertices.end(), v) == first.vertices.end()) {
            first.vertices.push_back(v);
        }
    }
    return first;
}

Complete operator+(Complete &first, Complete &second)
{
    Complete another = Complete(first);
    return another += second;
}

Simple operator+(Complete &first, TGraph &second)
{
    auto first_eds = first.GetEdges();
    auto second_eds = second.GetEdges();
    std::vector<std::string> edges;
    for (auto e: first_eds) {
        std::stringstream ss;
        std::string straight, reverse;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> straight >> reverse;
        if (std::find(edges.begin(), edges.end(), straight) == edges.end() &&
            std::find(edges.begin(), edges.end(), reverse) == edges.end()) {
            edges.push_back(straight);
        }
    }
    for (auto e: second_eds) {
        std::stringstream ss;
        std::string straight, reverse;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> straight >> reverse;
        if (std::find(edges.begin(), edges.end(), straight) == edges.end() &&
            std::find(edges.begin(), edges.end(), reverse) == edges.end()) {
            edges.push_back(straight);
        }
    }
    return Simple(edges);
}

Simple operator+(Complete &first, Weighted &second)
{
    throw std::logic_error("Can not sum");
}

Weighted operator+=(Weighted &first, Weighted &second)
{
    auto second_eds = second.GetEdges();
    auto second_w = second.GetWeights();
    auto len = second_eds.size();
    for (int i = 0; i < len; ++i) {
        if (std::find(first.edges.begin(), first.edges.end(), second_eds[i]) == first.edges.end() &&
            std::find(first.edges.begin(), first.edges.end(), 
            std::make_pair(second_eds[i].second, second_eds[i].first)) == first.edges.end()) {
            first.edges.push_back(second_eds[i]);
            first.weights.push_back(second_w[i]);
        }
    }
    return first;
}

Weighted operator+(Weighted &first, Weighted &second)
{
    Weighted another = Weighted(first);
    return another += second;
}

Weighted operator+(Weighted &first, TGraph &second)
{
    throw std::logic_error("Can not sum");
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

Complete::Complete(const Complete &other)
{
    *this = Complete(other.GetVertices());
}

const std::vector<char> Complete::GetVertices() const
{
    return vertices;
}

const std::vector<std::pair<char, char>> Complete::GetEdges() const
{
    std::vector<std::pair<char, char>> edges;
    for (auto iter = vertices.begin(); iter != vertices.end(); iter++) {
        for (auto another = iter + 1; another != vertices.end(); another++) {
            edges.push_back(std::make_pair(*iter, *another));
        }
    }
    return edges;
}

const std::string Complete::ToString() const
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

std::unique_ptr<TGraph> Complete::AsWeighted(int default_weight) const
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
    return std::make_unique<Weighted>(std::make_unique<WeightParams> (edges, weights));
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

Simple::Simple(const Simple &other)
{
    std::vector<std::string> eds;
    for (auto e: other.GetEdges()) {
        std::stringstream ss;
        ss << e.first << e.second;
        eds.push_back(ss.str());
    }
    *this = Simple(eds);
}

const std::vector<char> Simple::GetVertices() const
{
    return vertices;
}

const std::vector<std::pair<char, char>> Simple::GetEdges() const
{
    return edges;
}

const std::string Simple::ToString() const
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

std::unique_ptr<TGraph> Simple::AsWeighted(int default_weight) const
{
    std::vector<std::string> eds;
    std::vector<int> weights;
    for (auto e: edges) {
        std::stringstream ss;
        ss << e.first << e.second;
        eds.push_back(ss.str());
        weights.push_back(default_weight);
    }
    return std::make_unique<Weighted>(std::make_unique<WeightParams> (eds, weights));
}