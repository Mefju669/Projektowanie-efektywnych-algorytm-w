#include <iostream>
#include <algorithm>
#include "Algorytmy.h"
#include "GraphMatrix.h"
#include <functional>
#include <iomanip>
#include <locale>

#include <vector>
#include <cmath>
using namespace std;
using namespace std::chrono;




void Algorytmy::pelnePomiary(string nazwaIni) //funkcja przeprowadzająca pełne pomiary na podstawie pliku .ini
{

    vector<int> liczbyPowtorzen;
    vector<string> nazwyGrafow;
    vector<char>sasiedztwa;
    vector<double> dlugosciEpok;


    ifstream plik(nazwaIni, ios::in);

    if (plik.is_open())
    {
        string linia;
        while (getline(plik, linia))
        {

            double dlugoscEpoki;
            char sasiedztwo;
            istringstream myStream(linia);
            int liczbaPowtorzen;
            string nazwaGrafu;

            if (myStream >> dlugoscEpoki >> sasiedztwo >> nazwaGrafu >>liczbaPowtorzen)
            {

                dlugosciEpok.push_back(dlugoscEpoki);
                sasiedztwa.push_back(sasiedztwo);
                nazwyGrafow.push_back(nazwaGrafu);
                liczbyPowtorzen.push_back(liczbaPowtorzen);



            }
        }

        plik.close();
    }
    else
    {
        cout << "Cos poszlo nie tak przy otwieraniu pliku" << endl;
        return;
    }

    ofstream outputFile("pomiary.csv");
    if (!outputFile.is_open())
    {
        cerr << "Cos poszlo nie tak przy otwieraniu pliku" << endl;
        return;
    }






    for (size_t i = 0; i < nazwyGrafow.size(); i++)
    {
        loadGraphFromFile(nazwyGrafow[i]);

        outputFile << nazwyGrafow[i] << ";" << "Czas [ms]" << ";" << "Blad [%]" << endl;

        for (int j = 0; j < liczbyPowtorzen[i]; j++)
        {
            cout << endl;
            cout << "Pomiar nr " << j + 1 << endl;

            sAlgorithm(dlugosciEpok[i], sasiedztwa[i]);

            // Ustawienie odpowiednich parametrów dla zapisu do pliku CSV
            outputFile << fixed << setprecision(3);

            // Zapis czasu i błędu do pliku
            outputFile << ";" << Algorytmy::czas;
            string bladStr = to_string(Algorytmy::blad);
            replace(bladStr.begin(), bladStr.end(), '.', ',');
            outputFile << ";" << bladStr;
            outputFile << endl;
        }

        outputFile<<endl;
    }


    outputFile.close();
}


