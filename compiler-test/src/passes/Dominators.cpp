#include "Dominators.hpp"

void Dominators::run() {
    for (auto &f1 : m_->get_functions()) {
        auto f = &f1;
        if (f->get_basic_blocks().size() == 0)
            continue;
        for (auto &bb1 : f->get_basic_blocks()) {
            auto bb = &bb1;
            idom_.insert({bb, {}});
            dom_frontier_.insert({bb, {}});
            dom_tree_succ_blocks_.insert({bb, {}});
        }

        create_idom(f);
        create_dominance_frontier(f);
        create_dom_tree_succ(f);
    }
}

void Dominators::create_idom(Function *f) {
    BasicBlock* entry = f->get_entry_block();
    num_node(entry);
    idom_[entry] = entry;
    bool changed = true;
    while(changed){
        changed = false;
        int t = int_to_bb.size()-1;
        for(int k = t-1;k >= 1;k--){
            auto bb = int_to_bb[k];
            BasicBlock *first = NULL;
            for (auto i : bb->get_pre_basic_blocks()) {
                if (get_idom(i)) {
                    first = i;
                    break;
                }
            }
            assert(first);
            auto new_idom = first;
            for(auto q:bb->get_pre_basic_blocks()){
                if(new_idom == q)
                    continue;
                if(idom_[q]){
                    new_idom = intersect(q,new_idom);
                }
            }
            if(idom_[bb] != new_idom){
                idom_[bb] = new_idom;
                changed = true;
            }
        }
    }
}

void Dominators::create_dominance_frontier(Function *f) {
    // TODO 分析得到 f 中各个基本块的支配边界集合
    for(auto&b:f->get_basic_blocks()){
        if(b.get_pre_basic_blocks().size()>=2){
            for(auto p:b.get_pre_basic_blocks()){
                auto runner = p;
                while(runner!=idom_[&b]){
                    dom_frontier_[runner].insert(&b);
                    runner = idom_[runner];
                }
            }
        }
    }
}

void Dominators::create_dom_tree_succ(Function *f) {
    // TODO 分析得到 f 中各个基本块的支配树后继
    for(auto&p:f->get_basic_blocks()){
        if(idom_[&p]!= &p)
            dom_tree_succ_blocks_[idom_[&p]].insert(&p);
    }
}
