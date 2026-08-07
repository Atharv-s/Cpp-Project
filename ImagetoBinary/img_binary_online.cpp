/*
================================================================================
  IMAGE <-> BINARY CONVERTER
  Compatible with: OnlineGDB, Replit, Godbolt, cpp.sh, ideone
  No external libraries — pure C++11
================================================================================

  WHAT IT DOES:
    MODE 1 (DEMO):    Auto-runs a round-trip test with a built-in tiny PNG
    MODE 2 (ENCODE):  Paste base64 image data -> get binary text output
    MODE 3 (DECODE):  Paste binary text -> get base64 image data back

  HOW TO USE ON OnlineGDB:
    1. Paste this entire file into the editor
    2. Click "Run"
    3. Follow the menu prompts in the terminal

================================================================================
*/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <cstring>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  BASE64 ENCODE / DECODE
//  Used so users can paste image data as printable text
// ─────────────────────────────────────────────────────────────

static const string B64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64Encode(const vector<uint8_t>& data) {
    string out;
    int i = 0;
    uint8_t a3[3], a4[4];
    size_t len = data.size();
    size_t idx = 0;

    while (len--) {
        a3[i++] = data[idx++];
        if (i == 3) {
            a4[0] = (a3[0] & 0xFC) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) | ((a3[1] & 0xF0) >> 4);
            a4[2] = ((a3[1] & 0x0F) << 2) | ((a3[2] & 0xC0) >> 6);
            a4[3] = a3[2] & 0x3F;
            for (int j = 0; j < 4; j++) out += B64_CHARS[a4[j]];
            i = 0;
        }
    }
    if (i) {
        for (int j = i; j < 3; j++) a3[j] = 0;
        a4[0] = (a3[0] & 0xFC) >> 2;
        a4[1] = ((a3[0] & 0x03) << 4) | ((a3[1] & 0xF0) >> 4);
        a4[2] = ((a3[1] & 0x0F) << 2) | ((a3[2] & 0xC0) >> 6);
        a4[3] = a3[2] & 0x3F;
        for (int j = 0; j < i + 1; j++) out += B64_CHARS[a4[j]];
        while (i++ < 3) out += '=';
    }
    return out;
}

vector<uint8_t> base64Decode(const string& encoded) {
    int i = 0;
    uint8_t a3[3], a4[4];
    vector<uint8_t> out;
    string in;

    // Strip whitespace
    for (char c : encoded)
        if (B64_CHARS.find(c) != string::npos || c == '=')
            in += c;

    size_t len = in.size();
    size_t idx = 0;

    while (len-- && in[idx] != '=' && B64_CHARS.find(in[idx]) != string::npos) {
        a4[i++] = in[idx++];
        if (i == 4) {
            for (int j = 0; j < 4; j++)
                a4[j] = (uint8_t)B64_CHARS.find(a4[j]);
            a3[0] = (a4[0] << 2) | (a4[1] >> 4);
            a3[1] = (a4[1] << 4) | (a4[2] >> 2);
            a3[2] = (a4[2] << 6) | a4[3];
            for (int j = 0; j < 3; j++) out.push_back(a3[j]);
            i = 0;
        }
    }
    if (i) {
        for (int j = i; j < 4; j++) a4[j] = 0;
        for (int j = 0; j < 4; j++)
            a4[j] = (uint8_t)(B64_CHARS.find(a4[j]) == string::npos ? 0 : B64_CHARS.find(a4[j]));
        a3[0] = (a4[0] << 2) | (a4[1] >> 4);
        a3[1] = (a4[1] << 4) | (a4[2] >> 2);
        a3[2] = (a4[2] << 6) | a4[3];
        for (int j = 0; j < i - 1; j++) out.push_back(a3[j]);
    }
    return out;
}

// ─────────────────────────────────────────────────────────────
//  BINARY CONVERSION CORE
// ─────────────────────────────────────────────────────────────

// bytes -> binary string (space-separated 8-bit groups, 8 per line)
string bytesToBinary(const vector<uint8_t>& bytes) {
    ostringstream oss;
    for (size_t i = 0; i < bytes.size(); i++) {
        for (int b = 7; b >= 0; b--)
            oss << ((bytes[i] >> b) & 1);
        if ((i + 1) % 8 == 0 || i + 1 == bytes.size())
            oss << "\n";
        else
            oss << " ";
    }
    return oss.str();
}

