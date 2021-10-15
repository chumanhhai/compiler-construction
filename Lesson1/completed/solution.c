#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Node {
    char word[32];
    int occurrence;
    int x[512];
    int y[512];
    struct Node *left;
    struct Node *right; 
};

const char INVALID_CHARS[] = " \0\n\t()[]{},/;':!@#$%^&*+-";
const int SIZE_INVALID_CHARS = sizeof(INVALID_CHARS)/sizeof(char)-1;

struct Node *tree;

char stopWords[1024];

struct Node *createNode(char *word, int x, int y) {
    struct Node *node = (struct Node*)malloc(sizeof(struct Node));
    strcpy(node->word, word);
    (node->x)[0] = x;
    (node->y)[0] = y;
    node->occurrence = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void insert(struct Node **parent, char *word, int x, int y) {
    if(*parent == NULL) {
        *parent = createNode(word, x, y);
        return;
    }
    int cmp = strcmp((*parent)->word, word);
    if(cmp == 0) {
        int occur = (*parent)->occurrence;
        ((*parent)->x)[occur] = x;
        ((*parent)->y)[occur] = y;
        (*parent)->occurrence = (*parent)->occurrence + 1;
    } else if(cmp > 0) insert(&((*parent)->left), word, x, y);
    else insert(&((*parent)->right), word, x, y);
}

void printTree(struct Node *root) {
    if(root == NULL) return;
    // print left
    printTree(root->left);
    // print node
    printf("%s %d", root->word, root->occurrence);
    for(int i = 0; i < root->occurrence; i++) {
        printf(", (%d, %d)", (root->x)[i], (root->y)[i]);
    }
    printf("\n");
    // print right
    printTree(root->right);
}

void freeTree(struct Node *root) {
    if(root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int isStopWord(char *word) {
    if(strstr(stopWords, word)) return 1;
    return 0;
}

int isInvalidChar(char ch) {
    for(int i = 0; i < SIZE_INVALID_CHARS; i++) {
        if(ch == INVALID_CHARS[i])
            return 1;
    }
    return 0;
}

void normalizeWord(char *word) {
    // lowercase word
    for(int i = 0; i < strlen(word); i++)
        word[i] = tolower(word[i]);
    // delete comma at the end
    int i = strlen(word) - 1;
    while(i >= 0 && word[i] == '.') {
        word[i] = '\0';
        i--;
    }
}

void extractWordInEachLine(char *line, int rowNumber) {
    int i = 0, j;
    char word[32];
    while(i < strlen(line)) {
        while(i < strlen(line) && isInvalidChar(line[i])) i++;
        j = i;
        while(j < strlen(line) && !isInvalidChar(line[j])) j++;
        if(i < strlen(line)) {
            strncpy(word, &line[i], j-i);
            word[j-i] = '\0';
            normalizeWord(word);
            if(strlen(word) > 0 && !isStopWord(word)) {
                insert(&tree, word, rowNumber, i+1);
            }
        }
        i = j;
    }
}

void readStopWord(char *stopWordPath) {
    FILE *fp;
    char buff[32];
    fp = fopen(stopWordPath, "r");

    while(fgets(buff, 32, fp) != NULL) {
        strcat(stopWords, buff);
    }
    fclose(fp);
}

void buildIndexTable(char *textPath) {
    FILE *fp;
    char buff[1024];
    fp = fopen(textPath, "r");
    int rowNumber = 0;

    while(fgets(buff, 1024, fp) != NULL) {
        rowNumber++;
        extractWordInEachLine(buff, rowNumber);
    }
    fclose(fp);
}

void process(char *textPath, char *stopWordPath) {
    readStopWord(stopWordPath);
    buildIndexTable(textPath);
    printTree(tree);
    freeTree(tree);
}

int main() {
    process("./vanban.txt", "./stopw.txt");
    return 0;
}