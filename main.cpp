#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "customer.h"

using namespace std;

vector<Customer*> client; //globalny wektor zawierajacy wskaznik na obiekt customer
int marszruty=0,pojemnosc=0,clients=0;

int wczytaj_plik(string file)
{
Customer *customer;
int i,j=0;
string nazwa_instancji,smieci;
fstream plik;

    plik.open(file.c_str(), ios::in);


////////////////////////////////////// czesc naglowkowa ////////////////////////////////////////

    plik >> nazwa_instancji;
    plik >> smieci >> smieci >> smieci; //wczytuje tytuly zmiennych w pliku
    plik >> marszruty >> pojemnosc;
    for(i=0;i<12; i++)
        plik >> smieci; // wczytuje nazwy kolumn

////////////////////////////////////// czesc naglowkowa ////////////////////////////////////////


    i=0;
    int T[10];
    time_t start=clock();
    while( true && (clock()<start+0.4*CLOCKS_PER_SEC))
    {

       for(j=0;j<7; j++)
          plik >> T[j];
              customer = new Customer(T[0],T[1],T[2],T[3],T[4],T[5],T[6]);
         if(T[0]==i)
        {
            client.push_back(customer);
            i++;
        }
        if( !(plik.good()) ) // odczytuj dopoki nie ma EOF
             break;
    }

    plik.close();
    return (i); //zwroc ilosc klientow
}

double odleglosc(int i_1, int i_2) // pomocnicza funkcja mierzaca odleglosc miedzy dwoma punktami
{
    int X1=(client[i_1]->X),Y1=client[i_1]->Y,X2=client[i_2]->X,Y2=client[i_2]->Y;

    //sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))
    return sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));
}

bool avalible(int start,double aktualny_czas,int pojemnosc_ciezarowki, int koniec)
// pomocnicza funkcja sprawdzajaca czy jest mozliwosc dojazdu
{
    // funkcja sprawdza czy klient nie zakonczyl przyjmowania towaru,
    // czy ciezarowka ma odpowiednia ilosc towaru, oraz czy zdazy dojechac przed zamknieciem depotu
    int zamkniecie_depotu=client[0]->DUE_DATE;
    double czas_dojazdu=odleglosc(start,koniec)+(aktualny_czas);
    int gotowosc_klienta=client[koniec]->READY_TIME;
    double dotarcie_do_celu;

    if(czas_dojazdu>gotowosc_klienta)
        dotarcie_do_celu=czas_dojazdu;
    else
        dotarcie_do_celu=gotowosc_klienta;

    if(dotarcie_do_celu<=client[koniec]->DUE_DATE)
    {
        if(dotarcie_do_celu+client[koniec]->SERVICE_TIME+odleglosc(0,koniec)<=zamkniecie_depotu && (pojemnosc_ciezarowki-client[koniec]->DEMAND>=0))
            return 1;
        else
            return 0;
    }
    else
        return 0;

}

vector<int> Init_Chromosome(vector<int> t,int customers)
{
    for(int i=1; i<customers; i++)
        t.push_back(i);
    int x=customers;
    for(int i=1; i<customers; i++)
    {
        int y=0;
        while(y==0)
        y=rand()%x;
        swap(t[y],t[i-1]);
        x--;
    }
    if(t[0]!=0)
        t[customers-1]=t[0];
    return t;
}

int main()
{
    int customers=wczytaj_plik("C101.txt");
    srand(time(NULL));
    vector<int> liczby;
    liczby=Init_Chromosome(liczby,customers);
    for(int i=0; i<customers; i++)
        cout << liczby[i] <<endl;
    return 0;
}
