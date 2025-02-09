#include "linearscan.hh"
#include <cassert>
#include <optional>

std::optional<VReg> RegisterAllocationState::lookupNewReg(VReg reg) const {
    if (oldRegToNewReg.find(reg) == oldRegToNewReg.end()) {
        return std::nullopt;
    }
    return oldRegToNewReg.find(reg)->second;
}

void RegisterAllocationState::allocateReg(VReg v) {
    if (free.size() == 0) {
        free.push_back(maxRegisters++);
    }
    oldRegToNewReg.emplace(v, free.back());
    free.pop_back();
}
void RegisterAllocationState::deallocateReg(VReg v) {  
    free.push_back(*this->lookupNewReg(v));
}

