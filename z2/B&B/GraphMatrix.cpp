#include "graphmatrix.h"
#include <fstream>
#include <sstream>

using namespace std; //ta klasa obsluguje grafy w postaci macierzy i podstawowe operacje na nich

GraphMatrix::GraphMatrix(int vertices)
{
    numVertices = vertices;
    adjacencyMatrix.resize(numVertices, vector<int>(numVertices, -1));
}

void GraphMatrix::addEdge(int source, int destination, int weight)
{
    adjacencyMatrix[source][destination] = weight;
}

void GraphMatrix::printGraph()
{
    for (int i = 0; i < numVertices; i++)
    {
        for (int j = 0; j < numVertices; j++)
        {
            if(adjacencyMatrix[i][j]==-1)
            {
                cout << adjacencyMatrix[i][j] << " ";
            }
            else if(adjacencyMatrix[i][j]!=-1)
            {
                cout <<" "<< adjacencyMatrix[i][j] << " ";
            }
        }
        cout << endl;
    }
}

bool GraphMatrix::hasEdge(int source, int destination)
{

    return adjacencyMatrix[source][destination] !=-1;
}



