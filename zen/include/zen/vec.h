#pragma once


#include <array>
#include <cmath>


namespace zen {


template <size_t N, class T>
struct vec : std::array<T, N> {
    vec() = default;
    explicit vec(T const &x) {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] = x;
        }
    }

    vec(vec &&) = default;
    vec(vec const &) = default;
    vec &operator=(vec const &) = default;

    vec(std::array<T, N> const &a) {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] = a[i];
        }
    }

    operator std::array<T, N>() {
        std::array<T, N> res;
        for (size_t i = 0; i < N; i++) {
            res[i] = (*this)[i];
        }
        return res;
    }

    template <class S>
    explicit vec(vec<N, S> const &x) {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] = T(x[i]);
        }
    }

    vec(std::initializer_list<T> const &x) {
        T val;
        auto it = x.begin();
        for (size_t i = 0; i < N; i++) {
            if (it != x.end())
                val = *it++;
            (*this)[i] = val;
        }
    }

    vec(T const &x, T const &y) : vec{x, y} {}
    vec(T const &x, T const &y, T const &z) : vec{x, y, z} {}

    template <class S>
    operator vec<N, S>() const {
        vec<N, S> res;
        for (size_t i = 0; i < N; i++) {
            res[i] = (*this)[i];
        }
        return res;
    }
};

template <class T>
struct is_vec : std::false_type {
};

template <size_t N, class T>
struct is_vec<vec<N, T>> : std::true_type {
};

template <class T>
inline constexpr bool is_vec_v = is_vec<T>::value;


template <size_t N, class T, class F>
inline auto vapply(F const &f, vec<N, T> const &a) {
    vec<N, T> res;
    for (size_t i = 0; i < N; i++) {
        res[i] = f(a[i]);
    }
    return res;
}

template <class T, class F>
inline auto vapply(F const &f, T const &a) {
    return f(a);
}


template <size_t N, class T, class S, class F>
inline auto vapply(F const &f, vec<N, T> const &a, vec<N, S> const &b) {
    vec<N, T> res;
    for (size_t i = 0; i < N; i++) {
        res[i] = f(a[i], b[i]);
    }
    return res;
}

template <size_t N, class T, class S, class F>
inline auto vapply(F const &f, T const &a, vec<N, S> const &b) {
    vec<N, T> res;
    for (size_t i = 0; i < N; i++) {
        res[i] = f(a, b[i]);
    }
    return res;
}

template <size_t N, class T, class S, class F>
inline auto vapply(F const &f, vec<N, T> const &a, S const &b) {
    vec<N, T> res;
    for (size_t i = 0; i < N; i++) {
        res[i] = f(a[i], b);
    }
    return res;
}

template <class T, class S, class F>
inline auto vapply(F const &f, T const &a, S const &b) {
    return f(a, b);
}


#define _PER_OP2(op) \
template <class T, class S, std::enable_if_t<is_vec_v<T> || is_vec_v<S>, bool> = true> \
inline auto operator op(T const &a, S const &b) -> decltype(auto) { \
  return vapply([] (auto const &x, auto const &y) { return x op y; }, a, b); \
}
#define _PER_IOP2(op) \
_PER_OP2(op) \
template <size_t N, class T, class S> \
inline vec<N, T> &operator op##=(vec<N, T> &a, S const &b) { \
  a = a op b; \
  return a; \
}
_PER_IOP2(+)
_PER_IOP2(-)
_PER_IOP2(*)
_PER_IOP2(/)
_PER_IOP2(%)
_PER_IOP2(&)
_PER_IOP2(|)
_PER_IOP2(^)
_PER_IOP2(>>)
_PER_IOP2(<<)
_PER_OP2(==)
_PER_OP2(!=)
_PER_OP2(<)
_PER_OP2(>)
_PER_OP2(<=)
_PER_OP2(>=)
#undef _PER_OP2

#define _PER_OP1(op) \
template <class T> \
inline auto operator op(T const &a) { \
  return vapply([] (auto const &x) { return op x; }, a); \
}
_PER_OP1(+)
_PER_OP1(-)
_PER_OP1(~)
_PER_OP1(!)
#undef _PER_OP1


