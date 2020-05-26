
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

double y_predict(vector<double> w,vector<double> x){
    if (w.size() == x.size() +1){
        double y_pre = 0;
        int i;
        x.insert(x.begin(),1);
        for (i =0; i < w.size(); i++){
            y_pre += w[i]*x[i];
        }
        if y_pre > 0.5{
            cout << "Patients with diabetes" << endl;
            cout << "Accuracy :" << y_pre << endl;
        }else{
            cout << "The patient does not have diabetes" << endl;
            cout << "Accuracy :" << y_pre << endl;
        }
        return true;
    }else
    {
        cout << "Error size w or x" << endl;
        return false;
    }
}

int main(){
    
    return 0;
}