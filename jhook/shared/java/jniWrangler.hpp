#pragma once

namespace shared {
	struct instanceKlass;

	struct jniWrangler {
	public:
		/* pointer to an interface - found in javaThreads
		   if using custom threading or attached via
			 AttachCurrentThread otherwise. */
		JNIEnv *_env{};

#if !USE_CUSTOM_ENVS
		/* auxiliary object, available when built in a default jni mode. */
		JavaVM *_vm{};
#endif

	public:
		inline constexpr jniWrangler( ) = default;

		inline constexpr bool valid( ) const {
			return _env->functions != nullptr;
		}

		inline constexpr JNIEnv *env( ) {
			return _env;
		}

		inline constexpr const JNIEnv *env( ) const {
			return _env;
		}

		void on_startup( );

		instanceKlass *find_class( const std::string_view name );
	};

	extern jniWrangler jni;
}