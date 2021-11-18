#include <vector>
#include <random>
#include <algorithm>

#include "work_node.h"

bool path_exists(std::vector<Node*> &tree, int start, int finish);

int gen_parts_num(int nodes_num, int parts_mean, double in_parts_variance);

void set_degrees(std::vector<Node*> &tree, int start, int stop, int degree_mean, double degree_variance);

void set_weights(std::vector<Node*> &tree, int nodes_num, int weight_min, int weight_max, double weight_variance);

std::vector<Node*> generate_tree(int nodes_num, int weight_min, int weight_max, double weight_variance,
                                int parts_mean, double in_parts_variance, int degree_mean, double degree_variance);