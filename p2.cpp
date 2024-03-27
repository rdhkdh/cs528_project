#include<bits/stdc++.h>
using namespace std;

//global variables
int K=20; //relative deadline
int M=10; //no of FOG nodes
int U=50; //no of users
vector<double> ST(0.5,M); //shared trust
vector<vector<double>> DT( U, vector<double>(M,0.5) ); //direct trust- UxM matrix
vector<double> cost(0,M); //execution cost per unit time

struct user{
    int id;
};

struct task{
    int a; //arrival time
    int p; //execution/processing time
    int d = a+K; //deadline d= a+K
    user owner; //info of user who sent this task
};

struct node{

};

// Function to generate Poisson-distributed values with mean lambda
int generatePoisson(double lambda)  
{
    random_device rd;
    mt19937 gen(rd());
    poisson_distribution<int> poisson(lambda);
    return poisson(gen);
}

void update_ST(int index)
{
    double sum=0;
    for(int i=0;i<U;i++)
    {
        sum += DT[i][index];
    }
    ST[index]= sum/double(U);
}

int main()
{

}