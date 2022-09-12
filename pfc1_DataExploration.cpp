#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

double sum(vector<double> v){
    double sum = 0;
    for(double d : v){
        sum += d;
    }
    return sum;
}

double mean(vector<double> v){
    double sum = 0;
    for(double d : v){
        sum += d;
    }
    return sum/v.size();
}

double median(vector<double> v){
    size_t size = v.size();

    if(size == 0){
        return 0;
    }
    else{
        std::sort(v.begin(), v.end());
        if(size%2){
            return (v[size/2-1] + v[size/2]) / 2;
        }
        else{
            return v[size/2];
        }
    }
}

double min(vector<double> v){
    double min = v[0];
    for(double d : v){
        if(d < min){
            min = d;
        }
    }
    return min;
}

double max(vector<double> v){
    double max = v[0];
    for(double d : v){
        if(d > max){
            max = d;
        }
    }
    return max;
}

void print_stats(vector<double> v){
    cout << "sum\t" << sum(v) << endl;
    cout << "mean\t" << mean(v) << endl;
    cout << "median\t" << median(v) << endl;
    cout << "min\t" << min(v) << "\tmax\t" << max(v) << endl;
}

double covar(vector<double> v1, vector<double> v2){
    double sum = 0;
    double mean_v1 = mean(v1);
    double mean_v2 = mean(v2);
    for(int i = 0; i<v1.size(); i++){
        sum += (v1[i] - mean_v1) * (v2[i] - mean_v2);
    }
    return sum / (v1.size() - 1);
}

double cor(vector<double> v1, vector<double> v2){
    double cov = covar(v1, v2);
    double mean_v1 = mean(v1);
    double mean_v2 = mean(v2);
    double stdev_v1 = 0;
    double stdev_v2 = 0;

    for(int i = 0; i<v1.size(); i++){
        stdev_v1 += pow(v1[i] - mean_v1, 2);
    }
    stdev_v1 = sqrt(stdev_v1 / v1.size());

    for(int i = 0; i<v2.size(); i++){
        stdev_v2 += pow(v2[i] - mean_v2, 2);
    }
    stdev_v2 = sqrt(stdev_v2 / v2.size());

    return cov / (stdev_v1 * stdev_v2);
}

int main(int argc, char** argv){

    ifstream inFS;
    string line;
    string rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector<double> rm(MAX_LEN);
    vector<double> medv(MAX_LEN);

    cout << "Opening file Boston.csv" << endl;

    inFS.open("Boston.csv");
    if (!inFS.is_open()){
        cout << "Could not open file Boston.csv" << endl;
        return 1;
    }

    cout << "Reaing line 1" << endl;
    getline(inFS, line);

    cout << "heading: " << line << endl;

    int numObservations = 0;
    while(inFS.good()){

        getline(inFS, rm_in, ',');
        getline(inFS, medv_in, '\n');

        rm.at(numObservations) = stof(rm_in);
        medv.at(numObservations) = stof(medv_in);

        numObservations++;
    }

    rm.resize(numObservations);
    medv.resize(numObservations);

    cout << "new length " << rm.size() << endl;

    cout << "Closing file Boston.csv." << endl;
    inFS.close();

    cout << "Number of records: " << numObservations << endl;

    cout << "\nStats for rm" << endl;
    print_stats(rm);

    cout << "\nStats for medv" << endl;
    print_stats(medv);

    cout << "\n Covariance = " << covar(rm, medv) << endl;

    cout << "\n Correlation = " << cor(rm, medv) << endl;

    cout << "\nProgram terminated.";

    return 0;
}