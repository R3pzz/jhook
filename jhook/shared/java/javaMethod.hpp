#pragma once

namespace shared {
	struct constPool;

	struct constMethod {
	public:
		std::uint8_t _pad0[ 8u ]{};
		/* holder's constant pool. used to access method names */
		constPool *_const_pool{};

		std::uint8_t _pad1[ 16u ]{};

		std::int32_t _method_size{};
		std::uint16_t _flags{};
		std::uint8_t _result_type{};
		std::uint16_t _code_size{};

		std::uint16_t _name_idx{};
		std::uint16_t _sig_idx{};

		std::uint16_t _method_idnum{};
		std::uint16_t _max_stack{};
		std::uint16_t _max_locals{};
		std::uint16_t _size_of_params{};
		std::uint16_t _orig_method_idnum{};

	public:
		inline constexpr constMethod( ) = default;

		inline javaSymbol *name( ) const {
			return _const_pool->at<javaSymbol *>( _name_idx );
		}

		inline javaSymbol *signature( ) const {
			return _const_pool->at<javaSymbol *>( _sig_idx );
		}

		/* intermediate representation code start */
		inline glob64Address ir_code_start( ) const {
			return glob64Address{ this }.offset( sizeof( constMethod ) );
		}

		/* intermediate representation code end */
		inline glob64Address ir_code_end( ) const {
			return glob64Address{ this }.offset( sizeof( constMethod ) + _code_size );
		}

		inline std::uint16_t ir_code_size( ) const {
			return _code_size;
		}

		inline void ir_code_to_buf( ) const {
			std::printf( "ir code dump(method name: %s, sig: %s, code size: %u, this: %p, code base: %p):\n", name( )->str( ).c_str( ), signature( )->str( ).c_str( ), ir_code_size( ), this, ir_code_start( ) );

			for ( std::size_t b{}; b < ir_code_size( ); b++ )
				std::printf( "	byte %x\n", *ir_code_start( ).offset( b ).as<std::uint8_t *>( ) );

			std::printf( "\n" );
		}

		inline void ir_code_to_stream( std::ofstream &stream ) const {
			stream << "ir code dump(method name: " << name( )->str( ) << ", sig: " << signature( )->str( ) << ", code size: " << ir_code_size( ) << "):" << std::endl;

			for ( std::size_t b{}; b < ir_code_size( ); b++ )
				stream << std::to_string( *ir_code_start( ).offset( b ).as<std::uint8_t *>( ) ) << " ";

			stream << std::endl;
		}
	};

	struct methodData {
	public:

	};

	struct methodCounters {
	public:

	};

	struct javaMethod {
	public:
		std::uint8_t _pad0[ 8u ]{};

		/* runtime stuff */
		constMethod *_const_method{};
		methodData *_data{};
		methodCounters *_counters{};

		/* used for future method dispatch and search */
		std::int32_t _access_flags{};
		std::int32_t _vtbl_idx{};

		std::uint8_t _pad2[ 8u ]{};

		/* these entry points have to be hooked with special
		   trampolines for maintaining the correct call convention */
		glob64Address _i2i_entry{};
		glob64Address _c2i_entry{};
		glob64Address _nmethod{};
		glob64Address _i2c_entry{};

	public:
		inline constexpr javaMethod( ) = default;

		inline bool valid( ) const {
			return _data ? true : false;
		}

		inline std::string name( ) const {
			return _const_method->name( )->str( );
		}

		inline std::string signature( ) const {
			return _const_method->signature( )->str( );
		}

		inline instanceKlass *owner( ) const {
			return _const_method->_const_pool->_holder;
		}

		void print_debug_info( ) const;
	};
}