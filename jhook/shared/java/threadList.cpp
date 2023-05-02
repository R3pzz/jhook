#include <shared/shared.hpp>

namespace shared {
#if USE_CUSTOM_ENVS
	threadList threadList::acquire( ) {
		static constexpr auto byte_seq = "4C 8B 35 ? ? ? ? 45 33 C0";

		/* cheap solution, gotta make some context for threads for batch scanning */
		static glob64Address jump{};
		if ( !jump ) {
			auto start = context.jvm_desc( )._base;
			auto end = context.jvm_desc( ).end( );

			jump = byteSequence<byteString::bytenum( byte_seq )>{ byteString{ byte_seq } }.seek( start, end ).offset( 0x3 );
		}

		return **jump.offset( *jump.as<std::int32_t *>( ) + 0x4 ).as<threadList **>( );
	}
#endif
}