#include <shared/shared.hpp>

namespace shared {
	void javaMethod::print_debug_info( ) const {
		std::printf( "method found!\n" );
//		std::printf( "	name: %s\n", name( ).c_str( ) );
		std::printf( "	address: %p\n", this );
//		std::printf( "	owner klass: %s\n", owner( )->_symb->str( ).c_str( ) );
		std::printf( "	entries:\n" );
		std::printf( "		i2i: %p\n", _i2i_entry );
		std::printf( "		i2c: %p\n", _i2c_entry );
		std::printf( "		c2i: %p\n", _c2i_entry );
		std::printf( "\n" );
	}
}