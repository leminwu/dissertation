#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/random.hpp>
#include "small_a.h"

float reserve(float age, float premium, float A, float a) {
	return 100000.0f * A - (1.0f-0.01f)*premium*a;
}


int main(int argc, char** argv) {
	
    const float A = 0.0f;
    const float B = 0.0000027f;
    const float c = 1.124f;


    const char* files[2][2] = {
        {"female_non-smokers_two_years_select", "female_smokers_two_years_select"}, // female
        {"male_non-smokers_two_years_select", "male_smokers_two_years_select"}, // male
    };
    
    float fudgeFactor[2][2] = {
        {0.85f, 0.8f},
        {0.75f, 0.7f}
    };

    std::vector<float> t[2][2][3]; // gender, smoker, duration

    for(int j=0; j<2; j++) {
        for(int k=0; k<2; k++) {
            std::cout << "Reading " << files[j][k] << "\n";
            t[j][k][0].resize(130);
            t[j][k][1].resize(130);
            t[j][k][2].resize(130);
            std::ifstream f(files[j][k]);
            std::string line;
            std::getline(f, line); // file header
            while(!f.eof()) {
                int age;
                float duration0, duration1, duration2plus;
                f >> age >> duration0 >> duration1 >> duration2plus;
                t[j][k][0][age] = duration0;
                t[j][k][1][age] = duration1;
                t[j][k][2][age] = duration2plus;
            }
            f.close();
        }
    }

    std::cout << "Generating people\n";
    boost::mt19937 generator;
    boost::uniform_int<> boolean(0,1);
    boost::uniform_real<> agedistribution(60, 80);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gender(generator, boolean);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > smoker(generator, boolean);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > age(generator, agedistribution);

    for(int j=0; j<100; j++) {
        float personage = age();
        bool persongender = gender();
        bool personsmoker = smoker();
//for loop for each year, 
        float i = 0.04f;
        float m = 12.0f;
        float delta = std::log(1+i);
        float pvuap = PVUnitAnnualPayment2(personage, A, B, c, i, t[persongender][personsmoker], 0, fudgeFactor[persongender][personsmoker]);
        float fom = ForceOfMortality(personage, A, B, c);
        float a = small_a(pvuap, m, delta, fom);
        float bigA = big_a(a, m, i);
        
        float a_reserve = small_a(pvuap, 1.0f, delta, fom);
        float bigA_reserve = big_a(a_reserve, 1.0f, i);
        float premium = Premium(pvuap, fom, delta, m, a, bigA);
        std::cout << personage << ' '
                << (persongender?'M':'F') << ' '
                << (personsmoker?'Y':'N') << ' '
                << premium << ' '
                << reserve(personage, premium, bigA_reserve, a_reserve)
                << std::endl;
        std::cout << "a="<< a_reserve<<  std::endl;
    }
}

