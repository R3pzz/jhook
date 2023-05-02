#pragma once

namespace shared {
	struct dll64Awaiter {
		constexpr dll64Awaiter( ) = default;

		inline static dll64Desc await( const std::string_view name ) {
			auto desc = dll64Desc::try_find( name );

			while ( !desc ) {
				std::this_thread::sleep_for( std::chrono::milliseconds{ 20u } );

				desc = dll64Desc::try_find( name );
			}

			return desc;
		}
	};
}