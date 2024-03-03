#include <iostream>
#include <algorithm>
#include "Algorytmy.h"
#include "GraphMatrix.h"
#include <functional>
using namespace std;
using namespace std::chrono;

void Algorytmy::pelnePomiary(string nazwaIni) //metoda pobierająca dane z pliku .ini, robiąca pomiary i zapis do pliku .csv
{
    vector<int> liczbyPowtorzen;
    vector<int> koszty;
    vector<string> nazwyGrafow;
    vector<vector<int>> cyklePoprawne;

    ifstream plik(nazwaIni, ios::in); //odczyt .ini

    if (plik.is_open())
    {
        string linia;
        while (getline(plik, linia))
        {
            istringstream myStream(linia);
            int liczbaPowtorzen, koszt;
            string nazwaGrafu;
            vector<int> cyklHamiltona;

            if (myStream >> liczbaPowtorzen >> koszt >> nazwaGrafu)
            {
                liczbyPowtorzen.push_back(liczbaPowtorzen);
                koszty.push_back(koszt);
                nazwyGrafow.push_back(nazwaGrafu);

                int liczba;

                while (myStream >> liczba)
                {
                    cyklHamiltona.push_back(liczba);
                }



                cyklePoprawne.push_back(cyklHamiltona);
            }
        }

        plik.close();
    }
    else
    {
        cout << "Cos poszlo nie tak przy otwieraniu pliku" << endl;
    }

    ofstream outputFile("pomiary.csv");
    if (!outputFile.is_open())
    {
        cerr << "Cos poszlo nie tak przy otwieraniu pliku" << endl;
    }

    for (int i = 0; i < nazwyGrafow.size(); i++) //pomiary i zapis wyników
    {
        loadGraphFromFile(nazwyGrafow[i]);

        outputFile << nazwyGrafow[i];

        for (int j = 0; j < liczbyPowtorzen[i]; j++)
        {
            cout << endl;
            cout << "Pomiar nr " << j + 1 << endl;

            branchAndBound();
            outputFile << ";" << Algorytmy::czas;

            bool flagaBledu = false;
            vector<int> cyklTymczasowy = Algorytmy::cyklHamiltona;

            if(cyklePoprawne[i].size()!=1)
            {
                for (int k = 0; k < cyklTymczasowy.size(); k++)
                {
                    if (cyklTymczasowy[k] != cyklePoprawne[i][k])
                        flagaBledu = true;

                }
            }
            if (flagaBledu)
            {
                // Sprawdź, czy ścieżka jest w odwrotnej kolejności.

                for (int k = 0; k < cyklTymczasowy.size(); k++)
                {
                    if (cyklTymczasowy[k] != cyklePoprawne[i][cyklePoprawne[i].size() - 1 - k])
                    {
                        flagaBledu = false;  // Ścieżka jest poprawna, ale w odwrotnej kolejności.
                        break;
                    }
                }
            }


            if (koszty[i] != Algorytmy::koszt|| flagaBledu == true)
            {
                cout << "Obliczony koszt lub cykl Hamiltona sie nie zgadza" << endl;
            }
        }
        outputFile << endl;
    }

    outputFile.close();
}





void Algorytmy::branchAndBound()
{
    // Początek pomiaru czasu
    zacznijLiczyc();


    //inicjalizacja zmiennych
    int numVertices = graphMatrix.numVertices;  //liczba wierzchołków
    vector<int> bestTour;  // vector przechowujący najlepszy cykl Hamiltona
    int bestCost = heurystycznyKosztPoczatkowy();
    vector<int> currentTour(numVertices, -1);  // vector przechowujący aktualna trase
    vector<bool> visited(numVertices, false);  // vector przechowujący informację który wierzchołek odwiedzono
    currentTour[0] = 0; // Rozpocznynamy przeszukiwanie od pierwszego (numeracja od 0) wierzchołka
    visited[0] = true; //pierwszy (nr 0) wierzchołek oznaczony jako odwiedzony

    // wywołanie głównej funkcji odpowiedzialnej za algorytm
    glownaFunkcjaBNB(currentTour, visited, 0, 1, bestTour, bestCost);

    // Koniec pomiary czasu
    skonczLiczyc();

    int wyn;

    if (numVertices <= 14)
    {
        // Pomiar czasu w milisekundach dla małych instancji
        wyn = duration_cast<milliseconds>(czasKoncowy - czasPoczatkowy).count();
    }
    else if (numVertices > 14)
    {
        // Pomiar czasu w sekundach dla dużych instancji
        wyn = duration_cast<seconds>(czasKoncowy - czasPoczatkowy).count();
    }

    // Aktualizazja wyników
    czas = wyn;
    cyklHamiltona = bestTour;
    koszt = bestCost;

    // Wyświetlenie wyników
    if (bestTour.empty())
    {
        cout << "Nie znaleziono cyklu hamiltona." << endl;
    }
    else
    {
        cout << "Cykl hamiltona: ";
        for (int i = 0; i < numVertices; i++)
        {
            cout << bestTour[i] << " ";
        }
        cout << bestTour[0] << endl;
        cout << "Koszt: " << bestCost << endl;
    }

    cout << "Czas: " << wyn << endl;
}





