#include <iostream>
using namespace std;
namespace small_a {
	#include "small_a.h"
}

float SurvivalRate(float x, float t) {
	if(t < 2) {
		return small_a::SurvivalRateSelect(int k, int x, float A, float B, float c, float fudgeFactor);
	} else {
		return small_a::SurvivalRateUltimate(int k, int x, float A, float B, float c, float fudgeFactor);
	}
}

float SmallA() {
	float sum = 0.0f;	
	for(int i=0; i<) {
		sum += ;
	}
}

float Expense(int renewal, float indexFactor, float interestRate, float j) {

}

float Annuity(int S, float j) {

}

float Premium(float j) {
	int I;
	return Expense(renewal, indexFactor, interestRate) + Annuity(S, j) + I;
}

int main(int argc, char** argv) {
	cout << "" << endl;
	int renewal;
	float indexFactor;
	float j = (1.0 + indexFactor) / (1.0 + interestRate);
	return 0;
}

