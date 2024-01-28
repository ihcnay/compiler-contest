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
    Type *Int32Type = module->get_int32_type();
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    auto builder = new IRBuilder(nullptr, module);
    builder->set_insert_point(bb);
    auto array = module->get_array_type(Int32Type,10);
    auto a = builder->create_alloca(array);
    auto a0GEP = builder->create_gep(a, {CONST_INT(0), CONST_INT(0)});
    builder->create_store(CONST_INT(10), a0GEP);
    auto a0LOAD = builder->create_load(a0GEP);
    auto a1GEP = builder->create_gep(a, {CONST_INT(0), CONST_INT(1)});
    auto mulresult = builder->create_imul(a0LOAD,CONST_INT(2));
    builder->create_store(mulresult, a1GEP);
    auto re = builder->create_load(a1GEP);
    builder->create_ret(re);

    std::cout << module->print();
    delete module;
    return 0;
}
