A quantum coin flip is a great minimal project: put a qubit in equal superposition with a Hadamard gate, then "measure" it — the act of measurement collapses it to 0 or 1 with 50/50 probability, mimicking a fair coin (but rooted in real quantum mechanics rather than classical randomness).
How it works:

A qubit starts at |0⟩ = [1, 0].
The Hadamard gate transforms it into (1/√2)[1, 1] — equal superposition.
Measurement uses the Born rule: probability of each outcome is the squared magnitude of its amplitude (here, exactly 50%).
Each "flip" creates a fresh qubit, applies H, and samples a result — just like a fair coin, but grounded in actual quantum formalism.

Some natural extensions if you want to take it further:

Biased coin: replace H with a custom rotation gate (e.g. R_y(θ)) to get non-50/50 probabilities.
Multi-qubit dice: use 3 qubits in superposition to simulate a fair 8-sided die (or reject/retry for 6 sides).
Entangled coins: two qubits in a Bell state, where flip outcomes become correlated — a good next step toward understanding entanglement.
