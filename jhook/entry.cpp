#include <shared/shared.hpp>

#include <intrin.h>

void init( HINSTANCE instance ) {
	std::FILE *confile{};

	AllocConsole( );
	AttachConsole( ATTACH_PARENT_PROCESS );

	freopen_s( &confile, "CONOUT$", "w", stdout );

	try {
		shd::context.on_startup( instance );

		std::printf( "context created!\n" );
		
		while ( !shd::context.should_exit( ) )
			std::this_thread::sleep_for( std::chrono::milliseconds{ 200u } );
	}
	catch ( const char *err ) {
		std::printf( "caught an exception: %s\n", err );

		std::this_thread::sleep_for( std::chrono::seconds{ 5u } );
	}

	std::fclose( confile );

	FreeConsole( );

	FreeLibraryAndExitThread( reinterpret_cast<HMODULE>( instance ), 1u );
}

BOOL WINAPI DllMain( HINSTANCE instance, DWORD reason, LPVOID res ) {
	if ( reason != DLL_PROCESS_ATTACH )
		return FALSE;

	std::jthread{ [ = ]( ) { init( instance ); } }.detach( );

	return TRUE;
}