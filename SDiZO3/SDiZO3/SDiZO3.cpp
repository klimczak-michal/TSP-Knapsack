#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <conio.h>
#include <time.h>
#include <iomanip>
#include <string>
#include <algorithm>
#define max 10000
#define maks(a,b) (a > b ? a : b) //max z (a,b), a>b -> return a, else -> return b
#define min(a,b) (a > b ? b : a)
using namespace std;

//------------------------------------------------
//---------------------PLECAK---------------------
//------------------------------------------------
int liczbaprzedmiotow; //rozmiar tablicy z przedmiotami
int pojemnoscplecaka; //max. waga plecaka
int waga[max];		//tablica wag
int wartosc[max];	//tablica wartosci
float wspprzedmiotu[max];	//tablica wspolczynnikow (wartosc/waga)
int i, j;					//indeksy pomocnicze do petli
int tmp[max];				//pomocnicza tablica do przechowywania wektora 0/1 (plecak[max])
int plecak[max]; // if == 1 -> przedmiot w plecaku, == 0 -> nie
float zyskkoncowy = -1;
int wagakoncowa;

//programowanie dynamiczne
float wyniki[max][max] = { 0 };
unsigned int przedmioty[max][max] = { 0 };


//FUNKCJE POMOCNICZE
LARGE_INTEGER startTimer()
{
	LARGE_INTEGER start;
	DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
	QueryPerformanceCounter(&start);
	SetThreadAffinityMask(GetCurrentThread(), oldmask);
	return start;
}

LARGE_INTEGER endTimer()
{
	LARGE_INTEGER stop;
	DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
	QueryPerformanceCounter(&stop);
	SetThreadAffinityMask(GetCurrentThread(), oldmask);
	return stop;
}

LARGE_INTEGER performanceCountStart, performanceCountEnd;
LARGE_INTEGER freq;

//ZEROWANIE ZMIENNYCH WYKORZYSTYWANYCH WIELOKROTNIE W KAZDYM Z ALGORYTMOW
void wyczysc()
{
	wagakoncowa = 0;
	zyskkoncowy = 0;
	for (int i = 0; i < max; i++)
	{
		plecak[i] = tmp[i] = 0;
		for (int j = 0; j < max; j++)
		{
			wyniki[i][j] = przedmioty[i][j] = 0;
		}
	}
	//---------------------------------------------------------------------
}

//ODCZYT Z PLIKU
void zpliku(string nazwa)
{
	//W razie gdyby wykonywano wiele algorytmow
	liczbaprzedmiotow = pojemnoscplecaka = wagakoncowa = i = j = 0;
	zyskkoncowy = 0;
	for (int i = 0; i < max; i++)
	{
		waga[i] = wartosc[i] = plecak[i] = tmp[i] = wspprzedmiotu[i] = 0;
		for (int j = 0; j < max; j++)
		{
			wyniki[i][j] = przedmioty[i][j] = 0;
		}
	}
	//---------------------------------------------------------------------
	fstream plik;

	plik.open(nazwa, std::ios::in);
	int i;
	plik >> liczbaprzedmiotow;
	for (i = 0; i < liczbaprzedmiotow; i++)
	{
		plik >> waga[i];
		plik >> wartosc[i];
	}
	plik.close();
	cout << "\nDane z pliku wczytano.";
	cout << "\nPojemnosc plecaka: ";
	cin >> pojemnoscplecaka;
	system("cls");
}

//WYNIK KONCOWY
void wyswietl() //wynik koncowy
{
	cout << "Zawartosc plecaka: " << endl << endl;
	cout << "Waga\t Wartosc\t Wspolczynnik" << endl;

	int wynik = 0;
	int wagak = 0;

	for (i = 0; i < liczbaprzedmiotow; i++)
		if (plecak[i] == 1) cout << waga[i] << "\t" << wartosc[i] << "\t\t" << wspprzedmiotu[i] << endl; // == 1 => w plecaku

	for (i = 0; i < liczbaprzedmiotow; i++)
		if (plecak[i] == 1) // to samo co wyzej
		{
		wynik += wartosc[i] * plecak[i];
		wagak += waga[i];
		}

	cout << "\nMaksymalny zysk: " << wynik << ", waga plecaka: " << wagak << endl;
}

