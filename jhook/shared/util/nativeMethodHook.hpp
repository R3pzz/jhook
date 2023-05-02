#pragma once

namespace shared {
	class NativeMethodHook {
	private:
		glob64Address _orig{}; /* original function address */
		glob64Address _target{}; /* address that we want to hijack */
		glob64Address _replace{}; /* address, with which we'll hijack */

	public:
		inline constexpr NativeMethodHook( ) = default;

		inline constexpr NativeMethodHook( const NativeMethodHook & ) = delete;

		inline constexpr NativeMethodHook &operator=( const NativeMethodHook & ) = delete;

		inline constexpr NativeMethodHook( const glob64Address target, const glob64Address replace )
			: _target{ target }, _replace{ replace } {}

		bool init( );

		bool enable( );
	};
}