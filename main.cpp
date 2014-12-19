#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "customer.h"

#define P 3
#define POPSIZE 8
#define GEN 13
#define M 1

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

void Init_Chromosome(int *t,int customers)
{

    for(int i=0; i<customers; i++)
        t[i]=i;

    int x=customers;

    for(int i=1; i<customers*P; i++)
    {
        int z=0,y=0;

        while(z==0 || y==0)
        {
            y=rand()%x;
            z=rand()%x;
        }
        swap(t[y],t[z]);
    }


}

double cost_calculator(int *tab, int customers, int capacity)
{   //CVRPTW
    double cost=0;
    double actual_time=0;
    int actual_capacity=capacity;
    int actual_place=0;
    int destination=tab[1];
    int executed=1;
    int vehicle=0;
    //fstream plik;
    //plik.open( "Out.txt", std::ios::out );
   //plik.setf(cout.fixed);

    while(executed!=(customers))
        {
            if(avalible(actual_place,actual_time,actual_capacity,destination)) // jesli jest mozliwy dojazd z punktu a do b to jedz
            {
                if(odleglosc(actual_place,destination)+actual_time>client[destination]->READY_TIME)
                {
                    actual_time+=odleglosc(actual_place,destination)+client[destination]->SERVICE_TIME;
                }
                else
                {
                    actual_time=client[destination]->READY_TIME+client[destination]->SERVICE_TIME;
                }

                actual_capacity-=client[destination]->DEMAND;
                //plik << destination << " ";
                actual_place=destination;
            }
            else // w przeciwnym wypadku jedz z a do 0
            {
                cost+=actual_time+odleglosc(actual_place,0);
                //cout << actual_time+odleglosc(actual_place,0) << endl;
                actual_time=0;
                actual_capacity=capacity;
                vehicle++;
                destination=0;
                actual_place=0;
                //plik << endl;
            }

            if (destination!=0) executed++;
            destination=tab[executed];


        }
        //cout << "ostatni klient: " << actual_place << endl;
        cost+=actual_time+odleglosc(actual_place,0);
        vehicle++;
        cout.precision(5);

        //cout << endl << "samochody: " << vehicle << endl << fixed;
        //plik.close();

    return cost;
}

bool contains(int *t, int x, int amount)
{
    for(int i=1; i<amount; i++)
        if(t[i]==x)
        return 1;

        return 0;

}

void mutation(int *t, int *c, int customers)
{
    int x = customers, a=0, b=0;
    while(a==b || a==0 || b==0)
    {
        a=rand()%x;
        b=rand()%x;
    }
    swap(t[a],t[b]);
    for(int i=0; i<customers; i++)
        c[i]=t[i];
}

void crossover(int *chromosome1, int *chromosome2, int *c1, int *c2, int customers)
{
    int i,j;
    i=rand()%customers;
    j=rand()%customers;
    while(i==0 || j==0 || (i==1 && j==customers-1) || (i==customers-1 && j==1))
    {
        i=rand()%customers;
        j=rand()%customers;
    }

    if(i>j)
        swap(i,j);

    int child1[customers],child2[customers];

    for(int x=0; x<customers; x++)
        child1[x]=child2[x]=0;


    for(int x=i; x<=j; x++)
    {
        child1[x]=chromosome1[x];
        child2[x]=chromosome2[x];
    }

    int actual_child=child1[j];
    int index;
    for(int y=1; y<customers; y++)
        if(chromosome2[y]==actual_child)
        index=y;
    index++;
    int x=j+1;


    while(true)
    {

        while(true)
        {
        if(!contains(child1,(chromosome2[index]),customers) && index<customers)
        {
            child1[x]=chromosome2[index];
            break;
        }
        else if (index<customers)
            index++;
        else
            index=1;
  //      cout << i << " " << j << endl;
        }

        if(x>=customers-1)
            x=0;
        if(x==i-1)
            break;
        x++;


    }


    actual_child=child2[j];
    for(int y=1; y<customers; y++)
        if(chromosome1[y]==actual_child)
        index=y+1;
        x=j+1;


        while(true)
    {
        while(true)
        if(!contains(child2,(chromosome1[index]),customers) && index<customers)
        {
            child2[x]=chromosome1[index];
            break;
        }
        else if (index<customers)
            index++;
        else
            index=1;

        if(x>=customers-1)
            x=0;
        if(x==i-1)
            break;
        x++;

    }


    for(int x=0; x<customers; x++)
    {
        c1[x]=child1[x];
    }
    for(int x=0; x<customers; x++)
        c2[x]=child2[x];
}

void show_results(int *tab, int customers, int capacity)
{   //CVRPTW
    double cost=0;
    double actual_time=0;
    int actual_capacity=capacity;
    int actual_place=0;
    int destination=tab[1];
    int executed=1;
    int vehicle=0;
    fstream plik;
    plik.open( "Out.txt", std::ios::out );
    plik.setf(cout.fixed);
    vector<int> wyniki;
    int rozmiar=0;

    while(executed!=(customers))
        {
            if(avalible(actual_place,actual_time,actual_capacity,destination)) // jesli jest mozliwy dojazd z punktu a do b to jedz
            {
                if(odleglosc(actual_place,destination)+actual_time>client[destination]->READY_TIME)
                {
                    actual_time+=odleglosc(actual_place,destination)+client[destination]->SERVICE_TIME;
                }
                else
                {
                    actual_time=client[destination]->READY_TIME+client[destination]->SERVICE_TIME;
                }

                actual_capacity-=client[destination]->DEMAND;
                wyniki.push_back(destination);
                rozmiar++;
                //plik << destination << " ";
                actual_place=destination;
            }
            else // w przeciwnym wypadku jedz z a do 0
            {
                cost+=actual_time+odleglosc(actual_place,0);
                //cout << actual_time+odleglosc(actual_place,0) << endl;
                actual_time=0;
                actual_capacity=capacity;
                vehicle++;
                destination=0;
                actual_place=0;
                //plik << endl;
                wyniki.push_back(0);
                rozmiar++;
            }

            if (destination!=0) executed++;
            destination=tab[executed];


        }
        //cout << "ostatni klient: " << actual_place << endl;
        cost+=actual_time+odleglosc(actual_place,0);
        vehicle++;
        cout.setf( ios::fixed );
        cout.precision(5);
        //cout << vehicle << " " << cost << endl << fixed;
        plik << vehicle << " " << cost << endl;
        for(int i=0; i<rozmiar; i++)
            if(wyniki[i]==0)
            plik << endl;
        else
            plik << wyniki[i] << " ";

        //cout << endl << "samochody: " << vehicle << endl << fixed;
        plik.close();

    //return cost;
}

