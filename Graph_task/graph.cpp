#include <iostream>

#include "factory.h"

int main()
{
    std::vector<char> v1 {'A','B','M', 'N'};
    std::vector<char> v2 {'E','F', 'K'};
    auto b1 = GraphFactory().Create(std::string("bipartite"), std::make_unique<BipartParams> (v1, v2));
    std::cout << "Bipartite Graph" << std::endl;
    for (auto e: b1->GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << b1->ToString() << std::endl;

    std::vector<char> v3 {'A','B','C','D'};
    std::vector<char> v4 {'E','F'};
    auto b2 = GraphFactory().Create(std::string("bipartite"), std::make_unique<BipartParams> (v3, v4));
    std::cout << "Bipartite Graph" << std::endl;
    for (auto e: b2->GetEdges()) {
        std::cout << e.first << e.second << " ";
    }
    std::cout << std::endl;
    std::cout << b2->ToString() << std::endl;

    // auto b3 = Bipartite(b2);
    // std::cout << b3->ToString() << std::endl;

    auto b4 = dynamic_cast<Bipartite*>(b1.get());
    // b2.release();
    // auto b4 = std::unique_ptr<Bipartite>(b1);
    auto b5 = dynamic_cast<Bipartite*>(b2.get());
    // b1.release();
    // auto b5 = std::unique_ptr<Bipartite>(b2);
    std::cout << "      bipart + bipart" << std::endl;
    std::cout << (*b4+*b5).ToString() << std::endl;
    *b4 += *b5;
    std::cout << b4->ToString() << std::endl;

    std::vector<std::string> wv1{"FD", "ED"};
    std::vector<int> wv2 {5,6};
    auto w2 = GraphFactory().Create("weighted", std::make_unique<WeightParams>(wv1, wv2));
    std::cout << "Weighted Graph" << std::endl;
    std::cout << w2->ToString() << std::endl;

    auto w3 = dynamic_cast<Weighted*>(w2.get());
    std::cout << "      bipart + weight" << std::endl;
    try {
        auto sum = *b4 + *w3;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    std::vector<std::string> v{"EF", "FA"};
    auto s2 = GraphFactory().Create("simple", std::make_unique<SimpleParams>(v));
    std::cout << "Simple Graph" << std::endl;
    std::cout << s2->ToString() << std::endl;

    auto c2 = GraphFactory().Create("complete", std::make_unique<CompleteParams> (v1));
    std::cout << "Complete Graph" << std::endl;
    std::cout << c2->ToString() << std::endl;
    
    auto c3 = dynamic_cast<Complete*>(c2.get());
    auto c4 = Complete(*c3);
    std::cout << c4.ToString() << std::endl;

    std::cout << b5->ToString() << std::endl;
    std::cout << s2->ToString() << std::endl;
    std::cout << "      bipart + simple" << std::endl;
    std::cout << (*b5 + *s2).ToString() << std::endl;

    std::cout << c2->ToString() << std::endl;
    std::cout << "      bipart + complete" << std::endl;
    std::cout << (*b5 + *c2).ToString() << std::endl;
    auto s3 = dynamic_cast<Simple*>(s2.get());
    std::cout << "      simple + bipart" << std::endl;
    std::cout << (*s3 + *b5).ToString() << std::endl;
    std::cout << "      simple + simple" << std::endl;
    std::cout << (*s3 + *s2).ToString() << std::endl;
    std::cout << "      simple + complete" << std::endl;
    std::cout << (*s3 + *c3).ToString() << std::endl;
    std::cout << "      simple + weighted" << std::endl;
    try {
        std::cout << (*s3 + *w3).ToString() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    
    std::cout << "      complete + bipart" << std::endl;
    std::cout << (c4 + *b5).ToString() << std::endl;
    std::cout << "      complete + simple" << std::endl;
    std::cout << (c4 + *s2).ToString() << std::endl;
    std::cout << "      complete + complete" << std::endl;
    std::cout << (c4 + c4).ToString() << std::endl;
    auto c5 = Complete({'M', 'N', 'K'});
    std::cout << c5.ToString() << std::endl;
    c5 += c4;
    std::cout << c5.ToString() << std::endl;
    std::cout << "      complete + weighted" << std::endl;
    try {
        std::cout << (c4 + *w3).ToString() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    std::vector<std::string> wv3{"AB", "AD", "BF"};
    std::vector<int> wv4 {10,12, 15};
    auto w5 = GraphFactory().Create("weighted", std::make_unique<WeightParams>(wv3, wv4));
    auto w4 = dynamic_cast<Weighted*>(w5.get());
    std::cout << "      weighted + bipart" << std::endl;
    try {
        std::cout << (*w4 + *b4).ToString() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    
    std::cout << "      weighted + simple" << std::endl;
    try {
        std::cout << (*w4 + *s2).ToString() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    
    std::cout << "      weighted + complete" << std::endl;
    try {
        std::cout << (*w4 + c4).ToString() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    std::cout << "      weighted + weighted" << std::endl;
    std::cout << (*w4 + *w3).ToString() << std::endl;
    *w4 += *w3;
    std::cout << w4->ToString() << std::endl;
    return 0;
}