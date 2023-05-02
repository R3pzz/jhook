#include <shared/shared.hpp>

namespace shared {
	bool NativeMethodHook::init( ) {
		if ( !_target || !_replace || _orig._ptr )
			return false;

		const auto res = MH_CreateHook( _target.as<PVOID>( ), _replace.as<PVOID>( ),
			glob64Address{ &_orig }.as<PVOID *>( )
		);
		
		return res == MH_OK ? true : false;
	}

	bool NativeMethodHook::enable( ) {
		if ( !_orig || !_target )
			return false;

		const auto res = MH_EnableHook( _target.as<PVOID>( ) );
		
		return res == MH_OK ? true : false;
	}
}