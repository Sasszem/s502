#ifndef GUARD_LOGGING
#define GUARD_LOGGING

#include <stdio.h>

/**
 * @brief pseudo-global accessor
 * @param setlevel new level to set log level to or 0 to don't change
 * @returns current log level
 */
int logging_level(int setlevel);

/**
 * @brief logging macro - works like printf
 * @param LVL minimum log level
 * Macro to fancy-print using printf.
 * Can be used like a printf.
 * Only prints if log level is set high enough
 */
#define LOG(LVL, ...) if (logging_level(0)>=LVL) {printf("\e[33mLOG:\e[39m\t"); printf(__VA_ARGS__);}

/**
 * @brief Conditional macro. Wraps contents into a conditional based on log level.
 * @param LVL minimum log level
 */
#define LOGDO(LVL, x) if(logging_level(0) >= (LVL)) {x;};

/**
 * @brief Fancy-print an error (cause of faliure). Works like printf.
 */
#define ERROR(...) printf("\e[41mERROR\e[49m:\t"); printf(__VA_ARGS__);

/**
 * @brief Fancy-print a fail (failed step). Works like printf.
 */
#define FAIL(...) printf("\e[31mFAIL:\e[39m\t"); printf(__VA_ARGS__);

#endif