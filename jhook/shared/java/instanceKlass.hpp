#pragma once

namespace shared {
	struct javaKlass {
	public:
		std::uint8_t _pad0[ 16u ]{};

		std::int32_t _vtbl_len{};

		std::uint8_t _pad1[ 4u ]{};

		javaSymbol *_symb{};

		std::uint8_t _pad2[ 80u ]{};

		glob64Address _mirror{};

		std::uint8_t _pad3[ 80u ]{};

	public:
		inline constexpr javaKlass( ) = default;

		template <class castTo>
		inline castTo *as( ) {
			return reinterpret_cast<castTo *>( this );
		}

		template <class castTo>
		inline const castTo *as( ) const {
			return reinterpret_cast<const castTo *>( this );
		}
	};

	struct instanceKlass : javaKlass {
	public:
		std::uint8_t _pad0[ 24u ]{};

		constPool *_const_pool{};

		std::uint8_t _pad1[ 168u ]{};

		javaArray<javaMethod *> *_methods{};
		javaArray<javaMethod *> *_def_methods{};
		javaArray<instanceKlass *> *_local_intfs{};
		javaArray<instanceKlass *> *_trans_intfs{};

		std::uint8_t _pad2[ 16u ]{};

		javaArray<std::uint8_t> _flds{};
		javaArray<std::uint8_t> _fld_status{};

	public:
		inline constexpr instanceKlass( ) = default;

		inline glob64Address start_of_vtable( ) const {
			return glob64Address{ this }.offset( 448u );
		}

		inline glob64Address start_of_itable( ) const {
			return start_of_vtable( ).offset( _vtbl_len );
		}

		inline javaMethod *method_at( const std::string_view name, const std::string_view sig = "" ) {
			for ( std::size_t it{}; it < _methods->_size; it++ ) {
				auto *method = _methods->at( it );
				if ( !method )
					continue;

				if ( method->name( ) != name )
					continue;

				if ( !sig.empty( ) && method->signature( ) != sig )
					continue;

				return method;
			}

			return nullptr;
		}

		inline const javaMethod *method_at( const std::string_view name, const std::string_view sig = "" ) const {
			for ( std::size_t it{}; it < _methods->_size; it++ ) {
				auto *method = _methods->at( it );
				if ( !method )
					continue;

				if ( method->name( ) != name )
					continue;

				if ( !sig.empty( ) && method->signature( ) != sig )
					continue;

				return method;
			}

			return nullptr;
		}
	};
}