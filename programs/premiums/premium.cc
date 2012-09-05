#include <iostream>
#include <vector>
#include <cmath>

float SurvivalRateUltimate(int k, int x, float A, float B, float c) {
	return std::exp((-B / std::log(c))*std::pow(c,x)*(std::pow(c,k)-1));
}

float SurvivalRateSelect(int k, int x, float A, float B, float c) {
	return std::exp(std::pow(0.9,2-k) * ((1-std::pow(0.9,k)) * A) / std::log(0.9) + (std::pow(c,k) - std::pow(0.9,k) * B * std::pow(c,x)) / std::log(0.9/c));
}

float PVUnitAnnualPayment(float age, float A, float B, float c) {
	float peoplesAreDead = 130;
	float i;
	float sum = 0;
	float V = 1.0f/(1.0f+i);
	for(int k=0; k<std::min(2,int(peoplesAreDead-age)); k++) {
		sum += std::pow(V, k) * SurvivalRateSelect(k, age, A, B, c);
	}
	for(int k=2; k<int(peoplesAreDead-age); k++) {
		sum += std::pow(V, k) * SurvivalRateUltimate(k, age, A, B, c);
	}
	return sum;
}

float BenefitPayment(float pvUnitAnnualPayment, int paymentsPerYear, float forceOfMortality) {
	return 0.0f;
}

float ForceOfMortality(float age, float a, float b, float c) {
	return a + b*std::pow(c,age);
}

float ForceOfMortalitySelected(float age, float a, float b, float c, float s) {
	float mu = ForceOfMortality(age+s,a,b,c);
	return mu*std::pow(0.9f,2-s);
}

float Premium(float expenses, float pvBenefitPayment) {
	return 0.0f;
}

void test() {
	float epsilon = 0.0001f;
	if(std::abs(SurvivalRateSelect(0, 60, 0, 0, 0) - 1.0f) < epsilon) {
		std::cerr << "Test SurvivalRateSelect failed\n";
	}
}

int main(int argc, char** argv) {
	test();

	if(argc != 6) {
		std::cerr << "Incorrect arguments: premium age a b c expenses\n";
		return 0;
	}

	float age = atof(argv[1]);
	float a = atof(argv[2]);
	float b = atof(argv[3]);
	float c = atof(argv[4]);
	float expenses = atof(argv[5]);

	float pvUnitAnnualPayment = PVUnitAnnualPayment(age, a, b, c);

	const int paymentsPerYear = 12;
	float forceOfMortality = ForceOfMortality(age, a, b, c);
	float pvBenefitPayment = BenefitPayment(pvUnitAnnualPayment, paymentsPerYear, forceOfMortality);

	std::cout << Premium(expenses, pvBenefitPayment) << "\n";
}
