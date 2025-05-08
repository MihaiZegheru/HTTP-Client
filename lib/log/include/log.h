#ifndef LOG_H__
#define LOG_H__

#include <cassert>
#include <iostream>

#define CHECK(exp, msg) assert((void(msg), (exp)))

#define LOG_INFO(msg) std::cout << msg << std::endl

#ifdef DEBUG
    #include <fstream>
    #ifndef LOG_FILE
        #define LOG_FILE "default.log"
    #endif
    extern std::ofstream _log_file;
    #define LOG_DEBUG(msg) _log_file << "DEBUG :: " << __FILE__ << ":" \
                                     << __LINE__ << " :: " << msg << std::endl
#else
    #define LOG_DEBUG(msg)
#endif

#endif // LOG_H__