//SORTOWANIE BABELKOWE, WG. WSPOLCZYNNIKA
void sortuj() //babelkowe
{
	int a;
	int b;
	float c;
	for (i = 0; i<liczbaprzedmiotow; i++)
		wspprzedmiotu[i] = (float)wartosc[i] / (float)waga[i];
	for (i = 0; i<liczbaprzedmiotow - 1; i++)
	{
		for (j = i + 1; j<liczbaprzedmiotow; j++)
		{
			if (wspprzedmiotu[i]  < wspprzedmiotu[j])
			{
				c = wspprzedmiotu[i];
				wspprzedmiotu[i] = wspprzedmiotu[j];
				wspprzedmiotu[j] = c;
				a = wartosc[i];
				wartosc[i] = wartosc[j];
				wartosc[j] = a;
				b = waga[i];
				waga[i] = waga[j];
				waga[j] = b;
			}
		}
	}
}

//WYSWIETLANIE WCZYTANEGO/WYGENEROWANEGO PROBLEMU DO ROZWIAZANIA
void struktura()
{
	if (liczbaprzedmiotow == 0) cout << "Brak przedmiotow do wyswietlenia" << endl;
	else
	{
		cout << "Pojemnosc plecaka: " << pojemnoscplecaka << endl;
		cout << "Liczba przedmiotow: " << liczbaprzedmiotow << endl;
		cout << "Waga \t Wartosc \t" << endl;

		for (int i = 0; i < liczbaprzedmiotow; i++)
		{
			cout << waga[i] << "\t" << wartosc[i] << endl;
		}
	}
	_getch();
	system("cls");
}

//PROGRAMOWANIE DYNAMICZNE
void dynamiczne(int ileprzedmiotow, int rozmiar)
{
	int i, j;

	for (i = 1; i <= ileprzedmiotow; i++){ //i - indeksy przedmiotow
		for (j = 0; j <= rozmiar; j++){ //j - wagi przedmiotow
			if (waga[i - 1] <= j){ //jezeli przedmiot i-1 miesci sie w plecaku o wadze j 
				wyniki[i][j] = maks(wyniki[i - 1][j], wartosc[i - 1] + wyniki[i - 1][j - waga[i - 1]]); //do tablicy wynikowej wpisywany jest max(wynik z podproblemu, wynik ze sprawdzanym przedmiotem)
				if (wartosc[i - 1] + wyniki[i - 1][j - waga[i - 1]]>wyniki[i - 1][j]) //wynik lepszy z przedmiotem
					przedmioty[i][j] = 1; //przedmiot w plecaku
				else
					przedmioty[i][j] = 0;
			}
			else{ //przedmiot i-1 nie miesci sie w plecaku
				przedmioty[i][j] = 0;
				wyniki[i][j] = wyniki[i - 1][j]; //przypisywana jest wartosc z podproblemu o indeksie przedmiotu - 1
			}
		}
	}
}

void wyswietldynamiczne() //wybor - dla ktorego algorytmu zostalo wywolane wyswietlanie
{
	cout << "Zawartosc plecaka: " << endl << endl;
	cout << "Waga\t Wartosc\t Wspolczynnik\t" << endl;

	for (int i = 0; i < liczbaprzedmiotow; i++)
		wspprzedmiotu[i] = (float)wartosc[i] / (float)waga[i];

	int tempwaga = pojemnoscplecaka;
	int indeks = liczbaprzedmiotow;
	int wagak = 0;
	zyskkoncowy = wyniki[liczbaprzedmiotow][pojemnoscplecaka];
	while (indeks>0){ //zaczynamy od ostatniego indeksu i idziemy w gore
		if (przedmioty[indeks][tempwaga] == 1){ // == 1 => w plecaku
			cout << waga[indeks - 1] << "\t" << wartosc[indeks - 1] << "\t\t" << wspprzedmiotu[indeks - 1] << endl;
			wagak += waga[indeks - 1];
			tempwaga -= waga[indeks - 1]; //'waga przedmiotu' kolumn w lewo
			indeks--; //jeden wiersz w gore
		}
		else{ //przedmiot poza plecakiem
			indeks--; //jeden wiersz w gore
		}
	}
	cout << "\nMaksymalny zysk: " << zyskkoncowy << ", waga plecaka: " << wagak << endl;
}


//ALGORYTM ZACH£ANNY
void zachlanny(int ileprzedmiotow, int rozmiar)
{
	int aktwaga = 0;

	for (int i = 0; i < ileprzedmiotow; i++)
	{
		if (waga[i] + aktwaga <= rozmiar)
		{
			plecak[i] = 1;
			aktwaga += waga[i];
		}
	}
}
//---------------------------------------------------------------------------------------

