#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int IO_ERROR = 0;
const int IO_SUCCESS = 1;
const int MAX_LEN = 64;

FILE *file1, *file2;

int openFile(FILE **file, char *filePath) {
    FILE *temp = fopen(filePath, "r");

    if(temp == NULL) return IO_ERROR;
    *file = temp;
    return IO_SUCCESS;
}

void closeFiles() {
    fclose(file1);
    fclose(file2);
}

void compare(char *strFile1, char *strFile2) {
    if(openFile(&file1, strFile1) == IO_ERROR) {
        printf("Error: Cannot open file '%s'", strFile1);
        return -1;
    }
    if(openFile(&file2, strFile2) == IO_ERROR) {
        printf("Error: Cannot open file '%s'", strFile2);
        return -1;
    }
    printf("\n------------ '%s' VS '%s' ------------\n", strFile1, strFile2);
    char buff1[MAX_LEN+1], buff2[MAX_LEN+1], *check1, *check2;
    int liNo = 0, flag = 0, cmp;
    check1 = fgets(buff1, MAX_LEN, file1);
    check2 = fgets(buff2, MAX_LEN, file2);
    while(check1 != NULL && check2 != NULL) {
        liNo++;
        normalize(buff1);
        normalize(buff2);
        cmp = strcmp(buff1, buff2);
        if(cmp != 0) {
            flag = 1;
            printf("****************************\n");
            printf("line %d\n", liNo);
            printf("file 1: %s\n", buff1);
            printf("file 2: %s\n", buff2);
        }
        check1 = fgets(buff1, MAX_LEN, file1);
        check2 = fgets(buff2, MAX_LEN, file2);
    }
    if(!check1 && check2) printf("File 1 is shorter than file 2");
    else if (check1 && !check2) printf("File 2 is shorter than file 1");
    else printf("100%% correct!");

    closeFiles();
}

void normalize(char *line) {
    int len = strlen(line);
    if(line[len-1] == '\n') {
        line[len-1] = '\0';
    }
}

int main(int argc, char *argv[]) {
    
    char *prefix1 = "../Lession2/test/result", *postfix1 = ".txt";
    char *prefix2 = "../Lession2/test/myResult", *postfix2 = ".txt";
    char inFileName[50], outFileName[50];
    int num = 7;
    for(int i = 1; i <= num; i++) {
        sprintf(inFileName, "%s%d%s", prefix1, i, postfix1);
        sprintf(outFileName, "%s%d%s", prefix2, i, postfix2);
        compare(inFileName, outFileName);
    }

    return 0;
}