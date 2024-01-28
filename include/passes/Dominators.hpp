#pragma once

#include "BasicBlock.hpp"
#include "PassManager.hpp"

#include <map>
#include <set>

class Dominators : public Pass {
  public:
    using BBSet = std::set<BasicBlock *>;

    explicit Dominators(Module *m) : Pass(m) {}
    ~Dominators() = default;
    void run() override;

    BasicBlock *get_idom(BasicBlock *bb) { return idom_.at(bb); }
    const BBSet &get_dominance_frontier(BasicBlock *bb) {
        return dom_frontier_.at(bb);
    }
    const BBSet &get_dom_tree_succ_blocks(BasicBlock *bb) {
        return dom_tree_succ_blocks_.at(bb);
    }

  private:
    void create_idom(Function *f);
    void create_dominance_frontier(Function *f);
    void create_dom_tree_succ(Function *f);

    // TODO 补充需要的函数
    BasicBlock* intersect(BasicBlock*a,BasicBlock*b){
      auto finger1 = a;
      auto finger2 = b;
      while(finger1 != finger2 ){
          while(node[finger1] < node[finger2] )
              finger1 = idom_[finger1];
          while(node[finger1] > node[finger2] )
              finger2 = idom_[finger2];
      }
      return finger1;
    }

    // void num_node(BasicBlock*entry){
    //   int_to_bb.push_back(NULL);
    //   std::vector<BasicBlock*>stack;
    //   stack.push_back(entry);
    //   while(!stack.empty()){
    //     std::vector<BasicBlock*>tstack(stack);
    //     stack.clear();
    //     for(auto t1:tstack){
    //       count++;
    //       node.insert({t1,count});
    //       int_to_bb.push_back(t1);
    //       for(auto t2:t1->get_succ_basic_blocks()){
    //           if(!node.count(t2)){
    //             stack.push_back(t2);
    //           }
    //       }
    //     }
    //   }
    // }
    int count = 0;
    std::map<BasicBlock *, int> node{};     //深度优先顺序给bb编号1，2。。。
    std::vector<BasicBlock*>int_to_bb;      //编号对应的bb
    std::vector<BasicBlock*>visited;

    void reset(){
      count = 0;
      node.clear();
      int_to_bb.clear();
      visited.clear();
    }
    void dfs(BasicBlock*entry){
      visited.push_back(entry);
      for(auto t:entry->get_succ_basic_blocks()){
        if(!std::count(visited.begin(),visited.end(),t)){
          dfs(t);
        }
      }
      count++;
      node.insert({entry,count});
      int_to_bb.push_back(entry);
    }
    void num_node(BasicBlock*entry){
      reset();
      int_to_bb.push_back(NULL);
      dfs(entry);
    }

    void dom_edge(BasicBlock*a,BasicBlock*b){
        auto x = a;
        while(idom_[b] != x){
          if(!dom_frontier_[x].count(b))
            dom_frontier_[x].insert(b);
          x = idom_[x];
        }
    }

    std::vector<BasicBlock*> post_order;

    
    std::map<BasicBlock *, BasicBlock *> idom_{};  // 直接支配
    std::map<BasicBlock *, BBSet> dom_frontier_{}; // 支配边界集合
    std::map<BasicBlock *, BBSet> dom_tree_succ_blocks_{}; // 支配树中的后继节点
};
