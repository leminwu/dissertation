#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <cstdlib>

float survival_rate(float a, float b, int x, std::vector<float>& l) {
    float rate = 1.0f - (l[x+1] / l[x]);
    if(b < 1.0f)
        return (1.0f - (a+b)*rate) / (1 - a*rate);
    else
        return (l[floor(x+a+b)] - ((x+a+b) - floor(x+a+b))*(l[floor(x+a+b)] - l[floor(x+a+b)+1])) / 
            (l[x] - a*(l[x]-l[x+1]));
}

int main(int argc, char** argv) {
    
    float a, b;
    int x;
    
    a = atof(argv[1]);
    b = atof(argv[2]);
    x = atoi(argv[3]);

    std::vector<float> CMI;
    std::copy(std::istream_iterator<float>(std::cin), std::istream_iterator<float>(), std::back_inserter(CMI));

    for(int i=0; i<CMI.size(); i++)
        std::cout << i << ":" << CMI[i] << '\n';

    std::cout << survival_rate(a, b, x, CMI) << '\n';

    return 1;
}

