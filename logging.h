#ifndef GUARD_LOGGING
#define GUARD_LOGGING

#include <stdio.h>

#ifdef LOGGING
#define LOG(...) printf("\e[33mLOG:\e[39m\t"); printf(__VA_ARGS__)
#define LOGC(...) printf(__VA_ARGS__);
#define LOGDO(x) x;

#else
#define LOG(...) 
#define LOGC(...)
#define LOGDO(x)
#endif

#define ERROR(...) printf("\e[41mERROR\e[49m:\t"); printf(__VA_ARGS__)
#define ERRORC(...) printf(__VA_ARGS__);

#endif