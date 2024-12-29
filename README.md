# Function Composition

Implement the function composition by C++ 20.

## Part1. Project function

- nothing
- identity
- defaultTo
- always

## Part2. Function composition

- gather: transform a function that takes a tuple into a function that takes multiple
- spread: tranform a function that takes multiple arguments into a function that takes a tuple
- reverse: reverse the order of the arguments of a function
- partial: left partial application
- rightPartial: right partial application
- curry: curried the function
- tap: execute a function and return the input
- alt: execute two functions and return the value which is not empty
- tryCatch: execute a function and catch the exception
- seq: execute a list of functions in sequence
- converge: apply the every value to the forker functions and gather the output to the joiner function
- map: apply the function to every element of the input
- useWith: scatter the input to the functions and gather the output
- reduce: the init value is the first argument, the second argument is the input of the function
- compose: put the input to the last function and put the output to the last second function, until the first function

# Reference

[函数式编程中的组合子-SegmentFault](https://segmentfault.com/a/1190000016803036)
