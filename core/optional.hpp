#ifndef CORE_OPTIONAL_HPP
#define CORE_OPTIONAL_HPP

#include <type_traits>

namespace core {

struct nullopt_t {};
constexpr inline nullopt_t nullopt{};

template <typename T>
    requires std::is_default_constructible_v<T>
class Optional {
   public:
    constexpr Optional() noexcept = default;
    constexpr Optional(nullopt_t) noexcept {};
    constexpr Optional(T value) noexcept : value_(value), has_value_(true) {}

    Optional &operator=(nullopt_t) noexcept {
        value_ = {};  // clear previous value
        has_value_ = false;
        return *this;
    }

    constexpr bool has_value() const noexcept { return has_value_; }

    constexpr explicit operator bool() const noexcept { return has_value_; }

    constexpr T &operator*() noexcept { return value_; }
    constexpr const T &operator*() const noexcept { return value_; }

    constexpr T *operator->() noexcept { return &value_; }
    constexpr const T *operator->() const noexcept { return &value_; }

   private:
    T value_{};
    bool has_value_ = false;
};
}  // namespace core

#endif  // CORE_OPTIONAL_HPP
