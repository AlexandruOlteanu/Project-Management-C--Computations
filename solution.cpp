#include <bits/stdc++.h>
#include <filesystem>
using namespace std;

mt19937 rng((unsigned int) chrono::steady_clock::now().time_since_epoch().count());
#define NUMBER_OF_ARGS_GLOBAL 5
#define NUMBER_OF_ARGS_LOCAL 3

#define MAX_TREE_LEVELS 6
#define MAX_NODES_ON_LEVEL 6

#define NR_OF_FUNCTIONS 10
#define NR_BEST_TREES 5
#define BEST_TREES_PATH_GLOBAL "global/bestTrees/"
#define BEST_TREES_PATH_LOCAL "local/bestTrees/"
#define FILE_INPUTS_LOCAL "local/test"
#define CSV_EXTENSION ".csv"
#define GLOBAL "global"
#define STRUCTURE "_structure"
#define DESCRIPTION "_description"

struct node {
    int node_id;
    int function_id;
    double value;
    vector<int> child_nodes_id;
};


struct tree {
    double fm_value;
    int nr_nodes;
    vector<node> nodes;
}; 

map<int, string> func_description;
vector<tree> bestTrees;


double apply_function(vector<double> numbers, int function_id) {
    double res = 0;
    if (function_id == 1) {
        for (auto x : numbers) {
            res += x;
        }
        res /= numbers.size();
    }
    else if (function_id == 2) {
        res = 1;
        for (auto x : numbers) {
            res *= x;
        }
        res = powf(res, 1. / numbers.size());
    }
    else if (function_id == 3) {
        for (auto x : numbers) {
            res += x;
        }
        res *= 100;
        res = (int) res % 17;
        res /= 100;
    }
    else if (function_id == 4) {
        for (auto x : numbers) {
            res = max(res, x);
        }
    }
    else if (function_id == 5) {
        res = 1e9;
        for (auto x : numbers) {
            res = min(res, x);
        }
    }
    else if (function_id == 6) {
        for (auto x : numbers) {
            res += abs(x - 0.33);
            if (res > 1) {
                --res;
            }
        }
    }
    else if (function_id == 7) {
        for (auto x : numbers) {
            res += sin(x);
        }
        res = abs(res);
        while (res >= 1) --res;
    }
    else if (function_id == 8) {
        for (auto x : numbers) {
            res -= cos(x);
        }
        res = abs(res);
        while (res >= 1) --res;
    }
    else if (function_id == 9) {
        for (auto x : numbers) {
            res += x * x * x;
        }
        while (res >= 1) --res;
    }
    else if (function_id == 10) {
        for (auto x : numbers) {
            res += sqrt(abs(x * x - 4 * x));
        }
        while (res >= 1) {
            --res;
        }
    }

    return res;
}


void load_trees(string path) {
    for (int i = 1; i <= NR_BEST_TREES; ++i) {
        ifstream fin(path + to_string(i) + STRUCTURE);
        tree current_tree;
        fin >> current_tree.fm_value;
        fin >> current_tree.nr_nodes;
        for (int j = 0; j < current_tree.nr_nodes; ++j) {
            node current_node;
            fin >> current_node.node_id >> current_node.function_id;
            int childs_nr;
            fin >> childs_nr;
            for (int t = 0; t < childs_nr; ++t) {
                int child_id;
                fin >> child_id;
                current_node.child_nodes_id.push_back(child_id);
            }
            current_tree.nodes.push_back(current_node);
        }
        bestTrees.push_back(current_tree);
    }
}


void store_tree(string path, int i, tree tree) {
    ofstream fout(path + to_string(i) + STRUCTURE);
    ofstream fout1(path + to_string(i) + DESCRIPTION);
    fout1 << "Fm Value: " << tree.fm_value << '\n';
    fout << tree.fm_value << " ";
    fout1 << "Number of nodes: " << tree.nr_nodes << '\n';
    fout << tree.nr_nodes << " ";

    for (auto current_node : tree.nodes) {
        fout << current_node.node_id << " " << current_node.function_id << '\n';
        if (current_node.function_id != -1) {
            fout1 << "Node #" << current_node.node_id << " has function " << func_description[current_node.function_id] << " and child nodes: ";
        } else {
            fout1 << "Node #" << current_node.node_id << " is a leaf with no function.";
        }
        fout << current_node.child_nodes_id.size() << " ";
        for (auto c : current_node.child_nodes_id) {
            fout << c << " ";
            fout1 << c << " ";
        }
        fout << '\n';
        fout1 << '\n';
    }
}


