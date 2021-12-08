#include <iostream>

#include "factory.h"

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
    std::cout << b.AsWeighted(2).ToString() << std::endl;

    std::vector<char> v1 {'A','B','C','D'};
    std::vector<char> v2 {'E','F'};
    auto b2 = GraphFactory().Create(std::string("bipartite"), std::make_unique<BipartParams> (v1, v2));
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
    std::cout << s.AsWeighted(12).ToString() << std::endl;

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
    std::cout << c.AsWeighted(5).ToString() << std::endl;


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