#define _PER_FN2(func) \
template <class T, class S> \
inline auto func(T const &a, S const &b) -> decltype(auto) { \
  return vapply([] (auto const &x, auto const &y) { return std::func(x, y); }, a, b); \
}
_PER_FN2(atan2)
_PER_FN2(pow)
_PER_FN2(max)
_PER_FN2(min)
#undef _PER_FN2

#define _PER_FN1(func) \
template <class T> \
inline auto func(T const &a) { \
  return vapply([] (auto const &x) { return std::func(x); }, a); \
}
_PER_FN1(sqrt)
_PER_FN1(sin)
_PER_FN1(cos)
_PER_FN1(tan)
_PER_FN1(asin)
_PER_FN1(acos)
_PER_FN1(atan)
_PER_FN1(exp)
_PER_FN1(log)
#undef _PER_FN1


template <class T, class S, class F>
inline auto mix(T const &a, S const &b, F const &f) {
    return a * f + b * (1 - f);
}


using vec2f = vec<2, float>;
using vec2d = vec<2, double>;
using vec2i = vec<2, int>;
using vec2l = vec<2, long>;
using vec2h = vec<2, short>;
using vec2b = vec<2, char>;
using vec2I = vec<2, unsigned int>;
using vec2L = vec<2, unsigned long>;
using vec2H = vec<2, unsigned short>;
using vec2B = vec<2, unsigned char>;
using vec3f = vec<3, float>;
using vec3d = vec<3, double>;
using vec3i = vec<3, int>;
using vec3l = vec<3, long>;
using vec3h = vec<3, short>;
using vec3b = vec<3, char>;
using vec3I = vec<3, unsigned int>;
using vec3L = vec<3, unsigned long>;
using vec3H = vec<3, unsigned short>;
using vec3B = vec<3, unsigned char>;
using vec4f = vec<4, float>;
using vec4d = vec<4, double>;
using vec4i = vec<4, int>;
using vec4l = vec<4, long>;
using vec4h = vec<4, short>;
using vec4b = vec<4, char>;
using vec4I = vec<4, unsigned int>;
using vec4L = vec<4, unsigned long>;
using vec4H = vec<4, unsigned short>;
using vec4B = vec<4, unsigned char>;


template <class vecT, class T>
vecT tovec(std::array<T, 1> const &a) {
    return {a[0]};
}

template <class vecT, class T>
vecT tovec(std::array<T, 2> const &a) {
    return {a[0], a[1]};
}

template <class vecT, class T>
vecT tovec(std::array<T, 3> const &a) {
    return {a[0], a[1], a[2]};
}

template <class vecT, class T>
vecT tovec(std::array<T, 4> const &a) {
    return {a[0], a[1], a[2], a[3]};
}

template <class T, size_t N>
auto tovec(std::array<T, N> const &a) {
    return tovec<vec<N, T>>(a);
}

template <class T>
auto tovec(T const &a) {
    return a;
}


template <class T, class S>
struct is_promotable {
    static constexpr bool value = false;
    using type = void;
};

template <class T, size_t N>
struct is_promotable<vec<N, T>, vec<N, T>> {
    static constexpr bool value = true;
    using type = vec<N, T>;
};

template <class T, size_t N>
struct is_promotable<vec<N, T>, T> {
    static constexpr bool value = true;
    using type = vec<N, T>;
};

template <class T, size_t N>
struct is_promotable<T, vec<N, T>> {
    static constexpr bool value = true;
    using type = vec<N, T>;
};

template <class T>
struct is_promotable<T, T> {
    static constexpr bool value = true;
    using type = T;
};

template <class T, class S>
inline constexpr bool is_promotable_v = is_promotable<std::decay_t<T>, std::decay_t<S>>::value;

template <class T, class S>
using is_promotable_t = typename is_promotable<std::decay_t<T>, std::decay_t<S>>::type;


template <class T, class S>
struct is_castable {
    static constexpr bool value = false;
};
template <class T, size_t N>
struct is_castable<vec<N, T>, T> {
    static constexpr bool value = true;
};

template <class T, size_t N>
struct is_castable<T, vec<N, T>> {
    static constexpr bool value = false;
};

template <class T>
struct is_castable<T, T> {
    static constexpr bool value = true;
};

template <class T, class S>
inline constexpr bool is_castable_v = is_castable<std::decay_t<T>, std::decay_t<S>>::value;

template <class T, class S>
inline constexpr bool is_decay_same_v = std::is_same_v<std::decay_t<T>, std::decay_t<S>>;


}
