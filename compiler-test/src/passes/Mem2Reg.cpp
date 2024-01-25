#include "Mem2Reg.hpp"
#include "IRBuilder.hpp"
#include "Value.hpp"

#include <memory>

void Mem2Reg::run() {
    // 创建支配树分析 Pass 的实例
    dominators_ = std::make_unique<Dominators>(m_);
    // 建立支配树
    dominators_->run();
    // 以函数为单元遍历实现 Mem2Reg 算法
    for (auto &f : m_->get_functions()) {
        if (f.is_declaration())
            continue;
        func_ = &f;
        if (func_->get_basic_blocks().size() >= 1) {
            // 对应伪代码中 phi 指令插入的阶段
            generate_phi();
            // 对应伪代码中重命名阶段
            rename(func_->get_entry_block());
        }
        // 后续 DeadCode 将移除冗余的局部变量的分配空间
    }
}

void Mem2Reg::generate_phi() {
    // TODO
    // 步骤一：找到活跃在多个 block 的全局名字集合，以及它们所属的 bb 块

    var_clear();
    auto &fun_bbs = func_->get_basic_blocks();
    for(auto &t:fun_bbs){

        t.add_instr_begin(PhiInst::create_phi(nullptr, &t));


        for(auto &k:t.get_instructions()){
            if(k.is_store()){
                Value*ins = k.get_operand(1);
                if(!is_global_variable(ins)&&!is_gep_instr(ins)){
                    if(!val_bb_map.count(ins)){
                        val_bb_map.insert({ins,{}});
                        val_bb_map[ins].push_back(&t);
                        vals.push_back(ins);
                        // for(auto &use:ins->get_use_list()){
                        //     Instruction*useby = (Instruction*)use.val_;
                        //     if(!std::count(val_bb_map[ins].begin(),
                        //                         val_bb_map[ins].end(),useby->get_parent())){
                        //         val_bb_map[ins].push_back(useby->get_parent());
                        //     }
                        // }
                        // val.push_back(ins);

                    }
                    else{
                        if(!count(val_bb_map[ins].begin(),val_bb_map[ins].end(),&t))
                            val_bb_map[ins].push_back(&t);
                    }
                    
                }
            }
        }
    }
    // 步骤二：从支配树获取支配边界信息，并在对应位置插入 phi 指令
    for(auto t:vals){
        if(val_bb_map[t].size()>1){
            std::vector<BasicBlock*>F;
            std::vector<BasicBlock*> W(val_bb_map[t]);
            while (W.size()){
                auto X = W.back();
                W.pop_back();
                for(auto Y:dominators_->get_dominance_frontier(X)){
                    if(!std::count(F.begin(),F.end(),Y)){
                        auto phi = PhiInst::create_phi(t->get_type()->get_pointer_element_type(), Y);
                        phi_to_val.insert({phi,t});
                        Y->add_instr_begin(phi);
                        F.push_back(Y);
                        if(!std::count(W.begin(),W.end(),Y)){
                            W.push_back(Y);
                        }
                    }
                }
            }
        }
    }
    
}

void Mem2Reg::rename(BasicBlock *bb) {
    // TODO
    // 步骤三：将 phi 指令作为 lval 的最新定值，lval 即是为局部变量 alloca 出的地址空间
    // 步骤四：用 lval 最新的定值替代对应的load指令
    // 步骤五：将 store 指令的 rval，也即被存入内存的值，作为 lval 的最新定值
    // 步骤六：为 lval 对应的 phi 指令参数补充完整
    // 步骤七：对 bb 在支配树上的所有后继节点，递归执行 re_name 操作
    // 步骤八：pop出 lval 的最新定值
    // 步骤九：清除冗余的指令
    std::vector<Instruction*>del_vec;
    if(bb == func_->get_entry_block())
        stack.clear();
    for(auto &k:bb->get_instructions()){
        if(k.is_phi()){
            stack[phi_to_val[&k]].push_back(&k);
        }
        else if(k.is_store()){
            if(!is_global_variable(k.get_operand(1))&&!is_gep_instr(k.get_operand(1))){
                if(!stack.count(k.get_operand(1))){
                    stack.insert({k.get_operand(1),{}});
                }
                stack[k.get_operand(1)].push_back(k.get_operand(0));
                del_vec.push_back(&k);
            }
        }
        else if(k.is_load()){
            if(!is_global_variable(&k)&&!is_gep_instr(&k)){
                if(stack.count(k.get_operand(0))){
                    k.replace_all_use_with(stack[k.get_operand(0)].back());
                    del_vec.push_back(&k);
                }
            }
        }
    }

    for(auto S:bb->get_succ_basic_blocks()){
        for(auto&inst:S->get_instructions()){
            if(inst.is_phi()){
                if(stack.count(phi_to_val[&inst])){
                    if(stack[phi_to_val[&inst]].size())
                        static_cast<PhiInst *>(&inst)->add_phi_pair_operand(stack[phi_to_val[&inst]].back(),bb);
                }
            }
        }
    }
    
    for(auto S:dominators_->get_dom_tree_succ_blocks(bb)){
        rename(S);
    }

    for(auto&inst:bb->get_instructions()){
        if(inst.is_store()){
            if(!is_global_variable(inst.get_operand(1))&&!is_gep_instr(inst.get_operand(1))){
                stack[inst.get_operand(1)].pop_back();
            }
        }
        else if(inst.is_phi()){
            
            stack[phi_to_val[&inst]].pop_back();
        }
    }
    for(auto ins:del_vec){
        bb->erase_instr(ins);
    }

}
