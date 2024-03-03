#include <iostream>
#include <algorithm>
#include "Algorytmy.h"
#include "GraphMatrix.h"
#include <functional>
#include <iomanip>
#include <locale>
#include <numeric>
#include <vector>
#include <cmath>
#include <random>
#include <iterator>

using namespace std;
using namespace std::chrono;




void Algorytmy::pelnePomiary(string nazwaIni) //funkcja przeprowadzająca pełne pomiary na podstawie pliku .ini
{

    vector<int> liczbyPowtorzen;
    vector<string> nazwyGrafow;


    ifstream plik(nazwaIni, ios::in);

    if (plik.is_open())
    {
        string linia;
        while (getline(plik, linia))
        {
            istringstream myStream(linia);
            int liczbaPowtorzen;
            string nazwaGrafu;

            if (myStream >> nazwaGrafu >>liczbaPowtorzen)
            {

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

            sAlgorithm();

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






void Algorytmy::sAlgorithm() {

    // Inicjalizacja pomiaru czasu
    zacznijLiczyc();

    // Wywołanie funkcji określającej szacowny koszt początkowy (alg najbliższego sąsiada)
    heurystycznyKosztPoczatkowy();

    // Wczytanie grafu
    int bestCost = graphMatrix.bestCost;
    int numVertices = graphMatrix.numVertices;
    vector<int> bestTour = graphMatrix.bestTour;

    //  Inicjalizacja parametrów algorytmu mrówkowego
    double alfa = 1.0;
    double beta = 3.0;
    double wspParowania = 0.5;
    double zostawionyFeromon = 10;
    int liczbaPowtorzen = 4;
    double feromonyPocz = static_cast<double>(numVertices) / static_cast<double>(bestCost);

    // Macierze przechowujące informacje o feromonach, kolejno feromony które są dodawane (początkowo 0),
    // oraz aktualny poziom feromonów na krawędziach
    vector<vector<double>> dodawaneFeromony(numVertices, vector<double>(numVertices, 0.0));
    vector<vector<double>> vectorFeromonow(numVertices, vector<double>(numVertices, feromonyPocz));

    // Inicjalizacja generatora liczb losowych
    mt19937 rng(random_device{}());

    // Pętla główna algorytmu
    for (int i = 0; i < liczbaPowtorzen; i++) {
        // Pętla po mrówkach w populacji (za każdą iteracją startuje z innego wierzchołka mrówka i przechodzi przez cykl Hamiltona)
        //liczba mrówek to liczba wierzchołków zgodnie z wymaganiami
        for (int a = 0; a < numVertices; a++) {
            int firstVertice = a;
            vector<int> sciezkaMrowki; //vector przechowujący aktualnie przechodzoną przez mrówkę ścieżkę
            sciezkaMrowki.push_back(firstVertice);
            vector<int> nieodwiedzoneWierzcholki;

            // Wypełnienie vectora nieodwiedzonych wierzchołków
            for (int y = 0; y < numVertices; y++) {
                if (y != firstVertice)
                    nieodwiedzoneWierzcholki.push_back(y);
            }

            // Przemieszanie listy nieodwiedzonych wierzchołków
            shuffle(nieodwiedzoneWierzcholki.begin(), nieodwiedzoneWierzcholki.end(), rng);

            // Pętla budująca trasę mrówki
            for (int x = 0; x < numVertices - 1; x++) {
                int currentVertice = sciezkaMrowki.back();
                int nextVertice = -1;

                //suma atrakcyjności dla wszystkich nieodwiedzonych wierzchołków
                double sum = obliczSumeAtrakcyjnosci(nieodwiedzoneWierzcholki, currentVertice, vectorFeromonow, graphMatrix, alfa, beta);


                // wybór losowo kolejnego wierzchołka z prawdopodobieństwe zależnym od jego atrakcyjności
                double losowaWartosc = uniform_real_distribution<double>(0.0, 1.0)(rng);
                double ruletka = 0.0;

                for (int n = 0; n < nieodwiedzoneWierzcholki.size(); n++) {
                    int neighborVertice = nieodwiedzoneWierzcholki[n];
                    double feromon = pow(vectorFeromonow[currentVertice][neighborVertice], alfa);
                    double visibility = 1.0 / graphMatrix.adjacencyMatrix[currentVertice][neighborVertice];

                    double atrakcyjnosc = pow(feromon, alfa) * pow(visibility, beta);

                    double prawdopodobienstwo = atrakcyjnosc / sum;
                    ruletka += prawdopodobienstwo;

                    if (losowaWartosc <= ruletka) {
                        nextVertice = neighborVertice;
                        break;
                    }
                }

                // Obsługa przypadku, gdy nie uda się wybrać wierzchołka wcześniejszym mechanizmem
                if (nextVertice == -1) {
                    nextVertice = nieodwiedzoneWierzcholki[0];
                }

                // Dodanie wybranego wierzchołka do trasy mrówki
                sciezkaMrowki.push_back(nextVertice);
                // Usunięcie wybranego wierzchołka z listy nieodwiedzonych
                nieodwiedzoneWierzcholki.erase(remove(nieodwiedzoneWierzcholki.begin(), nieodwiedzoneWierzcholki.end(), nextVertice), nieodwiedzoneWierzcholki.end());
            }

            // Dodanie punktu początkowego na koniec trasy mrówki
            sciezkaMrowki.push_back(firstVertice);
            // Obliczenie kosztu trasy
            int kosztSciezkiMrowki = kosztSciezki(sciezkaMrowki);

            // Aktualizacja najlepszej trasy, jeśli bieżąca trasa jest lepsza
            if (kosztSciezkiMrowki < bestCost || bestCost == -1) {
                bestCost = kosztSciezkiMrowki;
                bestTour = sciezkaMrowki;
            }

            // Obliczenie ilości feromonów do dodania na trasie mrówki (CAS)
            double feromonyDoDodania = zostawionyFeromon / static_cast<double>(kosztSciezkiMrowki);

            // Aktualizacja macierzy dodawanych feromonów
            for (int r = 0; r < sciezkaMrowki.size() - 1; r++)
                dodawaneFeromony[sciezkaMrowki[r]][sciezkaMrowki[r + 1]] += feromonyDoDodania;
        }

        // Aktualizacja feromonów na podstawie dodanych feromonów i parowania
        for (int i = 0; i < numVertices; i++) {
            for (int k = 0; k < numVertices; k++) {
                vectorFeromonow[i][k] = (vectorFeromonow[i][k] * wspParowania) + dodawaneFeromony[i][k];
                dodawaneFeromony[i][k] = 0.0;
            }
        }
    }

    // Zakończenie pomiaru czasu
    skonczLiczyc();

    // Obliczenie statystyk algorytmu
    Algorytmy::czas = duration_cast<milliseconds>(czasKoncowy - czasPoczatkowy).count();
    Algorytmy::koszt = bestCost;
    Algorytmy::blad = ((static_cast<double>(bestCost) / static_cast<double>(graphMatrix.optymalnyKoszt)) - 1) * 100;

    // Wyświetlenie wyników
    if (bestTour.empty()) {
        cout << "Nie znaleziono cyklu Hamiltona." << endl;
    } else {
        cout << "Cykl Hamiltona: ";
        for (int i = 0; i < numVertices; i++) {
            cout << bestTour[i] << " ";
        }
        cout << bestTour[0] << endl;
        cout << "Koszt:  " << Algorytmy::koszt << endl;
        cout << "Blad:  " << Algorytmy::blad << "%" << endl;
    }

    // Wyświetlenie czasu wykonania algorytmu
    cout << "Czas: " << Algorytmy::czas << " ms" << endl;
}


double Algorytmy::obliczSumeAtrakcyjnosci(const vector<int>& nieodwiedzoneWierzcholki, int currentVertice, const vector<vector<double>>& vectorFeromonow, const GraphMatrix& graphMatrix, double alfa, double beta) {
    double sum = 0.0;

    for (int n = 0; n < nieodwiedzoneWierzcholki.size(); n++) {
        int neighborVertice = nieodwiedzoneWierzcholki[n];
        double feromon = pow(vectorFeromonow[currentVertice][neighborVertice], alfa);
        double visibility = 1.0 / graphMatrix.adjacencyMatrix[currentVertice][neighborVertice];

        double atrakcyjnosc = pow(feromon, alfa) * pow(visibility, beta);

        sum += atrakcyjnosc;
    }

    return sum;
}




//funkcja poboczna licząca koszt zadanej ścieżki

int Algorytmy::kosztSciezki(vector<int> sciezkaMrowki)
{
    int koszt= 0;
    for(int i =0; i < sciezkaMrowki.size()-1; i++)
    {
        koszt = koszt + graphMatrix.adjacencyMatrix[sciezkaMrowki[i]][sciezkaMrowki[i+1]];
    }

    return koszt;
}


//funkcja poboczna licząca szacowany wstępny koszt algorytmem najbliższego sąsiada
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






//funkcja ładująca pojedynczy graf z pliku

void Algorytmy::loadGraphFromFile(const string& filename)
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

