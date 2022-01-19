#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <algorithm>
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

#define GENERATION_SIZE 10
#define EVOLUTION_TIME 10
#define SIZE 1000
#define A_LOT 99999999
#define TEST_SIZE 20

int arr[SIZE][SIZE];
vector<vector<int>> population;
vector<double> fit;
int gen_best = A_LOT;
int vertex_number = 0;
vector<int> gen_best_route;

int load_graph(const char *path) {
    vertex_number = 0;
    xml_document document;
    xml_parse_result result = document.load_file(path);
    if (result) {
        xml_node root_node = document.child("travellingSalesmanProblemInstance");
        xml_node graph_node = root_node.child("graph");
        for (xml_node vertex_node = graph_node.child("vertex"); vertex_node; vertex_node = vertex_node.next_sibling(
                "vertex")) {
            for (xml_node edge_node = vertex_node.child("edge"); edge_node; edge_node = edge_node.next_sibling(
                    "edge")) {
                arr[vertex_number][edge_node.text().as_int()] = (int) round(edge_node.attribute("cost").as_double());
            }
            vertex_number++;
        }
    } else {
        cout << result.description();
    }
    return vertex_number;
}

void show_graph() {
    for (int i = 0; i < vertex_number; i++) {
        for (int j = 0; j < vertex_number; j++) {
            cout << arr[i][j] << " ";
        }
        cout << "\n";
    }
}

void gen_pop() {
    vector<int> v;
    for (int i = 0; i < vertex_number; i++) {
        v.push_back(i);
    }
    vector<vector<int>> pop;
    for (int i = 0; i < GENERATION_SIZE; i++) {
        random_shuffle(v.begin(), v.end());
        population.push_back(v);
    }
}

void calculate_fit() {
    vector<double> f;
    for (vector<int> member : population) {
        int temp = 0;
        for (int i = 0; i < member.size() - 1; i++) {
            temp += arr[member.at(i)][member.at(i + 1)];
        }
        temp += arr[member.back()][member.front()];
        if (temp < gen_best) {
            gen_best = temp;
            gen_best_route = member;
        }
        f.push_back((double) 1 / temp);
    }
    double sum = 0;
    for (double d : f) {
        sum += d;
    }
    for (double &d : f) {
        d /= sum;
    }
    fit = f;
}

vector<int> pool_select() {
    int index = 0;
    double r = (double) rand() / RAND_MAX;
    while (r > 0) {
        r -= fit[index];
        index++;
    }
    index--;
    return population[index];
}

vector<int> crossover(vector<int> a, vector<int> b) {
    int start = rand() % a.size();
    int end = rand() % (a.size() - start) + start;
    vector<int> child = vector<int>(a.begin() + start, a.begin() + end);
    for (auto i : b) {
        if (find(child.begin(), child.end(), i) == child.end()) {
            child.push_back(i);
        }
    }
    return child;
}

void next_gen() {
    vector<vector<int>> next_pop;
    for (int i = 0; i < GENERATION_SIZE; i++) {
        vector<int> parentA = pool_select();
        vector<int> parentB = pool_select();
        vector<int> child = crossover(parentA, parentB);
        next_pop.push_back(child);
    }
    population = next_pop;
}

void gen_algorithm() {
    gen_best = A_LOT;
    gen_best_route.clear();
    gen_pop();
    for (int i = 0; i < EVOLUTION_TIME; i++) {
        calculate_fit();
        next_gen();
    }
}

void test() {
    char path[SIZE];
    cout<<"Enter the path to the file: \n";
    cin>>path;
    vertex_number = load_graph(path);
    int array[TEST_SIZE];
    auto time = 0;

    for(int & i : array){
        auto t1 = chrono::steady_clock::now();
        gen_algorithm();
        cout << gen_best << "\n";
        auto t2 = chrono::steady_clock::now();

        i = gen_best;

        time += chrono::duration_cast<chrono::microseconds>(t2-t1).count();
    }
    double sum = 0;
    cout<<"Average time for genetic algorithm for file "<<path<<" "<<time/TEST_SIZE<<"us"<<endl;
    for(int i: array){
        sum += i;
    }
    cout<<"With average distance of "<<sum/TEST_SIZE<<"\n";
}

int main() {
    bool exit = false;
    int ord;
    srand(time(nullptr));
    do {
        cout << "Menu: \n"
                "0. exit \n"
                "1. load graph \n"
                "2. show graph \n"
                "3. genetic \n"
                "4. test \n";
        cin >> ord;
        switch (ord) {
            case 1:
                char path[SIZE];
                cout << "Enter the path to the file: \n";
                cin >> path;
                vertex_number = load_graph(path);
                break;
            case 2:
                show_graph();
                break;
            case 3:
                gen_algorithm();
                cout << gen_best << "\n";
                for (int i: gen_best_route) {
                    cout << i << " ";
                }
                cout << "\n";
                break;
            case 4:
                test();
                break;
            default:
                exit = true;
        }
    } while (!exit);
    return 0;
}
