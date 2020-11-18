#ifndef GUARD_LOGGING
#define GUARD_LOGGING

#include <stdio.h>

/**
 * @brief pseudo-global accessor
 * @param setlevel new level to set log level to or 0 to don't change
 * @returns current log level
 */
int logging_level(int setlevel);

#define LOG(LVL, ...) if (logging_level(0)>=LVL) {printf("\e[33mLOG:\e[39m\t"); printf(__VA_ARGS__);}
#define LOGC(LVL, ...) if (logging_level(0)>=(LVL)) printf(__VA_ARGS__);
#define LOGDO(LVL, x) if(logging_level(0) >= (LVL)) {x;};


#define ERROR(...) printf("\e[41mERROR\e[49m:\t"); printf(__VA_ARGS__);
#define ERRORC(...) printf(__VA_ARGS__);
#define FAIL(...) printf("\e[31mFAIL:\e[39m\t"); printf(__VA_ARGS__);

#endif