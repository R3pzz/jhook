#pragma once

namespace shared {
	struct threadList {
	public:
		const std::uint32_t _count{};

		threadList *_next_list{};

		javaThread **_threads{};

		std::intptr_t _nested_handle_cnt{};

		inline constexpr threadList( ) = default;

		inline static void for_each_thread( threadList list,
			const std::function<bool( javaThread *thread )> &func,
			const std::size_t skip = 1u
		) {
			for ( std::size_t i = skip; i < list._count; i++ )
				if ( func( list._threads[ i ] ) )
					return;
		}

		static threadList acquire( );
	};
}