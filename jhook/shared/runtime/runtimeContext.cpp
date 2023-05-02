#include <shared/shared.hpp>

namespace shared {
	void RuntimeContext::on_startup( HINSTANCE instance ) {
		_native_instance = instance;

		_opengl32_desc = dll64Awaiter::await( "opengl32.dll" );
		_jvm_desc = dll64Awaiter::await( "jvm.dll" );

#if USE_CUSTOM_ENVS
		tlsManager::init( );
#endif

		jni.on_startup( );
	}

	RuntimeContext context{};
}