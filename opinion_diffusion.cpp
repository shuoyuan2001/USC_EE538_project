#include <bits/stdc++.h>
using namespace std;

// 全局数据结构（也可以改成类）
int N = 0; // 节点数（ voters ）
vector<int> opinions;            // opinions[i] in {0,1}
vector<vector<int>> adj;         // adjacency matrix adj[u][v] == 1 means u influences v
vector<pair<int,int>> edge_list; // raw edge list (u,v)

// --- Helper: trim whitespace from string ends
static inline string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Read opinions.txt; file lines like "2, 1"
void read_opinions_file(const string &path) {
    ifstream fin(path);
    if (!fin) {
        cerr << "Cannot open " << path << endl;
        exit(1);
    }
    string line;
    int max_idx = -1;
    vector<pair<int,int>> tmp;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        // allow format "i, x" optionally with spaces
        size_t comma = line.find(',');
        if (comma == string::npos) continue; // 忽略非标准行
        string a = trim(line.substr(0, comma));
        string b = trim(line.substr(comma + 1));
        int idx = stoi(a);
        int val = stoi(b);
        if (val != 0 && val != 1) {
            cerr << "Invalid opinion value at line: " << line << " (must be 0 or 1)\n";
            exit(1);
        }
        tmp.push_back({idx, val});
        max_idx = max(max_idx, idx);
    }
    if (max_idx < 0) {
        cerr << "No opinions read from " << path << endl;
        exit(1);
    }
    N = max(N, max_idx + 1);
    opinions.assign(N, 0);
    for (auto &p : tmp) opinions[p.first] = p.second;
}

// Read edge_list.txt; lines like "0, 2" meaning 0 influences 2
void read_edge_list_file(const string &path) {
    ifstream fin(path);
    if (!fin) {
        cerr << "Cannot open " << path << endl;
        exit(1);
    }
    string line;
    int max_idx = -1;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        string a = trim(line.substr(0, comma));
        string b = trim(line.substr(comma + 1));
        int u = stoi(a);
        int v = stoi(b);
        edge_list.push_back({u, v});
        max_idx = max(max_idx, max(u, v));
    }
    if (max_idx >= 0) {
        N = max(N, max_idx + 1);
    }
}

// (1) allocate matrix adj of appropriate size
// (2) run through edge list and populate adj
void build_adj_matrix()
{
    // allocate NxN zero matrix
    adj.assign(N, vector<int>(N, 0));
    // populate from edge_list
    for (auto &e : edge_list) {
        int u = e.first;
        int v = e.second;
        if (u < 0 || u >= N || v < 0 || v >= N) continue; // 忽略非法
        adj[u][v] = 1;
    }
}

// (3) Calculate the fraction of nodes with opinion 1 and return it.
double calculate_fraction_of_ones()
{
    if (N <= 0) return 0.0;
    int cnt = 0;
    for (int i = 0; i < N; ++i) if (opinions[i] == 1) ++cnt;
    return double(cnt) / double(N);
}

// For a given node, count majority opinion among its neighbours. Tie -> 0.
// 注意：edge_list 定义是 u influences v，即要看所有 u s.t. adj[u][node] == 1
int get_majority_friend_opinions(int node)
{
    int zeros = 0, ones = 0;
    for (int u = 0; u < N; ++u) {
        if (adj[u][node]) {
            if (opinions[u] == 0) ++zeros;
            else ++ones;
        }
    }
    if (ones > zeros) return 1;
    // tie or zeros > ones -> return 0
    return 0;
}

// Calculate new opinions for all voters and return if anyone's opinion changed
bool update_opinions()
{
    vector<int> new_opinions(N, 0);
    for (int i = 0; i < N; ++i) {
        new_opinions[i] = get_majority_friend_opinions(i);
    }
    bool changed = false;
    for (int i = 0; i < N; ++i) {
        if (new_opinions[i] != opinions[i]) {
            changed = true;
            break;
        }
    }
    opinions.swap(new_opinions);
    return changed;
}

int main(int argc, char** argv)
{
    // 文件名可以通过命令行传入，也可以用默认名
    string opinions_file = "opinions.txt";
    string edges_file = "edge_list.txt";
    if (argc >= 3) {
        opinions_file = argv[1];
        edges_file = argv[2];
    } else {
        cout << "Using default input files: opinions.txt and edge_list.txt\n";
    }

    // 1) 读文件
    read_opinions_file(opinions_file);
    read_edge_list_file(edges_file);

    // 如果 edge_list 里出现了比 opinions 更大的节点索引，需要扩展 opinions 向量到 N
    if ((int)opinions.size() < N) {
        int old = opinions.size();
        opinions.resize(N, 0); // 默认孤立节点 opinion 设为 0（你也可以按需改变）
    }

    // 2) build adjacency matrix
    build_adj_matrix();

    // 3) 迭代控制参数
    const int max_iterations = 10000; // 可按作业要求调整
    const int print_interval = 100;   // 样例里每 100 次打印一次
    int iteration = 0;

    // 初始状态打印
    cout << fixed << setprecision(4);
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;

    // (6) Run until consensus or max iterations
    bool consensus_reached = false;
    for (iteration = 1; iteration <= max_iterations; ++iteration) {
        bool changed = update_opinions();

        if (iteration % print_interval == 0) {
            cout << "Iteration " << iteration << ": fraction of 1's = " 
                 << calculate_fraction_of_ones() << endl;
        }

        if (!changed) {
            consensus_reached = true;
            break;
        }
    }

    // Print final result (iteration currently is the iteration where it terminated or max_iterations+1)
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;

    if (consensus_reached) {
        double final_fraction = calculate_fraction_of_ones();
        if (final_fraction == 1.0) {
            cout << "Consensus reached: all 1’s\n";
        } else if (final_fraction == 0.0) {
            cout << "Consensus reached: all 0’s\n";
        } else {
            // 虽然 update_opinions() 返回 false 意味着没有节点改变，但可能仍不是完全一致（例如孤立节点规则导致不变）
            cout << "Consensus reached: no node changed in last iteration (fraction of 1's = "
                 << final_fraction << ")\n";
        }
    } else {
        cout << "Stopped after max iterations (" << max_iterations << ").\n";
    }

    return 0;
}