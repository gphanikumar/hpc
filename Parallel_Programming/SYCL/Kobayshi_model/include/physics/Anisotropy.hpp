#pragma once

#include <sycl/sycl.hpp>
#include <variant>

// Using the concept of compile time polymorphism
// also Curious Recursive Template Pattern (CRTP)

// Base class template for static polymorphism
template <typename Derived>
struct AnisotropyBase{
    float strength;

    AnisotropyBase(float s) : strength(s){}

    float epsilon(float dx, float dy)const{
        return static_cast<const Derived*>(this)->calculate_epsilon(dx,dy);
    }
};


// 4 Fold anisotropy
struct FourFold : public AnisotropyBase<FourFold>{
    using AnisotropyBase::AnisotropyBase;

    float calculate_epsilon(float dx, float dy) const {
        float theta = sycl::atan2(dy, dx + 1e-12f);
        return 1.0f + strength * sycl::cos(4.0f * theta);
    }
};


// 6 Fold anisotropy
struct SixFold : public AnisotropyBase<SixFold>{
    using AnisotropyBase::AnisotropyBase;

        float calculate_epsilon(float dx, float dy) const {
        float theta = sycl::atan2(dy, dx + 1e-12f);
        return 1.0f + strength * sycl::cos(6.0f * theta);
    }
};
