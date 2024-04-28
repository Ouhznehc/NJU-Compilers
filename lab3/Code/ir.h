#ifndef __IR_H__
#define __IR_H__
#include "grammar.h"
#include "semantics.h"

typedef struct arg_t arg_t;
typedef struct ic_t ic_t;
typedef struct ir_t ir_t;


/// `ArgAddr` stands for variable which do not implement Copy trait, like Struct and Array.
/// So when operate on it, we need to dereference it first to get value. 
typedef enum {
    ArgVar, ArgTmp, ArgImm,
    ArgLabel, ArgFunc, ArgRelop, ArgSize,
    ArgAddr
} ArgKind;
typedef enum {
    IcLabel, IcFunc, IcReturn, IcGoto, IcArg, IcParam, IcRead, IcWrite,
    IcAssign, IcCall, IcMinus, IcLeftStar, IcRightStar, IcRef,
    IcAdd, IcSub, IcMul, IcDiv, IcDec,
    IcBranch
} InterCodeKind;

struct arg_t {
    ArgKind kind;
    union {
        int cons; // for const value like: label1, #5, DEC a 100, etc.
        char name[64]; // for variable name like: a, instance, etc.
    };
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

ir_t *ir_head, *ir_tail;
int var_no = 0, tmp_no = 0, label_no = 0;

void generate_ir(syntax_t* root, FILE* ir);

#endif