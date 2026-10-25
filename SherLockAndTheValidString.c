#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char* isValid(char* s) {
    if(!s || *s == '\0') return "YES";
    
    int charCount[26] = {0};
    for(int i=0 ; s[i] != '\0' ; i++){
        charCount[s[i] - 'a']++;
    }
    
    int c1 = 0;
    int c2 = 0;
    int c3 = 0;
    int c4 = 0;
    
    for(int i=0 ; i<26 ; i++){
        int count = charCount[i];
        if(count == 0) continue;
        
        if(c1 == 0 || c1 == count){
            c1 = count;
            c3++;
        } else if(c2 == 0 || c2 == count){
            c2 = count;
            c4++;
        } else{
            return "NO";
        }
    }
    if(c2 == 0) return "YES";
    
    if((c1 == 1 & c3 == 1) || (c2 == 1 && c4 == 1)){
        return "YES";
    }
    if((c1 - c2 == 1 && c3 == 1) || (c2 -c1 == 1 && c4 == 1)){
        return "YES";
    }
    return "NO";
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char* s = readline();

    char* result = isValid(s);

    fprintf(fptr, "%s\n", result);

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;

    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }

        alloc_length <<= 1;

        data = realloc(data, alloc_length);

        if (!data) {
            data = '\0';

            break;
        }
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';

        data = realloc(data, data_length);

        if (!data) {
            data = '\0';
        }
    } else {
        data = realloc(data, data_length + 1);

        if (!data) {
            data = '\0';
        } else {
            data[data_length] = '\0';
        }
    }

    return data;
}
