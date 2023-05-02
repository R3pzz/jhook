#pragma once

namespace shared {
	struct tlsManager {
	private:
		static std::uint32_t _tls_idx;

	public:
		inline constexpr tlsManager( ) = default;

		static void init( );

		/* when a call to JNIEnv::... function is executed,
		   it runs a check for whether we're calling its
			 function from the owner thread or not. this check
			 is being done by a call to ThreadLocalStorage::thread().
			 if it returns the value different from the env owner
			 thread address, app crashes. we'll have to spoof that
			 address and install it for current tls. */
		static void set_thread_current( javaThread *thread );

		static javaThread *thread_current( );
	};
}