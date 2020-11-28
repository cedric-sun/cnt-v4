// cesun, 11/28/20 3:38 AM.

#include "Logger.hpp"
#include <chrono>
#include <sstream>

std::string Logger::nowTime() {
    using namespace std::chrono;
    std::time_t now_c = system_clock::to_time_t(system_clock::now());
    std::tm now_tm = *std::localtime(&now_c);// copy the thread unsafe static buffer
    char buf[128];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", &now_tm);
    return std::string{buf};
}

std::string Logger::strVector(const std::vector<int> &vec) {
    std::stringstream ss;
    for (int i = 0; i < vec.size(); ++i) {
        if (i != 0) ss << ", ";
        ss << vec[i];
    }
    return ss.str();
}
