#ifndef CODE_CONST_CONDITION_H
#define CODE_CONST_CONDITION_H

#include <dataflow_analysis.h>

void const_condition_delete(IR_function* func);
void delete_dead_code(IR_function *func);

#endif