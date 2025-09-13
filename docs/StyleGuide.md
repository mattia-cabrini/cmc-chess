# Style Guide

## Naming Conventions

### Data Types and Related Functions

A data type `foo` shall be named:

- `foo_t` if it is an alias for a builtin type;
- `struct foo_t` if it is a struct.

A pointer type to `foo_t` or to `struct foo_t` shall be called `foo_p`.

**Example**

```
typedef struct board_t {
    piece_t board[64];
}* board_p;
```

A function related to a data type `foo` shall be named `foo_{...}`. If such
function need an instance of `foo` to operate on, that instance shall be passed
as first parameter and as a pointer.

**Example**

```
const char* board_check_move(board_p B, move_p M);
            ^^^^^           |
            Data type board |^^^^^^^
                            |Passing a pointer to struct board_t as first param
```

## Preprocessor

### Macro

Macro are not exactly welcomed in this project, as they are a gateway to hell.

There is no specific guideline on how to write a macro, just try to use common
sense.

However, if you have to write a macro, write a comment that explains why that
macro is necessary.

**When Not To Use Macros**

DO NOT USE a macro is your goal is to embed code into a function. Just write a
small function and let the optimizer embed the code instead of calling that
function at compile time. Performance are going to be good and code is going to
be simple.

### Define

DO NOT USE a `#define` if a `const` would do the job.

Since this is C89, some `#define` is required in order to work with arrays, both
in array declarations and in array initialization (if done at compile time), but
if a `const` could do the job, a `const` shall be used.

### Include Guard

Include file shall contain a guard to avoid circular dependencies.

The precompiler definition for such guard shall be named:

```
CMC_CHESS_{filename}_H_INCLUDED
```

## Comments

Comments are vary much welcome in this project. Just remember that only /**/
comments can work in C89.
