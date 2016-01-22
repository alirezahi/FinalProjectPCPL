#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct prep {
    char str[300];
    struct prep *next;
} head;

typedef struct prep prep;
typedef prep *prepptr;

struct simpletable {
    char name[300];
    int type;
    int linenum;
    struct simpletable *next;
} * headtable;

typedef struct simpletable SimpleTable;
typedef struct simpletable *SimpleTablePtr;

struct table {
    char str[300];
    struct table *next;
} tablehead;

typedef struct table table;
typedef table *tableptr;

struct token {
    char code[100];
    int lineNumber;
    struct token *next;
} * start;

typedef struct token Token;
typedef struct token * TokenPtr;

struct symboltable { // It Starts from symboltable->next :)
    char name[300];
    int index;
    int try;
    struct symboltable *next;
};

typedef struct symboltable symboltable;
typedef symboltable *symboltableptr;
symboltable *heads = NULL;

struct numbertable { // It Starts from numbertable->next :)
    char name[300];
    int index;
    int try;
    struct numbertable *next;
};

typedef struct numbertable numbertable;
typedef numbertable *numbertableptr;
numbertable *headn = NULL;

struct charactertable { // It Starts from charactertable->next :)
    char name[300];
    int index;
    int try;
    struct charactertable *next;
};

typedef struct charactertable charactertable;
typedef charactertable *charactertableptr;
charactertable *headc = NULL;

struct defined { // It Starts from defined->next :)
    char name[300];
    char kind[20];
    char value[200];
    int have ;
    struct defined *next;
};

typedef struct defined defined;
typedef defined *definedptr;
defined *headdefine = NULL;

void preprocess(FILE *inc);
void print();
void Tokenize(FILE * file);
void Add(char * code, int lineNumber);
void AddFirst(char * code, int lineNumber);
void tables();
int IsKeyword(char input[]);
int IsOperator(char input[]);
int IsNumber(char input[]);
int IsPunctuation(char input[]);
int IsCharacter(char input[]);
int IsIdentifier(char input[]);
int IsEND(char input[]);
int num_first_id(char input[]);
int prohibit(char a);
int prohibited_id(char input[]);
void find(char input[], int a);
void Add_table();
void Syntax();
void IRGenerate();
void symbol_table();
int IsRepetitive(char name[]);
int find_id_index(char name[]);
int find_num_index(char name[]);
int find_char_index(char name[]);
int u = 0 ;
int openBrace, closeBrace;
int IsIntNum(char input[]);
void undefined_id();

int main() {
    FILE *file;
    start = NULL;
    headtable = NULL;
    openBrace = 0;
    closeBrace = 0;
    file = fopen("/Users/Alireza/Downloads/save1.text", "r");
    Tokenize(file);
    tables();
//    print();
    symbol_table();
    Syntax();
    if (u == 0)
        IRGenerate();
    undefined_id();
    return 0;
}

void preprocess(FILE *inc) {
    char c;
    char include[20];
    char variable[40];
    char variable1[40];
    c = fgetc(inc);
    if (c == '#') {
        fscanf(inc, "%s", include);
        if (strcmp(include, "include") == 0) {
            int j = 0;
            fscanf(inc, "%s", variable);
            for (int i = 1; variable[i] != '>'; i++) {
                variable1[j] = variable[i];
                j++;
            }
        }
    }
    prep *head = NULL;
    head = (prep*) malloc(sizeof (prep));
    head->next = NULL;
    prep *a = NULL, *b;
    b = head;
    char begin[300];
    FILE *inc1;
    inc1 = fopen("main.c", "r");
    if (inc1 == NULL) {
        printf("File doesn't Exist .Try Again");
    }
    while (!feof(inc1)) {
        a = (prep*) malloc(sizeof (prep));
        a->next = NULL;
        b->next = a;
        fgets(begin, 300, inc1);
        strcpy(a->str, begin);
        b = a;
    }
    fclose(inc1);
    while (!feof(inc)) {
        a = (prep*) malloc(sizeof (prep));
        a->next = NULL;
        b->next = a;
        fgets(begin, 300, inc);
        strcpy(a->str, begin);
        b = a;
    }
    FILE *p;
    a = head->next;
    p = fopen("/Users/Alireza/Downloads/save1.text", "w");
    while (a != NULL) {
        fputs(a->str, p);
        a = a->next;
    }
    fclose(p);
    return;
}

void print() {
    SimpleTablePtr tmp = headtable;
    
    while (tmp != NULL) {
        printf("%s\t", tmp->name);
        printf("%d\t", tmp->linenum);
        printf("%d\n", tmp->type);
        tmp = tmp->next;
    }
    return;
}

void Tokenize(FILE * file) {
    char *pch;
    char code[100];
    int i = 1;
    while (!feof(file)) {
        fgets(code, 100, file);
        pch = strtok(code, " \n\t");
        while (pch != NULL) {
            Add(pch, i);
            pch = strtok(NULL, " \n\t");
        }
        //        char end[100] = "#END";
        //        Add(end, i);
        i++;
        pch = NULL;
    }
    return;
}

void Add(char *code, int lineNumber) {
    if (start == NULL)
        AddFirst(code, lineNumber);
    else {
        TokenPtr tmp, right;
        tmp = malloc(sizeof (Token));
        strcpy(tmp->code, code);
        tmp->lineNumber = lineNumber;
        right = start;
        while (right->next != NULL)
            right = right->next;
        right->next = tmp;
        right = tmp;
        right->next = NULL;
    }
    return;
}

void AddFirst(char *code, int lineNumber) {
    TokenPtr tmp;
    tmp = malloc(sizeof (Token));
    strcpy(tmp->code, code);
    tmp->lineNumber = lineNumber;
    start = tmp;
    start->next = NULL;
    return;
}

void tables() {
    TokenPtr tmp = start;
    while (tmp != NULL) {
        find(tmp->code, tmp->lineNumber);
        tmp = tmp->next;
    }
}

