#include "common.h"

map<string, string> Utils::threadDetails = {};
mutex Utils::utilsMutex;

void Utils::addThreadInfo(string name) {
    std::ostringstream oss;
    oss << this_thread::get_id();
    threadDetails.insert({oss.str(), name});
}
