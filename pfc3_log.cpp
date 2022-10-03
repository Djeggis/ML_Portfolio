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

vector<double> sigmoid(vector<double> z){
    vector<double> sig(800);
    for(int i=0; i<z.size();i++){
        sig[i] = 1.0 / (1+exp(-z[i]));
    }
    return sig;
}

vector<double> matrix_mult(vector<double>z, double mult){
    vector<double> multV(z.size());
    for(int i=0; i<z.size();i++){
        multV[i] = z[i]*mult;
    }
    return multV;
}

double matrix_matrix_mult(vector<double>v1, vector<double>v2){
    double sum = 0;
    for(int i=0; i<v1.size();i++){
        sum += v1[i]*v2[i];
    }
    return sum;
}

vector<double> matrix_sub(vector<double>v1, vector<double>v2){
    vector<double> subV(800);
    for(int i=0; i<v1.size();i++){
        subV[i] = v1[i]-v2[i];
    }
    return subV;
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

    int grad_descent = 500000;
    cout << "Beginning training on data with descent of " << grad_descent << "..." << endl;
    vector<double>prob_vector(800);
    vector<double>error_vector(800);
    double weight = 1;
    double learning_rate = .001;
    

    for(int i = 0; i<grad_descent; i++){
        prob_vector = sigmoid(matrix_mult(tr_sex, weight));
        error_vector = matrix_sub(tr_survived, prob_vector);
        weight = weight + (learning_rate * matrix_matrix_mult(tr_sex,error_vector));
    }

    auto end = chrono::steady_clock::now();
    cout << "Training completed in " << chrono::duration_cast<chrono::seconds>(end-start).count() << " seconds." << endl << "Weight = " << weight << endl;
    

    //PREDICTING
    vector<double>predict_vector(numObservations-800);
    vector<double>prob2_vector(numObservations-800);
    vector<double>finalpred_vector(numObservations-800);

    //calculate predicted values
    predict_vector = matrix_mult(te_sex,weight);


    //calculate probabilties vector
    for(int i=0; i<predict_vector.size();i++){
        prob2_vector[i] = exp(predict_vector[i]) / (1+exp(predict_vector[i]));
        //cout << prob2_vector[i] << " ";
    }

    

    //calculate final prediction
    for(int i=0; i<prob2_vector.size();i++){
        if(prob2_vector[i]>=.5){
            finalpred_vector[i] = 1;
        }
        else{
            finalpred_vector[i] = 0;
        }
    }

    
    double acc = accuracy(finalpred_vector, te_survived);
    double sens = sensitivity(finalpred_vector, te_survived);
    double spec = specificity(finalpred_vector, te_survived);

    cout << endl <<  "Accuracy: " << acc << "\nSensitivity: " << sens << "\nSpecificity: " << spec;
}