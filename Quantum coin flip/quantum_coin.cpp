// quantum_coin.cpp
//
// A "quantum coin flip" simulator.
//
// Concept:
//   A qubit's state is a complex vector [alpha, beta] where
//   |alpha|^2 + |beta|^2 = 1. |alpha|^2 is the probability of
//   measuring 0, |beta|^2 is the probability of measuring 1.
//
//   Starting from |0> = [1, 0], applying a Hadamard gate H gives:
//       H|0> = (1/sqrt(2)) * [1, 1]
//   This is an equal superposition: 50% chance of 0, 50% chance of 1.
//   "Measuring" the qubit collapses it to a definite classical bit,
//   which we use as our coin flip.
//
// Build:
//   g++ -std=c++17 -O2 quantum_coin.cpp -o quantum_coin
//
// Run:
//   ./quantum_coin 10000

#include <iostream>
#include <complex>
#include <array>
#include <random>
#include <cmath>
#include <string>

using Complex = std::complex<double>;

// A single-qubit state: {amplitude of |0>, amplitude of |1>}
struct Qubit {
    std::array<Complex, 2> amp = {Complex(1.0, 0.0), Complex(0.0, 0.0)}; // starts at |0>
};

// Apply a Hadamard gate: puts the qubit into equal superposition
void hadamard(Qubit& q) {
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    Complex a0 = q.amp[0];
    Complex a1 = q.amp[1];
    q.amp[0] = inv_sqrt2 * (a0 + a1);
    q.amp[1] = inv_sqrt2 * (a0 - a1);
}

// Measure the qubit: collapses state, returns 0 or 1
// according to the Born rule (probability = |amplitude|^2)
int measure(const Qubit& q, std::mt19937& rng) {
    double p0 = std::norm(q.amp[0]); // |alpha|^2
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double r = dist(rng);
    return (r < p0) ? 0 : 1;
}

int main(int argc, char** argv) {
    long long flips = 10000;
    if (argc > 1) {
        flips = std::stoll(argv[1]);
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    long long heads = 0, tails = 0;

    for (long long i = 0; i < flips; ++i) {
        Qubit q;              // fresh |0> qubit
        hadamard(q);          // put into superposition
        int result = measure(q, rng); // collapse -> classical bit

        if (result == 0) ++heads;
        else ++tails;
    }

    double p_heads = static_cast<double>(heads) / flips;
    double p_tails = static_cast<double>(tails) / flips;

    std::cout << "Quantum coin flip simulation\n";
    std::cout << "-----------------------------\n";
    std::cout << "Flips:        " << flips << "\n";
    std::cout << "Heads (|0>):  " << heads << "  (" << p_heads * 100.0 << "%)\n";
    std::cout << "Tails (|1>):  " << tails << "  (" << p_tails * 100.0 << "%)\n";

    return 0;
}