// binary string -> bytes
// accepts spaces, newlines freely
vector<uint8_t> binaryToBytes(const string& binText) {
    vector<uint8_t> bytes;
    string token;
    istringstream iss(binText);
    while (iss >> token) {
        // skip header comment tokens
        if (token[0] == '#') {
            string rest; getline(iss, rest);
            continue;
        }
        if (token.size() != 8)
            throw runtime_error("Expected 8-bit group, got: \"" + token + "\"");
        for (char c : token)
            if (c != '0' && c != '1')
                throw runtime_error("Invalid character in binary: '" + string(1,c) + "'");
        uint8_t byte = 0;
        for (int b = 0; b < 8; b++)
            byte = (byte << 1) | (token[b] == '1' ? 1 : 0);
        bytes.push_back(byte);
    }
    return bytes;
}

// ─────────────────────────────────────────────────────────────
//  FORMAT DETECTOR (magic bytes)
// ─────────────────────────────────────────────────────────────

string detectFormat(const vector<uint8_t>& b) {
    if (b.size() >= 4 && b[0]==0x89 && b[1]==0x50) return "PNG";
    if (b.size() >= 3 && b[0]==0xFF && b[1]==0xD8) return "JPEG";
    if (b.size() >= 3 && b[0]==0x47 && b[1]==0x49) return "GIF";
    if (b.size() >= 2 && b[0]==0x42 && b[1]==0x4D) return "BMP";
    if (b.size()>=12  && b[0]==0x52 && b[8]==0x57) return "WebP";
    return "Unknown";
}

// ─────────────────────────────────────────────────────────────
//  BUILT-IN TEST PNG  (2x2 gradient, valid PNG file)
// ─────────────────────────────────────────────────────────────

// This is a real, minimal PNG generated by Python zlib/struct.
// It's embedded as base64 so it survives copy-paste on any platform.
static const string DEMO_PNG_B64 =
    "iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAIAAAD91JpzAAAADklEQVQI12P4"
    "z8BQDwAEgAF/QualIQAAAABJRU5ErkJggg==";

// ─────────────────────────────────────────────────────────────
//  PRINT HELPERS
// ─────────────────────────────────────────────────────────────

void printSep(char c = '-', int n = 60) {
    cout << string(n, c) << "\n";
}

void printHeader(const string& title) {
    printSep('=');
    cout << "  " << title << "\n";
    printSep('=');
}

// Word-wrap long lines for terminal display
void printWrapped(const string& text, int width = 72, const string& prefix = "  ") {
    size_t pos = 0;
    while (pos < text.size()) {
        cout << prefix << text.substr(pos, width) << "\n";
        pos += width;
    }
}

// ─────────────────────────────────────────────────────────────
//  STATS
// ─────────────────────────────────────────────────────────────

void printStats(size_t byteCount) {
    cout << "  Bytes   : " << byteCount << "\n";
    cout << "  Bits    : " << byteCount * 8 << "\n";
    cout << "  Binary lines (8 bytes/line): "
         << (byteCount + 7) / 8 << "\n";
}

// ─────────────────────────────────────────────────────────────
//  MODE 1: DEMO — automatic round-trip test
// ─────────────────────────────────────────────────────────────

void runDemo() {
    printHeader("DEMO: Automatic Round-Trip Test");

    // 1. Decode built-in base64 PNG to bytes
    vector<uint8_t> original = base64Decode(DEMO_PNG_B64);
    cout << "\n[STEP 1] Built-in test image loaded\n";
    cout << "  Format  : " << detectFormat(original) << "\n";
    printStats(original.size());

    // 2. Bytes -> binary text
    cout << "\n[STEP 2] Converting bytes to binary text...\n";
    string binText = bytesToBinary(original);

    cout << "\n  First 3 lines of binary output:\n";
    cout << string(60, '-') << "\n";
    istringstream preview(binText);
    string line;
    for (int i = 0; i < 3 && getline(preview, line); i++)
        cout << "  " << line << "\n";
    cout << "  ...\n";
    cout << string(60, '-') << "\n";

    // 3. Binary text -> bytes back
    cout << "\n[STEP 3] Parsing binary text back to bytes...\n";
    vector<uint8_t> recovered = binaryToBytes(binText);
    printStats(recovered.size());

    // 4. Verify
    cout << "\n[STEP 4] Verifying...\n";
    bool match = (original == recovered);
    cout << (match
        ? "  RESULT: PASS - Byte-for-byte identical!\n"
        : "  RESULT: FAIL - Mismatch!\n");

    if (match) {
        // Show first 16 bytes as both hex and binary for illustration
        cout << "\n  First 16 bytes breakdown:\n";
        cout << string(60, '-') << "\n";
        cout << "  BYTE#  HEX   BINARY\n";
        cout << string(60, '-') << "\n";
        for (size_t i = 0; i < min((size_t)16, original.size()); i++) {
            cout << "  [" << setw(3) << i << "]  "
                 << "0x" << hex << uppercase << setw(2) << setfill('0')
                 << (int)original[i] << dec << setfill(' ') << "  ";
            for (int b = 7; b >= 0; b--)
                cout << ((original[i] >> b) & 1);
            cout << "\n";
        }
        cout << string(60, '-') << "\n";
    }

    // 5. Base64 of recovered (to show it's a valid image)
    cout << "\n  Base64 of recovered image (paste in browser to verify):\n";
    cout << string(60, '-') << "\n";
    string b64 = base64Encode(recovered);
    cout << "  data:image/png;base64,";
    printWrapped(b64, 68, "  ");
    cout << string(60, '-') << "\n";
}

