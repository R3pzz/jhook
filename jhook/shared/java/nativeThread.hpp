#pragma once

namespace shared {
	using gcThreadLocalData = std::uint64_t[ 19u ];
	
	struct threadList;

	/* thread exception cache. not sure that this is useful at all... */
	struct threadShadow {
		oopHandle *_exception{};

		const char *_exception_file{};

		std::int32_t _exception_line{};

		inline constexpr threadShadow( ) = default;

		inline constexpr bool has_exception( ) const {
			return _exception != nullptr;
		}
	};

	/* left blank for now, maybe revisit this later if needed */
	struct nativeThread : threadShadow {
		
	};
}