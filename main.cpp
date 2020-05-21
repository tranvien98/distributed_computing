#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


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

int main()
{
    vector<vector<vector<double>>> data = makeTrainAndTestData(4,6);
    //data[0] là test, data[1] là train

     // vector dung chuan hoa
    vector<double> standard = standardVector();

    // thu chuan hoa vector thu 4 dau tien cua tap test
    vector<double> item = data[0][4];
    vector<double> standardizedItem = standardize(standard,item);
    for (auto k :    standardizedItem)
        {
            cout << k << endl;
        }

    // tạp test sau khi chuan hoa
    vector<vector<double>> test;
    for(auto item : data[0]) {
        test.push_back(standardize(standard,item));
    }
    // in ra vector thu 4 sau khi chuan hoa
    for(auto item : test[4])
        cout << item << endl;
    // tap train sau khi chuan hoa
    vector<vector<double>> train;
    for(auto item : data[1]) {
        train.push_back(standardize(standard,item));
    }
    //in ra vector thu 4 cua tap train sau chuan hoa
    for(auto item : train[4])
        cout << item << endl;
    return 0;
}
