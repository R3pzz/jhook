#include <shared/shared.hpp>

#include <winternl.h>

namespace shared {
	bool JavaMethodHook::init( javaMethod *target, const glob64Address replace, const entryType entry_type ) {
		if ( _i2i_tramp._ptr || _i2c_tramp._ptr )
			return false;

		if ( !target || !replace )
			return false;

		_target = target;
		_replace = replace;
		_entry_type = entry_type == entryType::kNone ? dispatch_target_entry( target ) : entry_type;
		_orig_entries.store( target );

		switch ( _entry_type ) {
		case entryType::kUncompiled: {
			_i2i_tramp = make_i2i_trampoline( *this );
			_i2c_tramp = _i2i_tramp;
			_c2i_tramp = make_c2i_trampoline( *this );

			return ( _i2i_tramp._ptr && _i2c_tramp._ptr && _c2i_tramp._ptr ) ? true : false;
		}
		case entryType::kCompiled: {
			_i2c_tramp = make_i2c_trampoline( *this );

			_c2i_tramp = _replace;

			const auto res = MH_CreateHook( _target->_c2i_entry.as<PVOID>( ), _c2i_tramp.as<PVOID>( ),
				glob64Address{ &_orig_entries._c2i_backup }.as<PVOID *>( )
			);

			return ( _i2c_tramp._ptr && _c2i_tramp._ptr && res == MH_OK ) ? true : false;
		}
		case entryType::kI2CUncompiled: {
			_i2c_tramp = make_i2i_trampoline( *this );

			return _i2c_tramp._ptr ? true : false;
		}
		case entryType::kNone:
			return false;
		}

		return false;
	}

	bool JavaMethodHook::enable( ) {
		switch ( _entry_type ) {
		case entryType::kUncompiled: {
			if ( !_i2i_tramp || !_i2c_tramp || !_c2i_tramp )
				return false;

			_target->_i2i_entry = _i2i_tramp;
			_target->_i2c_entry = _i2c_tramp;
			_target->_c2i_entry = _c2i_tramp;

			return true;
		}
		case entryType::kCompiled: {
			if ( !_i2c_tramp || !_c2i_tramp )
				return false;

			_target->_i2c_entry = _i2c_tramp;

			const auto res = MH_EnableHook( _target->_c2i_entry.as<PVOID>( ) );

			return res == MH_OK ? true : false;
		}
		case entryType::kI2CUncompiled: {
			if ( !_i2c_tramp )
				return false;

			_target->_i2c_entry = _i2c_tramp;
		}
		case entryType::kNone:
			return false;
		}

		return true;
	}

	glob64Address JavaMethodHook::make_i2i_trampoline( const JavaMethodHook &hook ) {
		JIT_PREFACE;

		auto compute_param_start = [ &a ]( const x86::Gp &sp, const x86::Gp &method, const x86::Gp &temp = x86::rdx ) {
			const auto const_method = x86::ptr( method, 0x8u );
			const auto size_of_params = x86::ptr( temp, offsetof( constMethod, _size_of_params ), 2u );
			const auto start_of_params = x86::ptr( sp, temp, 3u, -8u );
			
			__ mov( temp, const_method );
			__ movzx( temp, size_of_params );
			__ lea( temp, start_of_params );
		};

		constexpr auto callee = x86::rbx;
		constexpr auto thread = x86::r15;
		constexpr auto rbcp = x86::r13;
		constexpr auto sp = x86::rsp;

		constexpr auto backup_callee = x86::ptr( x86::rdx, 0x0u );
		constexpr auto backup_thread = x86::ptr( x86::rdx, 0x8u );
		constexpr auto backup_sp = x86::ptr( x86::rdx, 0x10u );
		constexpr auto backup_rbcp = x86::ptr( x86::rdx, 0x18u );

		__ mov( x86::rdx, glob64Address{ &hook._i2i_reg_backup }._ptr );
		__ mov( backup_callee, callee );
		__ mov( backup_thread, thread );
		__ mov( backup_rbcp, rbcp );
		__ mov( backup_sp, sp );
		
		//compute_param_start( x86::rsp, x86::rbx ); /* not yet familiar with parameters... */

		__ mov( x86::rdx, hook._replace._ptr );
		__ call( x86::rdx );

		__ mov( x86::rdx, hook._orig_entries._i2i_backup._ptr );
		__ push( x86::rdx );
		__ ret( );

		JIT_RELOC_CODE( relocated );

		std::printf( "i2i stub generated\n" );

		return relocated;
	}

	/* todo: r3pzz: finish hijacking i2c stub */
	glob64Address JavaMethodHook::make_i2c_trampoline( const JavaMethodHook &hook ) {
		JIT_PREFACE;

		__ ret( );

		JIT_RELOC_CODE( relocated );

		std::printf( "i2c stub generated\n" );

		return relocated;
	}

	/* todo: r3pzz: finish hijacking c2i stub */
	glob64Address JavaMethodHook::make_c2i_trampoline( const JavaMethodHook &hook ) {
		JIT_PREFACE;

		__ ret( );

		JIT_RELOC_CODE( relocated );

		std::printf( "c2i stub generated\n" );

		return relocated;
	}
}