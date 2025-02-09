#ifndef LINEAR_SCAN_HH
#define LINEAR_SCAN_HH
#include <std20c/ir.hh>
#include <map>
#include <optional>

// state for linear scan; holds a register map + a reg pooler
struct RegisterAllocationState {

    std::map<VReg, VReg> oldRegToNewReg;    // map from old reg to new reg
    size_t maxRegisters{1};
    std::vector<VReg> free{0};
public:
    std::optional<VReg> lookupNewReg(VReg reg) const;
    // allocates a register that currently is not used; increases maxRegisters if not enough
    void allocateReg(VReg v);
    // deallocates a register
    // note: if double free => UB; this is maintained by class invariance
    void deallocateReg(VReg v);
    RegisterAllocationState() {}
};

    
#endif