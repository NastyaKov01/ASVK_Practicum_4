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
#include <set>

#include "generator.h"


struct Job
{
    int num;
    Job *next = NULL;
    Job *prev = NULL;

    Job(int val): num(val) {}
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

long long find_dep_end_time(int cur_node, std::vector<Node*> &tree)
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
                long long dep_end_time = find_dep_end_time(cur_node, tree);
                int proc_fin_time = find_proc_time(proc, processors, tree);
                std::cout <<"!!!" << dep_end_time << " " << proc_fin_time << std::endl;
                if (dep_end_time < proc_fin_time) {
                    dep_end_time = proc_fin_time;
                }
                std::cout << "!!!" << dep_end_time + dur << std::endl;
                tree[cur_node]->set_time(dep_end_time + dur);
                Job *new_job = new Job(cur_node);
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

bool check_correctness(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree)
{
    std::cout << "CHECK CORRECTNESS" << std::endl;
    for (int i = 0; i < num_proc; ++i) {
        
        Job *cur_node = processors[i], *prev = NULL;
        if (cur_node != NULL) {
            prev = cur_node->prev;
        }
        while (cur_node != NULL) {
            auto dep_time = find_dep_end_time(cur_node->num, tree);
            auto node_start_time = tree[cur_node->num]->get_time() - tree[cur_node->num]->get_duration();
            if (node_start_time < dep_time) {
                std::cout << "error " << cur_node->num << " node start time: " << node_start_time << " dep time: " << dep_time << std::endl;
                return false;
            }
            if (prev != NULL &&  node_start_time < tree[prev->num]->get_time()) {
                std::cout << "ERROR" << std::endl;
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

void change_processor(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree, int nodes_num)
{
    // std::default_random_engine generator(time(0));
    std::cout << "CHANGE PROCESSOR" << std::endl;
    std::uniform_int_distribution<int> node_distr(0, nodes_num-1);
    std::random_device generator;
    int cur_node = node_distr(generator), old_proc, new_proc;
    // cur_node = 7;
    Job *cur = find_job_by_number(cur_node, processors, num_proc, &old_proc);
    Job *prev = cur->prev;
    std::uniform_int_distribution<int> proc_distr(0, num_proc-1);
    new_proc = proc_distr(generator);
    while(new_proc == old_proc) {
        new_proc = proc_distr(generator);
    }
    
    // cur_node=7;
    // old_proc=2;
    // new_proc=0;

    std::cout << "node to change: " << cur_node << std::endl;
    std::cout << "old proc: " << old_proc << std::endl;
    std::cout << "new proc: " << new_proc << std::endl;

    auto dep_end_time = find_dep_end_time(cur_node, tree);
    // std::cout << "dep_time: " << dep_end_time << std::endl;
    auto node = processors[new_proc];
    // std::cout << "QQQ " << new_proc << " " << node->num << std::endl;

    while(node != NULL) {
        // std::cout << ")))" << std::endl;
        long long node_end_time = tree[node->num]->get_time();
        // std::cout << "node_time: " << node_end_time << std::endl;
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
                // std::cout << "          " << prev->num << std::endl;
                prev->next = cur->next;
                if (cur->next != NULL) {
                    cur->next->prev = prev;
                    // std::cout << "          " << cur->next->num << std::endl;
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
            std::cout << "кря" << std::endl;
            while (node != NULL) {
                std::cout << "&&&" << std::endl;
                long long prev_time;
                if (prev == NULL) {
                    prev_time = 0;
                } else {
                    prev_time = tree[prev->num]->get_time();
                }
                long long dep_end_time = find_dep_end_time(node->num, tree);
                if (prev_time < dep_end_time) {
                    prev_time = dep_end_time;
                }
                tree[node->num]->set_time(prev_time+tree[node->num]->get_duration());
                prev = node;
                node = node->next;
            }
            std::cout << "мяу" << std::endl;
            std::vector<int> queue;
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
                    std::cout << "блабла " << node->num << std::endl;
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
                    long long dep_end_time = find_dep_end_time(node->num, tree);
                    if (prev_time < dep_end_time) {
                        prev_time = dep_end_time;
                    }
                    if (node_start_time < prev_time) {
                        tree[node->num]->set_time(prev_time+tree[node->num]->get_duration());
                    }
                    node = node->next;
                }
                // std::cout << "queue: ";
                // for (auto i: queue) {
                //     std::cout << i << " ";
                // }
                if (check_correctness(processors, num_proc, tree)) {
                    // std::cout << "correct" << std::endl;
                    correct = true;
                    break;
                }
            }
            if (correct) {
                break;
            }
        }
    }
    // for (int i = 0; i < num_proc; ++i) {
    //     std::cout << "processor " << i << ": ";
    //     auto node = processors[i];
    //     while (node != NULL) {
    //         std::cout << node->num << " ";
    //         node = node->next;
    //     }
    //     long long end_time = find_proc_time(i, processors, tree);
    //     std::cout << "| " << end_time << std::endl;
    // }
}

void find_all_fol(int node_num, std::set<int> &fols, std::vector<Node*> &tree, 
                    std::map<int, Job*>  &processors, int num_proc) 
{
    auto direct_fols = tree[node_num]->get_followers();
    if (fols.find(node_num) == fols.end()) {
        fols.insert(node_num);
        for (auto f: direct_fols) {
            if (fols.find(f) == fols.end()) {
                find_all_fol(f, fols, tree, processors, num_proc);
            }
        }
        int proc;
        auto node = find_job_by_number(node_num, processors, num_proc, &proc)->next;
        while (node != NULL) {
            find_all_fol(node->num, fols, tree, processors, num_proc);
            node = node->next;
        }
    }
}

void find_all_dep(int node_num, std::set<int> &deps, std::vector<Node*> &tree, 
                    std::map<int, Job*>  &processors, int num_proc) 
{
    auto direct_deps = tree[node_num]->get_dependencies();
    if (deps.find(node_num) == deps.end()) {
        deps.insert(node_num);
        for (auto d: direct_deps) {
            if (deps.find(d) == deps.end()) {
                find_all_dep(d, deps, tree, processors, num_proc);
            }
        }
        int proc;
        auto node = find_job_by_number(node_num, processors, num_proc, &proc)->prev;
        while (node != NULL) {
            find_all_dep(node->num, deps, tree, processors, num_proc);
            node = node->prev;
        }
    }
}

void change_level(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree, int nodes_num)
{
    std::cout << "CHANGE LEVEL " << std::endl;
    std::uniform_int_distribution<int> node_distr(0, nodes_num-1);
    std::random_device generator;
    int cur_node = node_distr(generator), proc;

    // cur_node = 11;
    // std::cout << "cur node " << cur_node << std::endl;
    Job *cur = find_job_by_number(cur_node, processors, num_proc, &proc), *prev;
    Job *node = processors[proc];
    long long dep_end_time = find_dep_end_time(cur_node, tree);
    long long shift = tree[cur_node]->get_duration();
    long long node_end_time;
    long long node_start_time;
    int job_on_proc = 0, low_bound = 0, up_bound;
    while(node != NULL) {
        job_on_proc++;
        node = node->next;
    }
    up_bound = job_on_proc-1;
    std::set<int> fols, deps;
    auto dependencies = tree[cur_node]->get_dependencies();
    for (auto d: dependencies) {
        find_all_dep(d, deps, tree, processors, num_proc);
    }
    auto followers = tree[cur_node]->get_followers();
    for (auto f: followers) {
        find_all_fol(f, fols, tree, processors, num_proc);
    }

    int cnt = 0;
    node = processors[proc];
    while (node != NULL) {
        if (fols.find(node->num) != fols.end()) {
            up_bound = cnt;
            break;
        }
        cnt++;
        node = node->next;
    }
    node = processors[proc];
    cnt = 0;
    while (node != cur) {
        if (deps.find(node->num) != deps.end()) {
            low_bound = cnt;
        }
        cnt++;
        node = node->next;
    }

    // std::cout << "low " << low_bound << " up " << up_bound << std::endl;
    // std::cout << "node_start_time " << node_start_time << " dep_end_time " <<  dep_end_time << std::endl;

    int dif = up_bound - low_bound;
    std::uniform_int_distribution<int> step_distr(0, dif-1);
    // std::cout << "step distr " << step_distr.max() << " " << step_distr.min() << std::endl;
    int steps = step_distr(generator);
    // std::cout << "steps: " << steps << std::endl;
    // steps = 0;
    std::cout << low_bound << " " << up_bound << " " << dif << " " << steps << std::endl;
    node = processors[proc];
    for (int i = 0; i < low_bound + steps; ++i) {
        node = node->next;
    }
    std::cout << "cur num " << cur->num << std::endl;
    std::cout << "node num " << node->num << std::endl;

    if (node == processors[proc] && node_start_time >= dep_end_time && node != cur) {
        processors[proc] = cur;
        if (cur->prev) {
            cur->prev->next = cur->next;
        }
        if (cur->next) {
            cur->next->prev = cur->prev;
        }
        cur->prev = NULL;
        cur->next = node;
        node->prev = cur;
        tree[cur->num]->set_time(dep_end_time + shift);
    } else {
        if (node_start_time >= dep_end_time && node->prev) {
            node = node->prev;
        }
        if (cur->next == node) {
            if (cur->prev) {
                cur->prev->next = node;
            } else {
                processors[proc] = node;
            }
            if (node->next) {
                node->next->prev = cur;
            }
            node->prev = cur->prev;
            cur->next = node->next;
            node->next = cur;
            cur->prev = node;

        } else if (node->next != cur && node != cur) {
            if (cur->prev) {
                cur->prev->next = cur->next;
            } else {
                processors[proc] = cur->next;
            }
            if (cur->next) {
                cur->next->prev = cur->prev;
            }
            cur->next = node->next;
            if (node->next) {
                node->next->prev = cur;
            }
            node->next = cur;
            cur->prev = node;
        }
    }
    if (node != cur) {
        std::vector<int> queue;
        node = processors[proc];
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
                // std::cout << "cur num " << node->num << " fol: ";
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
                long long dep_end_time = find_dep_end_time(node->num, tree);
                // std::cout << "dep_time " << dep_end_time << " prev time " << prev_time << std::endl;
                if (prev_time < dep_end_time) {
                    prev_time = dep_end_time;
                }
                tree[node->num]->set_time(prev_time+tree[node->num]->get_duration());
                node = node->next;
            }
            // std::cout << std::endl << "queue: ";
            // for (auto i: queue) {
            //     std::cout << i << " ";
            // }
            // std::cout << std::endl << std::endl;
            bool res = check_correctness(processors, num_proc, tree);
            std::cout << "PUM" << std::endl;
            if (res) {
                // std::cout << "correct" << std::endl;
                correct = true;
                break;
            }
        }
    }
    
    // for (int i = 0; i < num_proc; ++i) {
    //     std::cout << "processor " << i << ": ";
    //     auto node = processors[i];
    //     while (node != NULL) {
    //         std::cout << node->num << " ";
    //         node = node->next;
    //     }
    //     long long end_time = find_proc_time(i, processors, tree);
    //     std::cout << "| " << end_time << std::endl;
    // }
}

double boltzmann_temp(double temp, int i)
{
    return temp / log(1 + i);
}

double probability(double F, double temp)
{
    return exp(-F / temp);
}

void free_processors(std::map<int, Job*>  &processors, int num_proc)
{
    for (int i = 0; i < num_proc; ++i) {
        auto cur = processors[i];
        auto next = cur;
        while(cur != NULL) {
            next = cur->next;
            delete cur;
            cur = next;
        }
    }
}

void fill_processors(std::map<int, Job*>  &processors, std::map<int, Job*>  &new_processors, int num_proc)
{
    for (int i = 0; i < num_proc; ++i) {
        Job *node = processors[i], *other_node = new_processors[i], *prev = NULL;
        while (node != NULL) {
            Job *new_job = new Job(node->num);
            if (other_node == NULL) {
                new_processors[i] = new_job;
                other_node = new_job;
            } else {
                other_node->next = new_job;
                new_job->prev = other_node;
                other_node = other_node->next;
            }
            node = node->next;
        }
    }
}

void simulating_annealing(std::map<int, Job*>  &processors, int num_proc, std::vector<Node*> &tree, std::vector<int> &roots,
                            int nodes_num, int iter_num, int one_temp_iter, double start_temp)
{
    std::map<int, Job*> cur_processors, new_processors;
    for (int i = 0; i < num_proc; ++i) {
        processors[i] = NULL;
        cur_processors[i] = NULL;
        new_processors[i] = NULL;
    }
    start_pos(processors, num_proc, tree, roots);
    for (int i = 0; i < num_proc; ++i) {
        std::cout << "processor " << i << ": ";
        auto node = processors[i];
        while (node != NULL) {
            std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
            node = node->next;
        }
        long long end_time = find_proc_time(i, processors, tree);
        std::cout << "| " << end_time;
        std::cout << std::endl;
    }
    fill_processors(processors, new_processors, num_proc);
    fill_processors(processors, cur_processors, num_proc);
    
    for (int i = 0; i < num_proc; ++i) {
        std::cout << "new_processor " << i << ": ";
        auto node = new_processors[i];
        while (node != NULL) {
            std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
            node = node->next;
        }
        long long end_time = find_proc_time(i, new_processors, tree);
        std::cout << "| " << end_time;
        std::cout << std::endl;
    }
    for (int i = 0; i < num_proc; ++i) {
        std::cout << "cur_processor " << i << ": ";
        auto node = cur_processors[i];
        while (node != NULL) {
            std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
            node = node->next;
        }
        long long end_time = find_proc_time(i, cur_processors, tree);
        std::cout << "| " << end_time;
        std::cout << std::endl;
    }
    long long max_time = max_duration (processors, num_proc, tree);
    std::cout << max_duration (processors, num_proc, tree) << std::endl;
    auto correct = check_correctness(processors, num_proc, tree);
    std::cout << "correctness at the beginning: " << correct << std::endl;

    // for (int k = 0; k < 100; ++k) {
    //     change_level(processors, num_proc, tree, nodes_num);
    //     for (int i = 0; i < num_proc; ++i) {
    //         std::cout << "processor " << i << ": ";
    //         auto node = processors[i];
    //         while (node != NULL) {
    //             std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
    //             node = node->next;
    //         }
    //         long long end_time = find_proc_time(i, processors, tree);
    //         std::cout << "| " << end_time;
    //         std::cout << std::endl;
    //     }
    //     bool correct = check_correctness(processors, num_proc, tree);
    //     std::cout << "correctness " << correct << std::endl;
    //     if (!correct) {
    //         std::cout << "ERROR" << std::endl;
    //         break;
    //     }
    // }

    double temp = start_temp;
    std::uniform_int_distribution<int> change_distr(0, 1);
    std::random_device generator;
    int step;
    bool check;
    // free_processors(new_processors, num_proc);
    // fill_processors(cur_processors, new_processors, num_proc);
    for (int i = 0; i < num_proc; ++i) {
        std::cout << "new_processor " << i << ": ";
        auto node = new_processors[i];
        while (node != NULL) {
            std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
            node = node->next;
        }
        long long end_time = find_proc_time(i, new_processors, tree);
        std::cout << "| " << end_time;
        std::cout << std::endl;
    }
    for (int i = 0; i < iter_num; ++i) {
        if (check) {
            break;
        }
        for (int k = 0; k < one_temp_iter; ++k) {
            step = change_distr(generator);
            if (step == 0) {
                change_processor(new_processors, num_proc, tree, nodes_num);
            } else {
                change_level(new_processors, num_proc, tree, nodes_num);
            }
            check = check_correctness(new_processors, num_proc, tree);
            if (!check) {
                std::cout << "ERROR PANIC" << std::endl;
                break;
            } else {
                std::cout << "CORRECT YET" << std::endl;
            }
            for (int j = 0; j < num_proc; ++j) {
                std::cout << "new_processor " << j << ": ";
                auto node = new_processors[j];
                while (node != NULL) {
                    std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
                    node = node->next;
                }
                long long end_time = find_proc_time(j, new_processors, tree);
                std::cout << "| " << end_time;
                std::cout << std::endl;
            }
            long long cur_time = max_duration (new_processors, num_proc, tree);
            std::cout << "cur time: " << cur_time << std::endl;
            if (cur_time < max_time) {
                max_time = cur_time;
                std::cout << "new time: " << max_time << std::endl;
                free_processors(processors, num_proc);
                fill_processors(new_processors, processors, num_proc);
                for (int j = 0; j < num_proc; ++j) {
                    std::cout << "      processor " << j << ": ";
                    auto node = processors[i];
                    while (node != NULL) {
                        std::cout << node->num << ":" << tree[node->num]->get_time() << " ";
                        node = node->next;
                    }
                    long long end_time = find_proc_time(i, processors, tree);
                    std::cout << "| " << end_time;
                    std::cout << std::endl;
                }
            } else {
                double prob = probability(cur_time - max_time, temp);
                std::cout << "prob: " << prob << std::endl;
                if (prob > 0.5) {
                    fill_processors(new_processors, cur_processors, num_proc);
                } else {
                    free_processors(new_processors, num_proc);
                    fill_processors(cur_processors, new_processors, num_proc);
                }
            }
        }
        temp = boltzmann_temp(temp, i);
    }

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
        tree = generate_tree(500, 10, 1000, 100000, 2, 4, 2, 4);
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
    simulating_annealing(processors, num_proc, tree, roots, nodes_num, 10, 5, 1000);

    for (int i = 0; i < nodes_num; ++i) {
        delete tree[i];
    }
    // for (int i = 0; i < num_proc; ++i) {
    //     auto cur = processors[i];
    //     auto next = cur;
    //     while(cur != NULL) {
    //         next = cur->next;
    //         delete cur;
    //         cur = next;
    //     }
    // }
    free_processors(processors, num_proc);
    return 0;
}