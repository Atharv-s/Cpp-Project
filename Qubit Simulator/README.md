A qubit simulator represents a qubit as two complex amplitudes:

∣ψ⟩=α∣0⟩+β∣1⟩

where:

α and β are complex numbers.
∣α∣
2
+∣β∣
2
=1.

The program supports:

Initializing a qubit,
Applying Hadamard (H) gate,
Applying Pauli-X (NOT) gate,
Measuring the qubit

Possible Extensions

You can expand this simulator by adding:

Multiple qubits (2–10 qubits),
Gates: Y, Z, S, T, CNOT, SWAP,
Quantum circuit execution from a list of gates,
State-vector visualization,
Quantum Fourier Transform (QFT),
Grover's Search algorithm,
Shor's algorithm (small-scale simulation)

Those additions turn this into a more complete quantum circuit simulator while remaining entirely in standard C++.
