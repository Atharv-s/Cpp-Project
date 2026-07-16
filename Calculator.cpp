#include <iostream>
#include <limits>
#include <cmath>
#include <iomanip>

using namespace std;

void printMenu() {
    cout << "\n===== C++ Calculator =====\n";
    cout << "  1. Add        (+)\n";
    cout << "  2. Subtract   (-)\n";
    cout << "  3. Multiply   (*)\n";
    cout << "  4. Divide     (/)\n";
    cout << "  5. Power      (^)\n";
    cout << "  6. Square Root (sqrt)\n";
    cout << "  7. Modulus    (%)\n";
    cout << "  0. Exit\n";
    cout << "===========================\n";
    cout << "Choose an option: ";
}

// Reads a double safely, re-prompting on invalid input
double readNumber(const string& prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear(); // clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard bad input
    }
    return value;
}

int readOption() {
    int choice;
    while (!(cin >> choice)) {
        cout << "Invalid option. Please enter a number between 0 and 7: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return choice;
}

int main() {
    cout << fixed << setprecision(4);
    bool running = true;

    while (running) {
        printMenu();
        int choice = readOption();

        if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        }

        if (choice < 0 || choice > 7) {
            cout << "Please choose a valid option (0-7).\n";
            continue;
        }

        // Square root only needs one operand
        if (choice == 6) {
            double a = readNumber("Enter number: ");
            if (a < 0) {
                cout << "Error: cannot take square root of a negative number.\n";
            } else {
                cout << "Result: " << sqrt(a) << "\n";
            }
            continue;
        }

        double a = readNumber("Enter first number: ");
        double b = readNumber("Enter second number: ");
        double result = 0;
        bool valid = true;

        switch (choice) {
            case 1:
                result = a + b;
                break;
            case 2:
                result = a - b;
                break;
            case 3:
                result = a * b;
                break;
            case 4:
                if (b == 0) {
                    cout << "Error: division by zero.\n";
                    valid = false;
                } else {
                    result = a / b;
                }
                break;
            case 5:
                result = pow(a, b);
                break;
            case 7:
                if (b == 0) {
                    cout << "Error: modulus by zero.\n";
                    valid = false;
                } else {
                    result = fmod(a, b);
                }
                break;
        }

        if (valid) {
            cout << "Result: " << result << "\n";
        }
    }

    return 0;
}