// ─────────────────────────────────────────────────────────────
//  MODE 2: ENCODE — user pastes base64, gets binary
// ─────────────────────────────────────────────────────────────

void runEncode() {
    printHeader("ENCODE: Image Base64 -> Binary Text");

    cout << "\nHow to get your image as Base64:\n";
    cout << "  Option A (Linux/Mac terminal):\n";
    cout << "    base64 yourimage.png\n\n";
    cout << "  Option B (Python one-liner):\n";
    cout << "    python3 -c \"import base64; print(base64.b64encode(open('img.png','rb').read()).decode())\"\n\n";
    cout << "  Option C (Online tools):\n";
    cout << "    https://base64.guru/converter/encode/image\n\n";
    cout << "  Option D (Use built-in demo):\n";
    cout << "    Enter 'DEMO' to use the built-in test PNG\n\n";

    cout << "Paste Base64 image data (single line, then press Enter twice):\n";
    cout << "> ";

    string input, line;
    // Read until blank line
    while (getline(cin, line)) {
        if (line.empty()) break;
        input += line;
    }

    if (input == "DEMO" || input == "demo") {
        input = DEMO_PNG_B64;
        cout << "[Using built-in demo PNG]\n";
    }

    if (input.empty()) {
        cout << "No input received.\n";
        return;
    }

    cout << "\nDecoding Base64...\n";
    vector<uint8_t> bytes;
    try {
        bytes = base64Decode(input);
    } catch (...) {
        cout << "Error: invalid Base64 input.\n";
        return;
    }

    if (bytes.empty()) { cout << "Error: decoded to 0 bytes.\n"; return; }

    cout << "Format  : " << detectFormat(bytes) << "\n";
    printStats(bytes.size());

    cout << "\nConverting to binary...\n\n";
    string binText = bytesToBinary(bytes);

    cout << string(60, '=') << "\n";
    cout << "BINARY OUTPUT (copy everything below this line):\n";
    cout << string(60, '=') << "\n";
    cout << "# BYTES:" << bytes.size() << "\n";
    cout << "# FORMAT:" << detectFormat(bytes) << "\n";
    cout << binText;
    cout << string(60, '=') << "\n";
    cout << "END OF BINARY OUTPUT\n";
    cout << string(60, '=') << "\n";
}

// ─────────────────────────────────────────────────────────────
//  MODE 3: DECODE — user pastes binary, gets base64
// ─────────────────────────────────────────────────────────────

void runDecode() {
    printHeader("DECODE: Binary Text -> Image Base64");

    cout << "\nPaste binary data below.\n";
    cout << "Format: space-separated 8-bit groups, e.g.:\n";
    cout << "  10001001 01010000 01001110 ...\n\n";
    cout << "Enter 'END' on a new line when done:\n";
    cout << string(60, '-') << "\n";

    string allBinary;
    string line;
    int lineCount = 0;

    while (getline(cin, line)) {
        if (line == "END" || line == "end") break;
        allBinary += line + "\n";
        lineCount++;
        if (lineCount % 20 == 0)
            cerr << "  ...read " << lineCount << " lines\n";
    }

    if (allBinary.empty()) { cout << "No input received.\n"; return; }

    cout << "\nParsing " << lineCount << " lines of binary...\n";

    vector<uint8_t> bytes;
    try {
        bytes = binaryToBytes(allBinary);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
        return;
    }

    if (bytes.empty()) { cout << "Error: parsed 0 bytes.\n"; return; }

    cout << "Format  : " << detectFormat(bytes) << "\n";
    printStats(bytes.size());

    string b64 = base64Encode(bytes);

    cout << "\n" << string(60, '=') << "\n";
    cout << "RECOVERED IMAGE (Base64):\n";
    cout << "To view: paste the data URI below into your browser address bar\n";
    cout << string(60, '=') << "\n";

    // Print as data URI for easy browser preview
    cout << "data:" << "image/" << [&](){
        string f = detectFormat(bytes);
        if (f == "JPEG") return "jpeg";
        if (f == "PNG")  return "png";
        if (f == "GIF")  return "gif";
        if (f == "BMP")  return "bmp";
        if (f == "WebP") return "webp";
        return "png";
    }() << ";base64,";

    // Print base64 in 72-char lines
    size_t pos = 0;
    while (pos < b64.size()) {
        cout << b64.substr(pos, 72) << "\n";
        pos += 72;
    }
    cout << string(60, '=') << "\n";

    // Also print raw base64
    cout << "\nRaw Base64 only:\n";
    cout << string(60, '-') << "\n";
    pos = 0;
    while (pos < b64.size()) {
        cout << b64.substr(pos, 72) << "\n";
        pos += 72;
    }
    cout << string(60, '-') << "\n";
}

