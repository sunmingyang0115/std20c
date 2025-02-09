#ifndef LIFETIME_HH
#define LIFETIME_HH
#include <std20c/ir.hh>
#include <vector>


struct LifeTimeChart {
    std::vector<std::vector<VReg>> registersBecomingAlive;
    std::vector<std::vector<VReg>> registersBecomingDead;
    LifeTimeChart(size_t size): registersBecomingAlive(size), registersBecomingDead(size) {}
};
LifeTimeChart generateLifetimes(const IR&);

#endif