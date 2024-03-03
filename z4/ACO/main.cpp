#include <iostream>
#include "GraphMatrix.h"
#include "Algorytmy.h"
#include <iomanip>

using namespace std;
using namespace std::chrono;

Algorytmy myAlgorytm;

int main(int argc, char* argv[])
{
    srand(time(NULL));

    char option;
    string fileName;
    string nazwaIni;
    int temperaturaPocz;
    int dlugoscEpoki;
    char chlodzenie;
    char sasiedztwo;

    do
    {
        cout << endl;
        cout << "==== MENU ===" << endl;
        cout << "1.Wykonaj algorytm SA dla jednego grafu" << endl;
        cout << "2.Wczytaj plik .ini i przeprowadz pelne pomiary dla niego" << endl;
        cout << "0.Wyjscie" << endl;
        cout << "Podaj opcje:";
        cin >> option;
        cout << endl;

        switch (option)
        {
        case '1':
            cout << " Podaj nazwe pliku: ";
            cin >> fileName;
            myAlgorytm.loadGraphFromFile(fileName);
            myAlgorytm.sAlgorithm();
            break;

        case '2':
            cout << "Podaj nazwe pliku inicjujacego:" << endl;
            cin >> nazwaIni;
            myAlgorytm.pelnePomiary(nazwaIni);
            cout << "Wyniki zapisano do pliku. Pomiary czasu sa podane w milisekundach" << endl;
            break;

              }

    }
    while (option != '0');

    return 0;
}