void Algorytmy::glownaFunkcjaBNB(vector<int>& currentTour, vector<bool>& visited, int currentCost, int level, vector<int>& bestTour, int& bestCost)
{
    int numVertices = graphMatrix.numVertices; //ilość wierzchołków grafu, pobierana jest z obiektu graphMatrix
    int lastVertex = currentTour[level - 1]; //ostatni wierzchołek w rozważanym potencjalnym cyklu Hamiltona
    vector<vector<int>>& adjacencyMatrix = graphMatrix.adjacencyMatrix;

    if (level == numVertices) //Jeśli odwiedzono wszystkie wierzchołki w danej permutacji i nie jest do tej pory gorsza niż najlepsza znana,
        //algorytm sprawdza czy jest połączenie między pierwszym i ostatnim wierzchołkiem

    {
        int edgeWeight = adjacencyMatrix[lastVertex][currentTour[0]];

//jeśli połączenie między pierwszym a ostatnim nie jest równe -1 (istnieje) i koszt znalezionego cyklu Hamiltona jest lepszy niż
//najlepszego dotychczas znalezionego najlepszy koszt jest aktualizowany na właśnie znaleziony, tak samo aktualizowana jest
//najlepsza ścieżka czyli bestTour
        if (edgeWeight != -1 && currentCost + edgeWeight < bestCost)
        {
            bestCost = currentCost + edgeWeight;
            bestTour = currentTour;
        }



    }
    else
    {
        for (int i = 0; i < numVertices; i++) //Przeszukujemy wszystkie wierzchołki, które nie zostały jeszcze odwiedzone w sprawdzanej permutacji

        {
            if (visited[i]==false) //Sprawdzamy czy wierzchołek jest oznaczony jako odwiedzony, jeśli nie z macierzy sczytywana jest waga krawędzi między nim,
                // a wierzchołkiem nr i.
            {
                int edgeWeight = adjacencyMatrix[lastVertex][i];

                if (edgeWeight != -1 && currentCost + edgeWeight < bestCost)
                    //Jeśli dotychczasowy koszt potencjalnego cyklu jest większy niż najlepszy już znany, nie sprawdzamy kolejnych wierzchołków, czyli obcinamy liście.
                    //A jeśli nie jest większy dodajemy wierzchołek do aktualnej trasy i oznaczamy jako odwiedzony. Następuje rekurencyjne wywołanie
                    //funkcji BnB dla zaktualizowanych zmiennych.
                {
                    currentTour[level] = i;
                    visited[i] = true;

                    glownaFunkcjaBNB(currentTour, visited, currentCost + edgeWeight, level + 1, bestTour, bestCost);

                    currentTour[level] = -1; //Na koniec następuje cofnięcie zmian w bieżącej ścieżce, które zostały wprowadzone podczas
                    //przeszukiwania rekurencyjnego, aby przywrócić stan do poprzedniego poziomu rekurencji.
                    visited[i] = false;
                }
            }
        }
    }
}

//heurystycznie algorytmem najbliższego sąsiada obliczam koszt początkowy jako ograniczenie (bound) górne
int Algorytmy::heurystycznyKosztPoczatkowy()
{
    int numVertices = graphMatrix.numVertices;
    vector<bool> visited(numVertices, false);
    vector<int> bestTour;
    int bestCost = 0;

    int currentVertex = 0; // Rozpocznij od wierzchołka 0.
    visited[currentVertex] = true;
    bestTour.push_back(currentVertex);

    for (int i = 1; i < numVertices; i++)
    {
        int minEdgeWeight = INT_MAX;
        int nextVertex = -1;

        for (int j = 0; j < numVertices; j++)
        {
            if (!visited[j] && graphMatrix.adjacencyMatrix[currentVertex][j] < minEdgeWeight)
            {
                minEdgeWeight = graphMatrix.adjacencyMatrix[currentVertex][j];
                nextVertex = j;
            }
        }

        if (nextVertex == -1)
        {
            return INT_MAX-1;
        }

        bestCost += minEdgeWeight;
        visited[nextVertex] = true;
        bestTour.push_back(nextVertex);
        currentVertex = nextVertex;
    }

    // Dodaj powrót do pierwszego wierzchołka, aby zamknąć cykl.
    bestCost += graphMatrix.adjacencyMatrix[currentVertex][bestTour[0]];

    return bestCost+1;
}




void Algorytmy::loadGraphFromFile(const string& filename) //funkcja ładująca pojedynczy graf z pliku

{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Nie mozna otworzyc pliku " << filename << endl;
        return;
    }

    int numVertices, numEdges;
    file >> numVertices;

    GraphMatrix graphMatrix(numVertices);

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
    cout << "Graf wczytany z pliku: " << filename << endl;
    graphMatrix.printGraph();
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
