#pragma once
#include <unistd.h>
#include <string>

#define ERR() if (errno != 0) perror((string{ "(" } + __FILE__ + ":" + to_string(__LINE__) + ")" + " "  + __PRETTY_FUNCTION__).c_str()), errno = 0