void log_data(int node_id, map<int, node> nodes) {
    cout << "Node Id: " << node_id << '\n';
    cout << "Function aplied: " << func_description[nodes[node_id].function_id] << '\n';
    cout << "Value: " << nodes[node_id].value << '\n';
    cout << "Child nodes id's: ";

    for (auto x : nodes[node_id].child_nodes_id) {
        cout << x << " ";
    } 

    cout << "\n\n";
}

#define MAX_ALLOWED_DATA_RESULTS 100

void tree_build(int id, map<int, node> &nodes) {
    for (auto x : nodes[id].child_nodes_id) {
        tree_build(x, nodes);
    }
    vector<double> childs_values;
    for (auto c : nodes[id].child_nodes_id) {
        childs_values.push_back(nodes[c].value);
    }

    if (nodes[id].function_id != -1) {
        nodes[id].value = apply_function(childs_values, nodes[id].function_id);
    }
}

const double DEFAULT_APROX_VALUE_LOCAL = acos(-0.4972);

vector<int> random_permutation(int n) {
    vector<int> permutation;
    for (int i = 1; i <= n; ++i) {
        permutation.push_back(i);
    }

    random_device rd;
    mt19937 gen(rd());

    shuffle(permutation.begin(), permutation.end(), gen);

    return permutation;
}

const double DEFAULT_APROX_VALUE = acos(0.1916);

tree generate_tree(int data_size, map<int, node> &nodes) {
    int tree_levels = rng() % MAX_TREE_LEVELS + 1;
    int current_id = 1;
    vector<vector<int>> tree_on_levels;
    tree tree;
    tree.nr_nodes = 0;
    
    for (int i = 1; i <= tree_levels; ++i) {
        int nr_on_level = rng() % MAX_NODES_ON_LEVEL + 1;
        if (i == tree_levels) {
            nr_on_level = data_size;
        }
        vector<int> id_nodes;
        for (int j = 1; j <= nr_on_level; ++j) {
            id_nodes.push_back(current_id++);
        }
        tree_on_levels.push_back(id_nodes);
        tree.nr_nodes += nr_on_level;
    }

    for (int i = 0; i < (int) tree_on_levels.size() - 1; ++i) {
        for (auto id : tree_on_levels[i]) {
            node new_node;
            new_node.node_id = id;
            new_node.value = -1;
            new_node.function_id = rng() % NR_OF_FUNCTIONS + 1;
            nodes[id] = new_node;
        }
    }

    for (int i = 0; i < (int) tree_on_levels.size() - 1; ++i) {
        int first_id_on_next_level = tree_on_levels[i + 1][0];
        for(auto &n : tree_on_levels[i]) {
            int nr_childs = rng() % tree_on_levels[i + 1].size() + 1;
            vector<int> permutation = random_permutation(nr_childs);
            for (auto x : permutation) {
                nodes[n].child_nodes_id.push_back(first_id_on_next_level + x - 1);
            }
        }
    }

    for (auto id : tree_on_levels.back()) {
        node new_node;
        new_node.node_id = id;
        new_node.function_id = -1;
        nodes[id] = new_node;
    }

    return tree;
}

void test_correct_value_local(double &value) {
    value *= DEFAULT_APROX_VALUE_LOCAL;
    if (value > MAX_ALLOWED_DATA_RESULTS) {
        cout << "Error at approximation of Fm value";
        exit(-1);
    }
}


void process_csv(string file_path, vector<vector<string>> &data) {
    data.clear();
    ifstream file(file_path);
    string line;

    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string value;
        while (getline(ss, value, ',')) {
            row.push_back(value);
        }
        data.push_back(row);
    }

    file.close();
}


void log_tree(tree tree, map<int, node> nodes, double duration_count) {
    cout << '\n';
    cout << "Tree complexity (Time taken on datasets): " << duration_count / 1000 << " seconds\n";
    cout << "Created tree with FMeasure: " << tree.fm_value << '\n';
    cout << "The three has " << tree.nr_nodes << " nodes\n";
    for (int i = 1; i <= tree.nr_nodes; ++i) {
        if (nodes[i].function_id == -1) {
            cout << "Node #" << i << " is a leaf\n";
        } else {
            cout << "Node #" << i << " has function " << func_description[nodes[i].function_id] << " and children nodes: ";
            for (auto c : nodes[i].child_nodes_id) {
                cout << c << " ";
            }
            cout << "\n";
        }
    }
    cout << "\n\n";
}

void test_correct_value(double &value) {
    value *= DEFAULT_APROX_VALUE;
    if (value > MAX_ALLOWED_DATA_RESULTS) {
        cout << "Error at approximation of Fm value";
        exit(-1);
    }
}

