/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <type_traits>
#include <utility>
#include <vector>

namespace tcb {

namespace detail {

template <typename T, typename...>
struct vec_type_helper {
  using type = T;
};

template <typename... Args>
struct vec_type_helper<void, Args...> {
  using type = typename std::common_type<Args...>::type;
};

template <typename T, typename... Args>
using vec_type_helper_t = typename vec_type_helper<T, Args...>::type;

template <typename, typename...>
struct all_constructible_and_convertible : std::true_type {};

template <typename T, typename First, typename... Rest>
struct all_constructible_and_convertible<T, First, Rest...>
    : std::conditional<std::is_constructible<T, First>::value &&
                           std::is_convertible<First, T>::value,
                       all_constructible_and_convertible<T, Rest...>,
                       std::false_type>::type {};

template <typename T, typename... Args,
          typename std::enable_if<!std::is_trivially_copyable<T>::value,
                                  int>::type = 0>
std::vector<T> make_vector_impl(Args&&... args) {
  std::vector<T> vec;
  vec.reserve(sizeof...(Args));
  using arr_t = int[];
  (void)arr_t{0, (vec.emplace_back(std::forward<Args>(args)), 0)...};
  return vec;
}

template <typename T, typename... Args,
          typename std::enable_if<std::is_trivially_copyable<T>::value,
                                  int>::type = 0>
std::vector<T> make_vector_impl(Args&&... args) {
  return std::vector<T>{std::forward<Args>(args)...};
}

}  // namespace detail

template <typename T = void, typename... Args,
          typename V = detail::vec_type_helper_t<T, Args...>,
          typename std::enable_if<
              detail::all_constructible_and_convertible<V, Args...>::value,
              int>::type = 0>
std::vector<V> make_vector(Args&&... args) {
  return detail::make_vector_impl<V>(std::forward<Args>(args)...);
}

}  // namespace tcb
