#ifdef __GNUC__
#pragma GCC system_header
#include "../lib/selene/include/selene.h"
#pragma GCC system_header
#include "../lib/voro++/src/voro++.hh"
#endif

// since selene produces a lot of ugly template warnings with gcc's -Wall and -Wextra, we will use this dirty hack and define selene as a system header for gcc. This will supress the warnings for this header only and print out warnings for all other issues in the code
