#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <time.h>
#include <iostream>
#include <omp.h>
#include <bits/stdc++.h>

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

vector<double> readByColumn(string filename, int pos) {
    ifstream fin;
    string line2;
    vector<double> result;
    // Open an existing file
    fin.open(filename);
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
        standard.push_back(maxValue(readByColumn("diabetes.csv",i)));
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

vector<vector<vector<double>>> makeTrainAndTestData(string filename, float train) {
    // lay du lieu ra tu file, nhan 1 luu vao true_lable_data, nhan 0 luu vao false_lable_data
    ifstream fin;
    string line;
    fin.open(filename);
    // diabetes.csv
    vector<vector<double>> true_lable_data;
    vector<vector<double>> false_lable_data;
    while(!fin.eof()){
       fin>>line;
       vector<double> temp = convert(split(line, ','));
       //temp = convert(temp);
       if(temp[8] != 0) {
            true_lable_data.push_back(temp);
        } else {
            false_lable_data.push_back(temp);
       }
    }

    // tinh toan so diem du lieu cho tap test
    vector<double> lables = readByColumn(filename,9);
    int true_lable = 0; // so diem du lieu co nhan 1
    int false_lable = 0; // so diem du lieu co nhan 0
    for(auto lable: lables) {
        if (lable == 1) true_lable++;
    }
    false_lable = lables.size() - true_lable;

    // so nhan 1 trong tap test
    int test_true_lable = ceil(true_lable*(1-train));
    // so nhan 0 trong tap test
    int test_false_lable = ceil(false_lable*(1-train));

    // tap du lieu test
    vector<vector<double>> test_data;
    // them cac diem co nhan 1 vao tap test
    srand((int) time(0));
    for(int i = 0; i < test_true_lable; i++) {
        int index = rand() % true_lable_data.size();
        test_data.push_back(true_lable_data.at(index));
        true_lable_data.erase(true_lable_data.begin() + index);
    }

    //them cac diem co nhan 0 vao tap test
    srand((int) time(0));
    for(int j = 0; j < test_false_lable; j++) {
        int index = rand() % false_lable_data.size();
        test_data.push_back(false_lable_data.at(index));
        false_lable_data.erase(false_lable_data.begin() + index);
    }

    // tap du lieu train
    vector<vector<double>> train_data;
    // them phan con lai cua tap du lieu chua nhan 1 va 0 vao trai_data
    train_data.insert(train_data.begin(), true_lable_data.begin(), true_lable_data.end());
	train_data.insert(train_data.end(), false_lable_data.begin(), false_lable_data.end());

    vector<vector<vector<double>>> data;
    // data[0] la test, data[1] la train
    data.push_back(test_data);
    data.push_back(train_data);

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
        #pragma omp parallel for \
            reduction(+:sum)
        for (i =0; i < y.size(); i++){
            if((y[i] == 0 && y_pre[i] < 0.5) || (y[i] == 1 && y_pre[i] >= 0.5)){
                sum += 1;
            }
        }
        return double(sum)*100/y.size();
    }else
    {
        cout << "Size y and y_pre different" << endl;
        return 0;
    }
}

// tinh accuracy cua tap test voi bo weight
vector<double> evaluate(vector<vector<double>> test, vector<double> w){

    vector<double> y, y_pre, rel;
    double loss = 0, y_temp;
    int n_col = test[0].size() - 1;
    for(auto item : test){
        y.push_back(item[n_col]);
        item.pop_back();
        y_temp = sigmoid(y_predict(w,item));
        y_pre.push_back(y_temp);
        loss += cost(y_temp, item[n_col]);
    }
    loss = loss/test.size();
    rel.push_back(loss);
    rel.push_back(accuracy(y,y_pre));
    return rel;
}



void logistic_regression(vector<vector<double>> train, vector<vector<double>> test, int numOfIteration, double learning_rate )
{
    
    int i,j,k ;
    vector<double> w ,deta_w, x , y, y_pre;
    double y_pre_temp, y_temp, loss, acc_train, r;

    int n_col = train[0].size() -1;
    // khoi tai weight
    #pragma omp parallel for private(r)
    for (i=0; i<n_col; i++){
        r = (double) rand()/RAND_MAX;
        w.push_back(r);
        deta_w.push_back(0);
        x.push_back(0);
    }
    for(auto item : train){
        y.push_back(item[n_col]);
        y_pre.push_back(0);
    }
    ofstream file1, file2;
    file1.open ("result.txt");
    file2.open ("weight.txt");
    
    for (i=0; i < numOfIteration; i++){
        loss = 0;
        #pragma omp parallel
        for (k=0; k<n_col; k++){
                deta_w[k] = 0;
        }
        #pragma omp parallel for private(j, k, y_pre_temp, x) shared(loss, deta_w, y, y_pre, train, n_col)
        for (j =0; j < train.size(); j++){

            // for (k=0; k<n_col; k++){
            //     x[k] = train[j][k];
            // }
            x = train[j];
            x.pop_back();

            y_pre_temp = sigmoid(y_predict(w,x));
            // y_temp = train[j][n_col];
            y_pre[j] = y_pre_temp;
            // cap nhat loss
            loss += cost(y_pre_temp, y[j]);


            for (k=0; k<n_col; k++){
                deta_w[k] += (y_pre_temp - y[j])*x[k];
            }
        }
        #pragma omp barrier
        // cap nhat weight bang thuat toan Gradient descent
        #pragma omp for
        for (k=0; k<n_col; k++){
            w[k] = w[k] - learning_rate*deta_w[k];
            file2 << w[k] << ";";
        }
        #pragma omp end parallel
        file2 << endl;
        loss = loss/train.size();
        acc_train = accuracy(y,y_pre);
        vector<double> result_test = evaluate(test, w);
        file1 << loss << "__" << acc_train << '%' << "__" << result_test[0] << "__" << result_test[1] << "%" << endl;
        // cout << loss << "__" << acc_train << '%' << "__" << result_test[0] << "__" << result_test[1] << "%" << endl;
    }
    file1.close();
    file2.close();
}

int main()
{
    vector<vector<vector<double>>> data = makeTrainAndTestData("diabetes.csv", 0.8);
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

    int numOfIteration = 1000; // so lan lap thuat toan
    double learning_rate = 0.002;
    clock_t start, end;
    start = clock(); 
    logistic_regression(train, test, numOfIteration, learning_rate);
    end = clock(); 
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC); 
    cout << "Time taken by logistic_regression is : " << fixed  
         << time_taken << setprecision(5); 
    cout << " sec " << endl; 
    return 0;
}
