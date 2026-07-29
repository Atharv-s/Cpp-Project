#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Task {
    string title;
    bool completed;
};

void showTasks(const vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\nNo tasks available.\n";
        return;
    }

    cout << "\n----- TO-DO LIST -----\n";
    for (size_t i = 0; i < tasks.size(); i++) {
        cout << i + 1 << ". ["
             << (tasks[i].completed ? 'X' : ' ')
             << "] " << tasks[i].title << endl;
    }
}

int main() {
    vector<Task> tasks;
    int choice;

    do {
        cout << "\n===== TO-DO APP =====\n";
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Mark Task as Completed\n";
        cout << "4. Delete Task\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            Task task;
            cout << "Enter task: ";
            getline(cin, task.title);
            task.completed = false;
            tasks.push_back(task);
            cout << "Task added successfully!\n";
            break;
        }

        case 2:
            showTasks(tasks);
            break;

        case 3: {
            showTasks(tasks);

            if (!tasks.empty()) {
                int index;
                cout << "Enter task number to mark as completed: ";
                cin >> index;

                if (index >= 1 && index <= tasks.size()) {
                    tasks[index - 1].completed = true;
                    cout << "Task marked as completed.\n";
                } else {
                    cout << "Invalid task number.\n";
                }
            }
            break;
        }

        case 4: {
            showTasks(tasks);

            if (!tasks.empty()) {
                int index;
                cout << "Enter task number to delete: ";
                cin >> index;

                if (index >= 1 && index <= tasks.size()) {
                    tasks.erase(tasks.begin() + index - 1);
                    cout << "Task deleted successfully.\n";
                } else {
                    cout << "Invalid task number.\n";
                }
            }
            break;
        }

        case 5:
            cout << "Exiting To-Do App. Goodbye!\n";
            break;

        default:
            cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}
