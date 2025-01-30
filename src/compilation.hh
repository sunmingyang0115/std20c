#ifndef COMPILER_HH
#define COMPILER_HH
#include <map>
#include <string>
#include <vector>
enum Type {
    STRING_TYPE, NUMBER_TYPE, ENTITY_TYPE, VECTOR_TYPE, VOID_TYPE
};
struct Function {
    std::vector<Type> args;
    Type ret;
};

// need some way to take care of scopes in ifstmts and while loops
// make semantic analysis generate IR
/**
    Number a = 10 * 4; Number b = 0; Number c; Number d;
    while(b < a) {
        Number d = c + a;
        c = d;
    }

    =>

    semantic analysis:
        - needed for type checking
            - can type check without IR struct (via linear scan) (low constant O(n) time and memory)
        - creates symboltable:
            map<Token*, size_t> // variable addr to id
            map<size_t, Type>   // id to 


    lowering:
    main: IR {
        // (not in the struct) {a := $0; b := Number; c := Number, d ::= Number, d' ::= Number}
        cassign(t1, 10)
        cassign(t2, 4)
        binOp(t3, t1, t2)
        assign(a, t2)
        cassign(b, 0)
        cassign(c, 0)
        // insert whileloop shit here
        assign(d', binOp(c, PLUS, a))
        assign(c, d')
        // insert whileloop shit here
    }

 */

struct Token;

/**
    SymbolTable has two main functions: eliminates scopes for variables and resolves variable/function types 
    This will be generated in the semantic analysis step, which is needed for lowering
 */
struct SymbolTable {
    using VariableID = std::size_t;
    /**
        tokenToVID => maps all ID (variable) Tokens to a unique variable identifier (VID)
            excludes ID tokens referenced in declaration
            needed as strings by themselves are not enough to due to scopes
     */
    std::map<Token*, VariableID> tokenToVID;
    //  vidToType => maps VID to respective type
    std::map<VariableID, Type> vidToType;
    //  maps function name to respective function type
    std::map<std::string, Function> funNameToType; 
};


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
    ent_equal: (Entity, Entity) => Number
    
    checkblock: (Vector, String) => Number

    placeblock: (Vector, String) => Void
    destroyblock: (Vector) => Void
    lightning: (Vector) => Void
    summon: (Vector, String) => Entity
    wait: (Number) => Void

global var:
    SELF: Entity
    TARGET: Entity
 */

#endif