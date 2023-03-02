#pragma once
#include <stdexcept>

// Simple span
namespace light_enum::detail {
	template <typename T>
	class span {
	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		constexpr span(const_pointer first, const_pointer last) noexcept
			: first_{ first }, last_{ last } {}
		[[nodiscard]] constexpr auto begin() const noexcept { return first_; }
		[[nodiscard]] constexpr auto end() const noexcept { return last_; }
		[[nodiscard]] constexpr auto size() const noexcept { return last_ - first_; }
		[[nodiscard]] constexpr auto empty() const noexcept { return first_ == last_; }
		[[nodiscard]] constexpr auto& operator[](size_t idx) const noexcept { return *(first_ + idx); }
		[[nodiscard]] auto at(size_t idx) const -> const_reference {
			if (idx >= size()) {
				throw std::out_of_range{ "out of range" };
			}
			return *(first_ + idx);
		}
	private:
		const_pointer first_{ nullptr };
		const_pointer last_{ nullptr };
	};
}
