
#include <iostream>
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

int predict(vector<double> w,vector<double> x){
    if (w.size() == x.size()){
        double y_pre = 0;
        int i;
        for (i =0; i < w.size(); i++){
            y_pre += w[i]*x[i];
        }
        if(y_pre > 0.5){
            cout << "Patients with diabetes" << endl;
            cout << "Accuracy :" << y_pre << "%" << endl;
        }else{
            cout << "The patient does not have diabetes" << endl;
            cout << "Accuracy :" << y_pre << "%" << endl;
        }
        return 1;
    }else
    {
        cout << "Error size w or x" << endl;
        return 0;
    }
}

vector<double> get_weight(string filename, int n) {
    vector<double> w;
    ifstream fin;
    string line2;
    int i =0;
    // Open an existing file
    fin.open(filename);
    while(n--){
       fin>>line2;
    }
    vector<string> temp = split(line2, ';');
    for (auto i : temp){
        w.push_back(atof(i.c_str()));
    }
    return w;

}

int main(){
    
    double a;
    vector<double> x;
    cout << "Please enter patient information for diagnosis" << endl;
    
    cout << "Enter Pregnancies (0-17):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter Glucose (0-199):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter BloodPressure (0-122):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter SkinThickness(0-99):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter Insulin (0- 846):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter BMI (0-67.1):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter DiabetesPedigreeFunction (0.08-2.42):"; 
    cin >> a;
    x.push_back(a);
    cout << "Enter Age (21-81):"; 
    cin >> a;
    x.push_back(a);
    vector<double> standard = standardVector();
    x = standardize(standard,x);
    x.insert(x.begin(),1);

    int n = 222;
    vector<double> w = get_weight("weight.txt", n);
    
    predict(w,x);
    
    return 0;
}