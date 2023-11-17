#ifndef INCLUDED_MULTI_LAMBDA_H
#define INCLUDED_MULTI_LAMBDA_H

/// @file muti_lambda.h
/// @brief multi_lambda container class
/// @copyright 2023 Dawid Zalewski
/// @note This software is licensed under MIT license.

#include <type_traits>
#include <concepts>

namespace functors 
{
    namespace traits
    {
        /**
         * @brief Returns the first type in a parameter pack
        */
        template <typename T, typename...Ts>
        using head_t = T;


        template <auto, typename...>
        struct nth_type;

        template <std::integral auto I, typename...Ts>
            requires (I >= 0 && I < sizeof...(Ts))
        struct nth_type<I, Ts...>
        {
            template <auto>
            struct ignore_t{
                ignore_t(auto&&);
            };

            template <auto...Is, typename T>
            static constexpr auto inner(ignore_t<Is>..., std::type_identity<T>, auto...) -> T;

            template <auto...Is>
            static constexpr auto outer(std::index_sequence<Is...>) -> decltype(inner<Is...>(std::type_identity<Ts>()...));

            using type = decltype(outer(std::make_index_sequence<I>()));
        };

        template <std::integral auto I, template <typename...> typename Wrapper,  typename...Ts>
            requires (I >= 0 && I < sizeof...(Ts))
        struct nth_type<I, Wrapper<Ts...>>
        {
            using type = typename nth_type<I, Ts...>::type;
        };


        /**
         * @brief Returns the nth type in a parameter pack or a typelist
        */
        template <std::integral auto I, typename...Ts>
        using nth_type_t = typename nth_type<I, Ts...>::type;

        template <typename>
        struct lambda_type_deducer;

        template <typename R, typename Lambda, typename...Args>
        struct lambda_type_deducer<R(Lambda::*)(Args...)> 
        {
        using type = R(Args...);
        };

        template <typename R, typename Lambda, typename...Args>
        struct lambda_type_deducer<R(Lambda::*)(Args...) const> 
        {
        using type = R(Args...) const;
        };

        template <typename Lambda, typename Lambda_ = std::remove_reference_t<Lambda>>
        struct lambda_type
        {
        using type = typename lambda_type_deducer<decltype(&Lambda_::operator())>::type;
        };

        /**
         * @brief Returns the type of a lambda's function call operator
        */
        template <typename Lambda>
        using lambda_type_t = typename lambda_type<Lambda>::type;

        /**
         * @brief Concept to check if a type is functor-like
        */
        template <typename T>
        concept lambda = 
            std::is_class_v<std::remove_cvref_t<T>> &&
            std::is_member_function_pointer_v<decltype(&std::remove_cvref_t<T>::operator())>;

        /**
         * @brief Concept to check if a type is a closure with specific function call operator type
        */ 
        template <typename T, typename R_Args>
        concept closure_of = 
            lambda<T> &&
            std::is_same_v<R_Args, lambda_type_t<T>>;
    
    } // namespace traits

    template <typename...>
    class multi_lambda;

    template <typename R, typename...Args, typename...Lambdas>
    class multi_lambda<R(Args...), Lambdas...>
    {
    };

    template <typename R, typename...Args, typename...Lambdas>
    class multi_lambda<R(Args...) const, Lambdas...>
    {
    };
}


#endif // INCLUDED_MULTI_LAMBDA_H