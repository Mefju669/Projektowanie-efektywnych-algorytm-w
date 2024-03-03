#include <iostream>
#include "GraphMatrix.h"
#include "Algorytmy.h"
using namespace std;
using namespace std::chrono;

Algorytmy myAlgorytm;

int main(int argc, char* argv[])
{
    srand(time(NULL));

    char option;
    string fileName;
    string nazwaIni;

    do
    {
        cout << endl;
        cout << "==== MENU ===" << endl;
        cout << "1.Wykonaj algorytm Breach and Bound dla jednego grafu" << endl;
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
            cout << endl;
            myAlgorytm.branchAndBound();
            break;

        case '2':
            cout << "Podaj nazwe pliku inicjujacego:" << endl;
            cin >> nazwaIni;
            myAlgorytm.pelnePomiary(nazwaIni);
            cout << "Wyniki zapisano do pliku. Instancje <=14 maja pomiary czasu w ms, pozostale w sekundach." << endl;
            break;

              }

    }
    while (option != '0');

    return 0;
}
