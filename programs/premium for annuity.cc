#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/random.hpp>
#include "premium for annuity.h"

void policy(float personage, bool persongender, bool personsmoker, float A, float B, float c, float i, float m, const float fudgeFactor[2][2],  std::vector<float> t[2][2][3]) {
    float delta = std::log(1+i);
    float pvuap = PVUnitAnnualPayment2(personage, A, B, c, i, t[persongender][personsmoker], 0, fudgeFactor[persongender][personsmoker]);
    float pvuap_10 = PVUnitAnnualPayment2(personage+10, A, B, c, i, t[persongender][personsmoker], 0, fudgeFactor[persongender][personsmoker]);
    float fom = ForceOfMortality(personage, A, B, c);
    float fom_10 = ForceOfMortality(personage+10, A, B, c);
    float a = small_a(pvuap, m, delta, fom);
    float a_10= small_a(pvuap_10, m, delta, fom_10);
    float bigA = big_a(a, m, i);
    
    float pvAnnuity = PVUnitAnnuity(personage, A, B, c, i, t[persongender][personsmoker], 0, fudgeFactor[persongender][personsmoker]);
    
    float a_reserve = small_a(pvAnnuity, m, delta, fom);
    float bigA_reserve = big_a(a_reserve, m, i);
    
    float premium = Premium(pvuap, fom, delta, m, a, bigA, a_10, i,  personage, B, c, fudgeFactor[persongender][personsmoker]);
    float res = reserve(personage, premium, bigA_reserve, a_reserve);
    float annres = AnnualReserve(res, premium, personage, i, A, B, c, fudgeFactor[persongender][personsmoker]);
    std::cout << personage << ' '
    << (persongender?'M':'F') << ' '
    << (personsmoker?'Y':'N') << ' '
    << premium << ' '
    << res << ' '
    << annres << ' '
    << premium*a
    << std::endl;
    std::cout << "areserve="<< a_reserve<<  std::endl;
    std::cout << "Areserve="<< bigA_reserve<<  std::endl;
    std::cout << "a="<< a<<  std::endl;
    std::cout << "A="<< bigA<<  std::endl;
}

int main(int argc, char** argv) {
	
    const float A = 0.0f;
    const float B = 0.0000027f;
    const float c = 1.124f;
    
    const float i = 0.05f;
    const float m = 12.0f;
    
    const float fudgeFactor[2][2] = {
        {0.98f, 0.95f},
        {0.97f, 0.93f}
    };
    
    const float testages[] = {60.0f, 65.0f, 70.0f, 75.0f, 80.0f};
    
    // testing
    for(int ai=0; ai<(sizeof(testages)/sizeof(testages[0])); ai++) {
        for(int gi=0; gi<2; gi++) {
            for(int si=0; si<2; si++) {
                policy(testages[ai], gi, si, A, B, c, i, m, fudgeFactor, 0);
            }
        }
    }
    
    const char* files[2][2] = {
        {"female_non-smokers_two_years_select", "female_smokers_two_years_select"}, // female
        {"male_non-smokers_two_years_select", "male_smokers_two_years_select"}, // male
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
        policy(personage, persongender, personsmoker, A, B, c, i, m, fudgeFactor, t);
    }
}

