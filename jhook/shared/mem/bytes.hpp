#pragma once

namespace shared {
	struct byteString : std::string_view {
		inline constexpr byteString( ) = default;

		inline constexpr byteString( const char *str )
			: std::string_view{ str } {}

		inline constexpr byteString( const std::string_view str )
			: std::string_view{ str } {}

		inline constexpr byteString( const std::string str )
			: std::string_view{ str } {}

		inline consteval std::size_t bytenum( ) const {
			std::size_t res{};

			for ( std::size_t it{}; it < size( ); it += 2u ) {
				if ( at( it ) == '?' ) {
					res++;
				}
				else {
					res++, it++;
				}
			}

			return res;
		}

		inline static consteval std::size_t bytenum( const std::string_view str ) {
			std::size_t res{};

			for ( std::size_t it{}; it < str.size( ); it += 2u ) {
				if ( str.at( it ) == '?' ) {
					res++;
				}
				else {
					res++, it++;
				}
			}

			return res;
		}
	};

	template <std::size_t len>
	struct byteSequence {
		struct byteDesc {
			enum struct byteType : std::uint32_t {
				kRegular = 0u,
				kWildcard = 1u,
			} _type{ byteType::kRegular };

			std::uint8_t _val{};

			inline constexpr byteDesc( ) = default;

			inline constexpr byteDesc( char first ) {
				_type = byteType::kWildcard;
			}

			inline constexpr byteDesc( const char first, const char second ) {
				_val = static_cast<std::uint8_t>( b2i( first ) * 0x10u + b2i( second ) );
			}

			inline static constexpr std::size_t b2i( const std::size_t b ) {
				if ( b >= '0' && b <= '9' )
					return b - '0';

				if ( b >= 'A' && b <= 'F' )
					return b - 'A' + 10u;

				return b - 'a' + 10u;
			}
		};

		using byteArray = std::array< byteDesc, len >;

		byteArray _arr{};

		inline constexpr byteSequence( ) = default;

		inline constexpr byteSequence( const byteArray &arr )
			: _arr{ arr } {}

		inline constexpr byteSequence( const byteString &sig ) {
			parse( sig );
		}

		inline constexpr void parse( const std::string_view seq ) {
			std::size_t j{};

			for ( std::size_t i{}; i < seq.size( ); i += 2u ) {
				if ( seq.at( i ) == '?' ) {
					_arr.at( j ) = byteDesc{ '?' };
					j++;
				}
				else {
					_arr.at( j ) = byteDesc{ seq.at( i ), seq.at( ++i ) };
					j++;
				}
			}
		}

		inline glob64Address seek( const glob64Address start, const glob64Address end ) const {
			if ( _arr.empty( ) )
				return glob64Address{};

			for ( auto i = start; i < end; i++ ) {
				auto found = true;

				for ( std::size_t j{}; j < _arr.size( ); j++ ) {
					auto byte = *i.offset( j ).as<std::uint8_t *>( );

					if ( _arr.at( j )._val == byte
						|| _arr.at( j )._type == byteDesc::byteType::kWildcard )
						continue;

					found = false;

					break;
				}

				if ( found )
					return i;
			}

			return glob64Address{};
		}
	};
}