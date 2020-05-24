#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <time.h>
#include <iostream>
#include <bits/stdc++.h> 

#define N_COT 9
using namespace std;

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
    string line2;
    vector<double> result;
    // Open an existing file
    fin.open("diabetes.csv");
    while(!fin.eof()){
       fin>>line2;
       vector<string> temp = split(line2, ',');
       result.push_back(atof(temp[pos-1].c_str()));
       //atof(num.c_str())
    }
    return result;
}

vector<double> convert(vector<string> temp) {
    vector<double> result;
    for (auto i : temp){
        result.push_back(atof(i.c_str()));
        //temp[i] = atof(temp[i].c_str());
    }
    return result;
}

double maxValue(vector<double> item) {
    double max = 0.0;
    for(auto i : item) {
        if(i > max) max = i;
    }
    return max;
}


vector<double> standardVector() {
    vector<double> standard;
    for(int i = 1; i < 9; i++) {
        standard.push_back(maxValue(readByColumn(i)));
    }
    standard.push_back(1);
    return standard;
}

vector<double> standardize(vector<double> standard, vector<double> item) {
    vector<double> result;
    int j = 0;
    for(auto i : item) {
        result.push_back((double) (i/standard[j]));
        j++;
    }
    return result;
}

vector<vector<vector<double>>> makeTrainAndTestData(int test, int train) {
    ifstream fin;
    string line;
    vector<vector<vector<double>>> data;
    fin.open("diabetes.csv");
    // co 684 nhan 1, 1316 nhan 0
    int test_1 = (684/10)*test;
    int test_0 = (1316/10)*test;
    int i = 0, j = 0;
    vector<vector<double>> testData;
    vector<vector<double>> trainData;
    while(!fin.eof()){
       fin>>line;
       vector<double> temp = convert(split(line, ','));
       //temp = convert(temp);
       if(temp[8] != 0) {
            if(i < test_1) {
                testData.push_back(temp);
                i++;
              } else {
                trainData.push_back(temp);
              }
       } else {
           if(j < test_0) {
            testData.push_back(temp);
            j++;
           } else {
            trainData.push_back(temp);
           }
       }
    }
    data.push_back(testData);
    data.push_back(trainData);
    return data;
}

double sigmoid(double x){
    return 1/(1+exp(-x));
}

// tinh chi phi voi cross entropy
double cost(double y_pre, double y){
    return -(y*log(y_pre) + (1 - y)*log(1 - y_pre));
}

// tinh gia tri y du doan theo model
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

// tinh do chinh xac
double accuracy(vector<double> y, vector<double> y_pre){
    int sum = 0;
    if (y.size() == y_pre.size()){
        int i;
        for (i =0; i < y.size(); i++){
            if((y[i] == 0 && y_pre[i] < 0.5) || (y[i] == 1 && y_pre[i] >= 0.5)){
                sum += 1;
            }
        }
        return double(sum)/y.size();
    }else
    {
        cout << "Size y and y_pre different" << endl;
        return 0;
    } 
}

// tinh accuracy cua tap test voi bo weight 
double evaluate(vector<vector<double>> test, vector<double> w){

    vector<double> y, y_pre;
    for(auto item : test){
        y.push_back(item[N_COT]);
        item.pop_back();
        y_pre.push_back(sigmoid(y_predict(w,item)));
    }
    return accuracy(y,y_pre);
}



vector<double> logistic_regression(vector<vector<double>> train, vector<vector<double>> test, int numOfIteration, double learning_rate )
{
    int i,j,k ;
    vector<double> w ,deta_w, x; 
    double y_pre, y, loss;
    // khoi tai weight 
    for (i=0; i<N_COT; i++){
        double r = (double) rand()/RAND_MAX;
        w.push_back(r);
        deta_w.push_back(0);
        x.push_back(0);
    }
    
    for (i=0; i < numOfIteration; i++){
        loss = 0;
        for (k=0; k<=N_COT; k++){
                deta_w[k] = 0;
        }
        for (j =0; j < train.size(); j++){
            
            for (k=0; k<N_COT; k++){
                x[k] = train[j][k];
            }
            

            y_pre = sigmoid(y_predict(w,x));
            y = train[j][N_COT];

            // cap nhat loss
            loss += cost(y_pre, y);
            

            for (k=0; k<=N_COT; k++){
                deta_w[k] += (y_pre - y)*x[k];
            }
        }

        loss = loss/train.size();
        cout << loss << "--" << evaluate(test, w) << "%" << endl;
        
        // cap nhat weight bang thuat toan Gradient descent
        for (k=0; k<=N_COT; k++){
            w[k] = w[k] - learning_rate*deta_w[k];
        }
        
    }
    return w;
}

int main()
{
    vector<vector<vector<double>>> data = makeTrainAndTestData(2,8);
    //data[0] là test, data[1] là train

     // vector dung chuan hoa
    vector<double> standard = standardVector();

    // tạp test sau khi chuan hoa
    vector<vector<double>> test;
    vector<double> x , w;
    for(auto item : data[0]) {
        x = standardize(standard,item);
        x.insert(x.begin(),1);
        test.push_back(x);
    }
    vector<vector<double>> train;
    for(auto item : data[1]) {
        x = standardize(standard,item);
        x.insert(x.begin(),1);
        train.push_back(x);
    }

    int numOfIteration = 500; // so lan lap thuat toan 
    double learning_rate = 0.003;
    w = logistic_regression(train, test, numOfIteration, learning_rate);
    for (auto a : w) cout << a << ',';
    return 0;
}
    