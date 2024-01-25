#include "cminusf_builder.hpp"

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *INT32PTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

Value* CminusfBuilder::visit(ASTProgram &node) {
    VOID_T = module->get_void_type();
    INT1_T = module->get_int1_type();
    INT32_T = module->get_int32_type();
    INT32PTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();

    Value *ret_val = nullptr;
    for (auto &decl : node.declarations) {
        ret_val = decl->accept(*this);
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTNum &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value* valp;
    if(node.type == TYPE_INT){
        if(context.in_loop){
            builder->set_insert_point(context.outBB);
            valp = builder->create_alloca(INT32_T);
            builder->set_insert_point(context.cur_insert_point);
        }
        else
            valp = builder->create_alloca(INT32_T);
        builder->create_store(CONST_INT(node.i_val),valp);
    }
    else if(node.type == TYPE_FLOAT){
        if(context.in_loop){
            builder->set_insert_point(context.outBB);
            valp = builder->create_alloca(FLOAT_T);
            builder->set_insert_point(context.cur_insert_point);
        }
        else
            valp = builder->create_alloca(FLOAT_T);
        builder->create_store(CONST_FP(node.f_val),valp);
    }
    return builder->create_load(valp);
}

Value* CminusfBuilder::visit(ASTVarDeclaration &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value* val = nullptr;
    if(scope.in_global()){
        if(node.num == nullptr){
            if(node.type == TYPE_INT){
                val = GlobalVariable::create(node.id,module.get(),INT32_T,false,CONST_INT(0));
            }
            else{
                val = GlobalVariable::create(node.id,module.get(),FLOAT_T,false,CONST_FP(0.0));
            }
        }else{
            ArrayType* arrayp = nullptr;
            if(node.type == TYPE_INT)
                arrayp = ArrayType::get(INT32_T,node.num->i_val);
            else
                arrayp = ArrayType::get(FLOAT_T,node.num->i_val);
            val = GlobalVariable::create(node.id,module.get(),arrayp,false,ConstantZero::get(arrayp,module.get()));
        }
        scope.push(node.id,val);
    }
    else{
        if(node.num == nullptr){
            if(node.type == TYPE_INT){
                if(context.in_loop){
                    builder->set_insert_point(context.outBB);
                    val = builder->create_alloca(INT32_T);
                    builder->set_insert_point(context.cur_insert_point);
                }
                else
                    val = builder->create_alloca(INT32_T);
            }
            else{
                if(context.in_loop){
                    builder->set_insert_point(context.outBB);
                    val = builder->create_alloca(FLOAT_T);
                    builder->set_insert_point(context.cur_insert_point);
                }
                else
                    val = builder->create_alloca(FLOAT_T);
            }
        }else{
            ArrayType* arrayp = nullptr;
            if(node.type == TYPE_INT)
                arrayp = ArrayType::get(INT32_T,node.num->i_val);
            else
                arrayp = ArrayType::get(FLOAT_T,node.num->i_val);
            if(context.in_loop){
                builder->set_insert_point(context.outBB);
                val = builder->create_alloca(arrayp);
                builder->set_insert_point(context.cur_insert_point);
            }
            else
                val = builder->create_alloca(arrayp);
        }            
        scope.push(node.id,val);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTFunDeclaration &node) {
    FunctionType *fun_type;
    Type *ret_type;
    std::vector<Type *> param_types;
    if (node.type == TYPE_INT)
        ret_type = INT32_T;
    else if (node.type == TYPE_FLOAT)
        ret_type = FLOAT_T;
    else
        ret_type = VOID_T;

    for (auto &param : node.params) {
        // TODO: Please accomplish param_types.
        if(param->type == TYPE_INT){
            if(param->isarray == true)
                param_types.push_back(INT32PTR_T);
            else
                param_types.push_back(INT32_T);
        }
        else if(param->type == TYPE_FLOAT){
            if(param->isarray == true)
                param_types.push_back(FLOATPTR_T);
            else
                param_types.push_back(FLOAT_T);
        }
    }

    fun_type = FunctionType::get(ret_type, param_types);
    auto func = Function::create(fun_type, node.id, module.get());
    scope.push(node.id, func);
    context.func = func;
    auto funBB = BasicBlock::create(module.get(), "", func);
    builder->set_insert_point(funBB);
    context.cur_insert_point = funBB;
    scope.enter();
    context.is_entered = true;
    std::vector<Value *> args;
    for (auto &arg : func->get_args()) {
        args.push_back(&arg);
    }
    for (int i = 0; i < node.params.size(); ++i) {
        // TODO: You need to deal with params and store them in the scope.
        Value* p = node.params[i]->accept(*this);
        builder->create_store(args[i],p);
    }
    node.compound_stmt->accept(*this);
    if (not builder->get_insert_block()->is_terminated())
    {
        if (context.func->get_return_type() == VOID_T)
            builder->create_void_ret();
        else if (context.func->get_return_type() == FLOAT_T)
            builder->create_ret(CONST_FP(0.0));
        else
            builder->create_ret(CONST_INT(0));
    }
    scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTParam &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value* a;
    if(node.type == TYPE_INT){
        if(node.isarray == true)
            a = builder->create_alloca(INT32PTR_T);
        else
            a = builder->create_alloca(INT32_T);
        scope.push(node.id,a);
    }
    else if(node.type == TYPE_FLOAT){
        if(node.isarray == true)
            a = builder->create_alloca(FLOATPTR_T);
        else
            a = builder->create_alloca(FLOAT_T);
        scope.push(node.id,a);
    }
    return a;
}

Value* CminusfBuilder::visit(ASTCompoundStmt &node) {
    // TODO: This function is not complete.
    // You may need to add some code here
    // to deal with complex statements.

    bool exit = false;
    if(context.is_entered == false){
        scope.enter();
        exit = true;
    }
    else
        context.is_entered = false;
    for (auto &decl : node.local_declarations) {
        decl->accept(*this);
    }
    for (auto &stmt : node.statement_list) {
        stmt->accept(*this);

        if (builder->get_insert_block()->is_terminated())
            break;
    }
    if(exit)
        scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTExpressionStmt &node) {
    // TODO: This function is empty now.
    // Add some code here.
    if(node.expression != nullptr)
        return node.expression->accept(*this);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTSelectionStmt &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value*p = nullptr;
    if(node.expression != nullptr){
        p = node.expression->accept(*this);
    }
    if(p == nullptr)
        return nullptr;
    auto trueBB = BasicBlock::create(module.get(), "", context.func);
    auto outBB = BasicBlock::create(module.get(), "", context.func);
    Value* cmp;
    if(p->get_type()->is_integer_type()){
        cmp = builder->create_icmp_ne(p,CONST_INT(0));
    }
    else{
        cmp = builder->create_fcmp_ne(p,CONST_FP(0.0));
    }
    if(node.else_statement == nullptr){
        builder->create_cond_br(cmp, trueBB, outBB);
        builder->set_insert_point(trueBB);
        context.cur_insert_point = trueBB;
        node.if_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(outBB);
    }
    else{
        auto falseBB = BasicBlock::create(module.get(), "", context.func);
        builder->create_cond_br(cmp,trueBB,falseBB);
        builder->set_insert_point(trueBB);
        context.cur_insert_point = trueBB;
        node.if_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(outBB);
        builder->set_insert_point(falseBB);
        context.cur_insert_point = falseBB;
        node.else_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(outBB);
    }
    builder->set_insert_point(outBB);
    context.cur_insert_point = outBB;
    return nullptr;
}

Value* CminusfBuilder::visit(ASTIterationStmt &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value*p = nullptr;
    if(node.expression == nullptr)
        return nullptr;
    auto conditionBB = BasicBlock::create(module.get(), "", context.func);
    auto lastBB = builder->get_insert_block();
    context.outBB = lastBB;

    context.in_loop = true;
    builder->set_insert_point(conditionBB);
    context.cur_insert_point = conditionBB;
    p = node.expression->accept(*this);
    
    auto loopBB = BasicBlock::create(module.get(), "", context.func);
    auto outBB = BasicBlock::create(module.get(), "", context.func);
    Value* cmp;
    if(p->get_type()->is_integer_type()){
        cmp = builder->create_icmp_ne(p,CONST_INT(0));
    }
    else{
        cmp = builder->create_icmp_ne(p,CONST_FP(0.0));
    }
    builder->create_cond_br(cmp,loopBB,outBB);
    builder->set_insert_point(loopBB);
    
    context.cur_insert_point = loopBB;
    node.statement->accept(*this);
    context.in_loop = false;
    if (not builder->get_insert_block()->is_terminated())
        builder->create_br(conditionBB);

    builder->set_insert_point(lastBB);
    if (not builder->get_insert_block()->is_terminated())
        builder->create_br(conditionBB);
    
    builder->set_insert_point(outBB);
    context.cur_insert_point = outBB;
    
    return nullptr;
}

Value* CminusfBuilder::visit(ASTReturnStmt &node) {
    if (node.expression == nullptr) {
        builder->create_void_ret();
        return nullptr;
    } else {
        // TODO: The given code is incomplete.
        // You need to solve other return cases (e.g. return an integer).
        auto retu_type = context.func->get_function_type()->get_return_type();
        Value*p = node.expression->accept(*this);
        if(retu_type != p->get_type()){
            if (retu_type->is_integer_type())
                p = builder->create_fptosi(p, INT32_T);
            else
                p = builder->create_sitofp(p, FLOAT_T);
        }
        builder->create_ret(p);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTVar &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value*pvar = scope.find(node.id);
    bool treqlval = context.req_lval;
    context.req_lval = false; 
    if(node.expression == nullptr){
        if(treqlval)
            return pvar;
        else{
            if(pvar->get_type()->get_pointer_element_type()->is_float_type()
            ||pvar->get_type()->get_pointer_element_type()->is_int32_type()
            ||pvar->get_type()->get_pointer_element_type()->is_pointer_type())
                return builder->create_load(pvar);
            else
                return builder->create_gep(pvar, {CONST_INT(0), CONST_INT(0)});
        }
    }
    else{
        Value* sub = node.expression->accept(*this);
        if(sub->get_type()->is_float_type())
            sub = builder->create_fptosi(sub,INT32_T);
        auto rightBB = BasicBlock::create(module.get(), "", context.func);
        auto terminateBB = BasicBlock::create(module.get(), "", context.func);
        Value*negetive = builder->create_icmp_lt(sub, CONST_INT(0));
        builder->create_cond_br(negetive,terminateBB,rightBB);
        builder->set_insert_point(terminateBB);
        context.cur_insert_point = terminateBB;
        auto neg_idx_except = scope.find("neg_idx_except");
        builder->create_call( static_cast<Function *>(neg_idx_except), {});
        if (context.func->get_return_type() == VOID_T)
            builder->create_void_ret();
        else if (context.func->get_return_type() == FLOAT_T)
            builder->create_ret(CONST_FP(0.0));
        else
            builder->create_ret(CONST_INT(0));

        builder->set_insert_point(rightBB);
        context.cur_insert_point = rightBB;
        Value*retp = nullptr;
        if(pvar->get_type()->get_pointer_element_type()->is_float_type()
        ||pvar->get_type()->get_pointer_element_type()->is_int32_type())
            retp = builder->create_gep(pvar,{sub});
        else if(pvar->get_type()->get_pointer_element_type()->is_pointer_type()){
            auto arrayval = builder->create_load(pvar);
            retp = builder->create_gep(arrayval,{sub});
        }
        else{
            retp = builder->create_gep(pvar, {CONST_INT(0), sub});
        }
        if(treqlval)
            return retp;
        else
            return builder->create_load(retp);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTAssignExpression &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value* val =  node.expression->accept(*this);
    context.req_lval = true;
    Value* varp = node.var->accept(*this);
    if(val->get_type() != varp->get_type()->get_pointer_element_type()){
        if(val->get_type() == INT32_T){
            val = builder->create_sitofp(val,FLOAT_T);
        }
        else{
            val = builder->create_fptosi(val,INT32_T);
        }
    }
    builder->create_store(val,varp);
    return builder->create_load(varp);
}

Value* CminusfBuilder::visit(ASTSimpleExpression &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value* lval = node.additive_expression_l->accept(*this);
    if(node.additive_expression_r == nullptr){
        return lval;
    }

    Value* rval = node.additive_expression_r->accept(*this);
    Value* cmp = nullptr;
    if(lval->get_type() != rval->get_type()){
        if(lval->get_type() == INT32_T)
            lval = builder->create_sitofp(lval,FLOAT_T);
        else
            rval = builder->create_sitofp(rval,FLOAT_T);
        if(node.op == OP_EQ){
            cmp = builder->create_fcmp_eq(lval,rval);
        }
        else if(node.op == OP_NEQ){
            cmp = builder->create_fcmp_ne(lval,rval);
        }
        else if(node.op == OP_GE){
            cmp = builder->create_fcmp_ge(lval,rval);
        }
        else if(node.op == OP_GT){
            cmp = builder->create_fcmp_gt(lval,rval);
        }
        else if(node.op == OP_LE){
            cmp = builder->create_fcmp_le(lval,rval);
        }
        else if(node.op == OP_LT){
            cmp = builder->create_fcmp_lt(lval,rval);
        }
    }
    else{
        if(lval->get_type() == INT32_T){
            if(node.op == OP_EQ){
                cmp = builder->create_icmp_eq(lval,rval);
            }
            else if(node.op == OP_NEQ){
                cmp = builder->create_icmp_ne(lval,rval);
            }
            else if(node.op == OP_GE){
                cmp = builder->create_icmp_ge(lval,rval);
            }
            else if(node.op == OP_GT){
                cmp = builder->create_icmp_gt(lval,rval);
            }
            else if(node.op == OP_LE){
                cmp = builder->create_icmp_le(lval,rval);
            }
            else if(node.op == OP_LT){
                cmp = builder->create_icmp_lt(lval,rval);
            }
        }
        else{
            if(node.op == OP_EQ){
                cmp = builder->create_fcmp_eq(lval,rval);
            }
            else if(node.op == OP_NEQ){
                cmp = builder->create_fcmp_ne(lval,rval);
            }
            else if(node.op == OP_GE){
                cmp = builder->create_fcmp_ge(lval,rval);
            }
            else if(node.op == OP_GT){
                cmp = builder->create_fcmp_gt(lval,rval);
            }
            else if(node.op == OP_LE){
                cmp = builder->create_fcmp_le(lval,rval);
            }
            else if(node.op == OP_LT){
                cmp = builder->create_fcmp_lt(lval,rval);
            }

        }
    }
    return builder->create_zext(cmp, INT32_T);          //zero expand
}

Value* CminusfBuilder::visit(ASTAdditiveExpression &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value* term = node.term->accept(*this);
    if(node.additive_expression == nullptr){
        return term;
    }
    else{
        Value*eval = node.additive_expression->accept(*this);
        if(node.op == OP_PLUS){
            if(term->get_type() == eval->get_type()){
                if(term->get_type() == INT32_T)
                    return builder->create_iadd(eval,term);
                else
                    return builder->create_fadd(eval,term);
            }
            else{
                if(term->get_type() == INT32_T)
                    term = builder->create_sitofp(term,FLOAT_T);
                else
                    eval = builder->create_sitofp(eval,FLOAT_T);
                return builder->create_fadd(eval,term);
            }
        }
        else if(node.op == OP_MINUS){
            if(term->get_type() == eval->get_type()){
                if(term->get_type() == INT32_T)
                    return builder->create_isub(eval,term);
                else
                    return builder->create_fsub(eval,term);
            }
            else{
                if(term->get_type() == INT32_T)
                    term = builder->create_sitofp(term,FLOAT_T);
                else
                    eval = builder->create_sitofp(eval,FLOAT_T);
                return builder->create_fsub(eval,term);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTTerm &node) {
    // TODO: This function is empty now.
    // Add some code here.
    Value*a = nullptr;
    if(node.term == nullptr){
        a = node.factor->accept(*this);
    }
    else{
        Value*aval = nullptr;
        Value*bval = nullptr;
        aval = node.term->accept(*this);
        bval = node.factor->accept(*this);
        if(node.op == OP_MUL){
            if(aval->get_type() == bval->get_type()){
                if(aval->get_type() == INT32_T)
                    a = builder->create_imul(aval,bval);
                else
                    a = builder->create_fmul(aval,bval);
            }
            else{
                if(aval->get_type() == INT32_T)
                    aval = builder->create_sitofp(aval,FLOAT_T);
                else
                    bval = builder->create_sitofp(bval,FLOAT_T);
                a = builder->create_fmul(aval,bval);
            }
        }
        else if(node.op == OP_DIV){
            if(aval->get_type() == bval->get_type()){
                if(aval->get_type() == INT32_T)
                    a = builder->create_isdiv(aval,bval);
                else
                    a = builder->create_fdiv(aval,bval);
            }
            else{
                if(aval->get_type() == INT32_T)
                    aval = builder->create_sitofp(aval,FLOAT_T);
                else
                    bval = builder->create_sitofp(bval,FLOAT_T);
                a = builder->create_fdiv(aval,bval);
            }
        }
    }
    return a;
}

Value* CminusfBuilder::visit(ASTCall &node) {
    // TODO: This function is empty now.
    // Add some code here.
    auto fun = static_cast<Function *>(scope.find(node.id));
    auto param_type = fun->get_function_type()->param_begin();
    std::vector<Value *> args;
    Value*t = nullptr;
    for (auto &arg : node.args) {
        t = arg->accept(*this);
        if(t->get_type()!=*param_type)
        {
            if(t->get_type() == INT32_T){
                t = builder->create_sitofp(t, FLOAT_T);
            }
            else if(t->get_type() == FLOAT_T){
                t = builder->create_fptosi(t, INT32_T);
            }
        }
        args.push_back(t);
        param_type++;
    }
    Value* retu = builder->create_call(fun,args);
    return retu;
}
