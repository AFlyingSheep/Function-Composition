#ifndef FUNCTION_COMPOSITION_HPP
#define FUNCTION_COMPOSITION_HPP

#include <functional>
#include <iostream>
#include <tuple>
#include <utility>

#include "tools.hpp"

// transform a function that takes a tuple into a function that takes
// multiple
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
  return [ fn, &preargs... ]<typename... Args>(Args && ... args) {
    return fn(std::forward<Preargs>(preargs)..., std::forward<Args>(args)...);
  };
}

// right partial application
template <typename FN, typename... Postargs>
auto rightPartial(FN fn, Postargs&&... postargs) {
  return [ fn, &postargs... ]<typename... Args>(Args && ... args) {
    return reverse(partial(reverse(fn), std::forward<Postargs>(postargs)...))(
        std::forward<Args>(args)...);
  };
}

// template <typename FN>
// auto curry(FN fn) {
//   auto nextCurried = [fn]<typename... PreArgs>(auto&& nextCurried,
//                                                PreArgs&&... preargs) {
//     return [fn, &preargs...,
//             &nextCurried]<typename NextArgs>(NextArgs&& nextargs) {
//       if constexpr (std::is_invocable_v<FN, PreArgs..., NextArgs>) {
//         return fn(std::forward<PreArgs>(preargs)...,
//                   std::forward<NextArgs>(nextargs));
//       } else {
//         printf("call nextCurried\n");
//         return nextCurried(nextCurried, preargs..., nextargs);
//       }
//     };
//   };
//   return nextCurried(nextCurried);
// }

// curry a function
template <typename FN>
class Curry {
  template <typename... Args>
  class CurryMiddle {
   public:
    using CaptureTupleType = std::tuple<std::decay_t<Args>...>;

    CurryMiddle(FN fn, std::tuple<Args...> args) : fn(fn), args(args) {}

    template <typename NextArg>
    auto operator()(NextArg&& nextArg) {
      if constexpr (std::is_invocable_v<FN, Args..., NextArg>) {
        return std::apply(fn, std::tuple_cat(args, std::make_tuple(nextArg)));
      } else {
        return CurryMiddle<Args..., NextArg>(
            fn, std::tuple_cat(args, std::make_tuple<std::decay_t<NextArg>>(
                                         std::forward<NextArg>(nextArg))));
      }
    }

   private:
    FN fn;
    CaptureTupleType args;
  };

 public:
  Curry(FN fn) : fn(fn) {}
  template <typename Arg>
  auto operator()(Arg&& arg) {
    return CurryMiddle<Arg>(fn, std::forward<Arg>(arg));
  }

 private:
  FN fn;
};

template <typename FN>
auto curry(FN fn) {
  return Curry(fn);
}

#endif
