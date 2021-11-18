class Node
{
    int duration = 0;
    int degree = 0;
    long long end_time = 0;
    std::vector<int> dependencies;
    std::vector<int> followers;
public:
    void set_duration(int value) { duration = value; }

    int get_duration() { return duration; }

    void set_time(long long value) { end_time = value; }

    long long get_time() { return end_time; }

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