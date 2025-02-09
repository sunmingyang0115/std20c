# std20c language Overview

## Types
The following types are used to define variables:
- `Number` -- representing a single decimal number
- `String` -- representing strings of text
- `Entity` -- a reference to an entity in the Minecraft World
- `Vector` -- three numbers to represent a special quantity used for direction and position

This language is statically typed, meaning types in this language are not interchangable (i.e. variables can only be assigned values same to it's respective type).

## Literals
There are only two literals in this language, one for `Number` and one for `String`.

`Number` literals can be created with a digit sequence. Decimal seperator is supported.

`String` literals can be created by writing a sequences of non-newline ascii characters enclosed by a double quotation mark.

## Variables
Variables identifiers in this language are abstractions that denote a storage location, and can be referenced in later areas of the code.

### Global Variables
The library provides two predefined variables: `SELF` and `TARGET`, which reflect std20's slot 0 and slot 1 respectively.

### Declaring Variables
Variables cannot be accessed or written to before variable declaration.

Variables must be declared with a type. This type cannot change.

### Assigning Variables
Variables can be only assign to expressions of the same type. An assignment is treated like an expression and evaluates to the value that the variable is assigned as.

### Scopes
Scopes control whether variable identifiers are visible and can be accessed/written to. In short variables are 'alive' in their respective scope.

If two scopes are nested, variables in the outer scope can be accessed in the inner scope (assuming the variable is defined before). However the outer scope cannot access in the variables in the inner scope.

If two scopes are nested, then defining a variable with the same name as a variable in the outer variable is permitted. References to variables always refer to the variable closest to scope of reference.

Scopes in this language is defined as portions of code enclosed by braces. To create a new scope, encapsulate the piece of code with braces.

Function identifiers are special and are not affected by scopes.

## Binary Operators
The language has 12 binary operators that work only on `Number`

### Basic Arithmetic
- `+` -- Addition
- `-` -- Subtraction
- `*` -- Multiplication
- `/` -- Division

### Comparisons
There are 6 comparison operators as well:
- `==` -- equality
- `!=` -- inequality
- `>=` -- greater than/equal to
- `<=` -- less than/equal to
- `>` -- greater than
- `<` -- less than

These operators return `1` if the comparison succeeds, and `0` if it fails.

### Logical Operators
Logical operators short-circuit evaluated, meaning the second argument is evaluated only if the result of the first argument is not sufficient to determine the value of the expression.

- `||` -- logical or
- `&&` -- logical and

These operators return `1` if the logical evaluation succeeds, and `0` if it fails.

## Control Structures

Control structures in this language performs some operation when a condition is held. Valid conditions are specified with `Number` only.

### If statement
If statements are conditional statements that execute code if the conditional succeeds, and can be used to execute something else if it fails.
- `if` -- used to specify if a piece of code is to be evaluated given condition; can be continued with `else` and/or `else if`
- `else if` -- used to check another condition to test if the predecessing statement fails; can be continued with `else` and/or `else if`
- `else` -- used to conclude an if statement by executing a piece of code if all predecessing statements fails
### While loops

While loops contain a piece of code that can be executed again as long as the condition holds.

## Functions
This language supports functions. Functions are able to have a return and accepts arguments as expressions.

The language does not support user-defined functions. However it has a couple predefined ones.

Function return types is the same as variable types with an additional `Void` type to mean no return value.

Function arguments is the same as variable types with an additional `Object` type to mean it accepts any type. Passed function arguments are evaluated from left-to-right before calling the function.

The functions are named the same way as the instructions in std20 assembly, and as such has the same behaviour.

### Number functions
- `Number cos(Number x);`
- `Number sin(Number x);`
- `Number round(Number x);`
- `Number sqrt(Number x);`

### Vector functions
- `Vector makevec(Number x, Number y, Number c);`
- `Number vx(Vector a);`
- `Number vy(Vector a);`
- `Number vz(Vector a);`
- `Vector vadd(Vector a, Vector b);`
- `Vector vsub(Vector a, Vector b);`
- `Vector vmul(Vector a, Number b);`
- `Vector vdiv(Vector a, Number b);`
- `Number vdist(Vector a);`
- `Vector vnorm(Vector a);`
- `Number vdot(Vector a, Vector b);`
- `Vector vcross(Vector a, Vector b);`

### String functions
- `Number slength(String s)`
- `String scharat(String s, Number idx)`
- `Number scodeat(String s, Number idx)`
- `String ssubstr(String s, Number start, Number end)`
- `String sconcat(String a, String b);`
- `Number ssearch(String a, String b);`
- `Number scmp(String a, String b);`
- `String sify(Object x);`
- `String sifyd(Object x);`
- `Void print(Object a);`
### World functions
- `Entity findent(Vector pos, Number n);`
- `Vector entpos(Entity ent);`
- `Vector entvel(Entity ent);`
- `Vector entfacing(Entity ent);`
- `Vector checkblock(Vector pos, String block);`
- `Void accelent(Entity ent, Velocity vel);`
- `Void damageent(Entity ent, Number dmg);`
- `Void mountent(Entity bottom, Entity top);`
- `Void fireballpwr(Entity fireball, Number power);`
- `Void explode(Vector pos, Number power);`
- `Void placeblock(Vector pos, String block);`
- `Void destroyblock(Vector pos);`
- `Void lightning(Vector pos);`
- `Entity summon(Vector pos, String type);`

### Other functions
- `Void wait(Number ticks);`