int IsKeyword(char input[200]) {
    if (strcmp(input, "main") == 0)
        return 10;
    else if (strcmp(input, "if") == 0)
        return 11;
    else if (strcmp(input, "else") == 0)
        return 12;
    else if (strcmp(input, "int") == 0)
        return 13;
    else if (strcmp(input, "float") == 0)
        return 13;
    else if (strcmp(input, "char") == 0)
        return 14;
    else if (strcmp(input, "bool") == 0)
        return 15;
    else if (strcmp(input, "void") == 0)
        return 16;
    else if (strcmp(input, "NULL") == 0)
        return 17;
    else if (strcmp(input, "TRUE") == 0)
        return 18;
    else if (strcmp(input, "FALSE") == 0)
        return 18;
    else if (strcmp(input, "return") == 0)
        return 19;
    return 0;
}

int IsOperator(char input[200]) {
    if (strcmp(input, "=") == 0)
        return 10;
    else if (strcmp(input, "+") == 0)
        return 11;
    else if (strcmp(input, "-") == 0)
        return 11;
    else if (strcmp(input, "/") == 0)
        return 11;
    else if (strcmp(input, "*") == 0)
        return 11;
    else if (strcmp(input, ">") == 0)
        return 12;
    else if (strcmp(input, "<") == 0)
        return 12;
    else if (strcmp(input, "<=") == 0)
        return 12;
    else if (strcmp(input, ">=") == 0)
        return 12;
    else if (strcmp(input, "==") == 0)
        return 12;
    else if (strcmp(input, "!=") == 0)
        return 12;
    else if (strcmp(input, "&&") == 0)
        return 12;
    else if (strcmp(input, "||") == 0)
        return 12;
    return 0;
}

int IsNumber(char input[200]) {
    int i;
    unsigned long length;
    length = strlen(input);
    for (i = 0; i < length; i++) {
        if (!isdigit(input[i]) && input[i] != '.') {
            return 0;
        }
    }
    return 1;
}

int IsIntNum(char input[200]) {
    int i;
    unsigned long length;
    length = strlen(input);
    for (i = 0; i < length; i++){
        if (!isdigit(input[i])) {
            return 0;
        }
    }
    return 1;
}

int IsPunctuation(char input[200]) {
    if (strcmp(input, ",") == 0)
        return 10;
    else if (strcmp(input, ";") == 0)
        return 11;
    else if (strcmp(input, "(") == 0)
        return 12;
    else if (strcmp(input, ")") == 0)
        return 13;
    else if (strcmp(input, "{") == 0)
        return 14;
    else if (strcmp(input, "}") == 0)
        return 15;
    return 0;
}

int IsCharacter(char input[200]) {
    long int length;
    length = strlen(input);
    if (length == 3) {
        if ((input[0] == '\'') && (input[2] == '\''))
            return 1;
    }
    return 0;
}

int IsIdentifier(char input[200]) {
    if (num_first_id(input))
        return 0;
    if (prohibited_id(input))
        return 0;
    return 1;
}

int num_first_id(char input[200]) {
    char input1 = input[0];
    if (isdigit(input1))
        return 1;
    return 0;
}

int prohibit(char a) {
    if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z'))
        return 0;
    if (a >= '0' && a <= '9')
        return 0;
    return 1;
}

int prohibited_id(char input[200]) {
    long int length;
    length = strlen(input);
    for (int i = 0; i < length; i++) {
        if (prohibit(input[i]) == 1)
            return 1;
    }
    return 0;
}

void find(char input[200], int a) {
    if (IsKeyword(input) == 10) {
        Add_table(input, 10, a);
        return;
    } else if (IsKeyword(input) == 11) {
        Add_table(input, 11, a);
        return;
    } else if (IsKeyword(input) == 12) {
        Add_table(input, 12, a);
        return;
    } else if (IsKeyword(input) == 13) {
        Add_table(input, 13, a);
        return;
    } else if (IsKeyword(input) == 13) {
        Add_table(input, 13, a);
        return;
    } else if (IsKeyword(input) == 14) {
        Add_table(input, 14, a);
        return;
    } else if (IsKeyword(input) == 15) {
        Add_table(input, 15, a);
        return;
    } else if (IsKeyword(input) == 16) {
        Add_table(input, 16, a);
        return;
    } else if (IsKeyword(input) == 17) {
        Add_table(input, 17, a);
        return;
    } else if (IsKeyword(input) == 18) {
        Add_table(input, 18, a);
        return;
    } else if (IsKeyword(input) == 19) {
        Add_table(input, 19, a);
        return;
    }
    if (IsOperator(input) == 10) {
        Add_table(input, 20, a);
        return;
    } else if (IsOperator(input) == 11) {
        Add_table(input, 21, a);
        return;
    } else if (IsOperator(input) == 12) {
        Add_table(input, 22, a);
        return;
    }
    if (IsNumber(input) == 1) {
        Add_table(input, 30, a);
        return;
    }
    if (IsPunctuation(input) == 10) {
        Add_table(input, 40, a);
        return;
    } else if (IsPunctuation(input) == 11) {
        Add_table(input, 41, a);
        return;
    } else if (IsPunctuation(input) == 12) {
        Add_table(input, 42, a);
        return;
    } else if (IsPunctuation(input) == 13) {
        Add_table(input, 43, a);
        return;
    } else if (IsPunctuation(input) == 14) {
        Add_table(input, 44, a);
        return;
    } else if (IsPunctuation(input) == 15) {
        Add_table(input, 45, a);
        return;
    }
    if (IsCharacter(input) == 1) {
        Add_table(input, 50, a);
        return;
    }
    if (IsIdentifier(input) == 1) {
        Add_table(input, 60, a);
        return;
    }
    Add_table(input, 70, a);
}

