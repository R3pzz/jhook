#pragma once

namespace shared {
	struct javaSymbol {
	public:
		std::uint32_t _hash_and_ref_cnt{};

		std::uint16_t _size{};

		std::uint8_t _body[ 2u ]{};

	public:
		inline constexpr javaSymbol( ) = default;

		inline auto str( ) const {
			return std::string{ reinterpret_cast<const char *>( &_body[ 0u ] ), _size };
		}
	};

	template <typename arrayClass>
	struct javaArray {
	public:
		std::int32_t _size{};
		arrayClass _mem[ 1u ]{};

	public:
		inline constexpr javaArray( ) = default;

		inline arrayClass at( const std::size_t i ) const {
			return _mem[ i ];
		}
	};
}