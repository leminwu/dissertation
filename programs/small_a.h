#include <cmath>
#include <vector>
using namespace std;

float SurvivalRateUltimate(int k, int x, float A, float B, float c, float fudgeFactor) {
	return fudgeFactor*std::exp((-B / log(c))*pow(c,x)*(pow(c,k)-1));
}

float SurvivalRateSelect(int k, int x, float A, float B, float c, float fudgeFactor) {
    float out = pow(0.9f,2-k);
    float first = ((1.0f-pow(0.9f,k)) * A) / log(0.9f);
    float second = ((pow(c,k) - pow(0.9f,k)) * B * pow(c,x)) / log(0.9f/c);
    //std::cout << "out = " << out << "\n";
    //std::cout << "first = " << first << "\n";
    //std::cout << "second = " << second << "\n";
	return fudgeFactor*exp(out * (first + second));
}

float AnnualReserve(float reserve, float P, float x, float i, float A, float B, float c, float fudgeFactor) {
    float V = 1.0f / (1.0f + i);
    float result = reserve;
    float S = 100000;
    float b = 0.02f;
    
    for(int t=9; t>1; t--) {
        float prev  = result;
        float p = SurvivalRateUltimate(1, x+t, A, B, c,fudgeFactor);
        result = V * (prev + (1.0f - p)*(S-prev)) - (1.0f - b)*P;
        
        float p1 = SurvivalRateUltimate(1, x+t-1, A, B, c,fudgeFactor);
        std::cout << result << ' ' ;
        std::cout << p1 << std::endl;
    }
    for(int t=1; t>=0; t--) {
        float prev=  result;
        float p = SurvivalRateSelect(1, x+t, A, B, c, fudgeFactor);
        result = V * (prev + (1.0f - p)*(S-prev)) - (1.0f - b)*P;
        
        float p1 = SurvivalRateUltimate(1, x+t-1, A, B, c,fudgeFactor);
        std::cout << result << ' ' ;
        std::cout << p1 << std::endl;
    }
    return result;
}

float PVUnitAnnualPayment(float age, float A, float B, float c, float i, vector<float> duration[3], int t, float fudgeFactor) {
	float peoplesAreDead = 130.0f;
	float sum = 0.0f;
	float V = 1.0f/(1.0f+i);
    for (int k=1; k<int(peoplesAreDead-age);k++){
        sum += pow(V,k)*(1.0f-duration[min(k-1,2)][int(age)+k-1]);
    }
    return sum;
}
    
float PVUnitAnnualPayment2(float age, float A, float B, float c, float i, vector<float> duration[3], int t, float fudgeFactor) {
        float peoplesAreDead = 120.0f;
        float sum = 0.0f;
        float V = 1.0f/(1.0f+i);

	for(int k=t; k<std::min(2,int(peoplesAreDead-age)); k++) {
        float term = std::pow(V, k) * SurvivalRateSelect(k,age,A,B,c, fudgeFactor);
        sum += term;
 //       std::cout << term << " + ";
	}
	for(int k=std::max(2,t); k<int(peoplesAreDead-age); k++) {
        float term = std::pow(V, k) 
            * SurvivalRateSelect(2, age, A, B, c, fudgeFactor)
            * SurvivalRateUltimate(k, age+2, A, B, c, fudgeFactor);
        sum += term;
//        std::cout << term << " + ";
	}
    std::cout << "\n";
	return sum;
}

float PVUnitAnnuity(float age, float A, float B,float c, float i, vector<float> duration[3], int t, float fudgeFactor) {
    float sum = 0.0f;
    float peoplesAreDead = 120.0f;
    float V = 1.0f/(1.0f+i);
    for(int k=0; k<int(peoplesAreDead-age); k++){
        float annuity_reserve= std::pow(V,k)*SurvivalRateUltimate(k,age+10,A,B,c, fudgeFactor);
        sum += annuity_reserve;
    }
    return sum;
}


float small_a(float pvUnitAnnualPayment, float m, float delta, float forceOfMortality) {
    return pvUnitAnnualPayment - (m-1)/(2*m) - ((m*m-1)/(12*m*m))*(delta+forceOfMortality);
}


float big_a(float a, float m, float i) {
	float V = 1.0f/(1.0f+i);
    return 1.0f - a*m*(1.0f - pow(V, 1.0f/m));
}

float ForceOfMortality(float age, float A, float B, float c) {
	return A + B*pow(c,age);
}

float Premium(float pvUnitAnnualPayment, float forceOfMortality, float delta, float m, float a, float A) {
    float S = 100000.0f;
    float b = 0.02f;
    float initial=400.0f;
    return (S*A)/((1.0f - b)*a - 0.4f);
}

float reserve(float age, float premium, float A, float a) {
	return 100000.0f * A - (1.0f-0.01f)*premium*a;
}
