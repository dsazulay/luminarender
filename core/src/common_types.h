#pragma once

// TODO: use concept to restrict T to be a numerical
template<typename T>
struct Vec2 {
    union {
        T x;
        T width;
    };
    union {
        T y;
        T height;
    };

    Vec2() = default;
    constexpr Vec2(T x, T y) : width{ x }, height{ y } {}
    Vec2 operator*(T scalar) const {
        return Vec2{ x * scalar, y * scalar };
    }
    Vec2 operator*(const Vec2& other) const {
        return Vec2{ x * other.x, y * other.y };
    }
    template<typename S>
    operator S() const {
        // implicit convert to the new type
        return S( x, y );
    }
};

template<typename T>
struct Vec3 {
    union {
        T x;
        T r;
        T width;
    };
    union {
        T y;
        T g;
        T height;
    };
    union {
        T z;
        T b;
        T depth;
    };


    Vec3() = default;
    Vec3(T x, T y, T z) : width{ x }, height{ y }, depth{ z } {}
};

template <typename T>
using Size = Vec2<T>;

template <typename T>
using Position = Vec2<T>;

using Color = Vec3<float>;
