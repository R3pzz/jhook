#pragma once

namespace shared::detail {
	template <typename addrType>
		requires std::is_integral_v<addrType>
	struct baseAddress {
		using addressType = addrType;
		using diffType = std::ptrdiff_t;

		addressType _ptr{};

		inline constexpr baseAddress( ) = default;

		inline constexpr baseAddress( const void *ptr )
			: _ptr{ reinterpret_cast<const std::uintptr_t>( ptr ) } {}

		inline constexpr baseAddress( const addressType ptr )
			: _ptr{ ptr } {}

		inline constexpr baseAddress( const jlong ptr )
			: _ptr{ static_cast<const addressType>( ptr ) } {}

		template <typename retType>
		inline constexpr auto as( ) const {
			return reinterpret_cast<retType>( _ptr );
		}

		inline constexpr auto &offset_self( const std::ptrdiff_t v ) {
			_ptr += v;

			return *this;
		}

		inline constexpr auto offset( const std::ptrdiff_t v ) const {
			return baseAddress<addressType>{ _ptr + v };
		}

		inline constexpr auto operator+( const std::ptrdiff_t v ) const {
			return baseAddress<addressType>{ _ptr + v };
		}

		inline constexpr auto operator-( const std::ptrdiff_t v ) const {
			return baseAddress<addressType>{ _ptr - v };
		}

		inline constexpr auto operator*( const std::ptrdiff_t v ) const {
			return baseAddress<addressType>{ _ptr *v };
		}

		inline constexpr auto operator/( const std::ptrdiff_t v ) const {
			return baseAddress<addressType>{ _ptr / v };
		}

		inline constexpr auto operator<( const baseAddress<addressType> &other ) const {
			return _ptr < other._ptr;
		}

		inline constexpr auto operator>( const baseAddress<addressType> &other ) const {
			return _ptr > other._ptr;
		}

		inline constexpr auto operator<=( const baseAddress<addressType> &other ) const {
			return _ptr <= other._ptr;
		}

		inline constexpr auto operator>=( const baseAddress<addressType> &other ) const {
			return _ptr >= other._ptr;
		}

		inline constexpr auto operator!( ) const {
			return !_ptr;
		}

		inline constexpr auto &operator++( ) {
			_ptr++;

			return *this;
		}

		inline constexpr auto &operator++( int ) {
			++_ptr;

			return *this;
		}

		inline constexpr auto &operator--( ) {
			_ptr--;

			return *this;
		}

		inline constexpr auto &operator--( int ) {
			--_ptr;

			return *this;
		}
	};
}

namespace shared {
	using glob64Address = detail::baseAddress<std::uintptr_t>;
	using rel32Address = detail::baseAddress<std::uint32_t>;
}