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
    std::vector<Type *> Ints(1, Int32Type);
    auto calleeTy = FunctionType::get(Int32Type, Ints);
    auto calleeFun = Function::create(calleeTy, "callee", module);
    auto bb = BasicBlock::create(module, "entry", calleeFun);
    builder->set_insert_point(bb);

    auto a = builder->create_alloca(Int32Type);
    std::vector<Value *> args;
    for (auto &arg: calleeFun->get_args()) {
        args.push_back(&arg);
    }
    builder->create_store(args[0], a);
    auto aload = builder->create_load(a);
    auto retBB = builder->create_imul(aload,CONST_INT(2));
    builder->create_ret(retBB);

    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto t = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(110), t);
    auto t1 = builder->create_load(t);
    auto call = builder->create_call(calleeFun,{t1});
    builder->create_ret(call);

    std::cout << module->print();
    delete module;
    return 0;
}
