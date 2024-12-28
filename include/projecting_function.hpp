#ifndef PROJECTING_FUNCTION_HPP
#define PROJECTING_FUNCTION_HPP

template <typename T>
void nothing() {}

template <typename T>
T identity(T t) {
  return t;
}

template <typename T>
auto defaultTo(T def) {
  return [def](auto&& opt) { return opt.has_value() ? opt.value() : def; };
}

template <typename T>
auto always(T t) {
  return [t](auto&&) { return t; };
}

#endif
