/**
 * @file
 * @brief global variable accessor function for logging
 */

int logging_level(int level) {
    static int current;
    if (current == 0) {
        current = 1;
    }
    if (level == 0) return current;
    return current = level;
}