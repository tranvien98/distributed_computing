#include <iostream>
#include<fstream>
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
int main()
{
    // dọc cot nao thi truyen vao so thu tu cot do, tinh tu 1
    vector<double> cot1 = readByColumn(7);
    for (auto i : cot1) cout << i << endl;
    return 0;
}
