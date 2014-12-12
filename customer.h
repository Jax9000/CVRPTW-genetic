#ifndef CUSTOMER_H
#define CUSTOMER_H

class Customer
{
public:
    int ID;
    int X;
    int Y;
    int DEMAND;
    int READY_TIME;
    int DUE_DATE;
    int SERVICE_TIME;
    bool WYKONANY;

    Customer(int iid, int ix, int iy, int id, int ir, int istop, int iserv);
    Customer();
    ~Customer();

};


#endif // CUSTOMERS_H

