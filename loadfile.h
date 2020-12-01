#ifndef GUARD_LOADFILE
#define GUARD_LOADFILE

#include "tokenslist.h"

/**
 * @brief load and parse one file
 *
 * Load file, parse into tokens, build TokensList, identify token types
 *
 * @param name file name / path to load
 * @returns list of tokens in file or NULL on error
 */
TokensList* load_file(char* name);

#endif