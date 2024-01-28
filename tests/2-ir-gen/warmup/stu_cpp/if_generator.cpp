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

#define CONST_FP(num) \
    ConstantFP::get(num, module)

int main() {
    auto module = new Module();
    auto builder = new IRBuilder(nullptr, module);
    Type *Int32Type = module->get_int32_type();
    auto mainIF = Function::create(FunctionType::get(Int32Type, {}), "main",module);
    auto bb = BasicBlock::create(module, "entry", mainIF);
    auto ifBB = BasicBlock::create(module, "ifBB", mainIF);
    auto elseBB = BasicBlock::create(module, "elseBB", mainIF);
    auto retBB = BasicBlock::create(module, "", mainIF);  // return 分支，
    builder->set_insert_point(bb);
    Type *Float = module->get_float_type();
    auto a = builder->create_alloca(Float);
    builder->create_store(CONST_FP(5.555),a);
    auto retAlloc = builder->create_alloca(Int32Type);
    auto aLoad = builder->create_load(a);
    auto fcmp = builder->create_fcmp_gt(aLoad, CONST_FP(1.0));
    builder->create_cond_br(fcmp, ifBB, elseBB);

    builder->set_insert_point(ifBB);
    builder->create_store(CONST_INT(233),retAlloc);
    builder->create_br(retBB);
    builder->set_insert_point(elseBB);
    builder->create_store(CONST_INT(0),retAlloc);
    builder->create_br(retBB);
    builder->set_insert_point(retBB);
    auto ret = builder->create_load(retAlloc);
    builder->create_ret(ret);

    std::cout << module->print();
    delete module;
    return 0;

}
