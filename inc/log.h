#include <stdio.h>

// Name of the application
#define PROGNAME "makegen"

// Integer to string conversion functions
#define STR2(s) #s
#define STR(s) STR2(s)

// Error logging function
#define ERR(msg, __VA_ARGS__) printf(PROGNAME ":" __FILE__ ":" STR(__LINE__) ": " \
                msg,  __VA_ARGS__)


