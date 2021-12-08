#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

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
    Weighted(std::unique_ptr<WeightParams> && params);
    Weighted(std::vector<std::string> eds, std::vector<int> w);
    ~Weighted() {}
    const std::vector<char> GetVertices();
    const std::vector<std::pair<char, char>> GetEdges();
    const std::string ToString();
};

class Bipartite: public TGraph
{
    std::vector<char> part1;
    std::vector<char> part2;
public:
    using ParType = BipartParams;
    Bipartite(std::unique_ptr<BipartParams> && params);
    Bipartite(std::vector<char> p1, std::vector<char> p2);
    ~Bipartite() {}
    const std::vector<char> GetVertices();
    const std::vector<std::pair<char, char>> GetEdges();
    const std::string ToString();
    Weighted AsWeighted(int default_weight);
};

class Complete: public TGraph
{
    std::vector<char> vertices;
public:
    using ParType = CompleteParams;
    Complete(std::unique_ptr<CompleteParams> && params);
    Complete(std::vector<char> verts);
    ~Complete() {}
    const std::vector<char> GetVertices();
    const std::vector<std::pair<char, char>> GetEdges();
    const std::string ToString();
    Weighted AsWeighted(int default_weight);
};

class Simple: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
public:
    using ParType = SimpleParams;
    Simple(std::unique_ptr<SimpleParams> && params);
    Simple(std::vector<std::string> eds);
    ~Simple() {}
    const std::vector<char> GetVertices();
    const std::vector<std::pair<char, char>> GetEdges();
    const std::string ToString();
    Weighted AsWeighted(int default_weight);
};