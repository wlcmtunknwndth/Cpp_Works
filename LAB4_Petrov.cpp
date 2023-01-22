#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include "windows.h" 
#include <math.h>
#include <cstdlib>
using namespace std;

float k1, k2, k3, k4;
float ideal;
float d_ideal;
bool flag;


float func(float* params, float x) {
    return params[0] * pow(x, 3) + params[1] * pow(x, 2) + params[2] * x + params[3];
}

float dist(float* params, float* ideal_params, float x0, float x1, float dx) {
    float sum = 0;
    for (float x = x0 + dx; x <= x1; x += dx) {
        sum += (abs(func(params, x) - func(ideal_params, x)));
    }
    return sum;
}

float integral(float* params, float x0, float x1, float dx) {
    float sum = 0;
    for (float x = x0 + dx; x <= x1; x += dx) {
        sum += (func(params, x - dx) + func(params, x)) / 2 * dx;
    }
    return sum;
}

float trapezoidalIntegral(float* params, float a, float b, int n) {
    const float width = (b - a) / n;

    float trapezoidal_integral = 0;
    for (int step = 0; step < n; step++) {
        const float x1 = a + step * width;
        const float x2 = a + (step + 1) * width;

        trapezoidal_integral += 0.5 * (x2 - x1) * (func(params, x1) + func(params, x2));
    }

    return trapezoidal_integral;
}

class ML {
public:
    float* params;
    float delta;
    ML(float* params, float max_d, bool flag) {
        this->params = new float[4];
        for (int i = 0; i < 4; i++) {
            this->params[i] = params[i] + (rand() % 201 - 100) / 100.0 * max_d;
        }
        if (flag == 0)
            delta = abs(ideal - integral(this->params, -100, 100, 0.5));
        else
            delta = dist(this->params, new float [4] {k1, k2, k3, k4}, -100, 100, 0.5);
    }

    void output() {
        cout << "params: " << endl;
        for (int i = 0; i < 4; i++)
            cout << params[i] << " ";
        cout << "delta: " << delta << endl;
    };
};

ML generation(ML origin, int count, float max_d, bool flag) {
    ML news = origin;
    for (int i = 0; i < count; i++) {
        ML temp = ML(origin.params, max_d, flag);
        if (temp.delta < news.delta)
            news = temp;
    }
    return news;
}

int main() {
    srand(time(0));
    int n = 0;
    k1 = 1;
    k2 = 2;
    k3 = 3;
    k4 = 4;
    cin >> flag;
    if (!flag) {
        ideal = trapezoidalIntegral(new float[4] {k1, k2, k3, k4}, -100, 100, 1000000);
        //ideal = integral(new float[4] {k1, k2, k3, k4}, -100, 100, 0.5);
        cout << "Ideal: " << ideal << endl;
        Sleep(10);
        ML f(new float[4] {0.4, 0.5, 0.6, 0.5}, 0.5, 0);
        int count = 0; 
        while (f.delta != 0) {
            ML gen = generation(f, 100, 0.5, 0);
            f = gen;
            f.output();
            count++;
            if (count % 30 == 0) {
                f.output();
            }
        }
        f.output();
    }
    else {
        d_ideal = dist(new float[4] {k1, k2, k3, k4}, new float [4] {k1, k2, k3, k4}, -100, 100, 0.5);
        cout << "Ideal: " << d_ideal << endl;
        Sleep(10);
        ML d_f(new float[4] {0.4, 1.5, 2.6, 0.5}, 0.5, 1);
        int count = 0;
        while (d_f.delta > 1000) {
            ML gen = generation(d_f, 100, 0.5, 1);
            d_f = gen;
            count++;
            if (count % 30 == 0) {
                d_f.output();
            }
        }
        d_f.output();
        cout << count << endl;
    }
    cout << k1 << " " << k2 << " " << k3 << " " << k4 << " " << endl;
    return 0;
}
