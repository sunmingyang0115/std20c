#ifndef COMPILER_HH
#define COMPILER_HH
#include <map>
#include <string>
#include <vector>
enum Type {
    String, Number, Entity, Vector, Void
};
struct Function {
    std::vector<Type> args;
    Type ret;
};


// type -> functions
using FunctionTable = std::map<std::string, Function>;

// id -> respective type
using SymbolTable = std::map<std::string, Type>;

/*
types:
    Number
    Vector
    Entity
    Void

methods:
    vec_new: (Number, Number, Number) => Vector
    vec_x: () => Number
    vec_y: () => Number
    vec_z: () => Number
    vec_add: (Vector, Vector) => Vector
    vec_sub: (Vector, Vector) => Vector
    vec_mul: (Vector, Number) => Vector
    vec_div: (Vector, Number) => Vector
    vec_dist: (Vector) => Number
    vec_norm: (Vector) => Number
    vec_dot: (Vector, Vector) => Number
    vec_cross: (Vector, Vector) => Vector

    round: (Number) => Number
    sqrt: (Number) => Number
    sin: (Number) => Number
    cos: (Number) => Number

    findent: (Vector, Number) => Entity
    ent_pos: (Entity) => Vector
    ent_vel: (Entity) => Vector
    ent_facing: (Entity) => Vector

    ent_accel: (Entity, Vector) => Void
    ent_damage: (Entity, Number) => Void
    ent_mount: (Entity, Entity) => Void
    ent_power: (Entity, Number) => Void
    ent_explode: (Entity, Number) => Void
    
    checkblock: (Vector, String) => Number

    placeblock: (Vector, String) => Void
    destroyblock: (Vector) => Void
    lightning: (Vector) => Void
    summon: (Vector, String) => Void
    wait: (Number) => Void
 */

#endif