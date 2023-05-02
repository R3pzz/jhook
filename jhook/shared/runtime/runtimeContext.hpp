#pragma once

namespace shared {
	class RuntimeContext {
	private:
		dll64Desc _opengl32_desc{};
		dll64Desc _jvm_desc{};

		HINSTANCE _native_instance{};

		bool _running{ true };

	public:
		inline constexpr RuntimeContext( ) = default;
		
		inline constexpr RuntimeContext( RuntimeContext && ) = delete;

		inline constexpr RuntimeContext( const RuntimeContext & ) = delete;

		inline constexpr RuntimeContext &operator=( RuntimeContext && ) = delete;

		inline constexpr RuntimeContext &operator=( const RuntimeContext & ) = delete;

		inline const dll64Desc &opengl32_desc( ) const {
			return _opengl32_desc;
		}

		inline const dll64Desc &jvm_desc( ) const {
			return _jvm_desc;
		}

		inline constexpr bool should_exit( ) const {
			return !_running;
		}

		void on_startup( HINSTANCE instance );
	};

	extern RuntimeContext context;
}