void Algorytmy::sAlgorithm(double dlugoscEpokiWielokrotnosc, char sasiedztwo)
{
    zacznijLiczyc(); //początek

    heurystycznyKosztPoczatkowy(); //obliczanie kosztu początkowego algorytmem najbliższego sąsiada

    /*inicjalizacja zmiennych:
    obliczenie temperatury początkowej na podstawie rozwiązania początkowego
    numVertices to ilość wierzchołków
    bestTour to tablica dynamiczna przechowująca najlepszy znany cykl Hamiltona
    bestCost analogicznie przechowuje najlepszy znany koszt
    currentTour przechowuje aktualnie rozważaną jako cykl Hamiltona trasę
    visited przechowuje informację o odwiedzonych wierzchołkach
    coolingRate to współczynnik do przemnażania przez niego temperatury celem geometrycznego jej obniżania*/

    double temperaturaPocz=0.99*graphMatrix.bestCost;
    int numVertices = graphMatrix.numVertices;
    vector<int> bestTour = graphMatrix.bestTour;
    int bestCost = graphMatrix.bestCost;
    vector<int> currentTour(numVertices, -1);
    vector<bool> visited(numVertices, false);
    currentTour[0] = 0;
    double coolingRate = 0.99;


    /*inicjalizacja zmiennych pomocniczych
    currentTemperature to obniżana w trakcie chłodzenia temperatura
    currentSolution to aktualnie rozważany cykl
    currentCost to koszt currentSolution
    dlugoscEpoki to obliczana z wieloktrotności rozmiaru sąsiedzwa długość
    epoki (ilość kroków)
    */
    double currentTemperature = temperaturaPocz;
    vector<int> currentSolution = bestTour;
    int currentCost = bestCost;
    int dlugoscEpoki=0;

    //obliczenie długości epoki jako liczby kroków na podstawie podanej jako argument wielokrotności rozmiaru sąsiedztwa
    if(sasiedztwo=='z')
    {
        dlugoscEpoki=((numVertices*(numVertices-1))/2)*dlugoscEpokiWielokrotnosc;
    }
    else
    {
        dlugoscEpoki=numVertices*dlugoscEpokiWielokrotnosc;
    }



    //główna pętla programu wykonująca zasadniczą część algorytmu do póki temperatura jest większa od 1
    while (currentTemperature > 1.0)
    {
        for (int i = 0; i < dlugoscEpoki; ++i)
        {

            if (sasiedztwo == 'z') //dla 2-zamiany
            {
                // Generowanie sąsiedniego rozwiązania przez 2-zamianę
                vector<int> newSolution = currentSolution;
                int pos1 = rand() % numVertices;
                int pos2 = rand() % numVertices;
                swap(newSolution[pos1], newSolution[pos2]);

                //sumowanie wag krawędzi pomiędzy sąsiednimi wierzchołkami w cyklu Hamiltona
                int newCost = 0;
                for (int j = 0; j < numVertices; ++j)
                {
                    int from = newSolution[j];
                    int to = newSolution[(j + 1) % numVertices];
                    newCost += graphMatrix.adjacencyMatrix[from][to];
                }

                // Akceptacja nowego rozwiązania
                if (newCost < currentCost || exp((currentCost - newCost) / currentTemperature) > ((double)rand() / RAND_MAX))
                {
                    currentSolution = newSolution;
                    currentCost = newCost;
                }
            }
            else if (sasiedztwo == 'l') //dla losowego przemieszczenia
            {
                //losowe przemieszczenie fragmentu vectora newSolution poprzez odwrócenie kolejności między 2 losowo wybranymi pozycjami
                vector<int> newSolution = currentSolution;
                int pos1 = rand() % numVertices;
                int pos2 = rand() % numVertices;
                int start = min(pos1, pos2);
                int end = max(pos1, pos2);
                reverse(newSolution.begin() + start, newSolution.begin() + end + 1);

                //sumowanie wag krawędzi pomiędzy sąsiednimi wierzchołkami w cyklu Hamiltona
                int newCost = 0;
                for (int j = 0; j < numVertices; ++j)
                {
                    int from = newSolution[j];
                    int to = newSolution[(j + 1) % numVertices];
                    newCost += graphMatrix.adjacencyMatrix[from][to];
                }

                /* Akceptacja nowego rozwiązania jeśli nowy koszt jest lepszy lub gorszy, ale wartość exp((currentCost - newCost) / currentTemperature)
                 jest większa niż losowa wartość z zakresu (0,1) aby uniknąć utknięcia w lokalnym minimum*/
                if (newCost < currentCost || exp((currentCost - newCost) / currentTemperature) > ((double)rand() / RAND_MAX))
                {
                    currentSolution = newSolution;
                    currentCost = newCost;
                }
                else
                {
                    // Jeżeli rozwiązanie nie zostało zaakceptowane, przywracamy poprzednią zamianę
                    reverse(newSolution.begin() + start, newSolution.begin() + end + 1);
                }
            }
            else
            {
                cerr << "Zle podanie sasiedztwo"<<endl;
                return;
            }

            // Aktualizacja najlepszego rozwiązania, jeśli znaleziono lepsze niż dotychczas znane
            if (currentCost < bestCost)
            {
                bestTour = currentSolution;
                bestCost = currentCost;
            }
        }

        // Schładzanie geometryczne
        currentTemperature *= coolingRate;



    }

    skonczLiczyc();

    czas = duration_cast<milliseconds>(czasKoncowy - czasPoczatkowy).count();

    Algorytmy::czas=czas;
    Algorytmy::koszt=bestCost;
    Algorytmy::blad=((static_cast<double>(bestCost)/static_cast<double>(graphMatrix.optymalnyKoszt))-1)*100;




    if (bestTour.empty())
    {
        cout << "Nie znaleziono cyklu Hamiltona." << endl;
    }
    else
    {
        cout << "Cykl Hamiltona: ";
        for (int i = 0; i < numVertices; i++)
        {
            cout << bestTour[i] << " ";
        }
        cout << bestTour[0] << endl;
        cout << "Koszt:  " << Algorytmy::koszt <<endl;
        cout << "Blad:  " << Algorytmy::blad << "%"<<endl;


    }

    cout << "Czas: " << czas << " ms" << endl;
}

