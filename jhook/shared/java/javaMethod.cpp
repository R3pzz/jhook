#include <shared/shared.hpp>

namespace shared {
	template <typename resType, typename... argsType>
	resType javaMethod::invoke( jobject target, argsType&&... args ) const {
		/* we gonna call c2i entry. hence we need to know
			 which entry does _from_compiled point to.
			 if it points to compiled code, we'll call it as
			 a usual assembly function. else, we'll marshal the
			 arguments as c2i_entry trampoline does.*/
		const auto compd_seq = _nmethod._ptr ? true : false;

		JIT_START;

		/* we're dealing with some native code */
		if ( compd_seq ) {
			auto mem = x86::ptr( _c2i_entry._ptr );

			__ push(  )
			__ call( mem );
			__ ret( );
		}
		/* we're dealing with a c2i adapter */
		else {

		}
	}

	template <typename resType, typename... argsType>
	resType javaMethod::invoke_static( argsType&&... args ) const {

	}

	void javaMethod::print_debug_info( ) const {
		std::printf( "method found!\n" );
		std::printf( "	name: %s\n", name( ).c_str( ) );
		std::printf( "	owner klass: %s\n", owner( )->_symb->str( ).c_str( ) );
		std::printf( "	entries:\n" );
		std::printf( "		i2i: %p\n", _i2i_entry );
		std::printf( "		i2c: %p\n", _i2c_entry );
		std::printf( "		c2i: %p\n", _c2i_entry );
		std::printf( "\n" );
	}
}