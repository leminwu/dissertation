#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <boost/random.hpp>
using namespace std;

double SurvivalRateUltimate(double A, double B, double c, int k, double age) {
    double ckminus1 = (pow(c,k)-1);
    double cx = pow(c,age);
    double boverlogc = (B / log10(c));
    double ak = A*k;
    double inner = -ak - boverlogc*cx*ckminus1;
    //cout << "ckminus1 = " << ckminus1 << endl;
    //cout << "cx = " << cx << endl;
    //cout << "boverlogc = " << boverlogc << endl;
    //cout << "ak = " << ak << endl;
    //cout << "inner = " << inner << endl;
    float result = exp(inner);
    cout << "SurvivalRateUltimate(int k="<<k<<", int age="<<age<<", double A="<<A<<", double B="<<B<<", double c="<<c<<") = " << result << endl;
	return result;
}

double SurvivalRateSelect(double A, double B, double c, int k, double age) {
    double out = pow(0.9f,2-k);
    double first = ((1.0f-pow(0.9f,k)) * A) / log10(0.9f);
    double second = ((pow(c,k) - pow(0.9f,k)) * B * pow(c,age)) / log10(0.9f/c);
	return exp(out * (first + second));
}

double SurvivalRateModel(double A, double B, double c, int t, double age) {
    if(t < 2) {
        return SurvivalRateSelect(A, B, c, t, age);
    } else {
        float twoYearSelect = SurvivalRateSelect(A, B, c, 2, age);
        float mult = SurvivalRateUltimate(A, B, c, t - 2, age + 2);
        return twoYearSelect * mult;
    }
}

double SurvivalRate(double A, double B, double c, int t, double age, vector<float> dur[3]) {
    double result;
    if(t == 0)
        result = (1.0 - dur[0][age+t]);
    else if(t == 1)
        result = (1.0 - dur[1][age+t]) / (1.0 - dur[0][age]);
    else
        result = (1.0 - dur[2][age+t]) / (1.0 - dur[0][age]);
    clog << "Survival rate is " << result << endl;
    return result;
}

double SmallA(double A, double B, double c, double age, double j, vector<float> dur[3]) {
    clog << "Calculating SmallA(age="<<age<<", j="<<j<<")" << endl;
	double sum = 0.0f;
	for(int i=0; i<(120 - (int)age); i++) {
		sum += pow(j, i) * SurvivalRate(A, B, c, i, age, dur);
	}
    return sum;
}

double Expense(double A, double B, double c, int renewal, double indexFactor, double age, double j, vector<float> dur[3]) {
    clog << "Calculating Expense(age="<<age<<")" << endl;
    double small_a = SmallA(A, B, c, age, j, dur);
    clog << "  small_a = " << small_a << endl;
    return (renewal / (1.0 + indexFactor)) * (small_a - 1.0);
}

double Annuity(double A, double B, double c, double indexFactor, double S, double j, double age, vector<float> dur[3]) {
    cout << "Calculate Annuity(age="<<age<<", j="<<j<<")" << endl;
    double sum = 0.0;
    for(int i=10; i<120-age; i++) {
        double term = SurvivalRate(A, B, c, i, age, dur);
        cout << "survival rate term for i="<<i<<" is " << term << endl;
        sum += pow(j,i) * term;
    }
    double coefficient = (S / pow(1.0 + indexFactor, 10));
    cout << "annuity coefficient is " << coefficient << endl;
    cout << "annuity sum is " << sum << endl;
    return coefficient * sum;
}

double Premium(double A, double B, double c, int I, double S, int renewal, double indexFactor, double age, double j, vector<float> dur[3]) {
    clog << "Calculating Premium(age="<<age<<", j="<<j<<")"<<endl;
    double expense = Expense(A, B, c, renewal, indexFactor, age, j, dur);
    double annuity = Annuity(A, B, c, indexFactor, S, j, age, dur);
    cout << "expense is " << expense << endl;
    cout << "annuity is " << annuity << endl;
	return expense + annuity + I;
}

void read_tables(vector<float> dur[2][2][3]) {
    const char* files[2][2] = {
        {"female_non-smokers_two_years_select", "female_smokers_two_years_select"}, // female
        {"male_non-smokers_two_years_select", "male_smokers_two_years_select"}, // male
    };

    for(int j=0; j<2; j++) {
        for(int k=0; k<2; k++) {
            cout << "Reading " << files[j][k] << "\n";
            dur[j][k][0].resize(130);
            dur[j][k][1].resize(130);
            dur[j][k][2].resize(130);
            ifstream f(files[j][k]);
            string line;
            getline(f, line); // file header
            while(!f.eof()) {
                int age;
                float duration0, duration1, duration2plus;
                f >> age >> duration0 >> duration1 >> duration2plus;
                dur[j][k][0][age] = duration0;
                dur[j][k][1][age] = duration1;
                dur[j][k][2][age] = duration2plus;
            }
            f.close();
        }
    }
}

int main(int argc, char** argv) {
    double A = 0.00022;
    double B = 0.0000027;
    double c = 1.124;
    double S = 50000;
	int I = 0;
	int renewal = 0;
	double indexFactor = 0.03;
    double interestRate = 0.05;
	double j = (1.0 + indexFactor) / (1.0 + interestRate);
    vector<float> dur[2][2][3];
    read_tables(dur);

    boost::mt19937 generator;
    boost::uniform_int<> boolean(0,1);
    boost::uniform_real<> agedistribution(55, 60);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gender(generator, boolean);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > smoker(generator, boolean);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > age(generator, agedistribution);

    for(int i=0; i<100; i++) {
        float personage = age();
        bool persongender = gender();
        bool personsmoker = smoker();
        cout << Premium(A, B, c, I, S, renewal, indexFactor, personage, j, dur[persongender][personsmoker]) << endl;
    }
	return 0;
}

