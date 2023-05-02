#pragma once

namespace shared {
	class RendererContext {
	public:
		/* some vector declarations. if our mathlib provider changes,
		   it will be easier to change just these six declarations then
			 those in every corner of this project... */
		using vec2Int = glm::ivec2;
		using vec2Uint = glm::uvec2;
		using vec2Float = glm::vec2;
		using vec3Int = glm::ivec3;
		using vec3Uint = glm::uvec3;
		using vec3Float = glm::vec3;

	public:
		inline constexpr RendererContext( ) = default;

		inline constexpr RendererContext( RendererContext && ) = delete;

		inline constexpr RendererContext( const RendererContext & ) = delete;

		inline constexpr RendererContext &operator=( RendererContext && ) = delete;

		inline constexpr RendererContext &operator=( const RendererContext & ) = delete;

		static void load_rect_outlined( const vec2Float &origin, const vec2Float &extent, const color &col1, const float round = 0.f );

		static void load_rect_filled( const vec2Float &origin, const vec2Float &extent, const color &col1, const float round = 0.f );

		static void load_rect_filled( const vec2Float &origin, const vec2Float &extent, const color &col1, const color &col2, const float round = 0.f );
	};
} 