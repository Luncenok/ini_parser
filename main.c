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

typedef struct Section {
    char *name;
    char **keys;
    char **values;
    int size;
    struct Section *next;
} Section;



// CONST VARIABLES
int FIRST_ARGUMENT = 1, MIN_KEY_VALUE_COUNT = 5;
char dot = '.', equals = '=', newline = '\n', zeroend = '\0';

int main(int argc, char *argv[]) {
    printDebug(argc, argv);
    FILE *pFile = getFile(argv[FIRST_ARGUMENT]);

    // SPLIT SECTION NAME AND KEY
    char *inputSection = strtok(argv[FIRST_ARGUMENT+1], &dot);
    char *inputKey= strtok(NULL, &zeroend);

    // READ FILE
    Section *firstSection = NULL;
    char *line = NULL;
    size_t lineSize;
    ssize_t length;
    int sectionKeyValueSize = 5;
    while ((length = getline(&line, &lineSize, pFile)) != -1) {
        line[length-1] = '\0'; length--;// Remove newline
        if (line[0] == zeroend) continue;

        // SECTION DETECTED
        else if (line[0]=='[' && line[length-1] == ']') {

            // CREATE NEW SECTION
            Section *newSection;
            newSection = malloc(sizeof(Section));
            newSection->next = firstSection;
            newSection->size = 0;
            sectionKeyValueSize = MIN_KEY_VALUE_COUNT;
            newSection->keys = malloc(sectionKeyValueSize * sizeof(char*));
            newSection->values = malloc(sectionKeyValueSize * sizeof(char*));

            // ASSIGN NAME TO THIS SECTION
            line[0] = zeroend; line[length-1] = zeroend; length-=2; line++;
            newSection->name = malloc(length+1);
            stpcpy(newSection->name, line);
            check(line);
            firstSection = newSection;

        // KEY = VALUE DETECTED
        } else {
            char *newKey = strtok(line, &equals); newKey[strlen(newKey)-1] = zeroend;
            char *newValue = strtok(NULL, &newline); newValue[0] = zeroend; newValue++;
            check(newKey); check(newValue);
            firstSection->keys[firstSection->size] = malloc(strlen(newKey));
            firstSection->values[firstSection->size] = malloc(strlen(newValue));
            strcpy(firstSection->keys[firstSection->size], newKey);
            strcpy(firstSection->values[firstSection->size], newValue);
            firstSection->size++;

            //ALLOCATE MORE MEMORY
            if (firstSection->size >= sectionKeyValueSize) {
                sectionKeyValueSize += MIN_KEY_VALUE_COUNT;
                char **newKeys = realloc(firstSection->keys, sectionKeyValueSize * sizeof(char *));
                char **newValues = realloc(firstSection->values, sectionKeyValueSize * sizeof(char *));
                if (newKeys == NULL || newValues == NULL) {
                    printf("Error allocating memory for keys and values\n");
                    return 0;
                } else {
                    firstSection->keys = newKeys;
                    firstSection->values = newValues;
                }
            }
        }
        line = NULL;
        lineSize = 0;
    }
    // END OF READING FILE

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
        if (foundKey) printf("%s", searchValue);
        else printf("Failed to find key \"%s\" in section [%s]\n", inputKey, inputSection);
    else printf("Failed to find section [%s]\n", inputSection);

    // END PROGRAM
    fclose(pFile);
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
    free(current);
    free(line);
    return 0;
}
