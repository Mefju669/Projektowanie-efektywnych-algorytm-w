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

    int heurystycznyKosztPoczatkowy();

    void branchAndBound();
    void glownaFunkcjaBNB(vector<int>& currentTour, vector<bool>& visited, int currentCost, int level, vector<int>& bestTour, int& bestCost);


    void loadGraphFromFile(const string& filename);
    void pelnePomiary(string nazwaIni);

    void zacznijLiczyc();
    void skonczLiczyc();
};
