#include "optimizer.hh"
#include "linearscan.hh"
#include "lifetime.hh"
#include "std20c/ir.hh"
#include <cassert>
#include <variant>
#include <vector>


IR generateOptimizedIR(const IR &old, const LifeTimeChart &lifetimes) {
    RegisterAllocationState state;
    IR ir;
    assert((old.instructions.size() == lifetimes.registersBecomingAlive.size() 
            && old.instructions.size() == lifetimes.registersBecomingDead.size()));
    for (size_t i = 0; i < old.instructions.size(); i++) {
        auto &ins = old.instructions[i];
        auto &nowDead = lifetimes.registersBecomingDead[i];
        auto &nowAlive = lifetimes.registersBecomingAlive[i];
        
        // first remove dead registers
        for (auto dead: nowDead) {
            state.deallocateReg(dead);
        }
        // add alive
        for (auto alive: nowAlive) {
            state.allocateReg(alive);
        }
        auto insCopy = ins;
        bool isSkippable = false;
        visitVReg(insCopy, [&](VReg &r){
            r = *state.lookupNewReg(r);
        }, [&](VReg &r){
            if (auto opt = state.lookupNewReg(r)) {
                r = *opt;
            } else {
                isSkippable = true;
            }
        });

        if (!isSkippable) {
            ir.instructions.push_back(insCopy);
        }

    }

    return ir;
}

IR optimizer(const IR &ir) {
    auto lifetimes = generateLifetimes(ir);
    return generateOptimizedIR(ir, std::move(lifetimes));
}