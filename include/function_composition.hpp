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

// tap: execute a function and return the input
// For not pure function, it should have a deep copy of the input
template <typename FN>
auto tap(FN fn) {
  return [fn]<typename T>(T&& t) {
    T t_copy = t;
    fn(t_copy);
    return std::forward<T>(t);
  };
}

// alt: execute two functions and return the value which is not empty
template <typename FN1, typename FN2>
auto alt(FN1 fn1, FN2 fn2) {
  return [ fn1, fn2 ]<typename... Args>(Args && ... args) {
    auto result1 = fn1(std::forward<Args>(args)...);
    auto result2 = fn2(std::forward<Args>(args)...);

    return result1.has_value() ? result1 : result2;
  };
}

// tryCatch: execute a function and catch the exception
template <typename FN, typename ExceptionHandler>
auto tryCatch(FN fn, ExceptionHandler exceptionHandler) {
  return [ fn, exceptionHandler ]<typename... Args>(Args && ... args) {
    try {
      return fn(std::forward<Args>(args)...);
    } catch (const std::exception& e) {
      (void*)e;
      return exceptionHandler(std::forward<Args>(args)...);
      // return exceptionHandler(e);
    }
  };
}

// seq: execute a list of functions in sequence
// TODO: it can be parallel to improve the performance
template <typename... FNs>
auto seq(FNs... fns) {
  return [fns...]<typename... Args>(Args && ... args) {
    (fns(std::forward<Args>(args)), ...);
  };
}

// converge: apply the every value to the forker functions and gather the output
// to the joiner function
// TODO: it can be parallel to improve the performance
template <typename JoinFN, typename... ForkFNs>
auto converge(JoinFN joiner, ForkFNs... forkers) {
  return [joiner, forkers...]<typename Arg>(Arg&& arg) {
    return joiner(forkers(std::forward<Arg>(arg))...);
  };
}

// map: apply the function to every element of the input
// ! Expecially for the vector.
template <typename FN>
auto map(FN fn) {
  return [fn]<typename T>(std::vector<T> input) {
    std::vector<T> output;
    for (auto& elem : input) {
      output.push_back(fn(elem));
    }
    return output;
  };
}

// useWith: scatter the input to the functions and gather the output
template <typename JoinFN, typename... TransformerFNs>
auto useWith(JoinFN joiner, TransformerFNs... transformers) {
  return [ joiner, transformers... ]<typename... Args>(Args && ... args) {
    return joiner(transformers(std::forward<Args>(args))...);
  };
}

// reduce: the init value is the first argument, the second argument is the
// input of the function
// ! Expecially for the vector.
template <typename FN, typename T>
auto reduce(FN reducer, T init) {
  return [reducer, init](std::vector<T> input) {
    auto result = init;
    for (auto& elem : input) {
      result = reducer(result, elem);
    }
    return result;
  };
}

// compose: put the input to the last function and put the output to the last
// second function, until the first function
// ! All functions should only have one argument except the last one
template <typename... FNs>
auto compose(FNs... fns) {
  return [fns...]<typename... Args>(Args && ... args) {
    return (..., (fns(std::forward<Args>(args)...)));
  };
}

#endif
