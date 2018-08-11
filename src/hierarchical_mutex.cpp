#include "hierarchical_mutex.h"

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value{ (std::numeric_limits<unsigned long>::max)() };
