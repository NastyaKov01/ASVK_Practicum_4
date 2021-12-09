#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <exception>

#include <iostream>

class Params
{
public:
    virtual ~Params() = default;
};

class BipartParams: public Params
{
public:    
    std::vector<char> part1;
    std::vector<char> part2;
    ~BipartParams() {}
    BipartParams(std::vector<char> p1, std::vector<char> p2);
};

class SimpleParams: public Params
{
 public:   
    std::vector<std::string> edges;
    SimpleParams(std::vector<std::string> eds);
};

class CompleteParams: public Params
{
public:    
    std::vector<char> vertices;
    CompleteParams(std::vector<char> verts);
};

class WeightParams: public Params
{
public:    
    std::vector<std::string> edges;
    std::vector<int> weights;
    WeightParams(std::vector<std::string> eds, std::vector<int> w);
};

class TGraph
{
public:
    virtual ~TGraph() {};
    virtual const std::string ToString() const = 0;
    virtual const std::vector<char> GetVertices() const = 0;
    virtual const std::vector<std::pair<char, char>> GetEdges() const = 0;
    virtual std::unique_ptr<TGraph> AsWeighted(int default_weight) const = 0;
};

class Weighted: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
    std::vector<int> weights;
public:
    using ParType = WeightParams;
    Weighted(std::unique_ptr<WeightParams> && params);
    Weighted(std::vector<std::string> eds, std::vector<int> w);
    Weighted(const Weighted &other);
    ~Weighted() {}
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;
    const std::vector<int> GetWeights() const;
    const std::string ToString() const;
    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    friend Weighted operator+=(Weighted &first, Weighted &second);
    friend Weighted operator+(Weighted &first, Weighted &second);
    friend Weighted operator+(Weighted &first, TGraph &second);
    friend Weighted operator-=(Weighted &first, Weighted &second);
    friend Weighted operator-(Weighted &first, TGraph &second);
};

class Simple: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
public:
    using ParType = SimpleParams;
    Simple(std::unique_ptr<SimpleParams> && params);
    Simple(std::vector<std::string> eds);
    Simple(const Simple &other);
    ~Simple() {}
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;
    const std::string ToString() const;
    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    friend Simple operator+=(Simple &first, TGraph &second);
    friend Simple operator+(Simple &first, TGraph &second);
    friend Simple operator+(Simple &first, Weighted &second);
    friend Simple operator-=(Simple &first, TGraph &second);
    friend Simple operator-(Simple &first, TGraph &second);
};

class Bipartite: public TGraph
{
    std::vector<char> part1;
    std::vector<char> part2;
public:
    using ParType = BipartParams;
    Bipartite(std::unique_ptr<BipartParams> && params);
    Bipartite(std::vector<char> p1, std::vector<char> p2);
    Bipartite(const Bipartite &other);
    ~Bipartite() {}
    const std::vector<char> GetVertices() const;
    const std::vector<char> GetPart1() const;
    const std::vector<char> GetPart2() const;
    const std::vector<std::pair<char, char>> GetEdges() const;
    const std::string ToString() const;
    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    friend Bipartite operator+=(Bipartite &first, Bipartite &second);
    friend Bipartite operator+(Bipartite &first, Bipartite &second);
    friend Bipartite operator+(Bipartite &first, Weighted &second);
    friend Simple operator+(Bipartite &first, TGraph &second);
    friend Bipartite operator-=(Bipartite &first, Bipartite &second);
    friend Bipartite operator-(Bipartite &first, Bipartite &second);
    friend Simple operator-(Bipartite &first, TGraph &second);
};

class Complete: public TGraph
{
    std::vector<char> vertices;
public:
    using ParType = CompleteParams;
    Complete(std::unique_ptr<CompleteParams> && params);
    Complete(std::vector<char> verts);
    Complete(const Complete &other);
    ~Complete() {}
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;
    const std::string ToString() const;
    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    friend Complete operator+=(Complete &first, Complete &second);
    friend Complete operator+(Complete &first, Complete &second);
    friend Simple operator+(Complete &first, TGraph &second);
    friend Simple operator+(Complete &first, Weighted &second);
    friend Complete operator-=(Complete &first, Complete &second);
    friend Complete operator-(Complete &first, Complete &second);
    friend Simple operator-(Complete &first, TGraph &second);
};
