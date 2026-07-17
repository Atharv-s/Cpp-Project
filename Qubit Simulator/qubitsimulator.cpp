#include <iostream>
#include <complex>
#include <cmath>
#include <random>

using namespace std;

class Qubit {
private:
    complex<double> alpha;   // Amplitude of |0>
    complex<double> beta;    // Amplitude of |1>

public:
    // Start in |0>
    Qubit() {
        alpha = {1.0, 0.0};
        beta = {0.0, 0.0};
    }

    void printState() {
        cout << "\nCurrent State:\n";
        cout << "(" << alpha << ") |0> + ("
             << beta << ") |1>\n";
    }

    // Pauli-X Gate
    void X() {
        swap(alpha, beta);
    }

    // Hadamard Gate
    void H() {
        complex<double> newAlpha = (alpha + beta) / sqrt(2.0);
        complex<double> newBeta  = (alpha - beta) / sqrt(2.0);

        alpha = newAlpha;
        beta = newBeta;
    }

    // Measurement
    int measure() {
        double p0 = norm(alpha);

        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);

        double r = dis(gen);

        if (r < p0) {
            alpha = {1,0};
            beta = {0,0};
            return 0;
        } else {
            alpha = {0,0};
            beta = {1,0};
            return 1;
        }
    }
};

int main() {

    Qubit q;

    cout << "===== Single Qubit Simulator =====\n";

    q.printState();

    cout << "\nApplying Hadamard Gate...\n";
    q.H();
    q.printState();

    cout << "\nApplying X Gate...\n";
    q.X();
    q.printState();

    cout << "\nMeasuring...\n";

    int result = q.measure();

    cout << "Measurement Result = " << result << endl;

    q.printState();

    return 0;
}
