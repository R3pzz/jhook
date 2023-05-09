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

	public:
		inline constexpr constMethod( ) = default;

		inline javaSymbol *name( ) const {
			return _const_pool->at<javaSymbol *>( _name_idx );
		}

		inline javaSymbol *signature( ) const {
			return _const_pool->at<javaSymbol *>( _sig_idx );
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

		/* this will try to invoke this method as
			 if it was called from an interpreter. */
		template <typename resType, typename... argsType>
		resType invoke( jobject target, argsType&&... args ) const;

		template <typename resType, typename... argsType>
		resType invoke_static( argsType&&... args ) const;

		void print_debug_info( ) const;
	};
}