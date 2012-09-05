include <iostream>

float reserve(float age, float premium, float A, float a) {
	return 100000.0f * A - (1-0.001)*premium*a;
}

int main(int argc, char** argv) {

	while(!std::cin.eof()) {
		float age;
		char smoker;
		char gender;
		float premium;
		std::cin >> age >> smoker >> gender >> premium;

		float a = 0.0f;

		std::cout << reserve(age, premium, A, a) << '\n';
	}


}
