#define _CRT_SECURE_NO_WARNINGS
#define SIZE_STRING 101
#define SIZE_HASHTABLE 100
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct TJson {
    char Type;
    int Value;
    char* String;
    struct TJson** Enumerations;
    struct TNode** Notes;
};

struct TNode {
    char* Key;
    struct TJson* Value;
    struct TNode* Next;
    struct TNode* Tail;
};

void WriteNote(struct TJson* json, char closingCaracter);
void WriteEnumeration(struct TJson* json, char closingCaracter);
void SkipSymbol();
void AddNode(struct TNode** notes, long long hash, char* key);
long long Hash(char* string);
char  WriteNumber(int* number);
void WriteString(char* string);
void FindRealNote(struct TJson** curJson, char* key, long long hash);
void PrintData(struct TJson* json);

void SkipSymbol() {
    char c = (char)getchar();
}

void AddNode(struct TNode** notes, long long hash, char* key) {
    if (notes[hash] == NULL) {
        notes[hash] = (struct TNode*)malloc(sizeof(struct TNode));
        notes[hash]->Value = (struct TJson*)malloc(sizeof(struct TJson));
        notes[hash]->Key = (char*)malloc(sizeof(char) * strlen(key));
        strcpy(notes[hash]->Key, key);
        notes[hash]->Next = NULL;
        notes[hash]->Tail = notes[hash];
    }
    else {
        notes[hash]->Tail->Next = (struct TNode*)malloc(sizeof(struct TNode));
        notes[hash]->Tail = notes[hash]->Tail->Next;
        notes[hash]->Tail->Next = NULL;
        notes[hash]->Tail->Value = (struct TJson*)malloc(sizeof(struct TJson));
        notes[hash]->Tail->Key = key;
    }

}

long long Hash(char* string) {
    long long hash = 0;
    for (int i = 0; i < SIZE_STRING; ++i) {
        if (string[i] == '\0') {
            break;
        }
        hash = ((513 * (long long)string[i] + hash + 765) % 103889) % SIZE_HASHTABLE;
    }
    return hash;
}

char WriteNumber(int* number) {
    char c = (char)getchar();
    while (c >= '0' && c <= '9') {
        *number *= 10;
        *number += c - '0';
        c = (char)getchar();
    }
    return c;
}

void WriteString(char* string) {
    char c;
    int sizeString = 0;
    while ((c = (char)getchar()) != '"') {
        //printf("%c \n");
        string[sizeString] = c;
        ++sizeString;
    }
    string[sizeString] = '\0';
}

void WriteEnumeration(struct TJson* json, char closingCaracter) {
    json->Enumerations = (struct TJson**)malloc(SIZE_HASHTABLE * sizeof(struct TJson*));
    for (int i = 0; i < SIZE_HASHTABLE; ++i) {
        json->Enumerations[i] = NULL;
    }
    unsigned int index = 0;
    char c = (char)getchar();
    while (c != closingCaracter) {
        if (c == '"') {
            char string[SIZE_STRING];
            WriteString(string);
            json->Enumerations[index] = (struct TJson*)malloc(sizeof(struct TJson));
            json->Enumerations[index]->Type = 2;
            json->Enumerations[index]->String = (char*)malloc(strlen(string) * sizeof(char));
            strcpy(json->Enumerations[index]->String, string);
            ++index;
        }
        else if (c >= '0' && c <= '9') {
            int number = c - '0';
            char c = WriteNumber(&number);
            json->Enumerations[index] = (struct TJson*)malloc(sizeof(struct TJson));
            json->Enumerations[index]->Type = 3;
            json->Enumerations[index]->Value = number;
            ++index;
        }
        else if (c == '{') {
            json->Enumerations[index] = (struct TJson*)malloc(sizeof(struct TJson));
            json->Enumerations[index]->Type = 0;
            WriteNote(json->Enumerations[index], c + 2);
            ++index;
        }
        else if (c == '[') {
            json->Enumerations[index] = (struct TJson*)malloc(sizeof(struct TJson));
            json->Enumerations[index]->Type = 1;
            WriteEnumeration(json->Enumerations[index], c + 2);
            ++index;
        }
        c = (char)getchar();
    }
}

void WriteNote(struct TJson* json, char closingCaracter) {
    json->Notes = (struct TNode**)malloc(SIZE_HASHTABLE * sizeof(struct TNode*));
    for (int i = 0; i < SIZE_HASHTABLE; ++i) {
        json->Notes[i] = NULL;
    }
    char c = (char)getchar();
    while (c != closingCaracter) {
        if (c == '"') {
            char key[SIZE_STRING];
            WriteString(key);
            long long hash = Hash(key);
            SkipSymbol(); // убираю двоеточие
            SkipSymbol(); // убираю пробел
            c = (char)getchar();
            if (c == '"') {
                char string[SIZE_STRING];
                WriteString(string);
                AddNode(json->Notes, hash, key);
                json->Notes[hash]->Tail->Value->Type = 2;
                json->Notes[hash]->Tail->Value->String = (char*)malloc(strlen(string) * sizeof(char));
                strcpy(json->Notes[hash]->Tail->Value->String, string);
            }
            else if (c >= '0' && c <= '9') {
                int number = c - '0';
                char c = WriteNumber(&number);
                AddNode(json->Notes, hash, key);
                json->Notes[hash]->Tail->Value->Type = 3;
                json->Notes[hash]->Tail->Value->Value = number;
            }
            else if (c == '{') {
                AddNode(json->Notes, hash, key);
                json->Notes[hash]->Tail->Value->Type = 0;
                WriteNote(json->Notes[hash]->Tail->Value, c + 2);
            }
            else if (c == '[') {
                AddNode(json->Notes, hash, key);
                json->Notes[hash]->Tail->Value->Type = 1;
                WriteEnumeration(json->Notes[hash]->Tail->Value, c + 2);
            }
        }
        c = (char)getchar();
    }
}

void FindRealNote(struct TJson** curJson, char* key, long long hash) {
    struct TNode* curNote = (*curJson)->Notes[hash];
    while (curNote->Next != NULL && strcmp(curNote->Key, key) != 0) {
        curNote = curNote->Next;
    }
    *curJson = curNote->Value;
}

void PrintData(struct TJson* json) {
    char c = (char)getchar();
    struct TJson* curJson = json;
    while (c != '\n' && c != EOF) {
        if (c == '"') {
            char key[SIZE_STRING];
            WriteString(key);
            long long hash = Hash(key);
            FindRealNote(&curJson, key, hash);
            c = (char)getchar();
        }
        else if (c >= '0' && c <= '9') {
            int index = c - '0';
            c = WriteNumber(&index);
            curJson = curJson->Enumerations[index];

        }
        else {
            c = (char)getchar();
        }
    }
    if (curJson->Type == 2) {
        printf("%s \n", curJson->String);
    }
    else {
        printf("%d \n", curJson->Value);
    }
}

int main(void) {
    struct TJson json;
    char c = getchar();
    if (c == '{') {
        json.Type = 0;
        WriteNote(&json, c + 2);
    }
    else {
        json.Type = 1;
        WriteEnumeration(&json, c + 2);
    }
    int getData;
    scanf("%d", &getData);
    SkipSymbol();//убираю лишний отступ
    for (int i = 0; i < getData; ++i) {
        PrintData(&json);
    }
    return 0;
}

