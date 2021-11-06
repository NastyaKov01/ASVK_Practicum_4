#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


class Node
{
    int duration = 0;
    std::vector<int> dependencies;
    std::vector<int> followers;
public:
    // Node(int dur): duration(dur) {}

    void set_duration(int value) { duration = value; }

    int get_duration() { return duration; }

    void set_dependence(int num)
    {
        dependencies.push_back(num);
    }

    void set_follower(int num)
    {
        followers.push_back(num);
    }

    std::vector<int>& get_dependencies()
    {
        return dependencies;
    }

    std::vector<int>& get_followers()
    {
        return followers;
    }
};

void read_tree(std::string name, std::vector<Node*> &tree)
{
    std::ifstream in(name);
    std::string line;
    bool num = false;
    int nodes_num, cnt = 0;
    while(getline(in,line)) {
        if (line[0] == '#' || line.empty() || isspace(line[0])) {
            continue;
        }
        std::stringstream ss;
        std::cout << line << std::endl;
        int fin = line.find("#");
        if (fin != -1) {
            std::string new_line;
            for (int i = 0; i < fin; ++i) {
                new_line.push_back(line[i]);
            }
            line = new_line;
        }
        ss << line;
        if (!num) {
            ss >> nodes_num;
            num = true;
            std::cout << "num= "<< nodes_num << std::endl;
            for (int i = 0; i < nodes_num; ++i) {
                Node * next_node = new Node();
                tree.push_back(next_node);
            }
        } else {
            int dur;
            ss >> dur;
            tree[cnt]->set_duration(dur);
            int tmp = -1;
            ss >> tmp;
            while (tmp != -1) {
                tree[cnt]->set_dependence(tmp);
                tree[tmp]->set_follower(cnt);
                tmp = -1;
                ss >> tmp;
            }
            cnt++;
        }
    }
    in.close();
}


int main(int argc, char **argv)
{
    std::vector<Node*> tree;
    read_tree(argv[1], tree);
    std::cout << "Tree in memory" << std::endl;
    for (auto &v: tree) {
        std::cout << v->get_duration();
        auto dep = v->get_dependencies();
        std::cout << "  dep: ";
        for (auto &i: dep) {
            std::cout << i << " ";
        }
        std::cout << "  fol: ";
        auto fol = v->get_followers();
        for (auto &i: fol) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }


    auto num = tree.size();
    for (int i = 0; i < num; ++i) {
        delete tree[i];
    }
    return 0;
}