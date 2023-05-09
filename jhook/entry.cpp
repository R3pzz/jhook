#include <shared/shared.hpp>

shd::JavaMethodHook jmh{};

void run_game_loop( JNIEnv *env, jobject object, jboolean render_world_in ) {
	using thisFunc = void( * )( JNIEnv *, jobject, jboolean );

	std::printf( "caller env at %p!\n", env );
	std::printf( "nice weather today bro!\n" );

	//return jmh.compiled_c2i_entry( ).as<thisFunc>( )( env, object, render_world_in );
}

void init( HINSTANCE instance ) {
	std::FILE *confile{};

	AllocConsole( );
	AttachConsole( ATTACH_PARENT_PROCESS );

	freopen_s( &confile, "CONOUT$", "w", stdout );

	try {
		shd::context.on_startup( instance );

		std::printf( "context created!\n" );

#if 0
		{
			auto *mc = shd::jni.find_class( "net/minecraft/client/Minecraft" );
			auto *method = mc->method_at( "runGameLoop" );

			jmh = shd::JavaMethodHook{ method, &run_game_loop, shd::JavaMethodHook::entryType::kC2ICompiled };

			jmh.init( );
			jmh.enable( );
		}
#endif

#if 1
		{
			auto *mc = shd::jni.find_class( "net/minecraft/client/Minecraft" );

			for ( std::size_t i{}; i < mc->_methods->_size; i++ )
				mc->_methods->at( i )->print_debug_info( );

			auto *rgl = mc->method_at( "runGameLoop" );
			auto *oop = shd::oopHandle::from_jobject( shd::jni.env( )->FindClass( "net/minecraft/client/Minecraft" ) );

			std::printf( "rgl at %p (klass oop: %p, c2i: %p)\n", rgl, oop, rgl->_c2i_entry );
		}
#endif
		
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