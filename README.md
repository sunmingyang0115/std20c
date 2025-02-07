## Magic Mod (std20) Compiler

The magic mod that this compiling to:
https://github.com/dthusian/spells-mod/blob/main/doc/std20.md

For an outline of the language:
https://github.com/sunmingyang0115/std20c/blob/main/docs.md

### How to use
Obtain an `std20c` executable either from releases or building from source (refer to installation section).

    $ std20c input [-o output]

### Examples
A simple fireball transport spell:
```
Vector targetPosition = entpos(TARGET);
targetPosition = vadd(targetPosition, makevec(0, 1.5, 0));
Entity fireball = summon(targetPosition, "fireball");

Vector targetDirection = vmul(entfacing(TARGET), 0.001);
accelent(fireball, targetDirection);
mountent(fireball, TARGET);
```
First 10 factorials:
```
Number n = 10;
Number f = 1;
Number i = 1;
while (i <= n) {
    f = f * i;
    i = i + 1;
    print(sify(f));
}
```

### Installation
Make sure you are in a linux environment and have a g++ version that supports C++17.
(Replace g++ in the Makefile if you are using a different compiler or whatever reason.)

    $ git clone https://github.com/sunmingyang0115/std20c
    $ cd std20c
    $ make

An executable named `std20c` should appear in current directory.