void Algorytmy::heurystycznyKosztPoczatkowy()
{
    int numVertices = graphMatrix.numVertices;
    vector<int> bestTour(numVertices, -1);
    int bestCost = INT_MAX;

    // zaczynamy od wierzchołka 0
    int startVertex = 0;

    // Resetowanie zmiennych pomocniczych
    vector<bool> visited(numVertices, false);
    vector<int> currentTour(numVertices, -1);
    int currentCost = 0;
    int currentVertex = startVertex;

    // Rozpoczęcie cyklu Hamiltona
    for (int i = 0; i < numVertices; ++i)
    {
        // Zaznaczenie bieżącego wierzchołka jako odwiedzonego
        visited[currentVertex] = true;
        currentTour[i] = currentVertex;

        // Znalezienie najbliższego sąsiada
        int nearestNeighbor = -1;
        int minWeight = INT_MAX;

        for (int neighbor = 0; neighbor < numVertices; ++neighbor)
        {
            if (!visited[neighbor] && graphMatrix.adjacencyMatrix[currentVertex][neighbor] < minWeight)
            {
                nearestNeighbor = neighbor;
                minWeight = graphMatrix.adjacencyMatrix[currentVertex][neighbor];
            }
        }

        // Sprawdzenie, czy znaleziono najbliższego sąsiada
        if (nearestNeighbor == -1)
        {
            // Przerwanie, jeśli nie znaleziono sąsiada
            break;
        }

        // Aktualizacja kosztu
        currentCost += minWeight;

        // Przejście do najbliższego sąsiada
        currentVertex = nearestNeighbor;
    }

    // Zakończenie cyklu Hamiltona do punktu początkowego (wierzchołka 0)
    currentCost += graphMatrix.adjacencyMatrix[currentTour[numVertices - 1]][startVertex];

    // Zapisanie wyników do zmiennych klasy
    Algorytmy::graphMatrix.bestCost = currentCost;
    Algorytmy::graphMatrix.bestTour = currentTour;
}









void Algorytmy::loadGraphFromFile(const string& filename) //funkcja ładująca pojedynczy graf z pliku

{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Nie mozna otworzyc pliku " << filename << endl;
        return;
    }

    int numVertices, numEdges, optymalnyKoszt;
    file >> optymalnyKoszt;
    file >> numVertices;

    GraphMatrix graphMatrix(numVertices, optymalnyKoszt);

    for (int i = 0; i < numVertices; i++)
    {
        int source, destination, weight;
        for (int j = 0; j < numVertices; j++)
        {
            file >> weight;
            graphMatrix.addEdge(i, j, weight);
        }
    }

    cout << endl;
    cout << "Wczytano graf " << filename << endl;
    //graphMatrix.printGraph();
    cout << endl;
    Algorytmy::graphMatrix = graphMatrix;

    file.close();
}

void Algorytmy::zacznijLiczyc() //czas startu
{
    czasPoczatkowy = high_resolution_clock::now();
}

void Algorytmy::skonczLiczyc() //czas stopu, potem obliczana jest różnica tych czasów
{
    czasKoncowy = high_resolution_clock::now();
}