// ─────────────────────────────────────────────────────────────
//  MODE 4: MANUAL — type/paste raw binary, inspect it live
// ─────────────────────────────────────────────────────────────

void runManual() {
    printHeader("MANUAL: Type or Paste Binary, See Bytes");

    cout << "\nEnter 8-bit binary groups one at a time.\n";
    cout << "Type 'done' to finish, 'clear' to reset.\n\n";

    vector<uint8_t> bytes;
    string token;

    while (true) {
        cout << "[" << setw(4) << bytes.size() << " bytes] > ";
        cin >> token;

        if (token == "done" || token == "exit" || token == "q") break;
        if (token == "clear") { bytes.clear(); cout << "Cleared.\n"; continue; }
        if (token == "show") {
            cout << "\n  Collected bytes (" << bytes.size() << "):\n";
            for (size_t i = 0; i < bytes.size(); i++) {
                cout << "  [" << setw(3) << i << "]  "
                     << "0x" << hex << uppercase << setw(2) << setfill('0')
                     << (int)bytes[i] << dec << setfill(' ') << "  ";
                for (int b = 7; b >= 0; b--) cout << ((bytes[i] >> b) & 1);
                cout << "  (dec " << (int)bytes[i] << ")\n";
            }
            if (!bytes.empty())
                cout << "  Format: " << detectFormat(bytes) << "\n";
            cout << "\n";
            continue;
        }

        if (token.size() != 8) {
            cout << "  ! Expected exactly 8 bits, got " << token.size() << "\n";
            continue;
        }
        bool valid = true;
        for (char c : token) if (c != '0' && c != '1') { valid = false; break; }
        if (!valid) { cout << "  ! Only 0s and 1s allowed\n"; continue; }

        uint8_t byte = 0;
        for (int b = 0; b < 8; b++) byte = (byte << 1) | (token[b] == '1' ? 1 : 0);
        bytes.push_back(byte);

        cout << "  -> 0x" << hex << uppercase << setw(2) << setfill('0')
             << (int)byte << dec << setfill(' ')
             << "  (decimal " << (int)byte << ")\n";
    }

    if (!bytes.empty()) {
        cout << "\nFinal result: " << bytes.size() << " bytes\n";
        cout << "Format detected: " << detectFormat(bytes) << "\n";
        cout << "Base64:\n";
        string b64 = base64Encode(bytes);
        printWrapped(b64, 72);
    }
}

// ─────────────────────────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────────────────────────

int main() {
    cout << "\n";
    printSep('=');
    cout << "  IMAGE <-> BINARY CONVERTER\n";
    cout << "  Pure C++11 | Works on OnlineGDB, Replit, Godbolt\n";
    printSep('=');

    while (true) {
        cout << "\n";
        cout << "  [1] DEMO    - Auto round-trip test (no input needed)\n";
        cout << "  [2] ENCODE  - Your image (Base64) -> Binary text\n";
        cout << "  [3] DECODE  - Binary text -> Your image (Base64)\n";
        cout << "  [4] MANUAL  - Type binary bits, inspect byte values\n";
        cout << "  [0] EXIT\n";
        cout << "\n  Choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n'); // flush rest of line

        cout << "\n";
        switch (choice) {
            case 1: runDemo();   break;
            case 2: runEncode(); break;
            case 3: runDecode(); break;
            case 4: runManual(); break;
            case 0:
                cout << "Goodbye.\n";
                return 0;
            default:
                cout << "  Invalid choice. Enter 0-4.\n";
        }
    }

    return 0;
}