void Add_table(char name[200], int a, int b) {
    if (headtable == NULL) {
        SimpleTablePtr tmp;
        tmp = malloc(sizeof (SimpleTable));
        strcpy(tmp->name, name);
        tmp->type = a;
        tmp->linenum = b;
        headtable = tmp;
        headtable->next = NULL;
    } else {
        SimpleTablePtr tmp, right;
        tmp = malloc(sizeof (SimpleTable));
        right = headtable;
        while (right->next != NULL)
            right = right->next;
        strcpy(tmp->name, name);
        tmp->type = a;
        tmp->linenum = b;
        right->next = tmp;
        right = tmp;
        right->next = NULL;
    }
    return;
}

void Syntax() {
    SimpleTablePtr pre = headtable;
    SimpleTablePtr tmp = headtable;
    while (tmp != NULL) {
        switch (tmp->type) {
            case 13:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 60:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 20:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 30:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 21:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 30:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 41:
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    case 60:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 41:
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                                    pre = pre->next->next;
                                                                    continue;
                                                                }
                                                        }
                                                    default:
                                                        u++;
                                                        printf("Error in line %d : %s should be Number/Identifier\n", tmp->linenum, tmp->name);
                                                        while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            pre = pre->next;
                                                            tmp = tmp->next;
                                                        }
                                                        //                                                        pre = pre->next;
                                                        continue;
                                                }
                                            case 41:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                pre = pre->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be \";\"/Operator\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be ;/Operator\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    case 60:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 21:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 30:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 41:
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    case 60:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 41:
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be Number/Identifier\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            case 41:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                pre = pre->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be \";\"/Operator\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be ;/Operator\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be Number/Identifier\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            case 40:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                        id : case 60:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 40:
                                                goto id;
                                            case 41:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                pre = pre->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be Identifier\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be Identifier\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            case 41:
                                pre = pre->next;
                                tmp = tmp->next;
                                pre = pre->next;
                                continue;
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be \";\"/Operator\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be \";\"/Operator\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    case 10:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 42:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 43:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 44:
                                                openBrace++;
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                pre = pre->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be \")\"\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be \")\"\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be \"(\"\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be \"(\"\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be Identifier\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be Identifier\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 14:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 60:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 20:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 50:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 41:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be Character\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be Character\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            case 41:
                                pre = pre->next;
                                tmp = tmp->next;
                                pre = pre->next;
                                continue;
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be ;/=\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be \";\"/=\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be Identifier\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be Identifier\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 15:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 60:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 20:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 18:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 41:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                pre = pre->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be TRUE/FALSE\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be TRUE/FALSE\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            case 41:
                                pre = pre->next;
                                tmp = tmp->next;
                                pre = pre->next;
                                continue;
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be ;/=\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be \";\"/=\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be Identifier\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be Identifier\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 60:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 20:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 30:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 21:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 30:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 41:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        pre = pre->next;
                                                        continue;
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            case 60:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 41:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        pre = pre->next;
                                                        continue;
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be  Number/Identifier\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    case 41:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        pre = pre->next;
                                        continue;
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be ;/Operator\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be \";\"/Operator\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            case 60:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 21:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 30:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 41:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        continue;
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                            pre = pre->next->next;
                                                            continue;
                                                        }
                                                }
                                            case 60:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 41:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        pre = pre->next;
                                                        continue;
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be ;\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be  Number/Identifier\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    case 41:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        pre = pre->next;
                                        continue;
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be ;/Operator\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            //                                            printf("%s %d %s %d\n", pre->name, pre->linenum, tmp->name, tmp->linenum);
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be \";\"/Operator\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be  Number/Identifier\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be Operator\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be Operator\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 16:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 10:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 42:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 43:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 44:
                                                openBrace++;
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                pre = pre->next;
                                                continue;
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be \")\"\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be \")\"\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be \"(\"\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be \"(\"\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be \"main\"\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be \"main\"\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 11:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 42:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 60:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 22:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 30:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 43:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 44:
                                                                openBrace++;
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be \")\"\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \")\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            case 60:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 43:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 44:
                                                                openBrace++;
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be \")\"\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \")\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be Number/Identifier\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be Operator\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            printf("%s %d %s %d\n", pre->name, pre->linenum, tmp->name, tmp->linenum);
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be Operator\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            case 30:
                                pre = pre->next;
                                tmp = tmp->next;
                                switch (tmp->type) {
                                    case 22:
                                        pre = pre->next;
                                        tmp = tmp->next;
                                        switch (tmp->type) {
                                            case 30:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 43:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 44:
                                                                openBrace++;
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    u++;
                                                                    printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be \")\"\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \")\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            case 60:
                                                pre = pre->next;
                                                tmp = tmp->next;
                                                switch (tmp->type) {
                                                    case 43:
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                        switch (tmp->type) {
                                                            case 44:
                                                                openBrace++;
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                                pre = pre->next;
                                                                continue;
                                                            default:
                                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                    printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                        pre = pre->next;
                                                                        tmp = tmp->next;
                                                                    }
                                                                    //                                                                    pre = pre->next;
                                                                    continue;
                                                                } else {
                                                                    u++;
                                                                    printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                                                                    pre = pre->next;
                                                                    continue;
                                                                }
                                                        }
                                                    default:
                                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                            u++;
                                                            printf("Error in line %d : %s should be \")\"\n", tmp->linenum, tmp->name);
                                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                                pre = pre->next;
                                                                tmp = tmp->next;
                                                            }
                                                            //                                                            pre = pre->next;
                                                            continue;
                                                        } else {
                                                            u++;
                                                            printf("Error in line %d : There should be \")\"\n", pre->linenum);
                                                            pre = pre->next;
                                                            continue;
                                                        }
                                                }
                                            default:
                                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                    u++;
                                                    printf("Error in line %d : %s should be Number/Identifier\n", tmp->linenum, tmp->name);
                                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                        pre = pre->next;
                                                        tmp = tmp->next;
                                                    }
                                                    //                                                    pre = pre->next;
                                                    continue;
                                                } else {
                                                    u++;
                                                    printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                                    pre = pre->next;
                                                    continue;
                                                }
                                        }
                                    default:
                                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                            u++;
                                            printf("Error in line %d : %s should be Operator\n", tmp->linenum, tmp->name);
                                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                                pre = pre->next;
                                                tmp = tmp->next;
                                            }
                                            //                                            pre = pre->next;
                                            continue;
                                        } else {
                                            u++;
                                            printf("Error in line %d : There should be Operator\n", pre->linenum);
                                            pre = pre->next;
                                            continue;
                                        }
                                }
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be Number/Identifier\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be Number/Identifier\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be \"(\"\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be \"(\"\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 12:
                pre = pre->next;
                tmp = tmp->next;
                switch (tmp->type) {
                    case 44:
                        openBrace++;
                        pre = pre->next;
                        tmp = tmp->next;
                        pre = pre->next;
                        continue;
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be \"{\"\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be \"{\"\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            case 45:
                closeBrace++;
                if (openBrace < closeBrace) {
                    tmp = tmp->next;
                    pre = pre->next;
                    continue;
                } else if (openBrace >= closeBrace) {
                    break;
                }
            case 19:
                tmp = tmp->next;
                switch (tmp->type) {
                    case 30:
                        pre = pre->next;
                        tmp = tmp->next;
                        switch (tmp->type) {
                            case 41:
                                pre = pre->next;
                                tmp = tmp->next;
                                pre = pre->next;
                                continue;
                            default:
                                if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                    u++;
                                    printf("Error in line %d : %s should be \";\"\n", tmp->linenum, tmp->name);
                                    while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                        pre = pre->next;
                                        tmp = tmp->next;
                                    }
                                    //                                    pre = pre->next;
                                    continue;
                                } else {
                                    u++;
                                    printf("Error in line %d : There should be \";\"\n", pre->linenum);
                                    pre = pre->next;
                                    continue;
                                }
                        }
                    case 41:
                        pre = pre->next;
                        tmp = tmp->next;
                        pre = pre->next;
                        continue;
                    default:
                        if (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                            u++;
                            printf("Error in line %d : %s should be \";\"\n", tmp->linenum, tmp->name);
                            while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                                printf("%s %d %s %d\n", pre->name, pre->linenum, tmp->name, tmp->linenum);
                                pre = pre->next;
                                tmp = tmp->next;
                            }
                            //                            pre = pre->next;
                            continue;
                        } else {
                            u++;
                            printf("Error in line %d : There should be \";\"\n", pre->linenum);
                            pre = pre->next;
                            continue;
                        }
                }
            default:
                u++;
                printf("Error in line %d : Ridi :D \n", tmp->linenum);
                //                while (tmp->type != 41) {
                //                    tmp = tmp->next;
                //                }
                //                tmp = tmp->next;
                while (tmp->type != 11 && tmp->type != 12 && tmp->type != 13 && tmp->type != 14 && tmp->type != 15 && tmp->type != 16 && tmp->type != 19 && tmp->type != 45) {
                    pre = pre->next;
                    tmp = tmp->next;
                }
                pre = pre->next;
                tmp = tmp->next;
                continue;
        }
    }
    return;
}

