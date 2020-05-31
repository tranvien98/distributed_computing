#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <time.h>
#include <stdlib.h>
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

double standardValue(vector<double> item) {
    double maxValue = 0.0;
    double minValue = 0.0;
    for(auto i : item) {
        if(i > maxValue) maxValue = i;
        if(i < minValue) minValue = i;
    }
    return maxValue-minValue;
}


vector<double> standardVector(string filename) {
    ifstream fin;
    string line;
    fin.open(filename);
    fin>>line;
    vector<string> line1 = split(line, ',');
    int length = line1.size();

    vector<double> standard;
    for(int i = 1; i < length; i++) {
        standard.push_back(standardValue(readByColumn(filename,i)));
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
    vector<double> temp;
    // diabetes.csv
    vector<vector<double>> true_lable_data;
    vector<vector<double>> false_lable_data;
    while(!fin.eof()){
       fin>>line;
       temp = convert(split(line, ','));
       //temp = convert(temp);
       if(temp.back() != 1) {
            temp.at(temp.size()-1) = 0;
            false_lable_data.push_back(temp);
        } else {
            true_lable_data.push_back(temp);
       }
    }

    // tinh toan so diem du lieu cho tap test
    vector<double> lables = readByColumn(filename,temp.size());
    int true_lable = 0; // so diem du lieu co nhan 1
    int false_lable = 0; // so diem du lieu co nhan 0
    for(auto lable: lables) {
        if (lable != 1) false_lable++;
    }
    true_lable = lables.size() - false_lable;

    // so nhan 1 trong tap test
    int test_true_lable = ceil(true_lable*(1-train));
    // so nhan 0 trong tap test
    int test_false_lable = ceil(false_lable*(1-train));

    // tap du lieu test
    vector<vector<double>> test_data;
    // them cac diem co nhan 1 vao tap test
    //srand((int) time(0));
    for(int i = 0; i < test_true_lable; i++) {
        int index = rand() % true_lable_data.size();
        test_data.push_back(true_lable_data.at(index));
        true_lable_data.erase(true_lable_data.begin() + index);
    }

    //them cac diem co nhan 0 vao tap test
    //srand((int) time(0));
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
        // #pragma omp parallel for reduction(+:y_pre)
        for (int i =0; i < w.size(); i++){
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
    // int sum = 0;
    if (y.size() == y_pre.size()){
        int sum = 0;
        #pragma omp parallel for reduction(+:sum) //num_threads(4) schedule(dynamic)
        for (int i =0; i < y.size(); i++){
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

    vector<double> y(test.size(),0), y_pre(test.size(),0), rel;
    double loss = 0, y_temp;
    int n_col = test[0].size() - 1;
    #pragma omp parallel for private(y_temp) shared(y, y_pre, test, n_col) \
        reduction(+:loss)
    for(int i=0; i< test.size(); i++){
        y[i] = test[i][n_col];
        test[i].pop_back();
        y_temp = sigmoid(y_predict(w,test[i]));
        y_pre[i] = y_temp;
        loss += cost(y_temp, y[i]);
    }
    loss = loss/test.size();
    rel.push_back(loss);
    rel.push_back(accuracy(y,y_pre));
    return rel;
}



void logistic_regression(vector<vector<double>> train, vector<vector<double>> test, int numOfIteration, double learning_rate )
{
    int n_col = train[0].size() -1;
    // int i,j,k ;
    vector<double> w(n_col,0.0), x(n_col,0.0) , y(train.size(),0), y_pre(train.size(),0);
    double y_pre_temp, y_temp, loss, acc_train, r;
    double deta_w[n_col] = {};
    // #pragma omp parallel
    // khoi tai weight
    // #pragma omp parallel for
    for (int i=0; i<n_col; i++){
        r = (double) rand()/RAND_MAX;
        w[i] = r;
    }
    // #pragma omp parallel for
    for(int i=0; i < train.size(); i++){
        y[i] = train[i][n_col];
    }

    ofstream file1, file2;
    file1.open ("result.txt");
    file2.open ("weight.txt");
    for (int i=0; i < numOfIteration; i++){
        loss = 0;
        // #pragma omp parallel
        // #pragma omp parallel for
        for (int k=0; k<n_col; k++){
                deta_w[k] = 0;
        }
        #pragma omp parallel for private(y_pre_temp, x) shared(y, y_pre, train, n_col) reduction(+:loss,deta_w[:n_col]) //collapse(2)
        for (int j =0; j < train.size(); j++){

            x = train[j];
            x.pop_back();

            y_pre_temp = sigmoid(y_predict(w,x));
            // y_temp = train[j][n_col];
            y_pre[j] = y_pre_temp;
            // cap nhat loss
            loss += cost(y_pre_temp, y[j]);

            // #pragma omp parallel for nowait
            for (int k=0; k<n_col; k++){
                deta_w[k] += (y_pre_temp - y[j])*x[k];
            }
        }
        // #pragma omp barrier
        // cap nhat weight bang thuat toan Gradient descent
        for (int k=0; k<n_col; k++){
            w[k] = w[k] - learning_rate*deta_w[k];
            file2 << w[k] << ";";
        }
        file2 << endl;
        loss = loss/train.size();

        acc_train = accuracy(y,y_pre);
        vector<double> result_test = evaluate(test, w);
        file1 << loss << ";" << acc_train << ";" << result_test[0] << ";" << result_test[1] <<  endl;
        // cout << loss << "__" << acc_train << '%' << "__" << result_test[0] << "__" << result_test[1] << "%" << endl;
    }
    // #pragma omp end parallel
    file1.close();
    file2.close();
}

int main()
{
    clock_t start, end;
    start = clock();
    // double start = omp_get_wtime();
    vector<vector<vector<double>>> data = makeTrainAndTestData("diabetes.csv", 0.8);
    //data[0] là test, data[1] là train

     // vector dung chuan hoa
    vector<double> standard = standardVector("diabetes.csv");


    // tạp test sau khi chuan hoa
    vector<vector<double>> test(data[0].size(),vector<double>(standard.size()+1));
    vector<double> x ;
    // #pragma omp parallel
    omp_set_num_threads(1);
    // int nProcessors = omp_get_max_threads();
    // cout << nProcessors << endl;
    // double time_taken1;
    // clock_t start1, end1;
    // start1 = clock();
    #pragma omp parallel for private(x) shared(data, test) schedule(dynamic)
    for(int i=0; i < data[0].size(); i++) {
        x = standardize(standard,data[0][i]);
        x.insert(x.begin(),1);
        test[i] = x;
    }
    // end1 = clock();
    // time_taken1 = double(end1 - start1) / double(CLOCKS_PER_SEC);
    // cout << "Time taken by test is : " << fixed
    //      << time_taken1 << setprecision(5);
    // cout << " sec " << endl;
    vector<vector<double>> train(data[1].size(),vector<double>(standard.size()+1));
    // start1 = clock();
    #pragma omp parallel for private(x) shared(data, train) schedule(dynamic)
    for(int i=0; i < data[1].size(); i++) {
        x = standardize(standard,data[1][i]);
        x.insert(x.begin(),1);
        train[i] = x;
    }
    // #pragma omp barrier
    // end1 = clock();
    // time_taken1 = double(end1 - start1) / double(CLOCKS_PER_SEC);
    // cout << "Time taken by train is : " << fixed
    //      << time_taken1 << setprecision(5);
    // cout << " sec " << endl;
    // int nProcessors = omp_get_max_threads();
    // cout << nProcessors << endl;

    int numOfIteration = 1000; // so lan lap thuat toan
    double learning_rate = 0.001;
    omp_set_nested(1);

    logistic_regression(train, test, numOfIteration, learning_rate);
    // #pragma omp end parallel
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;
    // double delta = omp_get_wtime() - start;
    // cout << "Time taken by program is : " << delta << "sec" << endl;
    return 0;
}
