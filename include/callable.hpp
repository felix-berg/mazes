// Created by felix on 11/11/22.
#pragma once

#include <concepts>

template <typename Func, typename Ret, typename ... Args>
concept impl_CallableWithSignature = requires(Func f, Args... args){
    { f(args...) } -> std::same_as<Ret>;
};

template <typename Func, typename T>
struct struct_CallableWithSignature;

template <typename Func, typename Ret, typename ... Args>
struct struct_CallableWithSignature<Func, Ret(Args...)> {
    static constexpr bool value = impl_CallableWithSignature<Func, Ret, Args...>;
};

template <typename Func, typename Sig>
concept CallableWithSignature = struct_CallableWithSignature<Func, Sig>::value;