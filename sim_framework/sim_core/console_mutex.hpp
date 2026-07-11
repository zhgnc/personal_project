#ifndef CONSOLE_MUTEX_HPP
#define CONSOLE_MUTEX_HPP

#include <mutex>

// Shared across all threads so concurrent std::cout writes from different
// SimSingleRun instances don't interleave into unreadable output.
inline std::mutex console_mutex;

#endif
