#include <shared/shared.hpp>

namespace shared {
	void jniWrangler::on_startup( ) {
		JNI_GetCreatedJavaVMs( &_vm, 1u, nullptr );

		if ( !_vm )
			return;

		_vm->AttachCurrentThread( reinterpret_cast<void **>( &_env ), nullptr );
		if ( !_env )
			std::printf( "failed to attach the environment!\n" );
	}

	instanceKlass *jniWrangler::find_class( const std::string_view name ) {
		if ( !valid( ) )
			return nullptr;

		if ( _env->PushLocalFrame( 1u ) )
			return nullptr;

		std::printf( "local frame pushed\n" );

		auto klass = _env->FindClass( name.data( ) );
		if ( !klass ) {
			_env->PopLocalFrame( nullptr );

			return nullptr;
		}

		_env->PopLocalFrame( nullptr );

		return oopHandle::from_jobject( klass )->as<instanceKlass>( );
	}

	jniWrangler jni{};
}