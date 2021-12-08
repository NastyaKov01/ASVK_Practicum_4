#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <map>

class Params
{
public:
    // Params() {}
    virtual ~Params() = default;
};

class BipartParams: public Params
{
public:    
    std::vector<char> part1;
    std::vector<char> part2;
    ~BipartParams() {}
    BipartParams(std::vector<char> p1, std::vector<char> p2)
    {
        for (auto v: p1) {
            part1.push_back(v);
        }
        for (auto v: p2) {
            part2.push_back(v);
        }
    }
};

class SimpleParams: public Params
{
 public:   
    std::vector<std::string> edges;

    SimpleParams(std::vector<std::string> eds)
    {
        for (auto e: eds) {
            edges.push_back(e);
        }
    }
};

class CompleteParams: public Params
{
public:    
    std::vector<char> vertices;

    CompleteParams(std::vector<char> verts)
    {
        for (auto v: verts) {
            vertices.push_back(v);
        }
    }
};

class WeightParams: public Params
{
public:    
    std::vector<std::string> edges;
    std::vector<int> weights;

    WeightParams(std::vector<std::string> eds, std::vector<int> w)
    {
        for (auto e: eds) {
            edges.push_back(e);
        }
        for (auto weight: w) {
            weights.push_back(weight);
        }
    }
};

class TGraph
{
public:
    virtual ~TGraph() {};
    // virtual TGraph AsWeighted(int default_weight) = 0;
    virtual const std::string ToString() = 0;
    virtual const std::vector<char> GetVertices() = 0;
    virtual const std::vector<std::pair<char, char>> GetEdges() = 0;
};

class Weighted: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
    std::vector<int> weights;
public:
    using ParType = WeightParams;
    Weighted(std::unique_ptr<WeightParams> && params)
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
    Weighted(std::vector<std::string> eds, std::vector<int> w)
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
    ~Weighted() {}

    const std::vector<char> GetVertices()
    {
        return vertices;
    }

    const std::vector<std::pair<char, char>> GetEdges()
    {
        return edges;
    }

    const std::string ToString()
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
};

class Bipartite: public TGraph
{
    std::vector<char> part1;
    std::vector<char> part2;
    // std::vector<std::pair<char, char>> edges;
public:
    using ParType = BipartParams;
    Bipartite(std::unique_ptr<BipartParams> && params)
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
    Bipartite(std::vector<char> p1, std::vector<char> p2)
    {
        for (auto v: p1) {
            part1.push_back(v);
        }
        for (auto v: p2) {
            part2.push_back(v);
        }
    }
    ~Bipartite() {}

    const std::vector<char> GetVertices()
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

    const std::vector<std::pair<char, char>> GetEdges()
    {
        std::vector<std::pair<char, char>> edges;
        for (auto v1: part1) {
            for (auto v2: part2) {
                edges.push_back(std::make_pair(v1, v2));
            }
        }
        return edges;
    }

    const std::string ToString()
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

    Weighted AsWeighted(int default_weight)
    {
        std::vector<std::string> edges;
        std::vector<int> weights;
        for (auto v1: part1) {
            for (auto v2: part2) {
                std::stringstream ss;
                ss << v1 << v2;
                edges.push_back(ss.str());
            }
            weights.push_back(default_weight);
        }
        return Weighted(edges, weights);
    }
};

class Complete: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
public:
    using ParType = CompleteParams;
    Complete(std::unique_ptr<CompleteParams> && params) {
        auto verts = params->vertices;
        auto len = verts.size();
        for (auto iter = verts.begin(); iter != verts.end(); iter++) {
            vertices.push_back(*iter);
            for (auto another = iter + 1; another != verts.end(); another++) {
                edges.push_back(std::make_pair(*iter, *another));
            }
        }
    }
    Complete(std::vector<char> verts) {
        for (auto iter = verts.begin(); iter != verts.end(); iter++) {
            vertices.push_back(*iter);
            for (auto another = iter + 1; another != verts.end(); another++) {
                edges.push_back(std::make_pair(*iter, *another));
            }
        }
    }
    ~Complete() {}

    const std::vector<char> GetVertices()
    {
        return vertices;
    }

    const std::vector<std::pair<char, char>> GetEdges()
    {
        return edges;
    }

    const std::string ToString()
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

    // Weighted AsWeighted(int default_weight)
    // {
    //     std::vector<std::string> edges;
    //     std::vector<int> weights;
    //     for (auto v1: part1) {
    //         for (auto v2: part2) {
    //             std::stringstream ss;
    //             ss << v1 << v2;
    //             edges.push_back(ss.str());
    //         }
    //         weights.push_back(default_weight);
    //     }
    //     return Weighted(edges, weights);
    // }
};