//PRZEGLAD ZUPELNY
//binarnie, w 1. petli k==1 przedmiot - w plecaku, w 2. petli k = indeks przedmiotu
//indeksy przedmiotow odpowiadaja indeksom wektora o wartosciach 0,1 np: [i=9, 8, 7, ... , 1, 0]
//sprawdzane sa kolejne wektory od [0, 0, ... , 0, 1] az do [1, 1, ... , 1, 1]
//maksymalne wartosci sa przechowywane w temp(Weight/Value)
//po zakonczeniu pracy w glownej petli ostateczny wynik (wektor) jest przypisywany do tablicy plecak[] (wykorzystywanej przy wyswietlaniu)
void przegladzupelny()
{
	int *A = (int *)calloc(liczbaprzedmiotow, sizeof(int));
	int i;

	for (i = 0;; i++) {
		int j = liczbaprzedmiotow;
		int tempWeight = 0;
		int tempValue = 0;
		int k;

		k = 1;
		for (j = 0; j < liczbaprzedmiotow; j++) {
			A[j] += k;

			k = A[j] / 2;
			A[j] = A[j] % 2;
		}
		if (k) break;

		for (k = 0; k < liczbaprzedmiotow; k++) {
			if (A[k] == 1) {
				tempWeight = tempWeight + waga[k];
				tempValue = tempValue + wartosc[k];
			}
		}

		if (tempValue > zyskkoncowy && tempWeight <= pojemnoscplecaka) {
			zyskkoncowy = tempValue;
			for (int i = 0; i < liczbaprzedmiotow; i++) plecak[i] = A[i];
		}
	}

	free(A); //zwolnienie pamieci

	for (int i = 0; i < liczbaprzedmiotow; i++)
		wspprzedmiotu[i] = (float)wartosc[i] / (float)waga[i]; //wspolczynniki wykorzystywane przy wyswietlaniu
}
//---------------------------------------------------------------

//GENERACJA LOSOWA PROBLEMU PLECAKOWEGO O ZADANYCH WARTOSCIACH
void random()
{
	//W razie gdyby wykonywano wiele algorytmow
	wagakoncowa = i = j = 0;
	zyskkoncowy = 0;
	for (int i = 0; i < max; i++)
	{
		waga[i] = wartosc[i] = plecak[i] = tmp[i] = wspprzedmiotu[i] = 0;
		for (int j = 0; j < max; j++)
		{
			wyniki[i][j] = przedmioty[i][j] = 0;
		}
	}
	//---------------------------------------------------------------------
	srand(time(NULL));


	for (int i = 0; i < liczbaprzedmiotow; i++)
	{
		wartosc[i] = (rand() % (int)(0.75*pojemnoscplecaka)) + (pojemnoscplecaka - (0.75 * pojemnoscplecaka));
		do {
			waga[i] = (rand() % (int)(pojemnoscplecaka / 2)) + 1;
		} while (waga[i] > wartosc[i]);
	}
}
//---------------------------------------------------------


//------------------------------------------------
//----------------------TSP-----------------------
//------------------------------------------------
int** miasta;
int* droga;
int odleglosc;
int rozmiar;

void TSPzpliku(string nazwa)
{
	fstream plik;
	plik.open(nazwa);
	if (plik.is_open() == false)
	{
		cout << "Nie udalo sie otworzyc pliku!" << endl;
		return;
	}

	plik >> rozmiar;

	droga = new int[rozmiar];
	miasta = new int*[rozmiar];

	for (int i = 0; i < rozmiar; i++)
		miasta[i] = new int[rozmiar]; // Tworzenie wierszy

	// wype³nienie macierzy
	for (int i = 0; i < rozmiar; i++)
		for (int j = 0; j < rozmiar; j++) plik >> miasta[i][j];

	plik.close();
	system("cls");
}

void TSPrandom(int rozmiar)
{
	droga = new int[rozmiar];
	miasta = new int*[rozmiar];
	odleglosc = 0;

	for (int i = 0; i < rozmiar; i++)
		miasta[i] = new int[rozmiar]; // Tworzenie wierszy

	// wype³nienie macierzy
	for (int i = 0; i < rozmiar; i++)
		for (int j = 0; j < rozmiar; j++)
		{
		if (i != j) miasta[i][j] = rand() % 20 + 1;
		else miasta[i][j] = 0;
		}

	//system("cls");
}

void TSPwyswietl()
{
	cout << "   ";
	for (int i = 0; i < rozmiar; i++) cout << setw(3) << i;
	cout << endl << endl;
	for (int i = 0; i < rozmiar; i++)
	{
		cout << setw(3) << i;
		for (int j = 0; j < rozmiar; j++) cout << setw(3) << (int)miasta[i][j];
		cout << endl;
	}

	_getch();
	system("cls");
}

