#include <shared/shared.hpp>

namespace shared {
#if USE_CUSTOM_ENVS
	std::uint32_t tlsManager::_tls_idx = 0u;

	void tlsManager::init( ) {
		static constexpr auto byte_seq = "8B 0D ? ? ? ? 89 44 24 48 65 48 8B 04 25 ? ? ? ? 89";

		/* cheap solution, gotta make some context for threads for batch scanning */
		auto start = context.jvm_desc( )._base;
		auto end = context.jvm_desc( ).end( );

		auto jump = byteSequence<byteString::bytenum( byte_seq )>{ byteString{ byte_seq } }.seek( start, end ).offset( 0x2 );

		_tls_idx = *jump.offset( *jump.as<std::int32_t *>( ) + 0x4 ).as<std::uint32_t *>( );

		std::printf( "tls index is %u\n", _tls_idx );
	}

	void tlsManager::set_thread_current( javaThread *thread ) {
		TlsSetValue( _tls_idx, thread );
	}

	javaThread *tlsManager::thread_current( ) {
		return reinterpret_cast<javaThread *>( TlsGetValue( _tls_idx ) );
	}
#endif
}