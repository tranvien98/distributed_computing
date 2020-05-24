#include <bits/stdc++.h>

using namespace std;

void zip(vector<vector<double>> &b)
{
    if (b.size() == 0)
        return;
    vector<vector<double>> trans_vec(b[0].size(), vector<double>());
    for (int i = 0; i < b.size(); i++)
    {
        for (int j = 0; j < b[i].size(); j++)
        {
            trans_vec[j].push_back(b[i][j]);
        }
    }
    b = trans_vec;
}

// tinh toan gia tri trung binh cua moi thuoc tinh
double mean(vector<double> numbers)
{
    return accumulate(numbers.begin(), numbers.end(), 0) / double(numbers.size());
}

// Tinh toan do lech chuan cho tung thuoc tinh
double standard_deviation(vector<double> numbers)
{
    double avg = mean(numbers);
    vector<double> a;
    for (unsigned i = 0; i < numbers.size(); i++)
        a.push_back(pow(numbers.at(i) - avg, 2));
    double variance = accumulate(a.begin(), a.end(), 0) / double(numbers.size() - 1);
    return sqrt(variance);
}

// Gia tri trung binh , do lech chuan
vector<tuple<double, double>> summarize(vector<vector<double>> dataset)
{
    zip(dataset);
    vector<tuple<double, double>> summaries;
    for (unsigned i = 0; i < dataset.size(); i++)
    {
        auto tup = make_tuple(mean(dataset.at(i)), standard_deviation(dataset.at(i)));
        summaries.push_back(tup);
    }
    summaries.pop_back();
    return summaries;
}

// Phan chia tap du lieu theo class
map<double, vector<vector<double>>> separate_data(vector<vector<double>> dataset)
{
    map<double, vector<vector<double>>> separated;
    for (int i = 0; i < dataset.size(); i++)
    {
        vector<double> vec = dataset.at(i);
        if (!separated.count(vec.back()))
            separated[vec.back()] = vector<vector<double>>();
        separated[vec.back()].push_back(vec);
    }
    return separated;
}

map<double, vector<tuple<double, double>>> summarize_by_class(vector<vector<double>> dataset)
{
    map<double, vector<vector<double>>> separated = separate_data(dataset);
    map<double, vector<tuple<double, double>>> summaries;
    for (map<double, vector<vector<double>>>::iterator it = separated.begin(); it != separated.end(); ++it)
        summaries[it->first] = summarize(it->second);

    return summaries;
}

double calculate_prob(double x, double mean, double stdev)
{
    float exponent = exp(-(pow(x - mean, 2) / (2 * pow(stdev, 2))));
    return (1 / (sqrt(2 * M_PI) * stdev)) * exponent;
}

// Tinh xac suat cho moi thuoc tinh phan chia theo class
map<double, double> calculate_class_prob(map<double, vector<tuple<double, double>>> summaries, vector<double> input_vector)
{
    map<double, double> probabilities;
    for (map<double, vector<tuple<double, double>>>::iterator it = summaries.begin(); it != summaries.end(); ++it)
    {
        probabilities[it->first] = 1;
        for (int i = 0; i < (it->second).size(); ++i)
        {
            double mean = get<0>((it->second).at(i));
            double stdev = get<1>((it->second).at(i));
            double x = input_vector[i];
            probabilities.at(it->first) *= calculate_prob(x, mean, stdev);
        }
    }
    return probabilities;
}

// Du doan vector thuoc phan lop nao
double predict(map<double, vector<tuple<double, double>>> summaries, vector<double> input_vector)
{
    map<double, double> probabilities = calculate_class_prob(summaries, input_vector);
    double best_label = 9999, best_prob = -1;
    for (map<double, double>::iterator it = probabilities.begin(); it != probabilities.end(); ++it)
        if (best_label == 9999 || it->second > best_prob)
        {
            best_prob = it->second; //probability
            best_label = it->first; // class value
        }
    return best_label;
}

// Du doan tap du lieu testing thuoc vao phan lop nao
vector<double> get_predictions(map<double, vector<tuple<double, double>>> summaries, vector<vector<double>> test_set)
{
    vector<double> predictions;
    for (int i = 0; i < test_set.size(); i++)
        predictions.push_back(predict(summaries, test_set[i]));
    return predictions;
}

// Tinh toan do chinh xac cua phan lop
double get_accuracy(vector<vector<double>> test_set, vector<double> predictions)
{
    double correct = 0;
    for (int i = 0; i < test_set.size(); ++i)
        if (test_set.at(i).back() == predictions.at(i))
            correct += 1;
    return (correct / double(test_set.size())) * 100.0;
}

/**
 * Reads csv file into table, exported as a vector of vector of doubles.
 * @param inputFileName input file name (full path).
 * @return data as vector of vector of doubles.
 */
vector<vector<double>> load_data(string inputFileName)
{

    vector<vector<double>> data;
    ifstream inputFile(inputFileName);
    int l = 0;

    while (inputFile)
    {
        l++;
        string s;
        if (!getline(inputFile, s))
            break;
        if (s[0] != '#')
        {
            istringstream ss(s);
            vector<double> record;

            while (ss)
            {
                string line;
                if (!getline(ss, line, ','))
                    break;
                try
                {
                    record.push_back(stof(line));
                }
                catch (const std::invalid_argument e)
                {
                    cout << "NaN found in file " << inputFileName << " line " << l
                         << endl;
                    e.what();
                }
            }

            data.push_back(record);
        }
    }

    if (!inputFile.eof())
    {
        cerr << "Could not read file " << inputFileName << "\n";
        __throw_invalid_argument("File not found.");
    }

    return data;
}

int main()
{
    double split_ratio = 0.8;
    vector<vector<double>> dataset = load_data("diabetes.csv");
    // chia train(train_set), test(copy)
    double train_size = dataset.size() * split_ratio;
    vector<vector<double>> train_set;
    vector<vector<double>> test_set = dataset;
    while (train_set.size() < train_size)
    {
        int index = rand() % test_set.size();
        train_set.push_back(test_set.at(index));
        test_set.erase(test_set.begin() + index);
    }
    //
    map<double, vector<tuple<double, double>>> summaries = summarize_by_class(train_set);
    vector<double> predictions = get_predictions(summaries, test_set);
    double accuracy = get_accuracy(test_set, predictions);
    cout << "Accuracy of my implement: " << accuracy << "%";
    return 0;
}
