#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <time.h>
#include <iostream>
#include <bits/stdc++.h> 

using namespace std;

#define N_COT 9
using namespace std;
// for string delimiter
vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

vector<double> readByColumn(int pos) {
    ifstream fin;
    string line1;
    string line2;
    vector<double> result;
    // Open an existing file
    fin.open("diabetes.csv");
    fin>>line1;
    while(!fin.eof()){
       fin>>line2;
       vector<string> temp = split(line2, ',');
       result.push_back(atof(temp[pos-1].c_str()));
       //atof(num.c_str())
    }
    return result;
}

double sigmoid(double x){
    return 1/(1+exp(-x));
}

double cost(double y_pre, double y){
    return -(y*log(y_pre) + (1 - y)*log(1 - y_pre));
}

double y_predict(vector<double> w,vector<double> x){
    if (w.size() == x.size()){
        double y_pre = 0;
        int i;
        for (i =0; i < w.size(); i++){
            y_pre += w[i]*x[i];
        }
        return y_pre;
    }else
    {
        cout << "Size w and x different" << endl;
        return false;
    }
    
}
int main()
{
    // dọc cot nao thi truyen vao so thu tu cot do, tinh tu 1
    vector<vector<double>> data ;
    vector<double> w ,deta_w, x0 ,x;
    int i, j, k;


    for (i=0; i<N_COT; i++){
        data.push_back(readByColumn(i+1));
    }
    for (i=0; i<data[0].size(); i++){
        x0.push_back(1);
    }
    data.insert(data.begin(),x0);
    for (i=0; i<N_COT; i++){
        double r = (double) rand()/RAND_MAX;
        w.push_back(r);
        deta_w.push_back(0);
        x.push_back(0);
    }
    // cout << w.size() << endl;
    // cout << data[0].size() << endl;
    int numOfIteration = 10; 
    double learning_rate = 0.01;
    double y_pre, y, loss;
    
    for (i=0; i<numOfIteration; i++){
        loss = 0;
        for (k=0; k<=N_COT; k++){
                deta_w[k] = 0;
        }
        for (j =0; j < data[0].size(); j++){
            
            for (k=0; k<N_COT; k++){
                x[k] = data[k][j];
            }
            // for (auto a : x) cout << a << ';';
            // cout << endl;
            // for (auto a : w) cout << a << ';';
            // cout << endl;
            y_pre = sigmoid(y_predict(w,x));
            y = data[N_COT][j];
            loss += cost(y_pre, y);
            // cout << y_predict(w,x) << endl;
            // cout << y_pre << endl;
            // cout << y << endl;
            // cout << loss << endl;
            for (k=0; k<=N_COT; k++){
                deta_w[k] += (y_pre - y)*x[k];
            }
        }

        // loss = loss/data[0].size();
        cout << loss << endl;
        
        for (k=0; k<=N_COT; k++){
            w[k] = w[k] - learning_rate*deta_w[k];
        }
        
    }
    
    // for (auto i : data)
    //     for (auto j : i) cout << j << endl;

    return 0;
}