#pragma once

#include <any>
#include <memory>
#include <variant>
#include <cstdint>
#include <optional>
#include "vec.h"


namespace zeno {

template <int First, int Last, typename Lambda>
inline constexpr bool static_for(Lambda const &f) {
    if constexpr (First < Last) {
        if (f(std::integral_constant<int, First>{})) {
            return true;
        } else {
            return static_for<First + 1, Last>(f);
        }
    }
    return false;
}

template <class T>
struct is_variant : std::false_type {
};

template <class ...Ts>
struct is_variant<std::variant<Ts...>> : std::true_type {
    using tuple_type = std::tuple<Ts...>;
};

template <class T>
struct is_shared_ptr : std::false_type {
};

template <class T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {
    using type = T;
};

using scalar_type_variant = std::variant
        < bool
        , uint8_t
        , uint16_t
        , uint32_t
        , uint64_t
        , int8_t
        , int16_t
        , int32_t
        , int64_t
        , float
        , double
        >;

template <size_t N>
using vector_type_variant = std::variant
        < vec<N, bool>
        , vec<N, uint8_t>
        , vec<N, uint16_t>
        , vec<N, uint32_t>
        , vec<N, uint64_t>
        , vec<N, int8_t>
        , vec<N, int16_t>
        , vec<N, int32_t>
        , vec<N, int64_t>
        , vec<N, float>
        , vec<N, double>
        >;

template <class T, class = void>
struct any_traits {
    using underlying_type = T;
};

template <class T>
struct any_traits<T, std::void_t<decltype(
        std::declval<scalar_type_variant &>() = std::declval<T>()
        )>> {
    using underlying_type = scalar_type_variant;
};

template <size_t N, class T>
struct any_traits<vec<N, T>, std::void_t<decltype(
        std::declval<scalar_type_variant &>() = std::declval<T>()
        )>> {
    using underlying_type = vector_type_variant<N>;
};

template <class T>
struct any_traits<T *, std::void_t<typename T::polymorphic_base_type>> {
    using underlying_type = typename T::polymorphic_base_type *;
};

template <class T>
struct any_traits<T const *, std::void_t<typename T::polymorphic_base_type>> {
    using underlying_type = typename T::polymorphic_base_type const *;
};

template <class T>
struct any_traits<std::unique_ptr<T>, std::void_t<typename T::polymorphic_base_type>> {
    using underlying_type = std::unique_ptr<typename T::polymorphic_base_type>;
};

template <class T>
struct any_traits<std::shared_ptr<T>, std::void_t<typename T::polymorphic_base_type>> {
    using underlying_type = std::shared_ptr<typename T::polymorphic_base_type>;
};

template <class T>
using any_underlying_type_t = typename any_traits<std::decay_t<T>>::underlying_type;

struct Any : std::any {
    Any() = default;

    Any(Any const &a) = default;

    template <class T>
    Any(T const &t)
    : std::any(static_cast<any_underlying_type_t<T> const &>(t))
    {}

    Any &operator=(Any const &a) = default;

    template <class T>
    Any &operator=(T const &t) {
        std::any::operator=(
                static_cast<any_underlying_type_t<T>>(t));
        return *this;
    }
};

template <class T>
std::optional<T> exact_any_cast(Any a) {
    if constexpr (std::is_same_v<std::decay_t<T>, Any>) {
        return std::make_optional(a);
    } else {
        using V = any_underlying_type_t<T>;
        if (typeid(V) != a.type()) return std::nullopt;
        decltype(auto) v = std::any_cast<V const &>(a);
        if constexpr (std::is_pointer_v<T>) {
            auto ptr = static_cast<T>(v);
            if (!ptr) return std::nullopt;
            return std::make_optional(ptr);
        } else if constexpr (is_shared_ptr<T>::value) {
            using U = typename is_shared_ptr<T>::type;
            decltype(auto) ptr = std::static_pointer_cast<U>(std::move(v));
            if (!ptr) return std::nullopt;
            return std::make_optional(std::move(ptr));
        } else if constexpr (is_variant<V>::value && !is_variant<T>::value) {
            if (!std::holds_alternative<T>(v)) return std::nullopt;
            return std::make_optional(std::get<T>(v));
        } else {
            return v;
        }
    }
}

template <class T>
std::optional<T> silent_any_cast(Any const &a) {
    if constexpr (std::is_same_v<T, Any>) {
        return std::make_optional(a);

    } else if constexpr (is_variant<T>::value) {
    using TupleT = typename is_variant<T>::tuple_type;
    T v;
    if (static_for<0, std::tuple_size_v<TupleT>>([&] (auto i) {
        using Ti = std::tuple_element_t<i, TupleT>;
        auto o = exact_any_cast<Ti>(std::move(a));
        if (o.has_value()) {
            v = o.value();
            return true;
        }
        return false;
    })) return std::make_optional(v);
    else return std::nullopt;

    } else {
        using V = any_underlying_type_t<T>;
        if (typeid(V) != a.type()) return std::nullopt;
        decltype(auto) v = std::any_cast<V const &>(a);
        if constexpr (std::is_pointer_v<T>) {
            auto ptr = dynamic_cast<T>(v);
            if (!ptr) return std::nullopt;
            return std::make_optional(ptr);
        } else if constexpr (is_shared_ptr<T>::value) {
            using U = typename is_shared_ptr<T>::type;
            auto ptr = std::dynamic_pointer_cast<U>(v);
            if (!ptr) return std::nullopt;
            return std::make_optional(ptr);
        } else if constexpr (is_variant<V>::value && !is_variant<T>::value) {
            return std::make_optional(std::visit([] (auto const &x) {
                return (T)x;
            }, v));
        } else {
            return std::make_optional(v);
        }
    }
}

using zany = Any;  /* deprecated: use zeno::Any instead */

}
