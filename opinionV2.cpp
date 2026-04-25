#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

/********************DO NOT EDIT**********************/
void read_opinion_data(string filename);
void read_edge_data(string filename);
void build_graph();
/****************************************************************/

int node_count = 0;   // total number of nodes

// store each node's opinion
vector<int> node_opinion;

// adjacency list (who influences current node)
vector<vector<int>> graph;

// edge list (raw input)
vector<vector<int>> connections;

void build_graph()
{
    // initialize adjacency list
    graph.assign(node_count, vector<int>());

    for (int i = 0; i < connections.size(); i++)
    {
        int from = connections[i][0];
        int to = connections[i][1];

        if (from < node_count && to < node_count)
        {
            graph[to].push_back(from); // who influences 'to'
        }
    }

    // ensure opinion vector size matches node count
    if (node_opinion.size() < node_count)
    {
        node_opinion.resize(node_count, 0);
    }
}

// compute fraction of nodes with opinion = 1
double fraction_of_ones()
{
    if (node_count == 0) return 0.0;

    int ones_count = 0;

    for (int i = 0; i < node_count; i++)
    {
        if (node_opinion[i] == 1)
            ones_count++;
    }

    return (double)ones_count / node_count;
}

// find majority opinion among neighbors
int majority_opinion(int idx)
{
    int count_one = 0;
    int neighbor_total = graph[idx].size();

    for (int i = 0; i < graph[idx].size(); i++)
    {
        int nb = graph[idx][i];

        if (node_opinion[nb] == 1)
            count_one++;
    }

    int count_zero = neighbor_total - count_one;

    if (count_one > count_zero)
        return 1;
    else
        return 0;
}

// update all nodes
bool update_all()
{
    vector<int> updated = node_opinion;
    bool any_change = false;

    for (int i = 0; i < node_count; i++)
    {
        int maj = majority_opinion(i);

        if (maj != node_opinion[i])
        {
            updated[i] = maj;
            any_change = true;
        }
    }

    node_opinion = updated;
    return any_change;
}

int main() {

    // read input
    read_opinion_data("opinions.txt"); 
    read_edge_data("edge_list.txt");

    build_graph();
    
    cout << "Total nodes: " << node_count << endl;
    
    int max_iter = 30;
    int step = 0;
    bool changed = true;
    
    cout << "Iteration " << step << ": fraction of 1's = " 
         << fraction_of_ones() << endl;
    
    while (step < max_iter && changed)
    {
        step++;
        changed = update_all();

        cout << "Iteration " << step
             << ": fraction of 1's = "
             << fraction_of_ones()
             << endl;
    }
    
    double final_ratio = fraction_of_ones();

    cout << "Iteration " << step << ": fraction of 1's = " 
         << final_ratio << endl;
    
    if(final_ratio == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_ratio == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << step << " iterations" << endl;
    
    return 0;
}


/*********** File Reading **************************/

void read_opinion_data(string filename)
{
    ifstream inFile(filename);
    int id, val;

    while(inFile >> id >> val)
    {
        node_opinion.push_back(val);
        if(id >= node_count) node_count = id + 1;
    }

    inFile.close();
}

void read_edge_data(string filename)
{
    ifstream inFile(filename);
    int u, v;
    
    while(inFile >> u >> v)
    {
        connections.push_back({u, v});

        if(u >= node_count) node_count = u + 1;
        if(v >= node_count) node_count = v + 1;
    }

    inFile.close();
}