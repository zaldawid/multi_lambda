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
        template <typename T, typename...>
        struct head {
        using type = T;
        };

        template <typename...Ts>
        using head_t = typename head<Ts...>::type;

        /**
         * @brief Returns the last value in a pack
        */
        template <std::integral auto I, std::integral auto...Is>
        static constexpr auto first_v = I;

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
        requires ( std::is_same_v<R(Args...), traits::lambda_type_t<Lambdas>> && ...)
    class multi_lambda<R(Args...), Lambdas...> : Lambdas...
    {
    public:

        using function_type = R(Args...);

        template <traits::closure_of<function_type>...Ls>
        constexpr 
        multi_lambda(Ls&&...lambdas):
            Lambdas{ std::forward<Ls>(lambdas) }...
        {}

        template <traits::closure_of<function_type>...Ls>
        [[nodiscard]]
        constexpr multi_lambda<function_type, Lambdas..., std::remove_cvref_t<Ls>...> 
        append(Ls&&...lambdas) && 
        {
            return 
            {
                std::move(static_cast<Lambdas&>(*this))..., 
                std::forward<Ls>(lambdas)...
            };
        }

        template <traits::closure_of<function_type>...Ls>
        [[nodiscard]]
        constexpr multi_lambda<function_type, Lambdas..., std::remove_cvref_t<Ls>...> 
        append(Ls&&...lambdas) & 
        {
            return 
            {
                static_cast<Lambdas&>(*this)..., 
                std::forward<Ls>(lambdas)...
            };
        }

        // this is a constant
        constexpr static 
        std::integral_constant<std::size_t, sizeof...(Lambdas)> 
        size{};

        /* Static dispatch */

        [[nodiscard]]
        constexpr std::array<R, sizeof...(Lambdas)> 
        operator()(Args...args) 
            requires (!std::is_same_v<R, void>) 
        {
            return std::array
            {
                Lambdas::operator()(std::forward<Args>(args)...)...
            };
        }

        constexpr void 
        operator()(Args...args) 
            requires (std::is_same_v<R, void>) 
        {
            (Lambdas::operator()(std::forward<Args>(args)...), ...);
        }
         
        template <std::integral auto...Is>
            requires ((Is >= 0 && Is < sizeof...(Lambdas)) && ...) &&
                     (!std::is_same_v<R, void>)
        [[nodiscard]]
        constexpr auto 
        operator()(Args...args) -> decltype(auto)
        {
            if constexpr (sizeof...(Is) == 1)
            {
                return get<traits::first_v<Is...>>()(std::forward<Args>(args)...);
            }
            else
            {
                return std::array
                {
                    get<Is>()(std::forward<Args>(args)...)...
                };
            }
        }
    
        template <std::integral auto...Is>
            requires ((Is >= 0 && Is < sizeof...(Lambdas)) && ...) &&
                    (std::is_same_v<R, void>)
        constexpr void 
        operator()(Args...args)
        {
            (get<Is>()(std::forward<Args>(args)...), ...);
        }

        /* Dynamic dispatch */
        
        /* Multidimensional subscript operator works only in C++23 */
        #if defined( __cpp_multidimensional_subscript ) && ( __cpp_multidimensional_subscript >= 202110L)

        [[nodiscard]]
        constexpr auto 
        operator[](std::integral auto i, std::integral auto...idx) 
            requires (!std::is_same_v<void, R>)
        {
            if constexpr (sizeof...(idx) == 0)
            {
                return  [this, i](Args...args) -> R
                        { 
                            return invoke_map[i](*this, std::forward<Args>(args)...); 
                        };
            }
            else
            {
                return  [this, i, idx...](Args...args)
                        { 
                            return std::array
                            {
                                invoke_map[i](*this, std::forward<Args>(args)...),
                                invoke_map[idx](*this, std::forward<Args>(args)...)... 
                            }; 
                        };
            }
        }
  
        [[nodiscard]]
        constexpr auto 
        operator[](std::integral auto...idx) 
            requires (std::is_same_v<void, R>)
        {
            return  [this, idx...](Args...args)
                    { 
                        (invoke_map[idx](*this, std::forward<Args>(args)...), ...);                       
                    };
        }

        #else // __cpp_multidimensional_subscript

        [[nodiscard]]
        constexpr auto 
        operator[](std::integral auto i) 
            requires (!std::is_same_v<void, R>)
        {
            return  [this, i](Args...args) -> R
                    { 
                        return invoke_map[i](*this, std::forward<Args>(args)...); 
                    };
        }
        
        [[nodiscard]]
        constexpr auto 
        operator[](std::integral auto i) 
            requires (std::is_same_v<void, R>)
        {
            return  [this, i](Args...args)
                    { 
                        invoke_map[i](*this, std::forward<Args>(args)...);                       
                    };
        }

        #endif // __cpp_multidimensional_subscript

        [[nodiscard]]
        constexpr auto
        invoke_at(std::integral auto i, Args...args) -> R
            requires (!std::is_same_v<void, R>)
        {
            return invoke_map[i](std::forward<Args>(args)...);
        }

        constexpr void
        invoke_at(std::integral auto i, Args...args)
            requires (std::is_same_v<void, R>)
        {
            invoke_map[i](std::forward<Args>(args)...);
        }

        /* Dispatch via invokers */

        template <std::integral auto...Is>
            requires ((Is >= 0 && Is < sizeof...(Lambdas)) && ...) &&
                     (!std::is_same_v<R, void>)
        [[nodiscard]]
        constexpr auto 
        get_invoker()
        {
            if constexpr (sizeof...(Is) == 1)
            {
                return [this](Args...args) -> R
                {
                    return get<traits::first_v<Is...>>()(std::forward<Args>(args)...);
                };
            }
            else
            {
                return [this](Args...args)
                {
                    return std::array
                    {
                        get<Is>()(std::forward<Args>(args)...)...
                    };
                };
            }
        }

        template <std::integral auto...Is>
            requires ((Is >= 0 && Is < sizeof...(Lambdas)) && ...) &&
                    (std::is_same_v<R, void>)
        [[nodiscard]]
        constexpr auto 
        get_invoker()
        {
            return [this](Args...args)
            {
                (get<Is>()(std::forward<Args>(args)...), ...);
            };
        }

        [[nodiscard]]
        constexpr auto 
        get_invoker(std::integral auto i, std::integral auto...idx)
            requires (!std::is_same_v<R, void>)
        {
            if constexpr (sizeof...(idx) == 0)
            {
                return [this, i](Args...args) -> R
                {
                    return invoke_map[i](*this, std::forward<Args>(args)...);
                };
            }
            else
            {
                return [this, i, idx...](Args...args)
                {
                    return std::array
                    {
                        invoke_map[i](*this, std::forward<Args>(args)...),
                        invoke_map[idx](*this, std::forward<Args>(args)...)...
                    };
                };
            }
        }

        [[nodiscard]]
        constexpr auto 
        get_invoker(std::integral auto...idx)
            requires (std::is_same_v<R, void>)
        {
            return [this, idx...](Args...args)
            {
                (invoke_map[idx](*this, std::forward<Args>(args)...), ...);
            };
        }







    private:

        template <std::integral auto I>
            requires (I >= 0 && I < sizeof...(Lambdas))
        [[nodiscard]]
        constexpr const auto&
        get() const 
        {
            return static_cast<std::add_const_t<traits::nth_type_t<I, Lambdas...>>&>(*this);
        }

        template <std::integral auto I>
            requires (I >= 0 && I < sizeof...(Lambdas))
        [[nodiscard]]
        constexpr const auto&
        get() 
        {
            return static_cast<traits::nth_type_t<I, Lambdas...>&>(*this);
        }

        constexpr static 
        std::array< R(*)(multi_lambda&, Args...), sizeof...(Lambdas) > 
        invoke_map 
        {
            [](multi_lambda& self, Args...args){ return static_cast<Lambdas&>(self)(std::forward<Args>(args)...); }...
        };



    };

    template <typename R, typename...Args, typename...Lambdas>
        requires ( std::is_same_v<R(Args...) const, traits::lambda_type_t<Lambdas>> && ...)
    class multi_lambda<R(Args...) const, Lambdas...> : Lambdas...
    {
    };

    template <typename...Lambdas>
    multi_lambda(Lambdas...) -> multi_lambda< traits::lambda_type_t< traits::head_t<Lambdas...> >, Lambdas...>;

} // namespace functors


#endif // INCLUDED_MULTI_LAMBDA_H