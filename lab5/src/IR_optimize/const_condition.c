#include <const_condition.h>

static int check_condition(IR_if_stmt *if_stmt) {
    IR_val op1 = if_stmt->rs1;
    IR_val op2 = if_stmt->rs2;
    if (!op1.is_const || !op2.is_const) return -1;
    switch (if_stmt->relop) {
        case IR_RELOP_EQ: return op1.const_val == op2.const_val;
        case IR_RELOP_NE: return op1.const_val != op2.const_val;
        case IR_RELOP_LT: return op1.const_val < op2.const_val;
        case IR_RELOP_GT: return op1.const_val > op2.const_val;
        case IR_RELOP_LE: return op1.const_val <= op2.const_val;
        case IR_RELOP_GE: return op1.const_val >= op2.const_val;
        default: assert(0);
    }
}

static bool delete_dead_dominated_blk(IR_function *func,  IR_block *block) {
    List_IR_block_ptr *pred = VCALL(func->blk_pred, get, block);
    for_list(IR_block_ptr, i, *pred) {
        if (i->val->dead == false) return false;
    }
    block->dead = true;
    return true;
}


static void delete_edge(IR_function *func,  IR_block *from, IR_block *to) {
    List_IR_block_ptr *from_succ = VCALL(func->blk_succ, get, from);
    for(ListNode_IR_block_ptr *i = from_succ->head; i;) {
        IR_block *blk = i->val;
        if(blk == to) {
            i = VCALL(*from_succ, delete, i);
        } else i = i->nxt;
    }
    VCALL(func->blk_succ, set, from, from_succ);
}

static void scan_the_graph(IR_function *func, IR_block* block) {
    if (block->visit) return;
    block->visit = true;
    List_IR_block_ptr *succ = VCALL(func->blk_succ, get, block);
    for_list(IR_block_ptr, i, *succ) {
        scan_the_graph(func, i->val);
    }
}

void delete_dead_code(IR_function *func) {
    scan_the_graph(func, func->entry);
    for_list(IR_block_ptr, i, func->blocks) {
        if (i->val->visit == false) {
            // printf("delete\n");
            i->val->dead = true;
        }
    }
}

void const_condition_delete(IR_function *func) {
    for_list(IR_block_ptr, i, func->blocks) {
        if(i->val->stmts.tail == NULL) continue;
        IR_stmt *last_stmt = i->val->stmts.tail->val;
        if (last_stmt->stmt_type != IR_IF_STMT) continue;
        IR_if_stmt *if_stmt = (IR_if_stmt*)last_stmt;
        int condition = check_condition(if_stmt);
        // printf("true_blk = %p, false_blk = %p\n", if_stmt->true_blk, if_stmt->false_blk);
        if (condition == 1) {
            delete_edge(func, i->val, if_stmt->false_blk);
        }
        if (condition == 0) {
            // if_stmt->true_blk->label = IR_LABEL_NONE;
            // if_stmt->false_blk->label = IR_LABEL_NONE;
        } 
    }
    return;
}