#include <iostream>
#include <tuple>
#include <vector>
#include <numeric>
#include <map>
#include <math.h>    //sqrt
#include <algorithm> // count
#include <stdlib.h>  /* rand */

using namespace std;

template <class Tuple>
decltype(auto) sum_tupple(Tuple const &tuple)
{
    auto sum_them = [](auto const &... e) -> decltype(auto) {
        return (e + ...);
    };
    return apply(sum_them, tuple);
};

// tinh toan gia tri trung binh cua moi thuoc tinh
double mean(tuple<double> numbers)
{
    return sum_tupple(numbers) / double(tuple_size<decltype(numbers)>::value);
}

double sum_vector(vector<double> &v)
{
    int initial_sum = 0;
    return accumulate(v.begin(), v.end(), initial_sum);
}

// Tinh toan do lech chuan cho tung thuoc tinh
template <class Tuple>
double standard_deviation(tuple<double> numbers)
{
    double avg = mean(numbers);
    vector<double> a;
    apply([](auto &&... args) { ((a.push_back(pow(args - avg, 2));), ...); }, numbers);
    double variance = sum_vector(a) / double(tuple_size<decltype(numbers)>::value - 1);
    return sqrt(variance);
}

// Gia tri trung binh , do lech chuan
vector<tuple<double, double>> summarize(vector<vector<double>> dataset)
{
    // BUG: convert sang c++
    vector<double, double> summaries = [(mean(attribute), standard_deviation(attribute)) for attribute in zip(*dataset)];

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
        if (count(separated.begin(), separated.end(), vec.back()))
        {
        }
        else
            // BUG: using vector.at()?
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
            double mean = get<0>(summaries);
            double stdev = get<1>(summaries);
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

// doc file
vector<vector<double>> load_data(string filepath)
{
    // TO DO
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
    cout << "Accuracy of my implement: " << accuracy;
    return 0;
}