int main()
{

    int customers=wczytaj_plik("C101.txt");



    bool straznik=1; //  straznik sprawdza czy da sie dojechac do wszystkich
                 // klientow wysylajac po jednej ciezarowce na klienta

    for(int i=1; i<customers; i++)
    {
        if (!(avalible(0,0,pojemnosc,i)))
        {
            straznik=0;
            break;
        }
    }


    if(straznik){

    if(customers==3)
    {
        int pop[3];
        pop[1]=1;
        pop[2]=2;
        show_results(pop,3,pojemnosc);
    }
    else if(customers==2)
    {
        int pop[2];
        pop[1]=1;
        show_results(pop,2,pojemnosc);
    }
    else //glowna funkcja
    {
    int **population = new int *[POPSIZE+GEN+2];
    for (int i = 0; i<POPSIZE+GEN+2; i++)
        population[i] = new int [customers];

    int i,j,a=360/POPSIZE,rl,s=0,t=0;
    srand(time(NULL));
    double the_best_cost;
    int the_best_population[customers];
    int nothing=0;

//////////////////////////////////////////////////////////////

    for(i=0;i<POPSIZE;i++)     // stworz pierwszy gatunek
        Init_Chromosome(population[i],customers);

    int generation=1;
    time_t start=clock();
    while(generation++ && (clock()<start+180*CLOCKS_PER_SEC))
    {

    for(i=POPSIZE; i<POPSIZE+M; i++) //mutacja
    {
        rl=rand()%360;
        for(j=1;j<POPSIZE;j++)
            if(floor(rl/a)==j-1)
                s=j-1;
        mutation(population[s],population[i],customers);
    }

    for(i=POPSIZE+M; i<POPSIZE+GEN; i++) //krzyzuj
    {
        do
        {
            rl=rand()%360;
            for(j=1;j<POPSIZE;j++)
                if(floor(rl/a)==j-1)
                    s=j-1;
            rl=rand()%360;
            for(j=1;j<POPSIZE;j++)
                if(floor(rl/a)==j-1)
                    t=j-1;
        }
        while(s==t);
        //cout << "\nS = " << s << ", t = " << t << endl;
        crossover(population[s],population[t], population[i], population[i++], customers);
    }


    double best_cost=cost_calculator(population[0],customers,pojemnosc);
    int best=0;

    for(i=0; i<POPSIZE; i++) //selekcja
    {

        for(j=i; j<POPSIZE+GEN; j++)
        {

            if(cost_calculator(population[j],customers,pojemnosc)<best_cost)
            {
                best=j;
                best_cost=cost_calculator(population[j],customers,pojemnosc);
            }

        }
        for(int x=0; x<customers; x++)
            swap(population[i][x],population[best][x]);
    }

    if(generation==2) //przypisanie najmniejszej wartosci
    {
        the_best_cost=cost_calculator(population[0],customers,pojemnosc);
        for(int y=0; y<customers; y++)
            the_best_population[y]=population[0][y];
        //cout << "nowy najlepszy osobnik: " << the_best_cost << endl;
    }

    if(the_best_cost>cost_calculator(population[0],customers,pojemnosc)) //czy najlepsza z danej generacji lepsza od wszystkich poprzednich?
    {
        the_best_cost=cost_calculator(population[0],customers,pojemnosc);
        for(int y=0; y<customers; y++)
            the_best_population[y]=population[0][y];
        nothing=0;
        //cout << "nowy najlepszy osobnik: " << the_best_cost << endl;
    }
    else
        nothing++; //ile razy pojawil progres


    cout.setf(ios::fixed);


    for(i=POPSIZE; i<POPSIZE+GEN; i++)
        for(int x=0; x<customers; x++)
        population[i][x]=0;




    if(nothing==400) // jesli nic sie nie zmienilo w ciagu 40 generacji
        break;
    if(generation==10000) // limit generacji
        break;

    //if(generation%10==0) // wyswietla generacje*10
    //    cout << "generacja x 10: " << (generation/10) << endl;

    }

    for (int i = 0; i<POPSIZE+GEN; i++)
        delete [] population[i];

    delete [] population;
    time_t stop=clock();
    double pomiar=(double)(clock()-start)/CLK_TCK;
    fstream plik;
    plik.open("Time.txt", ios::app | ios::out );
    plik << customers << " " << pomiar << " " << the_best_cost << endl;
    plik.close();
    //cout << pomiar << endl;


    show_results(the_best_population,customers,pojemnosc);
    }//IF ilosc klientow
    //cout << generation << endl;
    }//IF STRAZNIK
    else
    {
    fstream plik;
    plik.open( "Out.txt", std::ios::out );
    plik << "-1";
    plik.close();
    }





    return 0;
}
