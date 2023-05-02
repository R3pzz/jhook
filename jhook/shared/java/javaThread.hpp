#pragma once

namespace shared {
	/* should be inherited from nativeThread */
	struct javaThread {
	public:
		/* thread entry point function */
		using threadFunction = void( * )( javaThread *, nativeThread * );

		enum struct javaThreadState : std::uint32_t {
			kUninitialized = 0u,
			kNew = 2u,
			kNewTrans = 3u,
			kInNative = 4u,
			kInNativeTrans = 5u,
			kInVm = 6u,
			kInVmTrans = 7u,
			kInJava = 8u,
			kInJavaTrans = 9u,
			kBlocked = 10u,
			kBlockedTrans = 11u,
		};

		enum struct jniState : std::uint32_t {
			kNotAttached = 1u,
			kAttaching = 2u,
			kAttached = 3u,
		};

	public:
		std::uint8_t _pad0[ 784u ]{};

		//threadFunction _entry_point{};

		JNIEnv _env{};

		std::uint8_t _pad1[ 100u ]{};

		javaThreadState _thread_state{};

		std::uint8_t _pad2[ 38u ]{};

		jniState _jni_state{};

	public:
		inline constexpr javaThread( ) = default;

		inline constexpr bool jni_available( ) const {
			return _env.functions != nullptr
				&& _jni_state == jniState::kAttached;
		}

		inline constexpr bool active( ) const {
			return _thread_state == javaThreadState::kInJava;
		}
	};
}