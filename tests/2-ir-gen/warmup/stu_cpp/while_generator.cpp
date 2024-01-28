#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"

#include <iostream>
#include <memory>

#define CONST_INT(num) \
    ConstantInt::get(num, module)

int main() {
    auto module = new Module();
    auto builder = new IRBuilder(nullptr, module);
    Type *Int32Type = module->get_int32_type();
    auto mainWhile = Function::create(FunctionType::get(Int32Type, {}), "main",module);
    auto bb = BasicBlock::create(module, "entry", mainWhile);
    auto judgeBB = BasicBlock::create(module, "judgeBB", mainWhile);
    auto whileBB = BasicBlock::create(module, "whileBB", mainWhile);
    auto retBB = BasicBlock::create(module, "retBB", mainWhile);

    builder->set_insert_point(bb);
    auto aptr = builder->create_alloca(Int32Type);
    auto iptr = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(10),aptr);
    builder->create_store(CONST_INT(0),iptr);
    builder->create_br(judgeBB);

    builder->set_insert_point(judgeBB);
    auto i = builder->create_load(iptr);
    auto a = builder->create_load(aptr);
    auto cmp = builder->create_icmp_lt(i,CONST_INT(10));
    builder->create_cond_br(cmp, whileBB, retBB);

    builder->set_insert_point(whileBB);
    i = builder->create_load(iptr);
    builder->create_store(builder->create_iadd(i,CONST_INT(1)),iptr);
    i = builder->create_load(iptr);
    a = builder->create_load(aptr);
    builder->create_store(builder->create_iadd(a,i),aptr);
    builder->create_br(judgeBB);

    builder->set_insert_point(retBB);
    a = builder->create_load(aptr);
    builder->create_ret(a);

    std::cout << module->print();
    delete module;
    return 0;
}
