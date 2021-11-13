#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <set>


class Node
{
    int duration = 0;
    int degree = 0;
    std::vector<int> dependencies;
    std::vector<int> followers;
public:
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

    void del_dependence(int num)
    {
        dependencies.erase(std::find(dependencies.begin(), dependencies.end(), num));
    }

    void del_follower(int num)
    {
        followers.erase(std::find(followers.begin(), followers.end(), num));
    }

    std::vector<int>& get_dependencies()
    {
        return dependencies;
    }

    std::vector<int>& get_followers()
    {
        return followers;
    }

    bool is_in_dep(int num)
    {
        if (std::find(dependencies.begin(), dependencies.end(), num) == dependencies.end()) {
            return false;
        }
        return true;
    }

    bool is_in_fol(int num)
    {
        if (std::find(followers.begin(), followers.end(), num) == followers.end()) {
            return false;
        }
        return true;
    }

    int dep_num() { return dependencies.size(); }

    int fol_num() { return followers.size(); }

    void set_degree(int value) { degree = value; }

    int get_degree() { return degree; }
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

void write_to_dot(std::vector<Node*> &tree, int nodes_num)
{
    std::ofstream out("tree.dot");
    out << "digraph G {" << std::endl;
    std::string cur_node, fol_node;
    std::stringstream ss;
    for (int i = 0; i < nodes_num; ++i) {
        ss.clear();
        auto fol = tree[i]->get_followers();
        ss << '"' << i << ":" << tree[i]->get_duration() << '"';
        ss >> cur_node;
        for (auto &next: fol) {
            ss.clear();
            ss << '"' << next << ":" << tree[next]->get_duration() << '"';
            ss >> fol_node;
            out << cur_node << "->" << fol_node << ";" << std::endl;
        }
    }
    out << "}";
    out.close();
}

// void remove_cycles(std::vector<Node*> &tree, int cur, std::vector<int> path)
// {
//     std::cout << "cur = " << cur << std::endl;
//     std::cout << "cur_path = ";
//     for (auto node: path) {
//         std::cout << node << " ";
//     }
//     std::cout << std::endl;
//     if (std::find(path.begin(), path.end(), cur) != path.end()) {
//         int prev = *(path.end() - 1);
//         std::cout << "prev = " << prev << " cur = " << cur << std::endl;
//         tree[cur]->del_dependence(prev);
//         tree[prev]->del_follower(cur);
//     } else {
//         std::cout << "insert" << std::endl;
//         path.push_back(cur);
//         for (auto &next: tree[cur]->get_followers()) {
//             remove_cycles(tree, next, path);
//         }
//     }
// }

bool path_exists(std::vector<Node*> &tree, int start, int finish)
{
    if (start == finish) {
        return true;
    }
    bool res = false;
    auto fol = tree[start]->get_followers();
    for (auto next: fol) {
        bool tmp = path_exists(tree, next, finish);
        res = res || tmp;
    }
    return res;
}

int gen_parts_num(int nodes_num, int parts_mean, double in_parts_variance)
{
    std::normal_distribution<double> parts_distr(nodes_num/parts_mean, sqrt(in_parts_variance));
    std::default_random_engine generator(time(0));
    int in_parts_num = int(parts_distr(generator));
    if (in_parts_num < 0) {
        in_parts_num = -in_parts_num;
    }
    if (in_parts_num == 0) {
        in_parts_num = 1;
    }
    return in_parts_num;
}

void set_degrees(std::vector<Node*> &tree, int start, int stop, int degree_mean, double degree_variance)
{
    int degree, max_degree = stop - start - 1;
    std::normal_distribution<double> degree_distr(degree_mean, degree_variance);
    std::default_random_engine generator(time(0));
    for (int i = start; i < stop; ++i) {
        if (stop - start == 1) {
            break;
        }
        degree = stop - start;
        while (degree > stop - start - 1) {
            degree = int(degree_distr(generator));
            if (degree < 0) {
                degree = -degree;
            }
            degree = degree % max_degree;
        }
        if (degree == 0 && max_degree > 1) {
            max_degree--;
        }
        tree[i]->set_degree(degree);
        std::cout << "degree=" << degree << std::endl;
    }
    for (int i = start; i < stop; ++i) {
        if (tree[i]->get_degree() > max_degree) {
            tree[i]->set_degree(max_degree);
        }
    }
}

void set_weights(std::vector<Node*> &tree, int nodes_num, int weight_min, int weight_max, double weight_variance)
{
    std::default_random_engine generator(time(0));
    std::normal_distribution<double> weights_distr((weight_max-weight_min)/2, sqrt(weight_variance));
    for (int i = 0; i < nodes_num; ++i) {
        Node * next_node = new Node();
        int dur = int(weights_distr(generator));
        if (dur < 0) {
            dur = -dur;
        }
        next_node->set_duration(dur);
        std::cout << dur << " ";
        tree.push_back(next_node);
    }
    std::cout << std::endl;
}

std::vector<Node*> generate_tree(int nodes_num, int weight_min, int weight_max, double weight_variance,
                                int parts_mean, double in_parts_variance, int degree_mean, double degree_variance)
{
    std::vector<Node*> tree;
    std::default_random_engine generator(time(0));
    set_weights(tree, nodes_num, weight_min, weight_max, weight_variance);
    int start = 0, stop = nodes_num, degree;
    int in_parts_num = gen_parts_num(nodes_num, parts_mean, in_parts_variance);
    if (in_parts_num < nodes_num) {
        stop = in_parts_num;
    }
    while (start != nodes_num) {
        std::cout << "fold:" << stop-start << " " << start << " " << stop << std::endl;
        set_degrees(tree, start, stop, degree_mean, degree_variance);
        for (int i = start; i < stop; ++i) {
            if (stop - start == 1) {
                break;
            }
            std::uniform_int_distribution<int> distr(start, stop - 1);
            int val = distr(generator);
            int max_steps = tree[i]->get_degree() - tree[i]->dep_num();
            for (int k = 0; k < max_steps; ++k) {
                std::cout << "!!! i = " << i << " " << "val = " << val << std::endl;
                int cnt = 0;
                while (val == i || val == start || tree[i]->is_in_fol(val) || tree[i]->is_in_dep(val) || 
                        tree[val]->fol_num() + tree[val]->dep_num() == tree[val]->get_degree()) {
                    if (cnt == stop - start) {
                        break;
                    }
                    val = distr(generator);
                    cnt++;
                }
                std::cout << "i=" << i << " " << "val="<< val << std::endl;
                if (!path_exists(tree, val, i) && 
                    !(val == i || val == start || tree[i]->is_in_fol(val) || tree[i]->is_in_dep(val))) {
                    tree[i]->set_follower(val);
                    tree[val]->set_dependence(i);
                }
                val = distr(generator);
            }
        }
        std::vector<int> path;
        start = stop;
        in_parts_num = in_parts_num = gen_parts_num(nodes_num, parts_mean, in_parts_variance);
        if (start + in_parts_num >= nodes_num) {
            stop = nodes_num;
        } else {
            stop += in_parts_num;
        }
    }
    return tree;
}


int main(int argc, char **argv)
{
    bool tmp = false;
    tmp = tmp || true;
    std::cout << tmp << std::endl;
    std::vector<Node*> tree;
    if (argc == 1) {
        tree = generate_tree(20, 10, 1000, 100000, 2, 4, 2, 4);
    } else {
        read_tree(argv[1], tree);
    }
    auto nodes_num = tree.size();

    std::cout << "Tree in memory" << std::endl;
    std::cout << nodes_num << " nodes " << std::endl;
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
    write_to_dot(tree, nodes_num);
    
    for (int i = 0; i < nodes_num; ++i) {
        delete tree[i];
    }
    return 0;
}