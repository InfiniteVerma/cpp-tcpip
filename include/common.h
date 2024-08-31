#ifndef __RFC793_COMMON__
#define __RFC793_COMMON__

#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>

#include "iomanip"
using namespace std;

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

const std::string RESET = "\033[0m";
const std::string MAGENTA = "\033[35m";
const std::string GREEN = "\033[32m";
const std::string CYAN = "\033[36m";

class Utils {
   public:
    static void hexDump(char* data, int size) {
        cout << " =======\nHEX DUMP \n";
        const int bytesPerLine = 16;  // Number of bytes per line in the dump
        for (int i = 0; i < size; i += bytesPerLine) {
            // Print the offset
            std::cout << std::setw(8) << std::setfill('0') << std::hex << i << "  ";

            // Print the hex values
            for (int j = 0; j < bytesPerLine; ++j) {
                if (i + j < size) {
                    std::cout << std::setw(2) << std::setfill('0') << std::hex
                              << (static_cast<unsigned int>(static_cast<unsigned char>(data[i + j]))) << ' ';
                } else {
                    std::cout << "   ";  // Spaces for missing bytes
                }
            }

            std::cout << " ";

            // Print the ASCII representation
            for (int j = 0; j < bytesPerLine; ++j) {
                if (i + j < size) {
                    char c = data[i + j];
                    std::cout << (std::isprint(static_cast<unsigned char>(c)) ? c : '.');
                }
            }

            std::cout << std::endl;
        }
        cout << " =======\nHEX DUMP \n";
    }

    template <typename... Args>
    static void Log(const char* file, int line, Args... args) {
        ostringstream tmp;
        tmp << this_thread::get_id();
        ostringstream oss;

        string color = CYAN;
        if (threadDetails[tmp.str()] == "Kernel") color = GREEN;

        oss << "|" << threadDetails[tmp.str()].c_str() << "| ";
        // oss << "File: " << file << " Line: " << line << " ";
        (oss << ... << args);  // Fold expression to handle all args
        cout << color << oss.str() << RESET << endl;
    }

    static map<string, string> threadDetails;

    static void addThreadInfo(string);
};

// Variadic macro to capture all arguments and forward to Log function
#define LOG(...) Utils::Log(__FILE__, __LINE__, __VA_ARGS__)

#endif
