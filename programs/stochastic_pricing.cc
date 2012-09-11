#include <iostream>
#include <boost/random.hpp>
#include <cmath>
#include <functional>
using namespace std;

float survival_rate[] = {
1,
1,
0.8946,
0.8445024,
0.839435386,
0.834398773,
0.829392381,
0.824416026,
0.81946953,
0.814552713,
0.809665397,
};

float test_data[] = {
0.0f,
0.95518f,
-2.45007f,
-1.23376f,
0.55824f,
-0.62022f,
0.01353f,
-1.22754f,
0.07758f,
-0.61893f,
-0.25283f,
};

bool negative(float i) { return i < 0.0f; }

void write_header() {
    cout << "t,zt,rt,P,Allocated P,Fund at t-,Management,Fund at t,Profit,pit,NPV\n";
}
void write_row(int t, float zt, float rt, float P, float allocP, float fundTminus, float management, float fundT, float profit, float pit, float NPV) {
    cout << t << ',' << zt << ',' << rt << ',' << P << ',' << allocP << ',' << fundTminus << ',' << management << ',' << fundT << ',' << profit << ',' << pit << ',' << NPV << '\n';
}

float whatevs(std::vector<float>& x) {
    float ssd = 0.f;
    float mean = accumulate(x.begin(), x.end(), 0.0f) / (float)x.size();
    for(int i=0; i<x.size(); i++) {
        ssd += (x[i] - mean)*(x[i] - mean);
    }
    return sqrt((1.0f/x.size()) * ssd);
}

int main(int argc, const char *argv[]) {
    boost::mt19937 generator;
    boost::normal_distribution<> distribution;
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > generate(generator, distribution);

    vector<float> npvs;

    for(int i=0; i<1000; i++) {
        write_header();
        write_row(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -(0.13f*5200.0f), -(0.13f*5200.0f), -(0.13f*5200.0f));
        float lastFund = 0.0f;
        float NPV = -520.0f; //initial expense change
        for(int t=1; t<=10; t++) {
            //float zt = test_data[t];
            float zt = generate();
            float rt = exp(0.074928f + 0.15f*zt);
            float P = 5200.0f;
            float allocP = (t==1) ? 0.95f*P : 0.99f*P;
            //lastFund at time 1 is 0
            float fundTminus = (allocP + lastFund) * rt;
            float management = fundTminus*0.008f; //management charge change
            float fundT = fundTminus-management;
            // 0.3 is because expense is 0.7%P, and unallocated premium is 1%P
            float two2nine = (t > 1) ? 0.3f : 1.0f;
            //add ii, the interest rate for insurer, which equal 0.06 for now
            float profit = (P - allocP) * (1.0f+0.05f) * two2nine + management - fundT*0.006f*0.1f; //expected death benefit
            float oldprofit = profit;
            if(t == 10) {
                profit -= 0.994f * (max(52000.0f - fundT, 0.0f));
            }
            float pit = profit * survival_rate[t];
            NPV += pit * pow(1.0f + 0.15f, -t);
            lastFund = fundT;
            if(t==10) {
                write_row(t, zt, rt, P, allocP, fundTminus, management, fundT, oldprofit, pit, NPV);
            }
            write_row(t, zt, rt, P, allocP, fundTminus, management, fundT, profit, profit * survival_rate[t], NPV);

        }
        //cout << NPV << '\n';
        npvs.push_back(NPV);
    }

    sort(npvs.begin(), npvs.end());

    cout << "NPV median = " << ((npvs[499]+npvs[500])/2.0f) << '\n';
    cout << "NPV fifth percentile = " << ((npvs[49]+npvs[50])/2.0f) << '\n';
    cout << "NPV ninety fifth percentile = " << ((npvs[949]+npvs[950])/2.0f) << '\n';
    cout << "NPV mean = " << accumulate(npvs.begin(), npvs.end(), 0.0f)/1000.0f << "\n";
    cout << "NPV sd = " << whatevs(npvs) << '\n';
    cout << "NPV count negative values = " << count_if(npvs.begin(), npvs.end(), negative) << '\n';
    cout << "95% CI for NPV = " << '(' << accumulate(npvs.begin(), npvs.end(), 0.0f)/1000.0f - 1.96f * whatevs(npvs)/sqrt(npvs.size()) << "," << accumulate(npvs.begin(), npvs.end(), 0.0f)/1000.0f + 1.96f * whatevs(npvs)/sqrt(npvs.size()) << ')' << '\n';
     
    return 0;
}
