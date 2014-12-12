#include "customer.h"
#include <iostream>
using namespace std;

Customer::Customer()
{

}

Customer::Customer(int iid, int ix, int iy, int id, int ir, int istop, int iserv)
{
    ID=iid;
    X=ix;
    Y=iy;
    DEMAND=id;
    READY_TIME=ir;
    DUE_DATE=istop;
    SERVICE_TIME=iserv;
    WYKONANY=0;
}

Customer::~Customer()
{
    cout << "destruktor" << endl;
}