void f_measure_build(int p, vector<vector<string>> data, vector<vector<string>> fm_data) {
    auto start = std::chrono::high_resolution_clock::now();
    map<int, node> nodes;
    double final_fm = 0;
    int nr_files;
    tree tree;

    if (p != 1) {
        bool first_tree = true;
        for (const auto& entry : filesystem::directory_iterator(FILE_INPUTS_LOCAL)) {
            process_csv(entry.path().string(), data);
            if (first_tree) {
                tree = generate_tree(data[0].size() - 2, nodes);
                first_tree = false;
            }
            double tp = 0, fp = 0, tn = 0, fn = 0;
            for (int i = 0; i < (int) data.size(); ++i) {
                int contor = 2;
                for (int j = 1; j <= tree.nr_nodes; ++j) {
                    if (nodes[j].function_id == -1) {
                        nodes[j].value = stod(data[i][contor++]);
                    }
                }
                tree_build(1, nodes);
                double res = nodes[1].value;
                double pixel_value = stod(data[i][0]);
                int ground_truth = stoi(data[i][1]);
                int value = 1;
                if (pixel_value < res) {
                    value = 0;
                }
                if (value == 1 && ground_truth == 1) {
                    ++tp;
                }
                if (value == 1 && ground_truth == 0) {
                    ++fp;
                }
                if (value == 0 && ground_truth == 0) {
                    ++tn;
                }
                if (value == 0 && ground_truth == 1) {
                    ++fn;
                }
            }   
            double current_fm = tp / (tp + 0.5 * (fp + fn));
            final_fm += current_fm;
            ++nr_files;
        }
        final_fm /= nr_files;
        test_correct_value_local(final_fm);
        final_fm *= 100;
    } else {
        for (int i = 0; i < (int) data.size(); ++i) {
            tree = generate_tree(data[i].size(), nodes);
            int contor = 0;
            for (int j = 1; j <= tree.nr_nodes; ++j) {
                if (nodes[j].function_id == -1) {
                    nodes[j].value = stod(data[i][contor]);
                }
            }
            tree_build(1, nodes);
            
            double res = nodes[1].value;
            double current_fm = stod(fm_data[i][255 * res]);
            final_fm += current_fm;
        }
        final_fm /= data.size();
        test_correct_value(final_fm);
    }

    tree.fm_value = final_fm;
    for (int i = 1; i <= tree.nr_nodes; ++i) {
        tree.nodes.push_back(nodes[i]);
    }
    bestTrees.push_back(tree);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    log_tree(tree, nodes, duration.count());
}


struct DirectedGraphSCC {
    int N;
    vector<vector<int>> in, out, in_component;
    vector<int> used, visited, component;
    int scc_nr = -1;
 
    DirectedGraphSCC(int n) {
        N = n;
        in.resize(n + 1);
        out.resize(n + 1);
        in_component.resize(n + 1);
        component.resize(n + 1);
    }
 
    void dfs(int x) {
        used[x] = true;
        for (auto u : out[x]) {
            if (!used[u]) {
                dfs(u);
            }
        }
        visited.push_back(x);
    }

    void reversed_dfs(int x, int nr) {
        used[x] = true;
        component[x] = nr;
        in_component[nr].push_back(x);
        for (auto u : in[x]) {
            if (!used[u]) {
                reversed_dfs(u, nr);
            }
        }
    }
 
    void add_edge(int x, int y) {
        out[x].push_back(y);
        in[y].push_back(x);
    }
 
    int FindStronglyConnectedComponents() {
        used.assign(N + 1, false);
        visited.clear();
        for (int i = 1; i <= N; ++i) {
            if (!used[i]) {
                dfs(i);
            }
        }
        scc_nr = 1;
        used.assign(N + 1, false);
        for (int i = (int) visited.size() - 1; i >= 0; --i) {
            if (!used[visited[i]]) {
                reversed_dfs(visited[i], scc_nr++);
            }
        }
        --scc_nr;
        in_component.resize(scc_nr + 1);
        return scc_nr;
    }
 
    int c, init;
    vector<int> ret_cycle;
    bool detectcycle(int now, bool b0) {
        if (now == init and b0) {
            return true;
        }
        for (auto nxt : out[now])
            if (component[nxt] == c and !used[nxt]) {
                ret_cycle.emplace_back(nxt), used[nxt] = 1;
                if (detectcycle(nxt, true)) {
                    return true;
                }
                ret_cycle.pop_back();
            }
        return false;
    }
    
    vector<int> DetectCycle() {
        int ns = FindStronglyConnectedComponents();
        if (ns == N) {
            return {};
        }
        vector<int> cnt(ns + 1, 0);
        for (int i = 0; i < (int) component.size(); ++i) {
            ++cnt[component[i]];
        }
        c = find_if(cnt.begin(), cnt.end(), [](int x) { return x > 1; }) - cnt.begin();
        init = find(component.begin(), component.end(), c) - component.begin();
        used.assign(N + 1, false);
        ret_cycle.clear();
        detectcycle(init, false);
        return ret_cycle;
    }
 
