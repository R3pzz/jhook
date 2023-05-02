#pragma once

namespace shared {
	struct color {
	public:
		enum struct encoding : std::uint32_t {
			kNone = 0u,
			/* four floats, each ranges from zero to one
			   with zero meaning zero and one meaning 255 */
			k4Floats = 1u,
			/* four bytes, each ranges from zero to 255 */
			k4Bytes = 2u,
			/* packed int (four bytes, 8-bit each, packed into one int) */
			kPackedInt = 3u,
			/* special imgui case, same as kPackedInt */
#if USE_IMGUI_PROVIDER
			kImU32 = kPackedInt,
#endif
		};

		template <encoding enc>
		struct colorAs {};

		template <>
		struct colorAs<encoding::kNone> {};

		template <>
		struct colorAs<encoding::k4Floats> {
		public:
			using type = std::array<float, 4u>;

		public:
			type _v{};

		public:
			inline constexpr colorAs( const type &v )
				: _v{ v } {}

			inline constexpr operator type( ) const {
				return _v;
			}
		};

		template <>
		struct colorAs<encoding::k4Bytes> {
		public:
			using type = std::array<std::uint8_t, 4u>;

		public:
			type _v{};

		public:
			inline constexpr colorAs( const type &v )
				: _v{ v } {}

			inline constexpr operator type( ) const {
				return _v;
			}
		};

		template <>
		struct colorAs<encoding::kPackedInt> {
		public:
			using type = std::uint32_t;

		public:
			type _v{};

		public:
			inline constexpr colorAs( const type &v )
				: _v{ v } {}

			inline constexpr operator type( ) const {
				return _v;
			}
		};

	private:
		encoding _encoding{};

		union {
			/* encoding::k4Floats */
			std::array<float, 4u> _col4f{};
			/* encoding::k4Bytes */
			std::array<std::uint8_t, 4u> _col4b;
			/* encoding::kPackedInt */
			std::uint32_t _col_packed;
		};

#if USE_IMGUI_PROVIDER
		inline static constexpr std::array<float, 4u> im4_to_float4( const ImVec4 &vec ) {
			return { vec.x, vec.y, vec.z, vec.w };
		}
#endif

	public:
		inline constexpr color( ) = default;

		inline constexpr color( const std::array<float, 4u> &col4f )
			: _col4f{ col4f }, _encoding{ encoding::k4Floats } {}

		inline constexpr color( const std::array<std::uint8_t, 4u> &col4b )
			: _col4b{ col4b }, _encoding{ encoding::k4Bytes } {}

		inline constexpr color( const std::uint32_t &col_packed )
			: _col_packed{ col_packed }, _encoding{ encoding::kPackedInt } {}

#if USE_IMGUI_PROVIDER
		inline constexpr color( const ImVec4 &im4 )
			: _col4f{ im4_to_float4( im4 ) }, _encoding{ encoding::k4Floats } {}
#endif

		template <encoding enc>
		inline constexpr colorAs<enc> as( ) const {
			constexpr auto b4_to_packed = [ ]( const std::array<std::uint8_t, 4u> b4 ) {
				return *reinterpret_cast<const std::uint32_t *>( b4.data( ) );
			};

			constexpr auto packed_to_b4 = [ ]( const std::uint32_t packed ) {
				return std::array<std::uint8_t, 4u>{
					static_cast<std::uint8_t>( packed << 0u ),
					static_cast<std::uint8_t>( packed << 8u ),
					static_cast<std::uint8_t>( packed << 16u ),
					static_cast<std::uint8_t>( packed << 24u )
				};
			};

			if constexpr ( enc == encoding::k4Bytes ) {
				if ( _encoding == encoding::k4Bytes ) {
					return _col4b;
				}
				else if ( _encoding == encoding::k4Floats ) {
#if USE_IMGUI_PROVIDER
					const auto col = ImGui::ColorConvertFloat4ToU32(
						{ _col4f.at( 0u ), _col4f.at( 1u ),
						_col4f.at( 2u ), _col4f.at( 3u ) }
					);

					return packed_to_b4( col );
#else
					/* not implemented yet... */
					return color{ std::uint32_t{} };
#endif
				}
				else if ( _encoding == encoding::kPackedInt ) {
					return packed_to_b4( _col_packed );
				}
			}
			else if constexpr ( enc == encoding::k4Floats ) {
				if ( _encoding == encoding::k4Bytes ) {
#if USE_IMGUI_PROVIDER
					return ImGui::ColorConvertU32ToFloat4( b4_to_packed( _col4b ) );
#else
					/* not implemented yet... */
					return color{ std::array<float, 4u>{} };
#endif
				}
				else if ( _encoding == encoding::k4Floats ) {
					return *this;
				}
				else if ( _encoding == encoding::kPackedInt ) {
#if USE_IMGUI_PROVIDER
					return ImGui::ColorConvertU32ToFloat4( _col_packed );
#else
					/* not implemented yet... */
					return color{ std::array<float, 4u>{} };
#endif
				}
			}
			else if constexpr ( enc == encoding::kPackedInt ) {
				if ( _encoding == encoding::k4Bytes ) {
					return b4_to_packed( _col4b );
				}
				else if ( _encoding == encoding::k4Floats ) {
					return ImGui::ColorConvertFloat4ToU32(
						{ _col4f.at( 0u ), _col4f.at( 1u ),
						_col4f.at( 2u ), _col4f.at( 3u ) }
					);
				}
				else if ( _encoding == encoding::kPackedInt ) {
					return _col_packed;
				}
			}
			else {
				return colorAs<encoding::kNone>{};
			}
		}
	};
}