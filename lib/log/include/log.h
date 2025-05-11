#ifndef LOG_H__
#define LOG_H__

#include <cassert>
#include <filesystem>
#include <iostream>

#define CHECK(exp, msg) assert((void(msg), (exp)))

#define LOG_INFO(msg) std::cout << msg << std::endl

#define TEST_INFO(msg) std::cout << msg << std::endl

#ifdef DEBUG
    #include <fstream>
    extern std::ofstream _log_file, _log_test_file;
    #define LOG_DEBUG(msg) _log_file << "DEBUG :: " << __FILE__ << ":" \
                                     << __LINE__ << " :: " << msg << std::endl
    #define TEST_DEBUG(msg) _log_test_file << "DEBUG :: " << __FILE__ << ":" \
                                     << __LINE__ << " :: " << msg << std::endl
#else
    #define LOG_DEBUG(msg)
    #define TEST_DEBUG(msg)
#endif

#define TEST_CHECK(exp, msg) \
    do { \
        if (!(exp)) { \
            TEST_DEBUG(std::string("CHECK FAILED: ") + #exp + " :: " + msg); \
            assert(exp); \
        } \
    } while (0)


#endif // LOG_H__