void IRGenerate(){
    FILE * pFile;
    pFile = fopen ("/Users/Alireza/Downloads/myfile.txt","w");
    SimpleTablePtr tmp = headtable;
    int a , b;
    char loop[200];
    char p;
    int o = 1;
    int m = 1;
    while (tmp != NULL)
    {
        if (strcmp(tmp->name,"return") == 0){
            fprintf(pFile,"RETURN\nCALL MAIN\n");
            tmp = tmp->next;
        }
        if (strcmp(tmp->name,"while") == 0)
        {
            fprintf(pFile,"goto Y%d\n",m+1);
            fprintf(pFile,"Y%d:\n",m);
            tmp = tmp->next->next;
            SimpleTablePtr tmp1 = tmp;
            while (strcmp(tmp->name,")") != 0 )
                tmp = tmp->next ;
            while (strcmp(tmp->name,"}") != 0){
                if (tmp->type == 60){
                    a=find_id_index(tmp->name);
                    tmp = tmp->next;
                    if ((tmp->type >= 20) && (tmp->type <= 23))
                    {
                        tmp = tmp->next;
                        if (strcmp(tmp->name,"true") == 0){
                            fprintf(pFile,"T%d := TRUE\n",a);
                            tmp = tmp->next;
                        }
                        if (strcmp(tmp->name,"false") == 0){
                            fprintf(pFile,"T%d := FALSE\n",a);
                            tmp = tmp->next;
                        }
                        
                        if (tmp->type == 50){
                            fprintf(pFile,"C%d := %s\n",find_char_index(tmp->name),tmp->name);
                            fprintf(pFile,"T%d := C%d\n",a,find_char_index(tmp->name));
                            tmp = tmp->next->next;
                        }
                        if (tmp->type == 60){
                            b = find_id_index(tmp->name);
                            tmp = tmp->next;
                            if ((tmp->type >= 20) && (tmp->type <= 23))
                            {
                                p = tmp->name[0];
                                tmp = tmp->next;
                                if (tmp->type == 30)
                                {
                                    fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                    fprintf(pFile,"T%d := T%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                                    tmp = tmp->next;
                                    continue;
                                }
                                if (tmp->type == 60)
                                {
                                    fprintf(pFile,"T%d := T%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                                    tmp = tmp->next;
                                    continue;
                                }
                            }
                            if ((tmp->type >= 40) && (tmp->type <= 45))
                            {
                                fprintf(pFile,"T%d := T%d\n",a,b);
                            }
                        }
                        if (tmp->type == 30){
                            fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                            b = find_num_index(tmp->name);
                            strcpy(loop,tmp->name);
                            tmp = tmp->next;
                            if ((tmp->type >= 20) && (tmp->type <= 23))
                            {
                                p = tmp->name[0];
                                tmp = tmp->next;
                                if (tmp->type == 30)
                                {
                                    fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                    fprintf(pFile,"T%d := R%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                                    tmp = tmp->next;
                                    continue;
                                }
                                if (tmp->type == 60)
                                {
                                    fprintf(pFile,"T%d := R%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                                    tmp = tmp->next;
                                    continue;
                                }
                            }
                            if ((tmp->type >= 40) && (tmp->type <= 45))
                            {
                                fprintf(pFile,"T%d := R%d\n",a,b);
                            }
                        }
                    }
                }
                if (strcmp(tmp->next->name,"while") != 0)
                    tmp = tmp->next;
            }
            fprintf(pFile,"Y%d:\n",m+1);
            if (tmp1->type == 60)
            {
                a = find_id_index(tmp1->name);
                tmp1 = tmp1->next;
                strcpy(loop,tmp1->name);
                tmp1 = tmp1->next;
                if (tmp1->type == 60)
                    fprintf(pFile,"IF T%d %s T%d goto Y%d\n",a,loop,find_id_index(tmp1->name),m);
                if (tmp1->type == 30)
                    fprintf(pFile,"IF T%d %s R%d goto Y%d\n",a,loop,find_num_index(tmp1->name),m);
            }
            if (tmp1->type == 30)
            {
                a = find_num_index(tmp1->name);
                tmp1 = tmp1->next;
                strcpy(loop,tmp1->name);
                tmp1 = tmp1->next;
                if (tmp1->type == 60)
                    fprintf(pFile,"IF T%d %s T%d goto Y%d\n",a,loop,find_id_index(tmp->name),m);
                if (tmp1->type == 30)
                    fprintf(pFile,"IF T%d %s R%d goto Y%d\n",a,loop,find_num_index(tmp->name),m);
            }
            m++;
        }
        if (tmp->type == 60){
            a=find_id_index(tmp->name);
            tmp = tmp->next;
            if ((tmp->type >= 20) && (tmp->type <= 23))
            {
                tmp = tmp->next;
                if (strcmp(tmp->name,"true") == 0){
                    fprintf(pFile,"T%d := TRUE\n",a);
                    tmp = tmp->next;
                }
                if (strcmp(tmp->name,"false") == 0){
                    fprintf(pFile,"T%d := FALSE\n",a);
                    tmp = tmp->next;
                }
                if (tmp->type == 50){
                    fprintf(pFile,"C%d := %s\n",find_char_index(tmp->name),tmp->name);
                    fprintf(pFile,"T%d := C%d\n",a,find_char_index(tmp->name));
                    tmp = tmp->next->next;
                }
                if (tmp->type == 60){
                    b = find_id_index(tmp->name);
                    tmp = tmp->next;
                    if ((tmp->type >= 20) && (tmp->type <= 23))
                    {
                        p = tmp->name[0];
                        tmp = tmp->next;
                        if (tmp->type == 30)
                        {
                            fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                            fprintf(pFile,"T%d := T%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                            tmp = tmp->next;
                            continue;
                        }
                        if (tmp->type == 60)
                        {
                            fprintf(pFile,"T%d := T%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                            tmp = tmp->next;
                            continue;
                        }
                    }
                    if ((tmp->type >= 40) && (tmp->type <= 45))
                    {
                        fprintf(pFile,"T%d := T%d\n",a,b);
                    }
                }
                if (tmp->type == 30){
                    fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                    b = find_num_index(tmp->name);
                    strcpy(loop,tmp->name);
                    tmp = tmp->next;
                    if ((tmp->type >= 20) && (tmp->type <= 23))
                    {
                        p = tmp->name[0];
                        tmp = tmp->next;
                        if (tmp->type == 30)
                        {
                            fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                            fprintf(pFile,"T%d := R%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                            tmp = tmp->next;
                            continue;
                        }
                        if (tmp->type == 60)
                        {
                            fprintf(pFile,"T%d := R%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                            tmp = tmp->next;
                            continue;
                        }
                    }
                    if ((tmp->type >= 40) && (tmp->type <= 45))
                    {
                        fprintf(pFile,"T%d := R%d\n",a,b);
                    }
                }
            }
        }
        if ((strcmp(tmp->name,"void") == 0 || strcmp(tmp->name,"int") == 0) && (strcmp(tmp->next->name,"main") == 0))
        {
            tmp = tmp->next->next;
            fprintf(pFile,"PROCEDURE MAIN\n");
            tmp = tmp->next->next;
            if (strcmp(tmp->name,"{") == 0)
                fprintf(pFile,"BEGIN\n");
        }
        if (strcmp(tmp->name,"if") == 0)
        {
            fprintf(pFile,"IF ");
            tmp = tmp->next;
            tmp = tmp->next;
            if (tmp->type == 60)
            {
                a = find_id_index(tmp->name);
                tmp = tmp->next;
                strcpy(loop,tmp->name);
                tmp = tmp->next;
                if (tmp->type == 60)
                {
                    fprintf(pFile,"T%d %s T%d ",a,loop,find_id_index(tmp->name));
                    tmp=tmp->next;
                    tmp=tmp->next;
                }
                if (tmp->type == 30)
                {
                    fprintf(pFile,"T%d %s R%d ",a,loop,find_num_index(tmp->name));
                    tmp=tmp->next;
                    tmp=tmp->next;
                }
            }
            if (tmp->type == 30)
            {
                a = find_num_index(tmp->name);
                tmp = tmp->next;
                strcpy(loop,tmp->name);
                tmp = tmp->next;
                if (tmp->type == 60)
                {
                    fprintf(pFile,"T%d %s T%d ",a,loop,find_id_index(tmp->name));
                    tmp=tmp->next;
                    tmp=tmp->next;
                }
                if (tmp->type == 30)
                {
                    fprintf(pFile,"T%d %s R%d ",a,loop,find_num_index(tmp->name));
                    tmp=tmp->next;
                    tmp=tmp->next;
                }
            }
            fprintf(pFile,"goto L%d \n",o);
            if (strcmp(tmp->name,")")== 0)
                tmp = tmp->next;
            if (strcmp(tmp->name,"{") == 0)
            {
                SimpleTablePtr tmp1 = tmp;
                while (strcmp(tmp1->name,"}") != 0)
                {
                    tmp1 = tmp1->next;
                }
                if (tmp1->next != NULL){
                    if (strcmp(tmp1->next->name,"else") == 0)
                        fprintf(pFile,"ELSE goto L%d \n",o+1);
                }
                if (tmp1->next == NULL)
                    fprintf(pFile,"\n");
                fprintf(pFile,"L%d:\n",o);
                while (strcmp(tmp->name,"}") != 0)
                {
                    if (strcmp(tmp->name,";") == 0)
                        while (strcmp(tmp->name,";") != 0)
                            tmp = tmp->next;
                    if (tmp->type == 60){
                        a=find_id_index(tmp->name);
                        tmp = tmp->next;
                        if ((tmp->type >= 20) && (tmp->type <= 23))
                        {
                            tmp = tmp->next;
                            if (strcmp(tmp->name,"true") == 0){
                                fprintf(pFile,"T%d := TRUE\n",a);
                                tmp = tmp->next;
                            }
                            if (strcmp(tmp->name,"false") == 0){
                                fprintf(pFile,"T%d := FALSE\n",a);
                                tmp = tmp->next;
                            }
                            if (tmp->type == 50){
                                fprintf(pFile,"C%d := %s\n",find_char_index(tmp->name),tmp->name);
                                fprintf(pFile,"T%d := C%d\n",a,find_char_index(tmp->name));
                                tmp = tmp->next->next;
                                continue;
                            }
                            if (tmp->type == 60){
                                b = find_id_index(tmp->name);
                                tmp = tmp->next;
                                if ((tmp->type >= 20) && (tmp->type <= 23))
                                {
                                    p = tmp->name[0];
                                    tmp = tmp->next;
                                    if (tmp->type == 30)
                                    {
                                        fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                        fprintf(pFile,"T%d := T%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                                        tmp = tmp->next;
                                        continue;
                                        
                                    }
                                    if (tmp->type == 60)
                                    {
                                        fprintf(pFile,"T%d := T%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                                        tmp = tmp->next;
                                        continue;
                                    }
                                }
                                if ((tmp->type >= 40) && (tmp->type <= 45))
                                {
                                    fprintf(pFile,"T%d := T%d\n",a,b);
                                }
                            }
                            if (tmp->type == 30){
                                fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                b = find_num_index(tmp->name);
                                strcpy(loop,tmp->name);
                                tmp = tmp->next;
                                if ((tmp->type >= 20) && (tmp->type <= 23))
                                {
                                    p = tmp->name[0];
                                    tmp = tmp->next;
                                    if (tmp->type == 30)
                                    {
                                        fprintf(pFile,"T%d := R%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                                        tmp = tmp->next;
                                        continue;
                                    }
                                    if (tmp->type == 60)
                                    {
                                        fprintf(pFile,"T%d := R%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                                        tmp = tmp->next;
                                        continue;
                                    }
                                }
                                if ((tmp->type >= 40) && (tmp->type <= 45))
                                {
                                    fprintf(pFile,"T%d := R%d\n",a,b);
                                }
                            }
                        }
                    }
                    tmp = tmp->next;
                }
                if (strcmp(tmp->name,"}") == 0 & strcmp(tmp->next->name,"else") == 0)
                    tmp = tmp->next;
                if (tmp != NULL){
                    if (strcmp(tmp->name,"else") == 0)
                    {
                        tmp = tmp->next;
                        if (strcmp(tmp->name,"{") == 0)
                        {
                            tmp = tmp->next;
                            if (strcmp(tmp->name,";") == 0){
                                while (strcmp(tmp->name,";") != 0)
                                    tmp = tmp->next;
                            }
                            fprintf(pFile,"L%d:\n",o+1);
                            o +=2;
                            if ((tmp->type >= 10) && (tmp->type <= 19))
                                tmp = tmp->next;
                            if (tmp->type == 60){
                                a=find_id_index(tmp->name);
                                tmp = tmp->next;
                                if ((tmp->type >= 20) && (tmp->type <= 23))
                                {
                                    tmp = tmp->next;
                                    if (strcmp(tmp->name,"true") == 0){
                                        fprintf(pFile,"T%d := TRUE\n",a);
                                        tmp = tmp->next;
                                    }
                                    if (strcmp(tmp->name,"false") == 0){
                                        fprintf(pFile,"T%d := FALSE\n",a);
                                        tmp = tmp->next;
                                    }
                                    if (tmp->type == 50){
                                        fprintf(pFile,"C%d := %s\n",find_char_index(tmp->name),tmp->name);
                                        fprintf(pFile,"T%d := C%d\n",a,find_char_index(tmp->name));
                                        tmp = tmp->next->next;
                                    }
                                    if (tmp->type == 60){
                                        b = find_id_index(tmp->name);
                                        tmp = tmp->next;
                                        if ((tmp->type >= 20) && (tmp->type <= 23))
                                        {
                                            p = tmp->name[0];
                                            tmp = tmp->next;
                                            if (tmp->type == 30)
                                            {
                                                fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                                fprintf(pFile,"T%d := T%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                                                tmp = tmp->next;
                                                continue;
                                                
                                            }
                                            if (tmp->type == 60)
                                            {
                                                fprintf(pFile,"T%d := T%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                                                tmp = tmp->next;
                                                continue;
                                            }
                                        }
                                        if ((tmp->type >= 40) && (tmp->type <= 45))
                                        {
                                            fprintf(pFile,"T%d := T%d\n",a,b);
                                        }
                                    }
                                    if (tmp->type == 30){
                                        fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                        b = find_num_index(tmp->name);
                                        strcpy(loop,tmp->name);
                                        tmp = tmp->next;
                                        if ((tmp->type >= 20) && (tmp->type <= 23))
                                        {
                                            p = tmp->name[0];
                                            tmp = tmp->next;
                                            if (tmp->type == 30)
                                            {
                                                fprintf(pFile,"R%d := %s\n",find_num_index(tmp->name),tmp->name);
                                                fprintf(pFile,"T%d := R%d %c R%d\n",a,b,p,find_num_index(tmp->name));
                                                tmp = tmp->next;
                                                continue;
                                            }
                                            if (tmp->type == 60)
                                            {
                                                fprintf(pFile,"T%d := R%d %c T%d\n",a,b,p,find_id_index(tmp->name));
                                                tmp = tmp->next;
                                                continue;
                                            }
                                        }
                                        if ((tmp->type >= 40) && (tmp->type <= 45))
                                        {
                                            fprintf(pFile,"T%d := R%d\n",a,b);
                                        }
                                    }
                                }
                            }
                            if (strcmp(tmp->next->name,"while") != 0)
                                tmp = tmp->next;
                        }
                    }
                    else {
                        o += 2;
                    }
                }
            }
        }
        if (strcmp(tmp->name,"return") == 0)
            fprintf(pFile,"RETURN\nCALL MAIN\n");
        if (tmp == NULL)
            break;
        tmp = tmp->next;
    }
}

void symbol_table() {
    SimpleTablePtr tmp = headtable;
    heads = (symboltable*) malloc(sizeof (symboltable));
    heads->next = NULL;
    headn = (numbertable*) malloc(sizeof (numbertable));
    headn->next = NULL;
    headc = (charactertable*) malloc(sizeof (charactertable));
    headc->next = NULL;
    int i = 1;
    int j = 1;
    int k = 1;
    while (tmp != NULL) {
        if ((tmp->type == 60) && (IsRepetitive(tmp->name) == 0)) {
            symboltableptr tmp1, right;
            tmp1 = malloc(sizeof (symboltable));
            right = heads;
            while (right->next != NULL)
                right = right->next;
            strcpy(tmp1->name, tmp->name);
            tmp1->index = i;
            tmp1->try = 0;
            right->next = tmp1;
            right = tmp1;
            right->next = NULL;
            i++;
        }
        if ((tmp->type == 30) && (IsRepetitive(tmp->name) == 0)) {
            numbertableptr tmp1, right;
            tmp1 = malloc(sizeof (numbertable));
            right = headn;
            while (right->next != NULL)
                right = right->next;
            strcpy(tmp1->name, tmp->name);
            tmp1->index = j;
            tmp1->try = 0;
            right->next = tmp1;
            right = tmp1;
            right->next = NULL;
            j++;
        }
        if ((tmp->type == 50) && (IsRepetitive(tmp->name) == 0)) {
            charactertableptr tmp1, right;
            tmp1 = malloc(sizeof (charactertable));
            right = headc;
            while (right->next != NULL)
                right = right->next;
            strcpy(tmp1->name, tmp->name);
            tmp1->index = k;
            tmp1->try = 0;
            right->next = tmp1;
            right = tmp1;
            right->next = NULL;
            k++;
        }
        tmp = tmp->next;
        
    }
}

void undefined_id(){
    SimpleTablePtr tmp = headtable;
//    definedptr tmpd = headdefine;
//    char value[200];
    int y = 1000;
    while (tmp != NULL) {
        if ((tmp->type == 13 || tmp->type == 14 || tmp->type == 15) && tmp->next->type == 60){
            if (tmp->type == 13)
            {
                if (strcmp(tmp->name,"int") == 0)
                {
                    tmp = tmp->next;
                    if (headdefine != NULL)
                    {
                        definedptr tmptmp = headdefine;
                        while(tmptmp != NULL || y != 0)
                        {
                            if (strcmp(tmptmp->name,tmp->name) == 0)
                            {
                                printf("Variable %s in line %d is Redefined!\n",tmp->name,tmp->linenum);
                                y = 0 ;
                            }
                            printf("%s",tmptmp->name);
                            tmptmp = tmptmp->next;
                            printf("%s",tmptmp->name);
                        }
                        y=1000;
                    }
                    if (y != 0)
                    {
                        if (headdefine == NULL)
                        {
                        definedptr tmp1;
                        tmp1 = malloc(sizeof (defined));
                        strcpy(tmp1->name, tmp->name);
                        strcpy(tmp1->kind,"int");
                        headdefine = tmp1;
                        headdefine->next = NULL;
                        }
                        else
                        {
                        definedptr tmp1, right;
                        tmp1 = malloc(sizeof (defined));
                        right = headdefine;
                        while (right->next != NULL)
                            right = right->next;
                        strcpy(tmp1->name, tmp->name);
                        strcpy(tmp1->kind,"int");
                        right->next = tmp1;
                        right = tmp1;
                        right->next = NULL;
                        }
                    }
                }
                
                if (strcmp(tmp->name,"float") == 0)
                {
                    tmp = tmp->next;
                    if (headdefine != NULL)
                    {
                        definedptr tmptmp = headdefine;
                        while(tmptmp != NULL || y != 0)
                        {
                            if (strcmp(tmptmp->name,tmp->name) == 0)
                            {
                                printf("Variable %s in line %d is Redefined!\n",tmp->name,tmp->linenum);
                                y = 0 ;
                            }
                            tmptmp = tmptmp->next;
                        }
                    }
                    if (y != 0)
                    {
                        if (headdefine == NULL)
                        {
                            definedptr tmp1;
                            tmp1 = malloc(sizeof (defined));
                            strcpy(tmp1->name, tmp->name);
                            strcpy(tmp1->kind,"float");
                            headdefine = tmp1;
                            headdefine->next = NULL;
                            printf("%s",tmp1->name);
                        }
                        definedptr tmp1, right;
                        tmp1 = malloc(sizeof (defined));
                        right = headdefine;
                        while (right->next != NULL)
                            right = right->next;
                        strcpy(tmp1->name, tmp->name);
                        strcpy(tmp1->kind,"float");
                        right->next = tmp1;
                        right = tmp1;
                        right->next = NULL;
                    }
                }
            }
            if (tmp->type == 14)
            {
                tmp = tmp->next;
                definedptr tmptmp = headdefine;
                while(tmptmp != NULL || y != 0)
                {
                    if (strcmp(tmptmp->name,tmp->name) == 0)
                    {
                        printf("Variable %s in line %d is Redefined!\n",tmp->name,tmp->linenum);
                        y = 0 ;
                    }
                    tmptmp = tmptmp->next;
                }
                if (y != 0)
                {
                    definedptr tmp1, right;
                    tmp1 = malloc(sizeof (defined));
                    right = headdefine;
                    while (right->next != NULL)
                        right = right->next;
                    strcpy(tmp1->name, tmp->name);
                    strcpy(tmp1->kind,"char");
                    right->next = tmp1;
                    right = tmp1;
                    right->next = NULL;
                }
            }
            if (tmp->type == 15)
            {
                tmp = tmp->next;
                definedptr tmptmp = headdefine;
                while(tmptmp != NULL || y != 0)
                {
                    if (strcmp(tmptmp->name,tmp->name) == 0)
                    {
                        printf("Variable %s in line %d is Redefined!\n",tmp->name,tmp->linenum);
                        y = 0 ;
                    }
                    tmptmp = tmptmp->next;
                }
                if (y != 0)
                {
                    definedptr tmp1, right;
                    tmp1 = malloc(sizeof (defined));
                    right = headdefine;
                    while (right->next != NULL)
                        right = right->next;
                    strcpy(tmp1->name, tmp->name);
                    strcpy(tmp1->kind,"bool");
                    right->next = tmp1;
                    right = tmp1;
                    right->next = NULL;
                }
            }
        }
        if (tmp->type == 60)
        {
            int q =0 ;
            definedptr tmptmp = headdefine;
            while(tmptmp != NULL)
            {
                if (strcmp(tmptmp->name,tmp->name) == 0)
                    q++ ;
                tmptmp = tmptmp->next;
            }
            if (q == 0)
            {
                printf("Variable %s in line %d is not defined!\n",tmp->name,tmp->linenum);
            }
        }
        tmp = tmp->next;
        printf("%s\n\n",tmp->name);
    }
}

void type_checking(){
    SimpleTablePtr tmp = headtable;
    while (tmp != NULL){
        if(tmp->type == 15)
        {
            tmp = tmp->next->next ;
            if (tmp->type != 41)
            {
                if (tmp->type == 20){
                    tmp = tmp->next;
                    if (tmp->type != 18)
                        printf("in line %d \"%s\" was expected to be true/false\n",tmp->linenum,tmp->name);
                }
            }
        }
        if(tmp->type == 13){
            if (strcmp(tmp->name,"int") == 0)
            {
                tmp = tmp->next->next ;
                if (tmp->type != 41)
                {
                    if (tmp->type == 20)
                    {
                        tmp = tmp->next;
                        if (tmp->type == 60 && tmp->next->type == 41)
                        {
                        }
                        if (tmp->type == 30 && tmp->next->type == 41)
                        {
                            if (IsIntNum(tmp->name) == 0)
                            {
                                printf("in line %d \"%s\" was expected to be \"int value\"\n",tmp->linenum,tmp->name);
                            }
                        }
                        if (tmp->type == 60 && tmp->next->type == 60)
                        {
                            if (IsIntNum(tmp->name)){
                                
                            }
                        }
                        if (tmp->type == 60 && tmp->next->next->type == 30)
                        {
                            
                        }
                        if (tmp->type == 30 && tmp->next->next->type == 60)
                        {
                            
                        }
                        if (tmp->type == 30 && tmp->next->next->type == 30)
                        {
                            if (IsIntNum(tmp->name) == 0 && IsIntNum(tmp->next->next->name) == 1)
                            {
                                printf("in line %d \"%s\" was expected to be \"int value\"\n",tmp->linenum,tmp->name);
                            }
                            if (IsIntNum(tmp->name) == 1 && IsIntNum(tmp->next->next->name) == 0)
                            {
                                printf("in line %d \"%s\" was expected to be \"int value\"\n",tmp->next->next->linenum,tmp->next->next->name);
                            }
                            if (IsIntNum(tmp->name) == 0 && IsIntNum(tmp->next->next->name) == 0)
                            {
                                printf("in line %d \"%s\" was expected to be \"int value\" & in line %d \"%s\" was expected to be \"int value\"\n",tmp->linenum,tmp->name,tmp->next->next->linenum,tmp->next->next->name);
                            }
                        }
                    }
                }
            }
            if (strcmp(tmp->name,"float") == 0)
            {
                tmp = tmp->next->next ;
                if (tmp->type != 41)
                {
                    if (tmp->type == 20)
                    {}
                }
            }
        }
        if(tmp->type == 14)
        {
            tmp = tmp->next->next ;
            if (tmp->type != 41)
            {
                tmp = tmp->next;
                if (tmp->type != 50)
                    printf("in line %d \"%s\" was expected to be a character\n",tmp->linenum,tmp->name);
            }
        }
        tmp = tmp->next;
    }
}

int IsRepetitive(char name[100]) {
    symboltableptr tmp = heads->next;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0)
            return 1;
        tmp = tmp->next;
    }
    return 0;
}

int find_id_index(char name[200]) {
    symboltableptr tmp = heads->next;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            tmp->try = 1;
            return tmp->index;
        }
        tmp = tmp->next;
    }
    return 0;
}

int find_num_index(char name[200]) {
    numbertableptr tmp = headn->next;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            tmp->try = 1;
            return tmp->index;
        }
        tmp = tmp->next;
    }
    return 0;
}

int find_char_index(char name[200]) {
    charactertableptr tmp = headc->next;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            tmp->try = 1;
            return tmp->index;
        }
        tmp = tmp->next;
    }
    return 0;
}