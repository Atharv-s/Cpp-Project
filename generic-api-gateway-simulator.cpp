#include <iostream>
#include <string>

using namespace std;

const string API_KEY = "Secure123";

int main() {
    string key, endpoint, request;

    cout << "===== Secure API Gateway Simulator =====\n\n";

    cout << "Enter API Key: ";
    getline(cin, key);

    if (key != API_KEY) {
        cout << "\n401 Unauthorized\n";
        return 0;
    }

    cout << "Enter API Endpoint: ";
    getline(cin, endpoint);

    cout << "Enter Request Data: ";
    getline(cin, request);

    cout << "\n----- Gateway Processing -----\n";
    cout << "✓ Authentication Passed\n";
    cout << "✓ Input Validated\n";
    cout << "✓ Request Logged\n";
    cout << "✓ Rate Limit Check Passed\n";
    cout << "✓ Forwarding to Internal API\n";

    cout << "\n----- Internal API Response -----\n";
    cout << "{\n";
    cout << "  \"status\":\"success\",\n";
    cout << "  \"endpoint\":\"" << endpoint << "\",\n";
    cout << "  \"message\":\"Request processed successfully\"\n";
    cout << "}\n";

    return 0;
}