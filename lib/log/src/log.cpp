#include "log.h"

#include <unistd.h>
#include <limits.h>
#include <string>
#include <iostream>

std::string GetExecutableDir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path(result, (count > 0) ? count : 0);
    return path.substr(0, path.find_last_of('/')) + "/";
}

#ifdef DEBUG
    #ifndef LOG_FILE
    #define LOG_FILE GetExecutableDir() + "default.log"
    #endif
    #ifndef LOG_TEST_FILE
    #define LOG_TEST_FILE GetExecutableDir() + "default_test.log"
    #endif

    const std::string kLogFilePath = GetExecutableDir() + LOG_FILE;
    std::ofstream _log_file(kLogFilePath, std::ios::trunc);

    const std::string kLogTestFilePath = GetExecutableDir() + LOG_TEST_FILE;
    std::ofstream _log_test_file(kLogTestFilePath, std::ios::trunc);
#endif
