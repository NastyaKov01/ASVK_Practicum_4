#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <map>
#include <thread>

#include "generator.h"


struct Job
{
    int num;
    Job *next = NULL;
    Job *prev = NULL;

    Job(int val, Job *other): num(val), next(other) {}
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

long long find_proc_time(int proc, std::map<int, Job*>  &processors, std::vector<Node*> &tree)
{
    auto node = processors[proc];
    if (node == NULL) {
        return 0;
    }
    while(node->next != NULL) {
        node = node->next;
    }
    return tree[node->num]->get_time();
}

long long max_duration(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree)
{
    long long res = 0;
    for (int i = 0; i < num_proc; ++i) {
        long long cur_sum = find_proc_time(i, processors, tree);
        if (cur_sum > res) {
            res = cur_sum;
        }
    }
    return res;
}

int proc_with_min_time(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree)
{
    long long end_time = 0, tmp = 0;
    end_time = find_proc_time(0, processors, tree);
    std::cout << "%%%" << 0 << " " << tmp << " " << end_time << std::endl;
    int num = 0;
    for (int i = 1; i < num_proc; ++i) {
        tmp = find_proc_time(i, processors, tree);
        std::cout << "%%%" << i << " " << tmp << " " << end_time << std::endl;
        if (tmp < end_time) {
            end_time = tmp;
            num = i;
        }
    }
    return num;
}

long long find_dep_time(int cur_node, std::vector<Node*> &tree)
{
    auto dep = tree[cur_node]->get_dependencies();
    long long dep_end_time = 0;
    for (auto d:dep) {
        long long tmp = tree[d]->get_time();
        if (tmp > dep_end_time) {
            dep_end_time = tmp;
        }
    }
    return dep_end_time;
}

void adding(int root_num, std::map<int, Job*>  &processors, int num_proc, 
                std::vector<Node*> &tree, std::vector<int> &placed)
{
    std::cout << "***" << std::endl;
    std::vector<int> visited;
    visited.push_back(root_num);
    int cur_node = root_num;
    while (visited.size() != 0) {
        cur_node = *(visited.end()-1);
        std::cout << "cur_node: " << cur_node << " ";
        visited.pop_back();
        auto dep = tree[cur_node]->get_dependencies();
        bool free = true;
        for (auto d:dep) {
            if (std::find(placed.begin(), placed.end(), d) == placed.end()){
                free = false;
                std::cout << "unplaced node:" << d << std::endl;
                break;
            }
        }
        if (free) {
            std::cout << "free" << std::endl;
            if (std::find(placed.begin(), placed.end(), cur_node) == placed.end()) {
                placed.push_back(cur_node);
                std::cout << "cur placed: ";
                for (auto k: placed) {
                    std::cout << k << " ";
                }
                std::cout << std::endl;
                int proc = proc_with_min_time(processors, num_proc, tree);
                std::cout << proc << std::endl;
                auto dur = tree[cur_node]->get_duration();
                long long dep_end_time = find_dep_time(cur_node, tree);
                int proc_fin_time = find_proc_time(proc, processors, tree);
                std::cout <<"!!!" << dep_end_time << " " << proc_fin_time << std::endl;
                if (dep_end_time < proc_fin_time) {
                    dep_end_time = proc_fin_time;
                }
                std::cout << "!!!" << dep_end_time + dur << std::endl;
                tree[cur_node]->set_time(dep_end_time + dur);
                Job *new_job = new Job(cur_node, NULL);
                auto node = processors[proc];
                if (node == NULL) {
                    processors[proc] = new_job;
                } else {
                    while (node->next != NULL) {
                        node = node->next;
                    }
                    node->next = new_job;
                    new_job->prev = node;
                }
                for (int i = 0; i < num_proc; ++i) {
                    std::cout << "processor " << i << ": ";
                    auto node = processors[i];
                    while (node != NULL) {
                        std::cout << node->num << " ";
                        node = node->next;
                    }
                    long long end_time = find_proc_time(i, processors, tree);
                    std::cout << "| " << end_time;
                    std::cout << std::endl;
                }
                // processors[proc].push_back(cur_node);
            }
            std::cout << "###" << std::endl;
            
        }
        auto followers = tree[cur_node]->get_followers();
        std::default_random_engine generator(time(0));
        // std::shuffle(followers.begin(), followers.end(), generator);
        for (auto fol: followers) {
            visited.push_back(fol);
        }
    }
}

void start_pos(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree, std::vector<int> &roots)
{
    std::vector<int> placed;
    int nodes_num = tree.size();
    std::default_random_engine generator(time(0));
    // std::shuffle(roots.begin(), roots.end(), generator);
    while (placed.size() != nodes_num) {
        for (auto root: roots) {
            adding(root, processors, num_proc, tree, placed);
        }
    }
}

double boltzmann_temp(double temp, int i)
{
    return temp / log(1 + i);
}

double probability(double F, double temp)
{
    return exp(-F / temp);
}

bool check_correctness(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree)
{
    // std::cout << "CHECK CORRECTNESS" << std::endl;
    for (int i = 0; i < num_proc; ++i) {
        
        auto cur_node = processors[i];
        auto prev = cur_node->prev;
        while (cur_node != NULL) {
            auto dep_time = find_dep_time(cur_node->num, tree);
            auto node_start_time = tree[cur_node->num]->get_time() - tree[cur_node->num]->get_duration();
            if (node_start_time < dep_time) {
                return false;
            }
            if (prev != NULL &&  node_start_time < tree[prev->num]->get_time()) {
                return false;
            }
            prev = cur_node;
            cur_node = cur_node->next;
        }
    }
    return true;
}

Job *find_job_by_number(int cur_node, std::map<int, Job*>  &processors, int num_proc, int *old_proc)
{
    Job *cur;
    bool found = false;
    for (int i = 0; i < num_proc; ++i) {
        cur = processors[i];
        while (cur != NULL) {
            if (cur->num == cur_node) {
                *old_proc = i;
                found = true;
                break;
            }
            cur = cur->next;
        }
        if (found) {
            return cur;
        }
    }
    return cur;
}

void change_processor(std::map<int, Job*>  &processors, int num_proc,
                            std::vector<Node*> &tree, int nodes_num)
{
    // std::default_random_engine generator(time(0));
    std::uniform_int_distribution<int> node_distr(0, nodes_num-1);
    std::random_device generator;
    int cur_node = node_distr(generator), old_proc, new_proc;
    bool found = false;
    // cur_node = 5;
    Job *cur = find_job_by_number(cur_node, processors, num_proc, &old_proc);
    Job *prev = cur->prev;
    std::uniform_int_distribution<int> proc_distr(0, num_proc-1);
    new_proc = proc_distr(generator);
    while(new_proc == old_proc) {
        new_proc = proc_distr(generator);
    }
    
    // cur_node=5;
    // old_proc=0;
    // new_proc=1;

    std::cout << "node to change: " << cur_node << std::endl;
    std::cout << "old proc: " << old_proc << std::endl;
    std::cout << "new proc: " << new_proc << std::endl;

    auto dep_end_time = find_dep_time(cur_node, tree);
    std::cout << "dep_time: " << dep_end_time << std::endl;
    auto node = processors[new_proc];
    std::cout << "QQQ " << new_proc << " " << node->num << std::endl;

    while(node != NULL) {
        std::cout << ")))" << std::endl;
        long long node_end_time = tree[node->num]->get_time();
        std::cout << "node_time: " << node_end_time << std::endl;
        if (node_end_time < dep_end_time) {
            node = node->next;
        } else {
            int shift = tree[cur->num]->get_duration();
            long long node_start_time = tree[node->num]->get_time() - tree[node->num]->get_duration();
            if (node_start_time - shift >= dep_end_time && node->prev != NULL) {
                node = node->prev;
            }
            std::cout << "cur->num " << cur->num << std::endl;
            if (prev != NULL) {
                std::cout << "          " << prev->num << std::endl;
                prev->next = cur->next;
                if (cur->next != NULL) {
                    cur->next->prev = prev;
                    std::cout << "          " << cur->next->num << std::endl;
                }
            } else {
                processors[old_proc] = cur->next;
                if (cur->next != NULL) {
                    cur->next->prev = NULL;
                }
            }
            node_start_time = tree[node->num]->get_time() - tree[node->num]->get_duration();
            auto next_node = node->next;
            if (node == processors[new_proc] && node_start_time - shift >= dep_end_time) {
                processors[new_proc] = cur;
                cur->prev = NULL;
                cur->next = node;
                node->prev = cur;
                tree[cur->num]->set_time(dep_end_time + shift);
            } else {
                cur->next = node->next;
                if (node->next != NULL) {
                    node->next->prev = cur;
                }
                node->next = cur;
                cur->prev = node;
                long long node_time = tree[node->num]->get_time();
                if (dep_end_time > node_time) {
                    node_time = dep_end_time;
                }
                tree[cur->num]->set_time(node_time + shift);
            }
            if (prev != NULL) {
                node = prev->next;
            } else {
                node = processors[old_proc];
            }
            while (node != NULL) {
                std::cout << "&&&" << std::endl;
                long long prev_time;
                if (prev == NULL) {
                    prev_time = 0;
                } else {
                    prev_time = tree[prev->num]->get_time();
                }
                long long dep_end_time = find_dep_time(node->num, tree);
                if (prev_time < dep_end_time) {
                    prev_time = dep_end_time;
                }
                tree[node->num]->set_time(prev_time+tree[node->num]->get_duration());
                prev = node;
                node = node->next;
            }
            
            std::vector<int> queue;
            node = find_job_by_number(cur_node, processors, num_proc, &old_proc);
            queue.push_back(node->num);
            bool correct = false;
            while (queue.size() != 0) {
                int cur_node = queue[0];
                std::cout << "node from queue " << cur_node << std::endl;
                queue.erase(queue.begin());
                int old_proc;
                Job *node = find_job_by_number(cur_node, processors, num_proc, &old_proc);
                while (node != NULL) {
                    Job *prev = node->prev;
                    long long node_start_time = tree[node->num]->get_time() - tree[node->num]->get_duration();
                    Node *cur = tree[node->num];
                    for (auto fol: cur->get_followers()) {
                        if (std::find(queue.begin(), queue.end(), fol) == queue.end()) {
                            queue.push_back(fol);
                        }
                    }
                    long long prev_time;
                    if (prev != NULL) {
                        prev_time = tree[prev->num]->get_time();
                    } else {
                        prev_time = 0;
                    }
                    long long dep_end_time = find_dep_time(node->num, tree);
                    if (prev_time < dep_end_time) {
                        prev_time = dep_end_time;
                    }
                    if (node_start_time < prev_time) {
                        tree[node->num]->set_time(prev_time+tree[node->num]->get_duration());
                    }
                    node = node->next;
                }
                std::cout << "queue: ";
                for (auto i: queue) {
                    std::cout << i << " ";
                }
                // node = processors[0];
                // std::cout << "CHECK" << std::endl;
                // while (node != NULL) {
                //     if (node->prev == NULL)
                //         std::cout << "NULL ";
                //     std::cout << node->num << " " << tree[node->num]->get_time() << std::endl;
                //     if (node->next == NULL)
                //         std::cout << " NULL " << std::endl;
                //     node=node->next;
                // }
                // node = processors[1];
                // std::cout << "CHECK" << std::endl;
                // while (node != NULL) {
                //     if (node->prev == NULL)
                //         std::cout << "NULL ";
                //     std::cout << node->num << " " << tree[node->num]->get_time() << std::endl;
                //     if (node->next == NULL)
                //         std::cout << " NULL " << std::endl;
                //     node=node->next;
                // }
                // node = processors[2];
                // std::cout << "CHECK" << std::endl;
                // while (node != NULL) {
                //     if (node->prev == NULL)
                //         std::cout << "NULL ";
                //     std::cout << node->num << " " << tree[node->num]->get_time() << std::endl;
                //     if (node->next == NULL)
                //         std::cout << " NULL " << std::endl;
                //     node=node->next;
                // }
                // std::cout << std::endl;
                if (check_correctness(processors, num_proc, tree)) {
                    std::cout << "correct" << std::endl;
                    correct = true;
                    break;
                }
            }
            if (correct) {
                break;
            }
            // }
        }
    }
    for (int i = 0; i < num_proc; ++i) {
        std::cout << "processor " << i << ": ";
        auto node = processors[i];
        while (node != NULL) {
            std::cout << node->num << " ";
            node = node->next;
        }
        long long end_time = find_proc_time(i, processors, tree);
        std::cout << "| " << end_time << std::endl;
    }
}

void simulating_annealing(std::map<int, Job*>  &processors, int num_proc,
                            std::vector<Node*> &tree, int nodes_num, int iter_num, std::vector<int> &roots)
{
    for (int i = 0; i < num_proc; ++i) {
        // std::vector<int> p;
        processors[i] = NULL;
    }
    start_pos(processors, num_proc, tree, roots);
    for (int i = 0; i < num_proc; ++i) {
        std::cout << "processor " << i << ": ";
        auto node = processors[i];
        while (node != NULL) {
            std::cout << node->num << " ";
            node = node->next;
        }
        long long end_time = find_proc_time(i, processors, tree);
        std::cout << "| " << end_time;
        std::cout << std::endl;
    }
    std::cout << max_duration (processors, num_proc, tree) << std::endl;
    auto correct = check_correctness(processors, num_proc, tree);
    std::cout << "correctness at the beginning: " << correct << std::endl;
    change_processor(processors, num_proc, tree, nodes_num);
    correct = check_correctness(processors, num_proc, tree);
    std::cout << "correctness " << correct << std::endl;
}

int main(int argc, char **argv)
{
    bool tmp = false;
    tmp = tmp || true;
    std::cout << tmp << std::endl;
    std::vector<Node*> tree;
    std::vector<int> roots;
    if (argc == 1) {
        tree = generate_tree(50, 10, 1000, 100000, 2, 4, 2, 4);
    } else {
        read_tree(argv[1], tree);
    }
    auto nodes_num = tree.size();
    for (int i = 0; i < nodes_num; ++i) {
        if (tree[i]->get_dependencies().size() == 0) {
            roots.push_back(i);
        }
    }
    write_to_dot(tree, nodes_num);

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
    for (auto &k: roots) {
        std::cout << k << " ";
    }
    std::cout << std::endl;
    
 
    std::map<int, Job*> processors;
    int num_proc = 3;
    simulating_annealing(processors, 3, tree, nodes_num, 1000, roots);


    for (int i = 0; i < nodes_num; ++i) {
        delete tree[i];
    }
    for (int i = 0; i < num_proc; ++i) {
        auto cur = processors[i];
        auto next = cur;
        while(cur != NULL) {
            next = cur->next;
            free(cur);
            cur = next;
        }
    }
    return 0;
}