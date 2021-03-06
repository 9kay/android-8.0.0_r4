// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// template <size_t I, class U, class ...Args>
// void emplace(initializer_list<U> il,Args&&... args);

#include <cassert>
#include <string>
#include <type_traits>
#include <variant>

#include "archetypes.hpp"
#include "test_convertible.hpp"
#include "test_macros.h"

struct InitList {
  std::size_t size;
  constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
  std::size_t size;
  int value;
  constexpr InitListArg(std::initializer_list<int> il, int v)
      : size(il.size()), value(v) {}
};

template <class Var, size_t I, class... Args>
constexpr auto test_emplace_exists_imp(int) -> decltype(
    std::declval<Var>().template emplace<I>(std::declval<Args>()...), true) {
  return true;
}

template <class, size_t, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
  return false;
}

template <class Var, size_t I, class... Args> constexpr bool emplace_exists() {
  return test_emplace_exists_imp<Var, I, Args...>(0);
}

void test_emplace_sfinae() {
  using V =
      std::variant<int, TestTypes::NoCtors, InitList, InitListArg, long, long>;
  using IL = std::initializer_list<int>;
  static_assert(!emplace_exists<V, 1, IL>(), "no such constructor");
  static_assert(emplace_exists<V, 2, IL>(), "");
  static_assert(!emplace_exists<V, 2, int>(), "args don't match");
  static_assert(!emplace_exists<V, 2, IL, int>(), "too many args");
  static_assert(emplace_exists<V, 3, IL, int>(), "");
  static_assert(!emplace_exists<V, 3, int>(), "args don't match");
  static_assert(!emplace_exists<V, 3, IL>(), "too few args");
  static_assert(!emplace_exists<V, 3, IL, int, int>(), "too many args");
}

void test_basic() {
  using V = std::variant<int, InitList, InitListArg, TestTypes::NoCtors>;
  V v;
  v.emplace<1>({1, 2, 3});
  assert(std::get<1>(v).size == 3);
  v.emplace<2>({1, 2, 3, 4}, 42);
  assert(std::get<2>(v).size == 4);
  assert(std::get<2>(v).value == 42);
  v.emplace<1>({1});
  assert(std::get<1>(v).size == 1);
}

int main() {
  test_basic();
  test_emplace_sfinae();
}
