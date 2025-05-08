#include "log.h"

#ifdef DEBUG
    std::ofstream _log_file(LOG_FILE, std::ios::trunc);
#endif
