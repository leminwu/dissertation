#include <cmath>
#include <vector>
using namespace std;

float SurvivalRateUltimate(int k, int x, float A, float B, float c) {
	return std::exp((-B / log(c))*pow(c,x)*(pow(c,k)-1));
}

float SurvivalRateSelect(int k, int x, float A, float B, float c) {
	return exp(pow(0.9,2-k) * ((1-pow(0.9,k)) * A) / log(0.9) + (pow(c,k) - pow(0.9,k) * B * pow(c,x)) / log(0.9/c));
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
        float peoplesAreDead = 130.0f;
        float sum = 0.0f;
        float V = 1.0f/(1.0f+i);

	for(int k=t; k<std::min(2,int(peoplesAreDead-age)); k++) {
		sum += std::pow(V, k) *SurvivalRateSelect(k,age,A,B,c) ; 
	}
	for(int k=std::max(2,t); k<int(peoplesAreDead-age); k++) {
        sum += std::pow(V, k) * fudgeFactor * SurvivalRateUltimate(k, age+2, A, B, c);
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
    float b = 0.01f;
    float initial=400.0f;
    return (S*A)/((1.0f - b)*a - 0.4f);
}


