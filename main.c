#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

void printDebug(int argc, char *argv[]) {
    printf("Hello, World!!!\n");
    printf("argc = %d\n", argc);
    for(int ndx = 0; ndx != argc; ++ndx)
        printf("argv[%d] --> %s\n", ndx, argv[ndx]);
    printf("argv[argc] = %p\n", (void*)argv[argc]);
}

FILE *getFile(char *filename) {
    FILE *pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("Error in opening file!\n");
        exit(1);
    }
    return pFile;
}

void check(char *text) {
    for (int i = 0; i < strlen(text); ++i) {
        if (!(isalnum(text[i]) || text[i] == '-'))
            printf("Detected invalid identifier \"%c\" in [%s]\n", text[i], text);
    }
}

// Returns -1 if not a number and number otherwise
long number(char* str) {
    if (str == NULL) return -1;
    char *endptr;
    long num = strtol(str, &endptr, 10);

    if (*endptr != '\0') {
        return -1;
    }
    return num;
}

typedef struct Section {
    char *name;
    char **keys;
    char **values;
    int size;
    struct Section *next;
} Section;

char *findValue(const char *inputSection, const char *inputKey, Section *firstSection, bool andPrint) {
    if (inputSection==NULL) return NULL;

    char *searchValue = NULL;
    Section *curSection = firstSection;
    bool foundSection = false, foundKey = false;
    do {
        if (strcmp(curSection->name,inputSection) == 0) {
            foundSection = true;
            for (int i = 0; i < curSection->size; ++i) {
                if (strcmp(curSection->keys[i],inputKey) == 0) {
                    foundKey = true;
                    searchValue = malloc(strlen(curSection->values[i])+1);
                    strcpy(searchValue,curSection->values[i]);
                    break;
                }
            }
            break;
        }
        else curSection = curSection->next;
    } while (curSection != NULL);

    // PRINT RESULT
    if (foundSection)
        if (foundKey) { if (andPrint) printf("%s\n", searchValue); }
        else printf("Failed to find key \"%s\" in section [%s]\n", inputKey, inputSection);
    else printf("Failed to find section [%s]\n", inputSection);
    return searchValue;
}

// MAIN

// CONST VARIABLES
int FIRST_ARGUMENT = 1, MIN_KEY_VALUE_COUNT = 5;
char dot = '.', equals = '=', newline = '\n', zeroend = '\0';

Section *readFile(FILE *pFile) {
    Section *firstSection = NULL;
    char *line = NULL;
    size_t lineSize;
    ssize_t length;
    int sectionKeyValueSize = MIN_KEY_VALUE_COUNT;
    while ((length = getline(&line, &lineSize, pFile)) != -1) {
        line[length - 1] = '\0'; length--;// Remove newline
        if (line[0] == zeroend) continue;

        // SECTION DETECTED
        else if (line[0] == '[' && line[length - 1] == ']') {

            // CREATE NEW SECTION
            Section *newSection;
            newSection = malloc(sizeof(Section));
            newSection->next = firstSection;
            newSection->size = 0;
            sectionKeyValueSize = MIN_KEY_VALUE_COUNT;
            newSection->keys = malloc(sectionKeyValueSize * sizeof(char*));
            newSection->values = malloc(sectionKeyValueSize * sizeof(char*));

            // ASSIGN NAME TO THIS SECTION
            line[0] = zeroend; line[length - 1] = zeroend; length-=2; line++;
            newSection->name = malloc(length+1);
            stpcpy(newSection->name, line);
            check(line);
            firstSection = newSection;

        // KEY = VALUE DETECTED
        } else {

            // CREATE NEW KEY VALUE PAIR
            char *newKey = strtok(line, &equals); newKey[strlen(newKey) - 1] = zeroend;
            char *newValue = strtok(NULL, &newline); newValue[0] = zeroend; newValue++;
            check(newKey); check(newValue);
            firstSection->keys[firstSection->size] = malloc(strlen(newKey));
            firstSection->values[firstSection->size] = malloc(strlen(newValue));
            strcpy(firstSection->keys[firstSection->size], newKey);
            strcpy(firstSection->values[firstSection->size], newValue);
            firstSection->size++;

            // ALLOCATE MORE MEMORY
            if (firstSection->size >= sectionKeyValueSize) {
                sectionKeyValueSize += MIN_KEY_VALUE_COUNT;
                char **newKeys = realloc(firstSection->keys, sectionKeyValueSize * sizeof(char *));
                char **newValues = realloc(firstSection->values, sectionKeyValueSize * sizeof(char *));
                if (newKeys == NULL || newValues == NULL) {
                    printf("Error allocating memory for keys and values\n");
                    exit(0);
                } else {
                    firstSection->keys = newKeys;
                    firstSection->values = newValues;
                }
            }
        }
        line = NULL;
        lineSize = 0;
    }
    free(line);
    fclose(pFile);
    return firstSection;
}

bool evaluateExpressionValid(char *x, const char *exp, char *y) {
    long nx = number(x);
    long ny = number(y);
    if (nx != -1) {
        if (ny != -1) {
            // NUMBERS
            switch (exp[0]) {
                case '+': printf("%ld\n",nx+ny); break;
                case '*': printf("%ld\n",nx*ny); break;
                case '/': printf("%ld\n",nx/ny); break;
                case '-': printf("%ld\n",nx-ny); break;
            }
        }
    } else if (ny == -1 && exp[0] == '+') {
        // STRINGS
        printf("%s%s\n", x, y);
    } else {
        // INVALID
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    printDebug(argc, argv);
    FILE *pFile = getFile(argv[FIRST_ARGUMENT]);

    // SPLIT SECTION NAME AND KEY
    char *inputSection = NULL;
    char *inputKey = NULL;
    char *expSection = NULL;
    char *expKey = NULL;
    char *exp = NULL;
    bool isExpression = false;
    if (strcmp(argv[FIRST_ARGUMENT+1], "expression")==0) {
        isExpression = true;
        char *inputSectionDotKey =  strtok(argv[FIRST_ARGUMENT+2], " ");
        exp =                       strtok(NULL, " ");
        char *expSectionDotKey =    strtok(NULL, "\n");
        inputSection =              strtok(inputSectionDotKey, &dot);
        inputKey =                  strtok(NULL, &zeroend);
        expSection =                strtok(expSectionDotKey, &dot);
        expKey =                    strtok(NULL, &zeroend);

    } else {
        inputSection =              strtok(argv[FIRST_ARGUMENT+1], &dot);
        inputKey =                  strtok(NULL, &zeroend);
    }

    // READ FILE
    Section *firstSection = readFile(pFile);

    // SEARCH FOR VALUE
    char *searchValue = findValue(inputSection, inputKey, firstSection, !isExpression);
    char *searchValue2 = findValue(expSection, expKey, firstSection, !isExpression);
    if (isExpression) {
        if (!evaluateExpressionValid(searchValue, exp, searchValue2)) {
            printf("%s.%s %s %s.%s is invalid\n", inputSection, inputKey, exp, expSection, expKey);
        }
    }

    // END PROGRAM
    Section *current = firstSection;
    while (current != NULL) {
        for (int i = 0; i < current->size; ++i) {
            free(current->keys[i]);
            free(current->values[i]);
        }
        free(current->name);
        Section *temp = current;
        current = temp->next;
        free(temp);
    }
    if (searchValue != NULL) free(searchValue);
    if (searchValue2 != NULL) free(searchValue2);
    free(current);
    return 0;
}
