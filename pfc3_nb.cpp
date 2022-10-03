#define _USE_MATH_DEFINES

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <chrono>
using namespace std;

double accuracy(vector<double>v1, vector<double>v2){
    double accuracy = 0;
    for(int i=0; i<v1.size(); i++){
        if(v1[i] == v2[i]){
            accuracy += 1;
        }
    }
    return accuracy /= v1.size();
}

double sensitivity(vector<double>v1, vector<double>v2){
    double sens = 0;
    for(int i=0; i<v1.size(); i++){
        if(v1[i]==1 && v2[i]==1){
            sens += 1;
        }
    }
    return sens /= v1.size();
}

double specificity(vector<double>v1, vector<double>v2){
    double spec = 0;
    for(int i=0; i<v1.size(); i++){
        if(v1[i]==0 && v2[i]==0){
            spec += 1;
        }
    }
    return spec /= v1.size();
}

//PROBABILITY DENSITY
double calc_age_lh(double v, double mean_v, double var_v){
    return 1 / sqrt(2 * M_PI * var_v) * exp(-(pow(v-mean_v,2)/(2*var_v)));
}

int main(int argc, char** argv){
    ifstream inFS;
    string line;
    string index_in, pclass_in, survived_in, sex_in, age_in;
    const int MAX_LEN = 1050;
    vector<double> tr_pclass(MAX_LEN);
    vector<double> tr_survived(MAX_LEN);
    vector<double> tr_sex(MAX_LEN);
    vector<double> tr_age(MAX_LEN);

    vector<double> te_pclass(MAX_LEN);
    vector<double> te_survived(MAX_LEN);
    vector<double> te_sex(MAX_LEN);
    vector<double> te_age(MAX_LEN);

    cout << "Opening file titanic_project.csv" << endl;

    inFS.open("titanic_project.csv");
    if (!inFS.is_open()){
        cout << "Could not open file titanic_project.csv" << endl;
        return 1;
    }

    cout << "Reaing line 1" << endl;
    getline(inFS, line);

    cout << "heading: " << line << endl;

    int numObservations = 0;
    while(inFS.good()){

        getline(inFS, index_in, ',');
        getline(inFS, pclass_in, ',');
        getline(inFS, survived_in, ',');
        getline(inFS, sex_in, ',');
        getline(inFS, age_in, '\n');

        if(numObservations < 800){
            tr_pclass.at(numObservations) = stof(pclass_in);
            tr_survived.at(numObservations) = stof(survived_in);
            tr_sex.at(numObservations) = stof(sex_in);
            tr_age.at(numObservations) = stof(age_in);
        }
        else{
            te_pclass.at(numObservations-800) = stof(pclass_in);
            te_survived.at(numObservations-800) = stof(survived_in);
            te_sex.at(numObservations-800) = stof(sex_in);
            te_age.at(numObservations-800) = stof(age_in);
        }

        numObservations++;
    }

    tr_pclass.resize(800);
    tr_survived.resize(800);
    tr_sex.resize(800);
    tr_age.resize(800);

    te_pclass.resize(numObservations-800);
    te_survived.resize(numObservations-800);
    te_sex.resize(numObservations-800);
    te_age.resize(numObservations-800);

    cout << "Closing file titanic_project.csv." << endl;
    inFS.close();

    //TRAINING
    auto start = chrono::steady_clock::now();

    //PRIORS
    vector<double> apriori(2);
    vector<double> count_survived(2);
    apriori[0] = 0; //DID NOT SURVIVE
    apriori[1] = 0; //DID SURVIVE
    count_survived[0] = 0;
    count_survived[1] = 0;

    for(int i=0;i<tr_survived.size();i++){
        if(tr_survived[i]==1){ //IF SURVIVED
            apriori[1] = apriori[1] + 1;
            count_survived[1] = count_survived[1] + 1;
        }
        else{
            apriori[0] = apriori[0] + 1;
            count_survived[0] = count_survived[0] + 1;
        }
    }
    
    apriori[0] = apriori[0]/tr_survived.size();
    apriori[1] = apriori[1]/tr_survived.size();

    //QUALITATIVE LIKELIHOODS

    //PCLASS
    vector<double> lh_pclass(3);
    lh_pclass[0] = 0;
    lh_pclass[1] = 0;
    lh_pclass[2] = 0;

    for(int i=0;i<tr_survived.size();i++){
        if(tr_survived[i]==1){ //IF SURVIVED
            if(tr_pclass[i] == 1){
                lh_pclass[0] = lh_pclass[0] + 1;
            }
            else if(tr_pclass[i] == 2){
                lh_pclass[1] = lh_pclass[1] + 1;
            }
            else{
                lh_pclass[2] = lh_pclass[2] + 1;
            }
        }
    }
    lh_pclass[0] = lh_pclass[0]/count_survived[1];
    lh_pclass[1] = lh_pclass[0]/count_survived[1];
    lh_pclass[2] = lh_pclass[0]/count_survived[1];

    //SEX
    vector<double> lh_sex(2);
    lh_sex[0] = 0;
    lh_sex[1] = 0;
    for(int i=0;i<tr_survived.size();i++){
        if(tr_survived[i]==1){
            if(tr_sex[i] == 0){
                lh_sex[0] = lh_sex[0] + 1;
            }
            else if(tr_pclass[i] == 1){
                lh_sex[1] = lh_sex[1] + 1;
            }
        }
    }
    lh_sex[0] = lh_sex[0]/count_survived[1];
    lh_sex[1] = lh_sex[1]/count_survived[1];

    //QUANTITATIVE LIKELIHOOD

    //AGE
    vector<double> age_mean(2);
    age_mean[0] = 0;
    age_mean[1] = 0;
    vector<double> age_no(2);
    age_no[0] = 0;
    age_no[1] = 0;
    vector<double> age_var(2);
    age_var[0] = 0;
    age_var[1] = 0;
    for(int i=0;i<tr_survived.size();i++){
        if(tr_survived[i]==0){
            age_mean[0] += tr_age[i];
            age_no[0]++;
        }
        else{
            age_mean[1] += tr_age[i];
            age_no[1]++;
        }
    }
    age_mean[0] /= age_no[0];
    age_mean[1] /= age_no[1];

    //s^2 = (SUM(xi - mean_x)^2)/n-1
    double sum1 = 0;
    double sum2 = 0;
    for(int i=0;i<tr_survived.size();i++){
        if(tr_survived[i]==0){
            sum1 += pow(tr_age[i]-age_mean[0],2);
        }
        else{
            sum2 += pow(tr_age[i]-age_mean[1],2);
        }
    }
    age_var[0] = sqrt(sum1/(age_no[0]-1));
    age_var[1] = sqrt(sum1/(age_no[1]-1));

    //PREDICTING
    vector<double> pred(numObservations-800);

    for(int i=0;i<te_survived.size();i++){
        //CALCULATE NUM_S
        double num_s = lh_pclass[te_pclass[i]] * lh_sex[te_sex[i]] * apriori[1] * calc_age_lh(te_age[i], age_mean[1], age_var[1]);

        //CALCULATE NUM_P
        double num_p = (1-lh_pclass[te_pclass[i]]) * (1-lh_sex[te_sex[i]]) * apriori[0] * calc_age_lh(te_age[i], age_mean[0], age_var[0]);

        //CALCULATE DENOMINATOR
        double denom = num_s + num_p;

        //cout << num_s / denom << " " << num_p / denom;
        //dont really care about prob that deceased, can just use prob survived
        if(num_s/denom > .5){
            pred[i] = 1;
        }
        else{
            pred[i] = 0;
        }
    }

    auto end = chrono::steady_clock::now();
    cout << "Completed in " << chrono::duration_cast<chrono::seconds>(end-start).count() << " seconds.";

    double acc = accuracy(pred, te_survived);
    double sens = sensitivity(pred, te_survived);
    double spec = specificity(pred, te_survived);

    cout << endl <<  "Accuracy: " << acc << "\nSensitivity: " << sens << "\nSpecificity: " << spec;
}