#pragma once

namespace shared {
	struct instanceKlass;

	struct constPool {
	public:
		std::uint8_t _pad0[ 8u ]{};

		javaArray<std::uint8_t> *_tags{};

		std::uint8_t _pad1[ 8u ]{};

		instanceKlass *_holder{};

		javaArray<std::uint16_t> *_operands{};
		javaArray<klass *> *_resolved_klasses{};

		union {
			struct {
				std::uint16_t _major;
				std::uint16_t _minor;
			};

			std::uint32_t _version{};
		};

		std::uint16_t _sig_idx{};
		std::uint16_t _source_file_name_idx{};

		std::uint16_t _flags{};
		std::int32_t _length{};

		std::int32_t _res_ref_length{};

	public:
		inline constexpr constPool( ) = default;

		inline std::intptr_t *base( ) const {
			return glob64Address{ this }.offset( sizeof( decltype( *this ) ) ).as<std::intptr_t *>( );
		}

		template <typename targetType>
		inline targetType at( const std::size_t i ) const {
			return *reinterpret_cast<targetType *>( &base( )[ i ] );
		}
	};
}