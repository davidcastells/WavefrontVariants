#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define min2(a,b) (((a)<(b))?(a):(b))
#define max2(a,b) (((a)>(b))?(a):(b))
#define min3(a,b,c) min2(a, min2(b, c))
#define max3(a,b,c) max2(a, max2(b, c))

#define TEXT_SCAPE_BOLD         "\e[1m"
#define TEXT_SCAPE_ITALIC       "\e[3m"
#define TEXT_SCAPE_UNDERLINE    "\e[4m"
#define TEXT_SCAPE_END          "\e[0m"

#include <string>

bool ends_with(const std::string &filename, const std::string &ext);
bool file_is_binary(char* filename);


#endif
