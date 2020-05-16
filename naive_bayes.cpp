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
float mean(tuple<float> numbers)
{
    return sum_tupple(numbers) / float(tuple_size<decltype(numbers)>::value);
}

float sum_vector(vector<float> &v)
{
    int initial_sum = 0;
    return accumulate(v.begin(), v.end(), initial_sum);
}

// Tinh toan do lech chuan cho tung thuoc tinh
template <class Tuple>
float standard_deviation(tuple<float> numbers)
{
    float avg = mean(numbers);
    vector<float> a;

    apply([](auto &&... args) { ((a.push_back(pow(args - avg, 2));), ...); }, numbers);
    float variance = sum_vector(a) / float(tuple_size<decltype(numbers)>::value - 1);
    return sqrt(variance);
}

// Gia tri trung binh , do lech chuan
vector<tuple<float>> summarize(vector<vector<float>> dataset)
{
    // BUG: convert sang c++
    vector<float> summaries = [(mean(attribute), standard_deviation(attribute)) for attribute in zip(*dataset)];

    summaries.pop_back();
    return summaries;
}

// Phan chia tap du lieu theo class
map<float, vector<vector<float>>> separate_data(vector<vector<float>> dataset)
{
    map<float, vector<vector<float>>> separated;
    for (int i = 0; i < dataset.size(); i++)
    {
        vector<float> vec = dataset[i];
        if (count(separated.begin(), separated.end(), vec.back()))
        {
        }
        else
            separated[vec.back()] = vector<vector<float>>();
        separated[vec.back()].push_back(vec);
    }
    return separated;
}

map<float, vector<tuple<float>>> summarize_by_class(vector<vector<float>> dataset)
{
    map<float, vector<vector<float>>> separated = separate_data(dataset);
    map<float, vector<tuple<float>>> summaries;
    for (map<float, vector<vector<float>>>::iterator it = separated.begin(); it != separated.end(); ++it)
        summaries[it->first] = summarize(it->second);

    return summaries;
}

// Du doan vector thuoc phan lop nao
float predict(map<float, vector<tuple<float>>> summaries, vector<float> input_vector)
{
    // TO DO
}

// Du doan tap du lieu testing thuoc vao phan lop nao
vector<float> get_predictions(map<float, vector<tuple<float>>> summaries, vector<vector<float>> test_set)
{
    vector<float> predictions;
    for (int i = 0; i < test_set.size(); i++)
        predictions.push_back(predict(summaries, test_set[i]));
    return predictions;
}

// doc file
vector<vector<float>> load_data(string filepath)
{
    // TO DO
}

int main()
{
    float split_ratio = 0.8;
    vector<vector<float>> dataset = load_data("diabetes.csv");
    // chia train(train_set), test(copy)
    float train_size = dataset.size() * split_ratio;
    vector<vector<float>> train_set;
    vector<vector<float>> copy = dataset;
    while (train_set.size() < train_size)
    {
        int index = rand() % copy.size();
        train_set.push_back(copy.at(index));
        copy.erase(find(copy.begin(), copy.end(), 2));
    }
    //
    map<float, vector<tuple<float>>> summaries = summarize_by_class(train_set);

    return 0;
}