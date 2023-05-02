#pragma once

namespace shared {
	struct klass;

	/* oops are handles to c++ objects interjected between java classes. */
	struct oopHandle {
		/* type of the handle - kWeak requires a special load function */
		enum struct handleType : std::uint8_t {
			kObject = 0u,
			kWeak = 1u,
		};

		/* markWord and some 8-byte value */
		std::uint8_t _pad0[ 16u ]{};

		klass *_owner{};

		inline constexpr oopHandle( ) = default;

		template <class klassType>
		inline constexpr klassType *as( ) {
			return reinterpret_cast<klassType *>( _owner );
		}

		template <class klassType>
		inline constexpr const klassType *as( ) const {
			return reinterpret_cast<const klassType *>( _owner );
		}

		inline static oopHandle *from_jobject( const jobject obj ) {
			return *glob64Address{ obj }.as<oopHandle **>( );
		}
	};
}