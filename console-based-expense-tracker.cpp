#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Expense {
    int id{};
    std::string date;       // YYYY-MM-DD
    std::string category;
    std::string description;
    double amount{};
};

class ExpenseTracker {
private:
    std::vector<Expense> expenses;
    std::string filename;
    int nextId{1};

    static std::string toLower(std::string text) {
        std::transform(text.begin(), text.end(), text.begin(),
                       [](unsigned char character) {
                           return static_cast<char>(std::tolower(character));
                       });
        return text;
    }

    static std::string escapeCsv(const std::string& text) {
        std::string result = "\"";

        for (char character : text) {
            if (character == '"') {
                result += "\"\"";
            } else {
                result += character;
            }
        }

        result += '"';
        return result;
    }

    static std::vector<std::string> parseCsvLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;
        bool insideQuotes = false;

        for (std::size_t i = 0; i < line.size(); ++i) {
            const char character = line[i];

            if (character == '"') {
                if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    insideQuotes = !insideQuotes;
                }
            } else if (character == ',' && !insideQuotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += character;
            }
        }

        fields.push_back(field);
        return fields;
    }

    static int readInteger(const std::string& prompt) {
        int value{};

        while (true) {
            std::cout << prompt;

            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                return value;
            }

            std::cout << "Invalid input. Enter a whole number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                            '\n');
        }
    }

    static double readAmount(const std::string& prompt) {
        double value{};

        while (true) {
            std::cout << prompt;

            if (std::cin >> value && value >= 0.0) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                return value;
            }

            std::cout << "Invalid amount. Enter a non-negative number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                            '\n');
        }
    }

    static std::string readText(const std::string& prompt) {
        std::string value;

        do {
            std::cout << prompt;
            std::getline(std::cin, value);

            if (value.empty()) {
                std::cout << "This field cannot be empty.\n";
            }
        } while (value.empty());

        return value;
    }

    auto findExpense(int id) {
        return std::find_if(
            expenses.begin(), expenses.end(),
            [id](const Expense& expense) { return expense.id == id; });
    }

    void printExpense(const Expense& expense) const {
        std::cout << std::left
                  << std::setw(5) << expense.id
                  << std::setw(12) << expense.date
                  << std::setw(16) << expense.category
                  << std::setw(28) << expense.description
                  << std::right << std::setw(12) << std::fixed
                  << std::setprecision(2) << expense.amount << '\n';
    }

    void printHeader() const {
        std::cout << '\n'
                  << std::left
                  << std::setw(5) << "ID"
                  << std::setw(12) << "Date"
                  << std::setw(16) << "Category"
                  << std::setw(28) << "Description"
                  << std::right << std::setw(12) << "Amount" << '\n';

        std::cout << std::string(73, '-') << '\n';
    }

public:
    explicit ExpenseTracker(std::string file)
        : filename(std::move(file)) {
        load();
    }

    void load() {
        std::ifstream input(filename);

        if (!input) {
            return;
        }

        std::string line;
        std::getline(input, line); // Skip CSV header.

        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }

            const auto fields = parseCsvLine(line);

            if (fields.size() != 5) {
                continue;
            }

            try {
                Expense expense;
                expense.id = std::stoi(fields[0]);
                expense.date = fields[1];
                expense.category = fields[2];
                expense.description = fields[3];
                expense.amount = std::stod(fields[4]);

                expenses.push_back(expense);
                nextId = std::max(nextId, expense.id + 1);
            } catch (const std::exception&) {
                // Ignore malformed records.
            }
        }
    }

    bool save() const {
        std::ofstream output(filename);

        if (!output) {
            std::cerr << "Error: could not save " << filename << ".\n";
            return false;
        }

        output << "id,date,category,description,amount\n";

        for (const Expense& expense : expenses) {
            output << expense.id << ','
                   << escapeCsv(expense.date) << ','
                   << escapeCsv(expense.category) << ','
                   << escapeCsv(expense.description) << ','
                   << std::fixed << std::setprecision(2)
                   << expense.amount << '\n';
        }

        return true;
    }

    void addExpense() {
        Expense expense;
        expense.id = nextId++;
        expense.date = readText("Date (YYYY-MM-DD): ");
        expense.category = readText("Category: ");
        expense.description = readText("Description: ");
        expense.amount = readAmount("Amount: ");

        expenses.push_back(expense);
        save();

        std::cout << "Expense added with ID " << expense.id << ".\n";
    }

    void viewExpenses() const {
        if (expenses.empty()) {
            std::cout << "No expenses recorded.\n";
            return;
        }

        printHeader();

        double total = 0.0;

        for (const Expense& expense : expenses) {
            printExpense(expense);
            total += expense.amount;
        }

        std::cout << std::string(73, '-') << '\n'
                  << std::right << std::setw(61) << "Total: "
                  << std::setw(12) << std::fixed << std::setprecision(2)
                  << total << '\n';
    }

    void editExpense() {
        const int id = readInteger("Enter expense ID to edit: ");
        auto iterator = findExpense(id);

        if (iterator == expenses.end()) {
            std::cout << "Expense not found.\n";
            return;
        }

        std::cout << "Enter the replacement details.\n";
        iterator->date = readText("Date (YYYY-MM-DD): ");
        iterator->category = readText("Category: ");
        iterator->description = readText("Description: ");
        iterator->amount = readAmount("Amount: ");

        save();
        std::cout << "Expense updated.\n";
    }

    void deleteExpense() {
        const int id = readInteger("Enter expense ID to delete: ");
        auto iterator = findExpense(id);

        if (iterator == expenses.end()) {
            std::cout << "Expense not found.\n";
            return;
        }

        expenses.erase(iterator);
        save();

        std::cout << "Expense deleted.\n";
    }

    void searchExpenses() const {
        const std::string query =
            toLower(readText("Search category or description: "));

        bool found = false;
        double subtotal = 0.0;

        for (const Expense& expense : expenses) {
            const std::string searchable =
                toLower(expense.category + " " + expense.description);

            if (searchable.find(query) != std::string::npos) {
                if (!found) {
                    printHeader();
                }

                printExpense(expense);
                subtotal += expense.amount;
                found = true;
            }
        }

        if (!found) {
            std::cout << "No matching expenses found.\n";
            return;
        }

        std::cout << std::string(73, '-') << '\n'
                  << "Matching total: " << std::fixed
                  << std::setprecision(2) << subtotal << '\n';
    }

    void run() {
        while (true) {
            std::cout
                << "\n=== Expense Tracker ===\n"
                << "1. Add expense\n"
                << "2. View all expenses\n"
                << "3. Edit expense\n"
                << "4. Delete expense\n"
                << "5. Search expenses\n"
                << "6. Exit\n";

            switch (readInteger("Choose an option: ")) {
                case 1:
                    addExpense();
                    break;
                case 2:
                    viewExpenses();
                    break;
                case 3:
                    editExpense();
                    break;
                case 4:
                    deleteExpense();
                    break;
                case 5:
                    searchExpenses();
                    break;
                case 6:
                    save();
                    std::cout << "Expenses saved. Goodbye.\n";
                    return;
                default:
                    std::cout << "Invalid option. Choose 1 through 6.\n";
            }
        }
    }
};

int main() {
    ExpenseTracker tracker("expenses.csv");
    tracker.run();
    return 0;
}