int TSPdroga(int* droga)
{
	int wynik = 0;

	for (int i = 0; i < rozmiar; i++)
	{
		if (i == rozmiar - 1)
		{
			wynik += miasta[droga[i]][droga[0]];
		}
		else
		{
			wynik += miasta[droga[i]][droga[i + 1]];
		}
	}

	return wynik;
}

void TSPwynik()
{
	odleglosc = TSPdroga(droga);

	for (int i = 0; i < rozmiar; i++)
	{
		if (i == rozmiar - 1)
		{
			cout << droga[i] << "->" << droga[0];
		}
		else
		{
			cout << droga[i] << "->";
		}
	}

	cout << endl << "Odleglosc: " << odleglosc;
}

void TSPbruteForce(int rozmiar, int* droga)
{
	int tmp = 0;
	int* drogawynikowa = new int[rozmiar];

	for (int i = 0; i < rozmiar; i++)
	{
		droga[i] = i;
	}

	odleglosc = INT_MAX;

	while (true)
	{

		tmp = TSPdroga(droga);

		if (odleglosc > tmp)
		{
			odleglosc = tmp;
			for (int i = 0; i < rozmiar; i++)
			{
				drogawynikowa[i] = droga[i];
			}
		}

		int i = rozmiar - 2;
		for (; i >= 0; --i) {
			if (droga[i] < droga[i + 1])
				break;
		}
		if (i < 0)
			break;
		
		int j = rozmiar - 1;
		for (;; --j) {
			if (droga[i] < droga[j])
				break;
		}
		
		swap(droga[i], droga[j]);
		
		reverse(droga + i + 1, droga + rozmiar);
	}

	for (int i = 0; i < rozmiar; i++)
	{
		droga[i] = drogawynikowa[i];
	}

}

bool TSPvisited(int indeks, int* droga, int rozmiar)
{
	for (int i = 0; i < rozmiar; i++)
	{
		if (droga[i] == indeks) return true;
	}

	return false;
}

void TSPgreedy(int rozmiar, int* droga, int start)
{
	int indeks = 0, minOdleglosc = INT_MAX;
	int* tmpDroga = new int[rozmiar];

	odleglosc = 0;

	tmpDroga[0] = start; //wybranie wierzcho³ka startowego

	for (int i = 1; i < rozmiar; i++) //reszta trasy na razie nieustalona, wiêc indeksy = -1
	{
		tmpDroga[i] = -1;
	}

	do
	{
		for (int i = 0; i < rozmiar; i++)
		{
			if (TSPvisited(i, tmpDroga, rozmiar) == false && minOdleglosc > miasta[tmpDroga[indeks]][i] && tmpDroga[indeks] != i)
			{
				minOdleglosc = miasta[tmpDroga[indeks]][i];
				tmpDroga[indeks + 1] = i;
			}
		}

		minOdleglosc = INT_MAX;
		indeks++;

	} while (indeks < rozmiar);

	for (int i = 0; i < rozmiar; i++)
	{
		droga[i] = tmpDroga[i];
	}
}

