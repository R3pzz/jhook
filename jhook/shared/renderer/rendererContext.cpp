#include <shared/shared.hpp>

namespace shared {
	void RendererContext::load_rect_outlined( const vec2Float &origin, const vec2Float &extent, const color &col1, const float round ) {
		auto *draw_list = ImGui::GetBackgroundDrawList( );
		if ( !draw_list )
			return;

		draw_list->AddRect( origin, origin + extent, col1.as<color::encoding::kImU32>( ), round );
	}

	void RendererContext::load_rect_filled( const vec2Float &origin, const vec2Float &extent, const color &col1, const float round ) {
		auto *draw_list = ImGui::GetBackgroundDrawList( );
		if ( !draw_list )
			return;

		draw_list->AddRectFilled( origin, origin + extent, col1.as<color::encoding::kImU32>( ), round );
	}

	void RendererContext::load_rect_filled( const vec2Float &origin, const vec2Float &extent, const color &col1, const color &col2, const float round ) {
		auto *draw_list = ImGui::GetBackgroundDrawList( );
		if ( !draw_list )
			return;

		draw_list->AddRectFilledMultiColor( origin, origin + extent,
			col1.as<color::encoding::kImU32>( ),
			col1.as<color::encoding::kImU32>( ),
			col2.as<color::encoding::kImU32>( ),
			col2.as<color::encoding::kImU32>( )
		);
	}
}