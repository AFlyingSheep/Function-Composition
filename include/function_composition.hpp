#ifndef FUNCTION_COMPOSITION_HPP
#define FUNCTION_COMPOSITION_HPP

#include <iostream>
#include <tuple>
#include <utility>

#include "tools.hpp"

// transform a function that takes a tuple into a function that takes multiple
template <typename FN>
auto gather(FN fn) {
  return [fn]<typename... Args>(Args && ... args) {
    return fn(std::make_tuple(std::forward<Args>(args)...));
  };
}

// tranform a function that takes multiple arguments into a function that takes
// a tuple
template <typename FN>
auto spread(FN fn) {
  return [fn]<typename ArgTuple>(ArgTuple&& args) {
    return std::apply(fn, std::forward<ArgTuple>(args));
  };
}

// reverse the order of the arguments of a function
template <typename FN>
auto reverse(FN fn) {
  return [fn]<typename... Args>(Args && ... args) {
    auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
    auto reversedTuple = ReverseTuple(argsTuple);
    return std::apply(fn, reversedTuple);
  };
}

// left partial application
template <typename FN, typename... Preargs>
auto partial(FN fn, Preargs&&... preargs) {
  return [ fn, preargs... ]<typename... Args>(Args && ... args) {
    return fn(std::forward<Preargs>(preargs)..., std::forward<Args>(args)...);
  };
}

// right partial application
template <typename FN, typename... Postargs>
auto rightPartial(FN fn, Postargs&&... postargs) {
  return [ fn, postargs... ]<typename... Args>(Args && ... args) {
    return reverse(reverse(fn)(std::forward<Postargs>(postargs)...))(
        std::forward<Args>(args)...);
  };
}

#endif
