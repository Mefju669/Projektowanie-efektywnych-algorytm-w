#ifndef GRAPHMATRIX_H
#define GRAPHMATRIX_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class GraphMatrix
{

public:
    int numVertices;
    vector<vector<int>> adjacencyMatrix;

    GraphMatrix() = default;


    GraphMatrix(int vertices);
    void addEdge(int source, int destination, int weight);
    void printGraph();
    bool hasEdge(int source, int destination);



};

#endif
