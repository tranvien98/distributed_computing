#include <vector>
#include "matplotlibcpp.h"
#include <bits/stdc++.h>

using namespace std;
namespace plt = matplotlibcpp;

vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

vector<vector<double>> get_data(string filename) {

    vector<double> loss_train,loss_test,acc_train,acc_test;
    vector<vector<double>> rel;
    ifstream fin;
    string line2;
    int i =0;
    // Open an existing file
    fin.open(filename);
    
    while(!fin.eof()){
       fin>>line2;
       vector<string> temp = split(line2,';');
       loss_train.push_back(atof(temp[0].c_str()));
       loss_test.push_back(atof(temp[2].c_str()));
       acc_train.push_back(atof(temp[1].c_str()));
       acc_test.push_back(atof(temp[3].c_str()));
       //atof(num.c_str())
    }

    rel.push_back(loss_train);
    rel.push_back(loss_test);
    rel.push_back(acc_train);
    rel.push_back(acc_test);

    return rel;

}

int main() {


    vector<vector<double>> data = get_data("result.txt");
    // vector<double> label;
    // label.push_back("Tr Loss");
    // label.push_back("Val Loss");

    vector<double> inter;

    for(int i = 0; i < data[0].size(); i++){
        inter.push_back(i+1);
    } 

    // plt::figure_size(1200, 780);
    plt::named_plot("Train loss", inter,data[0], "k-");
    plt::named_plot("Test loss", inter, data[1], "r-");
    plt::plot(inter,data[0], "k-");
    plt::plot(inter, data[1], "r-");
    plt::legend();
    plt::xlabel("Epoch Number");
    plt::ylabel("Loss");
    plt::ylim(0,1);
    plt::save("./loss_curve.png");
    // plt::show();

    plt::close();
    plt::named_plot("Train Acc", inter,data[2], "k-");
    plt::named_plot("Test Acc", inter, data[3], "r-");
    plt::plot(inter,data[0], "k-");
    plt::plot(inter, data[1], "r-");
    plt::legend();
    plt::xlabel("Epoch Number");
    plt::ylabel("Acc");
    plt::ylim(0,100);
    plt::save("./acc_curve.png");

  return 0;
}