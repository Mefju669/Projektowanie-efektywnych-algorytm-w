#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include "GraphMatrix.h"
#include "Algorytmy.h"

using namespace std;
using namespace std::chrono;

class Algorytmy
{
private:
    GraphMatrix graphMatrix;
    int koszt, czas;
    string czasString;
    vector<int> cyklHamiltona;

public:
    high_resolution_clock::time_point czasPoczatkowy;
    high_resolution_clock::time_point czasKoncowy;

    void bruteForce();

    void loadGraphFromFile(const string& filename);
    void pelnePomiary(string nazwaIni);

    void zacznijLiczyc();
    void skonczLiczyc();
};
