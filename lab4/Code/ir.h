#ifndef __IR_H__
#define __IR_H__
#include "grammar.h"
#include "semantics.h"

typedef struct arg_t arg_t;
typedef struct ic_t ic_t;
typedef struct ir_t ir_t;
typedef struct var_t var_t;
typedef struct arglist_t arglist_t;


typedef enum {
    ArgVar, ArgTmp, ArgImm,
    ArgLabel, ArgFunc, ArgRelop, ArgSize,
} ArgKind;
typedef enum {
    // In order to match rules
    IcAdd = 6, IcSub = 7, IcMul = 8, IcDiv = 9, IcDec,
    IcLabel, IcFunc, IcReturn, IcGoto, IcArg, IcParam, IcRead, IcWrite,
    IcAssign, IcCall, IcMinus, IcLeftStar, IcRightStar, IcRef,
    IcBranch
} InterCodeKind;

/// `is_addr` indicate whether the arg is an address or a value
struct arg_t {
    ArgKind kind;
    bool is_addr;
    int cons; // for const value like: label1, #5, DEC a 100, etc.
    char name[64]; // for function name like: main etc.
};

// store var(not tmp), for reuse declared variable
struct var_t {
    arg_t* var;
    var_t* next;
    int size;
};

/// struct of intercode, in detail:
// Assignment: result := arg1
// binary op: result := arg1 op arg2
// single op: result := op arg1
// GOTO ARG PARAM FUNCTION LABEL RETURN: result
// CALL: result := CALL arg1
// goto: IF arg1 relop arg2 GOTO result
// declaration: DEC result arg1
struct ic_t {
    InterCodeKind op;
    arg_t *result, *arg1, *arg2, *relop; 
};

// List of intercodes
struct ir_t {
    ic_t* code;
    ir_t* next;
};

struct arglist_t {
    arg_t* arg;
    arglist_t* next;
    arglist_t* prev;
};

void generate_ir(syntax_t* root, FILE* fp);
void* malloc_safe(int size);

char* ic_to_string(ic_t* ic);
char* arg_to_string(arg_t* arg);
void insert_arg(arg_t* arg, arglist_t* head);

#endif