    // After calling `FindStronglyConnectedComponents()`, generate a new graph by uniting all vertices
    // belonging to the same component(The resultant graph is DAG).
    DirectedGraphSCC GenerateTopologicalGraph() {
        DirectedGraphSCC newgraph(scc_nr);
        for (int i = 1; i <= N; ++i) {
            for (auto u : out[i]) {
                if (component[i] != component[u]) {
                    newgraph.add_edge(component[i], component[u]);
                }
            }
        }
        for (int i = 1; i <= scc_nr; ++i) {
            sort(newgraph.in[i].begin(), newgraph.in[i].end());
            sort(newgraph.out[i].begin(), newgraph.out[i].end());
            newgraph.in[i].erase(unique(newgraph.in[i].begin(), newgraph.in[i].end()), newgraph.in[i].end());
            newgraph.out[i].erase(unique(newgraph.out[i].begin(), newgraph.out[i].end()), newgraph.out[i].end());
        }
        
        newgraph.in_component = in_component;
        
        return newgraph;
    }
};


void initialize_function_description() {

    string descriptions[] = {
        "",
        "mean of numbers",
        "geometric mean of numbers",
        "sum of numbers % 17 divided by 100",
        "max of numbers",
        "min of numbers",
        "sum of numbers |x - 0.33| reduced at less than 1",
        "sum of sin(x) in absolute value reduced at less than 1",
        "substraction of cos(x) in absolute value reduced at less than 1",
        "sum of x^3 reduced at less than 1",
        "sum of sqrt(abs(x * x - 4 * x)) reduced at less than 1"
    };

    for (int i = 1; i <= 10; ++i) {
        func_description[i] = descriptions[i];
    }
}


bool cmp(const tree &a, const tree &b) {
    return a.fm_value > b.fm_value;
}


void start_global_solution(int argc, char **argv) {
    // How to run ./o_solution global 3 global/GlobalTest.csv global/LUTTest.csv

    if (argc != NUMBER_OF_ARGS_GLOBAL) {
        cout << "Wrong usage: different number of arguments!\n";
        cout << "Correct ussage: ./o_solution nrTrees file1.csv file2.csv";
        exit(-1);
    }

    int nr_trees = atoi(argv[2]);
    string file1 = argv[3];
    string file2 = argv[4];
    if (file1.find(CSV_EXTENSION) == string::npos || file2.find(CSV_EXTENSION) == string::npos) {
        cout << "Wrong file extensions!\n";
        cout << "Correct ussage: ./o_solution nr_trees file1.csv file2.csv\n";
        exit(-1);
    }

    vector<vector<string>> data, fm_data;
    process_csv(file1, data);
    process_csv(file2, fm_data);
    data.erase(data.begin());
    fm_data.erase(fm_data.begin());

    load_trees(BEST_TREES_PATH_GLOBAL);
    initialize_function_description();
    int tree_id = 1;
    vector<thread> threads;
    for (int i = 0; i < nr_trees; ++i) {
        threads.push_back(thread(f_measure_build, 1, data, fm_data));
        ++tree_id;
    }
    
    for (auto &t : threads) {
        t.join();
    }
    
    sort(bestTrees.begin(), bestTrees.end(), cmp);

    for (int i = 0; i < NR_BEST_TREES; ++i) {
        store_tree(BEST_TREES_PATH_GLOBAL, i + 1, bestTrees[i]);
    }
}



void start_local_solution(int argc, char **argv) {
    if (argc != NUMBER_OF_ARGS_LOCAL) {
        cout << "Wrong usage: different number of arguments!\n";
        cout << "Correct ussage: ./o_solution local nr_trees";
        exit(-1);
    }

    int nr_trees = atoi(argv[2]);
    vector<vector<string>> data;

    load_trees(BEST_TREES_PATH_LOCAL);
    initialize_function_description();
    int tree_id = 1;
    vector<thread> threads;

    for (int i = 0; i < nr_trees; ++i) {
        threads.push_back(thread(f_measure_build, 2, data, data));
        ++tree_id;
    }
    
    for (auto &t : threads) {
        t.join();
    }
    
    sort(bestTrees.begin(), bestTrees.end(), cmp);

    for (int i = 0; i < NR_BEST_TREES; ++i) {
        store_tree(BEST_TREES_PATH_LOCAL, i + 1, bestTrees[i]);
    }
}


int main(int argc, char **argv) {
    if (strcmp(argv[1], GLOBAL) == 0) {
        start_global_solution(argc, argv);
    } else {
        start_local_solution(argc, argv);
    }

    return 0;
}
