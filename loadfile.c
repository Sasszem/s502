
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "loadfile.h"
#include "logging.h"

/**
 * Read a single token into a Token struct from file f
 * - read relevant part
 * - return 1 on success, 0 on EOF and -1 on error
 * Internal workings:
 * - read characters into puffer
 * - only write one whitespace
 * - don't write ; and any characters after
 * - stop on EOF and \n
 */
int read_token(FILE* f, Token* t) {

    int seen_whitespace = 1, eol = 0;
    int ptr, c;

    // -1 for trailing nullbyte
    for (ptr = 0; ptr < TOKEN_BUFFER_SIZE - 1;) {
        // read char
        c = fgetc(f);

        // stop condition
        if (c == EOF || c == '\n') {
            break;
        }

        // don't write if already reached a comment
        if (eol)
            continue;

        // whitespace handling
        if (c == '\t' || c == ' ') {
            if (!seen_whitespace) {
                c = ' ';
                seen_whitespace = 1;
            } else {
                continue;
            }
        } else { // all othet chars
            seen_whitespace = 0;

            // mark comment
            if (c == ';') {
                eol = 1;
                continue;
            }
        }

        // tha actual write
        // continues skipt this
        t->stripped[ptr++] = c;
    }

    // too long line
    if (ptr == TOKEN_BUFFER_SIZE - 1) {
        t->len = ptr;
        return -1;
    }

    // kill trailing whitespace
    if (t->stripped[ptr - 1] == ' ')
        ptr--;

    // zero-terminate
    t->stripped[ptr] = 0;
    // len
    t->len = ptr;

    // log
    if (ptr > 0) {
        LOG("READ TOKEN:\n");
        LOGDO(token_print(t));
    }

    // EOF return
    if (c == EOF)
        return 0;

    return 1;
}

/**
 * Parse token - test if it's an opcode, a label or a preprocessor statement
 */
int recognize_token(Token* t) {
    // how many token types does it fit
    int found = 0;

    // directive - starts with a dot
    if (t->stripped[0] == '.') {
        t->type = TT_PREPROC;
        found++;
    }

    // label - ends with a ':'
    if (t->stripped[t->len - 1] == ':') {
        t->type = TT_LABEL;
        found++;
    }

    // instruction - 3rd char is a space or len is 3
    if (t->stripped[3] == ' ' || t->stripped[3] == '\0') {
        t->type = TT_INSTR;
        found++;
    }

    // 0 or more than one match is a problem
    if (found != 1) {
        ERROR("Can not recognize token:\n");
        token_print(t);
        return -1;
    }

    LOG("Recognized token as %d:\n", t->type);
    LOGDO(token_print(t));

    return 0;
}

/**
 * High-level func to read the contents from a file
 * - reads file contents
 * - parses every line as a token
 * - generates token list
 * - returns said token list
 * - returns NULL on error
 */
TokensList* read_file(char* name) {

    // basic file open
    FILE* f = fopen(name, "r");
    if (f == NULL) {
        ERROR("An error occured opening the file %s!\n", name);
        ERROR("Error opening file: %s\n", strerror(errno));
        return NULL;
    }

    TokensList* tokenslist = tokenslist_new();
    if (tokenslist == NULL) goto ERR_MEM;
    Token tok;
    int lineno = 1;
    int res;

    // reading tokens while possible
    do {
        res = read_token(f, &tok);
        if (tok.len > 0) {
            strncpy(tok.source.fname, name, TOKEN_SOURCE_FILE_SIZE);
            tok.source.lineno = lineno;
            if (tokenslist_add(tokenslist, tok) < 0) goto ERR_MEM;
        }
        lineno++;
    } while (res > 0);

    fclose(f);

    // return error if stopped due to reading error
    if (res < 0) {
        ERROR("line is too long!\n");
        token_print(&tok);
        tokenslist_free(tokenslist);
        return NULL;
    }
    return tokenslist;
ERR_MEM:

    FAIL("read_file() failed!\n");
    fclose(f);
    if (tokenslist != NULL)
        tokenslist_free(tokenslist);
    return NULL;
}

/**
 * Do token recognition on all tokens in a list
 * Returns 0 on success, -1 on error
 */
int recognize_tokens(TokensList* t) {
    for (TokensListElement* ptr = t->head; ptr != NULL; ptr = ptr->next)
        if (recognize_token(&(ptr->token)) < 0) {
            FAIL("Can not recognize token types!\n");
            return -1;
        }
    return 0;
}

// this should not be here, or rather all the other function shoudl be elsewhere...
// TODO: refactor this
// maybe we can use a monadic pattern here?

/**
 * High level function that reads a whole file
 * - reads file
 * - tokenises it
 * - recognizes token types
 * - returns fiel as TokensList*
 * - returns NULL on faliure
 */
TokensList* load_file(char* name) {
    LOG("Reading file...\n");

    TokensList* list = read_file(name);
    if (list == NULL)
        goto ERROR;

    LOG("Running first analysis...\n");
    if (recognize_tokens(list) < 0)
        goto ERROR;

    return list;

ERROR:
    FAIL("Failed to load file contents: '%s'\n", name);
    if (list != NULL) {
        tokenslist_free(list);
    }
    return NULL;
}