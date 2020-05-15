#include <iostream>
#include <fstream>
using namespace std;
void read()
{
    ifstream fin;
    string line;
    // Open an existing file
    fin.open("diabetes.csv");
    while (!fin.eof())
    {
        fin >> line;
        cout << line << " " << endl;
    }
}
int main()
{
    read();
    return 0;
}