class Simple: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
public:
    using ParType = SimpleParams;
    Simple(std::unique_ptr<SimpleParams> && params)
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
    Simple(std::vector<std::string> eds)
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
    ~Simple() {}

    const std::vector<char> GetVertices()
    {
        return vertices;
    }

    const std::vector<std::pair<char, char>> GetEdges()
    {
        return edges;
    }

    const std::string ToString()
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
};


class GraphFactory {
    class TImpl
    {
        class ICreator {
        public:
            virtual ~ICreator(){}
            virtual std::unique_ptr<TGraph> Create(std::unique_ptr<Params>&& params) const = 0;
        };
        using TCreatorPtr = std::shared_ptr<ICreator>;
        using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
        TRegisteredCreators RegisteredCreators;
    public:
        template <class TCurrentObject>
        class TCreator : public ICreator{
            std::unique_ptr<TGraph> Create(std::unique_ptr<Params>&& params) const override{
                auto CurParams = dynamic_cast<typename TCurrentObject::ParType*>(params.get());
                params.release();
                return std::make_unique<TCurrentObject>(std::unique_ptr<typename TCurrentObject::ParType>(CurParams));
            }
        };

        TImpl() { RegisterAll();}

        template <typename T>
        void RegisterCreator(const std::string& type) {
            RegisteredCreators[type] = std::make_shared<TCreator<T>>();
        }
        void RegisterAll() {
            RegisterCreator<Bipartite>("bipartite");
            RegisterCreator<Complete>("complete");
            RegisterCreator<Simple>("simple");
            RegisterCreator<Weighted>("weighted");
        }
        std::unique_ptr<TGraph> CreateObject(const std::string& type, std::unique_ptr<Params>&& params) const {
            auto creator = RegisteredCreators.find(type);
            if (creator == RegisteredCreators.end()) {
                return nullptr;
            }
            return creator->second->Create(std::move(params));
        }
    };

    std::unique_ptr<const TImpl> Impl;
public:
    GraphFactory() : Impl(std::make_unique<TImpl>()) {}
    ~GraphFactory(){}
    std::unique_ptr<TGraph> Create(const std::string& type, std::unique_ptr<Params>&& params) const
    {
        return Impl->CreateObject(type, std::move(params));
    }
};

int main()
{
    Bipartite b({'A','B','C','D'}, {'E','F'});
    std::cout << "Bipartite Graph" << std::endl;
    for (auto v: b.GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: b.GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << b.ToString() << std::endl;

    std::vector<char> v1 {'A','B','C','D'};
    std::vector<char> v2 {'E','F'};
    // std::unique_ptr<Bipartite> b2 = GraphFactory().Create("bipartite", v1, v2);
    auto b2 = GraphFactory().Create(std::string("bipartite"), std::make_unique<BipartParams> (v1, v2));
    // auto b2 = std::make_unique<Bipartite>(std::vector<char>({'A','B','C','D'}), std::vector<char>({'E','F'}));
    std::cout << "Bipartite Graph" << std::endl;
    for (auto v: b2->GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: b2->GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << b2->ToString() << std::endl;


    Simple s({"EF", "FA"});
    std::cout << "Simple Graph" << std::endl;
    for (auto v: s.GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: s.GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << s.ToString() << std::endl;

    std::vector<std::string> v{"EF", "FA"};
    auto s2 = GraphFactory().Create("simple", std::make_unique<SimpleParams>(v));
    std::cout << "Simple Graph" << std::endl;
    for (auto v: s2->GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: s2->GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << s2->ToString() << std::endl;

    Complete c ({'A', 'B', 'C', 'D'});
    std::cout << "Complete Graph" << std::endl;
    for (auto v: c.GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: c.GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << c.ToString() << std::endl;


    auto c2 = GraphFactory().Create("complete", std::make_unique<CompleteParams> (v1));
    std::cout << "Complete Graph" << std::endl;
    for (auto v: c2->GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: c2->GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << c2->ToString() << std::endl;

    Weighted w({"FD", "ED"}, {5, 6});
    std::cout << "Weighted Graph" << std::endl;
    for (auto v: w.GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: w.GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << w.ToString() << std::endl;

    std::vector<std::string> wv1{"FD", "ED"};
    std::vector<int> wv2 {5,6};
    auto w2 = GraphFactory().Create("weighted", std::make_unique<WeightParams>(wv1, wv2));
    std::cout << "Weighted Graph" << std::endl;
    for (auto v: w2->GetVertices()) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (auto e: w2->GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << w2->ToString() << std::endl;
    return 0;
}