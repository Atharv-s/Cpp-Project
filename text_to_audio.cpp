// text_to_audio.cpp
//
// A simple C++ text-to-audio (text-to-speech) program.
//
// It reads text either from the command line or standard input, and converts
// it to speech audio. It supports two backends, chosen automatically per
// platform:
//
//   - Linux/macOS: shells out to the "espeak-ng" command-line TTS engine
//     (install with: sudo apt-get install espeak-ng   on Debian/Ubuntu,
//                    brew install espeak-ng            on macOS)
//   - Windows: uses the built-in SAPI (Speech API) COM interface, so no
//     extra install is needed there.
//
// USAGE:
//   ./text_to_audio "Hello, world!"          -> speaks text directly
//   ./text_to_audio -o out.wav "Hello!"      -> saves speech to a WAV file
//   echo "Hello there" | ./text_to_audio      -> reads text from stdin
//   ./text_to_audio -o out.wav -s 150 -p 40 "Custom speed and pitch"
//
// COMPILE:
//   Linux/macOS:  g++ -std=c++17 -O2 -o text_to_audio text_to_audio.cpp
//   Windows:      cl text_to_audio.cpp  (MSVC; links ole32/sapi automatically
//                 via #pragma comment below)

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <array>

#ifdef _WIN32
    #include <windows.h>
    #include <sapi.h>
    #include <atlbase.h>
    #pragma comment(lib, "sapi.lib")
#else
    #include <cstdio>
#endif

struct Options {
    std::string text;
    std::string outFile;      // empty = speak directly, no file saved
    int speed = 175;          // words per minute (espeak default ~175)
    int pitch = 50;           // 0-99, 50 = default
    bool haveText = false;
};

void printUsage(const char* prog) {
    std::cerr <<
        "Usage: " << prog << " [options] [\"text to speak\"]\n"
        "Options:\n"
        "  -o <file>   Save audio to a WAV file instead of speaking aloud\n"
        "  -s <wpm>    Speech speed in words per minute (default 175)\n"
        "  -p <0-99>   Voice pitch (default 50)\n"
        "  -h          Show this help message\n"
        "If no text is given as an argument, text is read from stdin.\n";
}

Options parseArgs(int argc, char* argv[]) {
    Options opts;
    std::ostringstream textStream;
    bool textCaptured = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        } else if (arg == "-o" && i + 1 < argc) {
            opts.outFile = argv[++i];
        } else if (arg == "-s" && i + 1 < argc) {
            opts.speed = std::atoi(argv[++i]);
        } else if (arg == "-p" && i + 1 < argc) {
            opts.pitch = std::atoi(argv[++i]);
        } else {
            // Treat as (part of) the text to speak.
            if (textCaptured) textStream << " ";
            textStream << arg;
            textCaptured = true;
        }
    }

    if (textCaptured) {
        opts.text = textStream.str();
        opts.haveText = true;
    }
    return opts;
}

std::string readStdin() {
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    return ss.str();
}

// Escapes double quotes so text can be safely embedded in a shell command.
std::string shellEscape(const std::string& text) {
    std::string escaped;
    escaped.reserve(text.size() + 8);
    for (char c : text) {
        if (c == '"' || c == '\\' || c == '$' || c == '`') {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}

#ifndef _WIN32
// ---- Linux/macOS backend: shells out to espeak-ng ----
int speakLinux(const Options& opts) {
    std::ostringstream cmd;
    cmd << "espeak-ng -s " << opts.speed << " -p " << opts.pitch << " ";
    if (!opts.outFile.empty()) {
        cmd << "-w \"" << opts.outFile << "\" ";
    }
    cmd << "\"" << shellEscape(opts.text) << "\"";

    int result = std::system(cmd.str().c_str());
    if (result != 0) {
        std::cerr << "Error: espeak-ng failed to run. Is it installed?\n"
                     "Install it with: sudo apt-get install espeak-ng\n";
        return 1;
    }
    if (!opts.outFile.empty()) {
        std::cout << "Saved speech audio to: " << opts.outFile << "\n";
    } else {
        std::cout << "Spoke text aloud.\n";
    }
    return 0;
}
#else
// ---- Windows backend: uses SAPI directly, no external process needed ----
int speakWindows(const Options& opts) {
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        std::cerr << "COM initialization failed.\n";
        return 1;
    }

    CComPtr<ISpVoice> voice;
    hr = voice.CoCreateInstance(CLSID_SpVoice);
    if (FAILED(hr)) {
        std::cerr << "Failed to create SAPI voice instance.\n";
        CoUninitialize();
        return 1;
    }

    // Map 0-99 pitch/speed style options to SAPI's -10..10 rate scale.
    long rate = (opts.speed - 175) / 15; // rough mapping around default 175 wpm
    if (rate < -10) rate = -10;
    if (rate > 10) rate = 10;
    voice->SetRate(rate);

    std::wstring wtext(opts.text.begin(), opts.text.end());

    if (!opts.outFile.empty()) {
        CComPtr<ISpStream> stream;
        CComPtr<ISpAudioFormat> format; // not used directly; SAPI can infer default
        // Bind stream to a WAV file for output.
        CSpStreamFormat sf;
        sf.AssignFormat(SPSF_22kHz16BitMono);
        hr = SPBindToFile(std::wstring(opts.outFile.begin(), opts.outFile.end()).c_str(),
                           SPFM_CREATE_ALWAYS, &stream, &sf.FormatId(), sf.WaveFormatExPtr());
        if (SUCCEEDED(hr)) {
            voice->SetOutput(stream, TRUE);
        }
    }

    hr = voice->Speak(wtext.c_str(), SPF_DEFAULT, nullptr);
    CoUninitialize();

    if (FAILED(hr)) {
        std::cerr << "Speech synthesis failed.\n";
        return 1;
    }
    if (!opts.outFile.empty()) {
        std::cout << "Saved speech audio to: " << opts.outFile << "\n";
    } else {
        std::cout << "Spoke text aloud.\n";
    }
    return 0;
}
#endif

int main(int argc, char* argv[]) {
    Options opts = parseArgs(argc, argv);

    if (!opts.haveText) {
        opts.text = readStdin();
        // Trim trailing newline for cleanliness.
        while (!opts.text.empty() && (opts.text.back() == '\n' || opts.text.back() == '\r')) {
            opts.text.pop_back();
        }
    }

    if (opts.text.empty()) {
        std::cerr << "No text provided.\n";
        printUsage(argv[0]);
        return 1;
    }

#ifdef _WIN32
    return speakWindows(opts);
#else
    return speakLinux(opts);
#endif
}
