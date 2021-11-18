#include "generator.h"

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
        tree.push_back(next_node);
    }
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
        // std::cout << "fold:" << stop-start << " " << start << " " << stop << std::endl;
        set_degrees(tree, start, stop, degree_mean, degree_variance);
        for (int i = start; i < stop; ++i) {
            if (stop - start == 1) {
                break;
            }
            std::uniform_int_distribution<int> distr(start, stop - 1);
            int val = distr(generator);
            int max_steps = tree[i]->get_degree() - tree[i]->dep_num();
            for (int k = 0; k < max_steps; ++k) {
                // std::cout << "!!! i = " << i << " " << "val = " << val << std::endl;
                int cnt = 0;
                while (val == i || val == start || tree[i]->is_in_fol(val) || tree[i]->is_in_dep(val) || 
                        tree[val]->fol_num() + tree[val]->dep_num() == tree[val]->get_degree()) {
                    if (cnt == stop - start) {
                        break;
                    }
                    val = distr(generator);
                    cnt++;
                }
                // std::cout << "i=" << i << " " << "val="<< val << std::endl;
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