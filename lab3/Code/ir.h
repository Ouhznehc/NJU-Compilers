#ifndef __IR_H__
#define __IR_H__
#include "grammar.h"
#include "semantics.h"

void generate_ir(syntax_t* root, FILE* ir);
#endif