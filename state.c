#include "debugmalloc.h"

#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "logging.h"


State* state_new() {
    State* ret = (State*)malloc(sizeof(State));

    if (ret == NULL) {
        ERROR("Memory allocation error in state_new()");
        return NULL;
    }

    if ((ret->defines = map_new()) == NULL) goto ERR;
    if ((ret->labels = map_new()) == NULL) goto ERR;

    ret->instr = NULL;
    ret->tokens = NULL;
    ret->PC = 0;
    ret->infile[0] = 0;
    strcpy(ret->outfile, "out.bin");
    return ret;

ERR:
    FAIL("state_new() failed!\n");
    state_free(ret);
    return 0;
}


int state_load_instr(State* s, char* fname) {
    s->instr = instruction_load(fname);
    if (s->instr == NULL) return -1;
    return 0;
}


void state_free(State* s) {
    if (!s) return;
    map_free(s->defines);
    tokenslist_free(s->tokens);
    map_free(s->labels);
    instruction_free(s->instr);
    free(s);
}

/**
 * @brief print some help on the useage of the program
 * @param pname argv[0] (executable name)
 */
void print_help(char* pname) {
    printf("6502 compiler program\nMade by Laszlo Barath in 2020\n\n");
    printf("Useage:\n> %s [args] <inputfile>\n", pname);
    printf("(inputfile must be at most %d chars long)\n", STATE_MAX_STRING_LEN - 1);
    printf("Arguments:\n");
    printf("\t--output (-o):\tSpecify output file. Expects a file name (max %d chars). Default is 'out.bin'.\n", STATE_MAX_STRING_LEN - 1);
    printf("\t--define (-d):\tSpecify a constant. Expects a name (max %d chars) and a (decimal) number.\n", MAP_MAX_KEY_LEN - 1);
    printf("\t--log (-l):\tSet log level. Expects a number in range 1-5. Default is 1.\n");
    printf("\t--help (-h):\tprints this message. Expects no parameters.\n");
    printf("\n");
}

/**
 * @brief print the "use --help" text
 */
void print_short_help() {
    printf("Use the '-h' or --help switches to get more info!\n");
}


int state_parse_commandline(State* s, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--out")) {
            // Output argument
            if (i == argc - 1) {
                ERROR("Argument '%s' excepts a parameter!\n", argv[i]);
                print_short_help();
                return -1;
            }
            char* param = argv[++i];
            if (strlen(param) > STATE_MAX_STRING_LEN - 1) {
                ERROR("Out parameter is too long! (max: %d chars)\n", STATE_MAX_STRING_LEN - 1);
                return -1;
            }
            if (param[0] == '-') {
                ERROR("Output file name can not start with a dash ('-')!\n");
                return -1;
            }
            strncpy(s->outfile, param, STATE_MAX_STRING_LEN);
        } else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log")) {
            // Log argument
            if (i == argc - 1) {
                ERROR("Argument '%s' excepts a parameter!\n", argv[i]);
                print_short_help();
                return -1;
            }
            char* param = argv[++i];
            int level;
            if (sscanf(param, "%d", &level) != 1) {
                ERROR("Can not parse log level!\n");
                return -1;
            }
            logging_level(level);
        } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--define")) {
            // Define argument
            if (i == argc - 1) {
                ERROR("Argument '%s' excepts a parameter!\n", argv[i]);
                print_short_help();
                return -1;
            }
            char* name = argv[++i];
            char* value = argv[++i];
            int num;
            if (sscanf(value, "%d", &num) != 1) {
                ERROR("Can not parse value of define '%s': '%s'!\n", name, value);
                return -1;
            }
            if (strlen(name) > MAP_MAX_KEY_LEN - 1) {
                ERROR("Define constant name is too long! (max: %d chars)\n", MAP_MAX_KEY_LEN - 1);
                return -1;
            }
            map_set(s->defines, name, num);
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            // help
            print_help(argv[0]);
            return -1;
        } else {
            // rest can only be inputfile
            if (strlen(argv[i]) > STATE_MAX_STRING_LEN - 1) {
                ERROR("Input file name is too long! (max: %d chars)\n", STATE_MAX_STRING_LEN - 1);
                return -1;
            }
            if (argv[i][0] == '-') {
                ERROR("Output file name ('%s') can not start with a dash ('-')!\n", argv[i]);
                return -1;
            }
            strncpy(s->infile, argv[i], STATE_MAX_STRING_LEN);
        }
    }
    if (s->infile[0] == 0) {
        ERROR("No input file was given!\n");
        print_short_help();
        return -1;
    }
    return 0;
}


