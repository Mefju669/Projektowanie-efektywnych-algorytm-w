#include <iostream>
#include <algorithm>
#include "Algorytmy.h"
#include "GraphMatrix.h"

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

            bruteForce();
            outputFile << ";" << Algorytmy::czas;

            bool flagaBledu = false;
            vector<int> cyklTymczasowy = Algorytmy::cyklHamiltona;

            for (int k = 0; k < cyklTymczasowy.size(); k++)
            {
                if (cyklTymczasowy[k] != cyklePoprawne[i][k])
                    flagaBledu = true;
            }

            if (koszty[i] != Algorytmy::koszt || flagaBledu == true)
            {
                cout << "Obliczony koszt lub cykl Hamiltona sie nie zgadza" << endl;
            }
        }
        outputFile << endl;
    }

    outputFile.close();
}

void Algorytmy::bruteForce() //algorytm brute force + pomiar czasu + wyniki działania w konsoli
{
    zacznijLiczyc();

    vector<vector<int>> adjacencyMatrix = graphMatrix.adjacencyMatrix; //dwuwymiarowy vector inicjowany jako kopia macierzy
    //sąsiedztwa przechowywanej w obiekcie graphMatrix
    int numVertices = graphMatrix.numVertices; //zmienna przechowująca ilość wierzchołków w grafie, także pobierana z obiektu graphMatrix
    vector<int> permutation(numVertices); //vector liczb całkowitych którzy zostaje utworzony z rozmiarem równym liczbie wierzchołków w grafie
    //ten wektor będzie używany do przechowywania permutacji wierzchołków
    int minCost = INT_MAX; //zmienna przechowująca koszt najlepszego znalezionego cyklu Hamiltona
    vector<int> minTour; //vector przechowujący najlepszą znalezioną permutację wierzchołków reprezentującą cykl Hamiltona o najniższym koszcie

    for (int i = 0; i < numVertices; i++) //pęt,a for która inicjuje vector permutation początkową permutacją, gdzie wierzchołki mają kolejne numery od 0 do numVertices-1
    {
        permutation[i] = i;
    }

    do //pętla do while która do póki istnieją kolejne permutacje wierzchołków danego grafu sprawdza je wszystkie aby ustalić najmniej kosztowny cykl Hamiltona
    {
        int currentCost = 0;
        bool isValidTour = true;

        for (int i = 0; i < numVertices - 1; i++) //w pętli for algorytm iteruje przez wszystkie pary sąsiadujących wierzchołków  w obecnej permutacji.


        {
            int source = permutation[i];
            int destination = permutation[i + 1];

            int edgeWeight = graphMatrix.adjacencyMatrix[source][destination];

            if (edgeWeight == -1) //dla każdej pary wierzchołków algorytm sprawdza czy między nimi istnieje krawędź tzn czy edgeWeight nie jest równej -1.
            {
                isValidTour = false; //jeśli krawędź nie istnieje ustawia flagę isValidTour na false co oznacza że obecna permutacja nie jest poprawnym cyklem Hamiltona
                break;
            }

            currentCost += edgeWeight;
        }

        int lastSource = permutation[numVertices - 1];
        int firstDestination = permutation[0];
        int edgeWeight = graphMatrix.adjacencyMatrix[lastSource][firstDestination];
             //algorytm sprawdza czy istnieje krawędź między ostatnum za pierwszym wierzchołkiem
        if (edgeWeight == -1) //jeśli nie ustawia flagę isValidTour na false aby odrzucić ten potencjalny cykl, bo nim nie jest
        {
            isValidTour = false;
        }
        else //jeśli tak dodaje jej wagę do dotychczasowego kosztu.
        {
            currentCost += edgeWeight;
        }

        if (isValidTour && currentCost < minCost) //jeśli znaleziony cykl jest cyklem Hamiltona krótszym niż najkrótszy dotychczas
        //znaleziony to minTour jest aktualizowane na aktualną permutację, a koszt na koszt tego cyklu Hamiltona
        {
            minCost = currentCost;
            minTour = permutation;
        }
    } while (next_permutation(permutation.begin(), permutation.end())); //pętla do while sprawdza czy istnieje kolejna permutacja do sprawdzenia.
    //jeśli tak to vector permutation zostaje zaktualizowany na kolejną permutację i następuje przejście pętli od nowa, jeśli nie algorytm brute force się kończy
    //a program przechodzi do dalszych operacji

    skonczLiczyc();

    int wyn;

    if(numVertices<=8)
    {
        wyn = duration_cast<nanoseconds>(Algorytmy::czasKoncowy - Algorytmy::czasPoczatkowy).count();
    }

    else if (numVertices <=10)
    {
        wyn = duration_cast<milliseconds>(Algorytmy::czasKoncowy - Algorytmy::czasPoczatkowy).count();
    }
    else if(numVertices>10)
    {
        wyn = duration_cast<seconds>(Algorytmy::czasKoncowy - Algorytmy::czasPoczatkowy).count();
    }

    Algorytmy::czas = wyn;
    Algorytmy::cyklHamiltona = minTour;
    Algorytmy::koszt = minCost;

    if (minTour.empty())
    {
        cout << "Nie znaleziono cyklu hamiltona." << endl;
    }
    else
    {
        cout << "Cykl hamiltona: ";
        for (int i = 0; i < numVertices; i++)
        {
            cout << minTour[i] << " ";
        }
        cout << minTour[0] << endl;
        cout << "Koszt: " << minCost << endl;
    }

    cout << "Czas: " << wyn  << endl;
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
