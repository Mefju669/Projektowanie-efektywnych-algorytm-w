#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include "GraphMatrix.h"
#include "Algorytmy.h"
#include <iomanip>

using namespace std;
using namespace std::chrono;



class Algorytmy
{
private:
    GraphMatrix graphMatrix;
    int czas;
    int koszt;;
    double blad;
    string czasString;
    vector<int> cyklHamiltona;


public:
    high_resolution_clock::time_point czasPoczatkowy;
    high_resolution_clock::time_point czasKoncowy;




    void sAlgorithm(double dlugoscEpokiWielokrotnosc, char chlodzenie);
    void heurystycznyKosztPoczatkowy();



    void loadGraphFromFile(const string& filename);
    void pelnePomiary(string nazwaIni);


    void zacznijLiczyc();
    void skonczLiczyc();
};