void menuPlecak()
{
	string nazwa;
	int wybor;
	do
	{
		cout << "1 - Wczytaj z pliku \n2 - Wyswietl strukture\n3 - Dynamiczne\n4 - Przeglad zupelny\n5 - Algorytm zachalnny\n6 - Generacja losowa\n7 - Koniec" << endl;
		cin >> wybor;
		system("cls");
		switch (wybor)
		{
		case 1: {
			string nazwa;
			cout << "Podaj nazwe pliku: ";
			cin >> nazwa;
			zpliku(nazwa);
			break;
		}
		case 2: struktura(); break;
		case 3: {
			wyczysc();
			QueryPerformanceFrequency(&freq); //pobieranie czêstotliwoœci pracy procesora
			performanceCountStart = startTimer(); //czas start
			dynamiczne(liczbaprzedmiotow, pojemnoscplecaka);
			performanceCountEnd = endTimer(); //czas stop
			double tm = (performanceCountEnd.QuadPart - performanceCountStart.QuadPart); //obliczenie wyniku
			cout << "\nZrobione! Czas wykonania: " << ((tm / freq.QuadPart * 1000) * 1000) << " mikrosekund" << endl;
			wyswietldynamiczne();
			_getch();
			system("cls");
			break; }
		case 4: {
			wyczysc();
			QueryPerformanceFrequency(&freq); //pobieranie czêstotliwoœci pracy procesora
			performanceCountStart = startTimer(); //czas start
			przegladzupelny();
			performanceCountEnd = endTimer(); //czas stop
			double tm = (performanceCountEnd.QuadPart - performanceCountStart.QuadPart); //obliczenie wyniku
			cout << "\nZrobione! Czas wykonania: " << ((tm / freq.QuadPart * 1000) * 1000) << " mikrosekund" << endl;
			wyswietl();
			_getch();
			system("cls");
			break; }
		case 5:{
			wyczysc();
			QueryPerformanceFrequency(&freq); //pobieranie czêstotliwoœci pracy procesora
			performanceCountStart = startTimer(); //czas start
			sortuj();
			zachlanny(liczbaprzedmiotow, pojemnoscplecaka);
			performanceCountEnd = endTimer(); //czas stop
			double tm = (performanceCountEnd.QuadPart - performanceCountStart.QuadPart); //obliczenie wyniku
			cout << "\nZrobione! Czas wykonania: " << ((tm / freq.QuadPart * 1000) * 1000) << " mikrosekund" << endl;
			wyswietl();
			_getch();
			system("cls");
			break; }
		case 6: {
			wyczysc();
			cout << "Podaj ilosc przedmiotow: ";
			cin >> liczbaprzedmiotow;
			cout << endl;

			cout << "Podaj wage plecaka: ";
			cin >> pojemnoscplecaka;
			cout << endl;

			int tmp = 0;
			do
			{
				random();
				tmp = 0;
				for (int i = 0; i < liczbaprzedmiotow; i++) tmp += waga[i];
			} while (tmp <= pojemnoscplecaka);

			//_getch();
			//system("cls");
			break; }
		case 7: break;
		default: cout << "Podano zly znak" << endl; break;
		}
	} while (wybor != 7);
}

void menuTSP()
{
	string nazwa;
	int wybor;
	do
	{
		cout << "1 - Wczytaj z pliku \n2 - Wyswietl strukture\n3 - Przeglad zupelny\n4 - Algorytm zachalnny\n5 - Generacja losowa\n6 - Koniec" << endl;
		cin >> wybor;
		system("cls");
		switch (wybor)
		{
		case 1: {
			string nazwa;
			cout << "Podaj nazwe pliku: ";
			cin >> nazwa;
			TSPzpliku(nazwa);
			break;
		}
		case 2: TSPwyswietl(); break;
		case 3: {
			QueryPerformanceFrequency(&freq); //pobieranie czêstotliwoœci pracy procesora
			performanceCountStart = startTimer(); //czas start
			TSPbruteForce(rozmiar, droga);
			performanceCountEnd = endTimer(); //czas stop
			double tm = (performanceCountEnd.QuadPart - performanceCountStart.QuadPart); //obliczenie wyniku
			cout << "\nZrobione! Czas wykonania: " << ((tm / freq.QuadPart * 1000) * 1000) << " mikrosekund" << endl;
			TSPwynik();
			_getch();
			system("cls");
			break; }
		case 4: {
			int start = rand() % rozmiar;
			QueryPerformanceFrequency(&freq); //pobieranie czêstotliwoœci pracy procesora
			performanceCountStart = startTimer(); //czas start
			TSPgreedy(rozmiar, droga, start);
			performanceCountEnd = endTimer(); //czas stop
			double tm = (performanceCountEnd.QuadPart - performanceCountStart.QuadPart); //obliczenie wyniku
			cout << "\nZrobione! Czas wykonania: " << ((tm / freq.QuadPart * 1000) * 1000) << " mikrosekund" << endl;
			TSPwynik();
			_getch();
			system("cls");
			break; }
		case 5: {
			cout << "Podaj ilosc miast: ";
			cin >> rozmiar;
			cout << endl;

			TSPrandom(rozmiar);

			//_getch();
			//system("cls");
			break; }
		case 6: break;
		default: cout << "Podano zly znak" << endl; break;
		}
	} while (wybor != 6);

	delete droga;
	droga = 0;

	for (int i = 0; i < rozmiar; i++)
	{
		delete miasta[i];
		miasta[i] = 0;
	}
	delete miasta;
	miasta = 0;
}

void menu()
{
	int wybor = 0;
	do
	{
		cout << "1 - Problem Plecakowy \n2 - Problem komiwojazera\n3 - Koniec" << endl;
		cin >> wybor;
		system("cls");

		switch (wybor)
		{
		case 1:
		{
			menuPlecak();
			break;
		}
		case 2:
		{
			menuTSP();
			break;
		}
		case 3: break;
		default: { cout << "Podano zly znak" << endl; system("cls"); break; }
		}

	} while (wybor != 3);
}

//MENU PROGRAMU
int main()
{
	srand(time(NULL));

	menu();

}