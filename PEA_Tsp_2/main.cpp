#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

#define MAXIMUM 9999999
#define SIZE 1000

int arr[SIZE][SIZE];
bool check[SIZE];
int vertex_number = 0;
int cost = 0;

int load_graph(const char *path) {
    vertex_number = 0;
    xml_document document;
    xml_parse_result result = document.load_file(path);
    if(result){
        xml_node root_node = document.child("travellingSalesmanProblemInstance");
        xml_node graph_node = root_node.child("graph");
        for (xml_node vertex_node = graph_node.child("vertex"); vertex_node; vertex_node = vertex_node.next_sibling("vertex")) {
            for (xml_node edge_node = vertex_node.child("edge"); edge_node; edge_node = edge_node.next_sibling("edge")) {
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
    for(int i=0;i<vertex_number;i++) {
        for(int j=0;j<vertex_number;j++) {
            cout << arr[i][j] << " ";
        }
        cout<<"\n";
    }
}

int bestDestination(int beginning)
{
    int nextNode = MAXIMUM;
    int minimumWeight = MAXIMUM;
    int minimumPath;

    for (int i = 0; i < vertex_number; i++)
    {
        if (arr[beginning][i] != MAXIMUM && !check[i])
        {
            if (arr[beginning][i] + arr[i][beginning] < minimumWeight)
            {
                minimumWeight = arr[beginning][i] + arr[i][beginning];
                minimumPath = arr[beginning][i];
                nextNode = i;
            }
        }
    }

    if (minimumWeight != MAXIMUM)
    {
        cost += minimumPath;
    }

    return nextNode;
}

void shortestPath(int beginning)
{
    int nextNode;

    check[beginning] = true;

    nextNode = bestDestination(beginning);
    if (nextNode == MAXIMUM)
    {
        nextNode = 0;
        cost += arr[beginning][nextNode];
        return;
    }

    shortestPath(nextNode);
}

int nearest_neighbor(int curr, vector<int>& available){
    int best = MAXIMUM;
    int pos = MAXIMUM;
    for(int i=0;i<available.size();i++){
        int temp = arr[curr][available[i]];
        if(temp<best){
            best = temp;
            pos = i;
        }
    }
    int dest = available[pos];
    available.erase(available.begin()+pos);
    return dest;
}

int init_route() {
    vector<int> route;
    route.reserve(vertex_number + 1);
    vector<int> available;
    available.reserve(vertex_number);
    for(int i=1;i<vertex_number;i++){
        available.push_back(i);
    }
    route.push_back(0);
    while(!available.empty()){
        route.push_back(nearest_neighbor(route.back(),available));
    }
    route.push_back(0);
    int temp = 0;
    for(int i=0;i<route.size()-1;i++){
        temp += arr[route.at(i)][route.at(i+1)];
    }
    return temp;
}

void test() {
    char path[SIZE];
    cout<<"Podaj sciezke do pliku: \n";
    cin>>path;
    vertex_number = load_graph(path);
    vector<int> vector;
    auto time = 0;
    auto timeNN = 0;

    for (int i = 0; i < 100; i++) {

        auto tt1 = chrono::steady_clock::now();
            shortestPath(0);
            cout<<cost<<"\n";
            cost = 0;
            for (int j = 0; j < vertex_number; j++)
            {
                check[j] = false;
            }
        auto tt2 = chrono::steady_clock::now();

        auto t1 = chrono::steady_clock::now();
            cout<<init_route()<<"\n";
        auto t2 = chrono::steady_clock::now();

        time += chrono::duration_cast<chrono::microseconds>(tt2-tt1).count();
        timeNN += chrono::duration_cast<chrono::microseconds>(t2-t1).count();
    }

    cout<<"Average time DP for file "<<path<<" "<<time/100<<"us"<<endl;
    cout<<"Average time NN for file "<<path<<" "<<timeNN/100<<"us"<<endl;
}


int main() {
    bool exit = false;
    int ord;
    do {
        cout<<"Menu: \n"
              "1. load graph \n"
              "2. show graph \n"
              "3. DP \n"
              "4. NN \n"
              "5. test \n";
        cin >> ord;
        switch (ord) {
            case 1:
                char path[SIZE];
                cout<<"Podaj sciezke do pliku: \n";
                cin>>path;
                vertex_number = load_graph(path);
                break;
            case 2:
                show_graph();
                break;
            case 3:
                shortestPath(0);
                cout<<cost<<endl;
                cost = 0;
                for (int i = 0; i < vertex_number; i++)
                {
                    check[i] = false;
                }
                break;
            case 4:
                cout<<init_route()<<"\n";
                break;
            case 5:
                test();
                break;
            default:
                exit = true;
        }
    } while (!exit);
    return 0;
}