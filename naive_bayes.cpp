#include <bits/stdc++.h>

#define NUM_THREAD 8

using namespace std;

/**
 * Reads csv file into table, exported as a vector of vector of floats.
 * @param inputFileName input file name (full path).
 * @return data as vector of vector of floats.
 */
vector<vector<float>> load_data(string inputFileName)
{

    vector<vector<float>> data;
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
            vector<float> record;

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
            if (record.back() != 1)
                record.at(record.size() - 1) = 0;
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

void zip(vector<vector<float>> &b)
{
    if (b.size() == 0)
        return;
    vector<vector<float>> trans_vec(b[0].size(), vector<float>());
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
float mean(vector<float> numbers)
{
    return accumulate(numbers.begin(), numbers.end(), 0) / float(numbers.size());
}

// Tinh toan do lech chuan cho tung thuoc tinh
float standard_deviation(vector<float> numbers)
{
    float avg = mean(numbers);
    vector<float> a;
    for (unsigned i = 0; i < numbers.size(); i++)
        a.push_back(pow(numbers.at(i) - avg, 2));
    float variance = accumulate(a.begin(), a.end(), 0) / float(numbers.size() - 1);
    return sqrt(variance);
}

// Gia tri trung binh , do lech chuan
vector<tuple<float, float>> summarize(vector<vector<float>> dataset)
{
    zip(dataset);
    vector<tuple<float, float>> summaries;
    for (unsigned i = 0; i < dataset.size(); i++)
    {
        auto tup = make_tuple(mean(dataset.at(i)), standard_deviation(dataset.at(i)));
        summaries.push_back(tup);
    }
    summaries.pop_back();
    return summaries;
}

// Phan chia tap du lieu theo class
map<float, vector<vector<float>>> separate_data(vector<vector<float>> dataset)
{
    map<float, vector<vector<float>>> separated;
    vector<float> vec;
    for (int i = 0; i < dataset.size(); i++)
    {
        vec = dataset.at(i);
        if (!separated.count(vec.back()))
            separated[vec.back()] = vector<vector<float>>();
        separated[vec.back()].push_back(vec);
    }
    return separated;
}

map<float, vector<tuple<float, float>>> summarize_by_class(vector<vector<float>> dataset)
{
    map<float, vector<vector<float>>> separated = separate_data(dataset);
    map<float, vector<tuple<float, float>>> summaries;
    for (map<float, vector<vector<float>>>::iterator it = separated.begin(); it != separated.end(); ++it)
        summaries[it->first] = summarize(it->second);

    return summaries;
}

float calculate_prob(float x, float mean, float stdev)
{
    float exponent = exp(-(pow(x - mean, 2) / (2 * pow(stdev, 2))));
    return (1 / (sqrt(2 * M_PI) * stdev)) * exponent;
}

// Tinh xac suat cho moi thuoc tinh phan chia theo class
map<float, float> calculate_class_prob(map<float, vector<tuple<float, float>>> summaries, vector<float> input_vector)
{
    map<float, float> probabilities;
    for (map<float, vector<tuple<float, float>>>::iterator it = summaries.begin(); it != summaries.end(); ++it)
    {
        probabilities[it->first] = 1;
        for (int i = 0; i < (it->second).size(); ++i)
        {
            float mean = get<0>((it->second).at(i));
            float stdev = get<1>((it->second).at(i));
            float x = input_vector[i];
            float a = calculate_prob(x, mean, stdev);
            probabilities.at(it->first) *= a;
        }
    }
    return probabilities;
}

// Du doan vector thuoc phan lop nao
float predict(map<float, vector<tuple<float, float>>> summaries, vector<float> input_vector)
{
    map<float, float> probabilities = calculate_class_prob(summaries, input_vector);
    float best_label = 9999, best_prob = -1;
    for (map<float, float>::iterator it = probabilities.begin(); it != probabilities.end(); ++it)
        if (best_label == 9999 || it->second > best_prob)
        {
            best_prob = it->second; //probability
            best_label = it->first; // class value
        }
    return best_label;
}

// Du doan tap du lieu testing thuoc vao phan lop nao
vector<float> get_predictions(map<float, vector<tuple<float, float>>> summaries, vector<vector<float>> test_set)
{
    vector<float> predictions;
    #pragma omp parallel for num_threads(NUM_THREAD)
    for (int i = 0; i < test_set.size(); i++)
        predictions.push_back(predict(summaries, test_set[i]));
    return predictions;
}

// Tinh toan do chinh xac cua phan lop
float get_accuracy(vector<vector<float>> test_set, vector<float> predictions)
{
    float correct = 0;
    for (int i = 0; i < test_set.size(); ++i)
        if (test_set.at(i).back() == predictions.at(i))
            correct += 1;
    return (correct / float(test_set.size())) * 100.0;
}

int main()
{
    auto start = chrono::system_clock::now();
    float split_ratio = 0.8;
    vector<vector<float>> dataset = load_data("data.csv");
    auto end = chrono::system_clock::now();
    chrono::duration<float> elapsed_seconds = end - start;
    cout << "Time load data: " << elapsed_seconds.count() << "\n";


    // chia train(train_set), test(copy)
    float train_size = int(dataset.size() * split_ratio);
    vector<vector<float>> train_set;
    vector<vector<float>> test_set = dataset;


    auto start_ = chrono::system_clock::now();
    srand((int)time(0));
    while (train_set.size() < train_size)
    {
        int index = rand() % test_set.size();
        train_set.push_back(test_set.at(index));
        test_set.erase(test_set.begin() + index);
    }
    
    end = chrono::system_clock::now();
    elapsed_seconds = end - start_;
    cout << "Time chia train test: " << elapsed_seconds.count() << "\n";
    //


    start_ = chrono::system_clock::now();
    map<float, vector<tuple<float, float>>> summaries = summarize_by_class(train_set);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start_;
    cout << "Time summarize_by_class: " << elapsed_seconds.count() << "\n";

    vector<float> predictions = get_predictions(summaries, test_set);
    float accuracy = get_accuracy(test_set, predictions);

    vector<vector<float>> data = load_data("benhnhan1.csv");
    vector<float> prediction = get_predictions(summaries, data);

    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    time_t end_time = std::chrono::system_clock::to_time_t(end);
    cout << "Accuracy of my implement: " << accuracy << "%\n";
    if (prediction.at(0) == 0)
        cout << "Khong mac benh\n";
    else
        cout << "Mac benh\n";
    cout << "Time handle: " << elapsed_seconds.count() << "\n";
    return 0;
}
