#include <iostream>
#include <string>

using namespace std;

const string CLEAR_SCREEN = "\033[2J\033[H";
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string CYAN = "\033[36m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string RED = "\033[31m";

void clearScreen() {
    cout << CLEAR_SCREEN << flush;
}

string readLine(const string& prompt = "") {
    if (!prompt.empty()) {
        cout << prompt;
    }

    string input;
    if (!getline(cin, input)) {
        return "";
    }

    return input;
}

int readChoice() {
    string input = readLine("Enter choice: ");

    if (!cin) {
        return 0;
    }

    try {
        return stoi(input);
    } catch (...) {
        return -1;
    }
}

void pauseScreen() {
    readLine("\nPress Enter to continue...");
}

void homeScreen() {
    clearScreen();

    cout << BOLD << CYAN;
    cout << "+==================================================+\n";
    cout << "|             ONLINE GDB CONSOLE GUI               |\n";
    cout << "+==================================================+\n";
    cout << RESET;

    cout << "|                                                  |\n";
    cout << "|   [1] Login                                      |\n";
    cout << "|   [2] Register                                   |\n";
    cout << "|   [3] About                                      |\n";
    cout << "|   [0] Exit                                       |\n";
    cout << "|                                                  |\n";
    cout << "+==================================================+\n\n";
}

void loginScreen() {
    clearScreen();

    cout << BOLD << CYAN;
    cout << "+------------------- LOGIN ------------------------+\n";
    cout << RESET << "\n";

    string username = readLine("Username: ");
    string password = readLine("Password: ");

    cout << "\n";

    if (username.empty()) {
        cout << RED << "Error: Username cannot be empty.\n" << RESET;
    } else if (password.empty()) {
        cout << RED << "Error: Password cannot be empty.\n" << RESET;
    } else {
        cout << GREEN << "Login submitted successfully.\n" << RESET;
        cout << GREEN << "Welcome, " << username << "!\n" << RESET;
    }

    pauseScreen();
}

void registerScreen() {
    clearScreen();

    cout << BOLD << CYAN;
    cout << "+------------------ REGISTER ----------------------+\n";
    cout << RESET << "\n";

    string username = readLine("Choose username: ");
    string password = readLine("Choose password: ");
    string confirmPassword = readLine("Confirm password: ");

    cout << "\n";

    if (username.empty()) {
        cout << RED << "Error: Username cannot be empty.\n" << RESET;
    } else if (password.empty()) {
        cout << RED << "Error: Password cannot be empty.\n" << RESET;
    } else if (password != confirmPassword) {
        cout << RED << "Error: Passwords do not match.\n" << RESET;
    } else {
        cout << GREEN << "Registration successful.\n" << RESET;
        cout << GREEN << "Account created for: " << username << "\n" << RESET;
    }

    pauseScreen();
}

void aboutScreen() {
    clearScreen();

    cout << BOLD << CYAN;
    cout << "+------------------- ABOUT ------------------------+\n";
    cout << RESET << "\n";

    cout << "This is a text-based GUI-style program.\n";
    cout << "It runs in OnlineGDB because OnlineGDB supports\n";
    cout << "console programs, not real graphical GUI programs.\n\n";

    cout << "For real GUI programs, you need a local environment\n";
    cout << "with a graphical library such as Qt, GTK, SFML,\n";
    cout << "SDL2, raylib, or Win32.\n";

    pauseScreen();
}

int main() {
    bool running = true;

    while (running) {
        homeScreen();

        int choice = readChoice();

        if (!cin) {
            break;
        }

        switch (choice) {
            case 1:
                loginScreen();
                break;

            case 2:
                registerScreen();
                break;

            case 3:
                aboutScreen();
                break;

            case 0:
                running = false;
                clearScreen();
                cout << GREEN << "Program closed. Goodbye!\n" << RESET;
                break;

            default:
                clearScreen();
                cout << RED << "Invalid option. Please choose 0, 1, 2, or 3.\n" << RESET;
                pauseScreen();
                break;
        }
    }

    return 0;
}
