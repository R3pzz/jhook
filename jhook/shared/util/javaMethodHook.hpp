#pragma once

namespace shared {
	class JavaMethodHook {
		friend static glob64Address make_i2i_trampoline( const JavaMethodHook &hook );
		friend static glob64Address make_i2c_trampoline( const JavaMethodHook &hook );
		friend static glob64Address make_c2i_trampoline( const JavaMethodHook &hook );

	public:
		/* so:
		* 1. method is brand-new, called from and to the interpreter:
		*		- we have to hook i2i entry in the normal way through a
		*			trampoline,
		*		- we have to hook i2c entry same as i2i entry.
		*		- we have to hook c2i entry as a normal c2i
		*			entry also through a trampoline.
		*	2. method has been compiled, _code is there:
		*		- we don't care about i2i anymore as the method has
		*			already been compiled and is not passed through
		*			the interpreter.
		*		- we have to hook i2c entry as a normal i2c
		*			entry through a trampoline.
		*		- we have to hook c2i entry with a callind convention, which is
		*			`result'(*)(JNIEnv **env, jobject **object, `other args...')
		*			and call original method from there with the same one via
		*			a regular minhook.
		*/
		// то что снизу(не готовый вариант) я переделаю в соответствии с выше сказанным и кину
		enum struct entryType : std::uint32_t {
			kNone = 0u,
#if 1
			/* interpreter-to-interpreter entry. used in uncompiled methods
			   that are called from an interpreter. */
			kI2I = 1u,
			/* interpreter-to-compiled entry. used in compiled methods
			   that are called from an interpreter. */
			kI2C = 2u,
			/* compiled-to-interpreter or compiled-to-compiled(when nmethod is there) entry.
				 used in interpreted methods that are called from a compiled one. */
			kC2I = 3u,
			/* c2i and i2c entries are hooked together. */
			kC2I2C = 4u,
			/* i2i and i2c entries are hooked together. */
			kC2I2I = 5u,
#endif
			/* i2i, i2c and c2i entries are hooked together. */
			kAll = 6u,
		};

		inline static entryType dispatch_target_entry( const javaMethod *method ) {
			if ( !method )
				return entryType::kNone;

			/* lack of an nmethod means there
			   is no compiled code for it. */
			if ( !method->_nmethod )
				return entryType::kC2I2I;
			/* else, the method has been compiled and is ready
			   to be called from compiled and interpreted code. */
			else
				return entryType::kC2I2C;

			return entryType::kNone;
		}

		/* structure that stores register values that
		   might be overwritten by the hook function
			 when called from i2c trampoline. */
		struct i2cRegisterBackup {
			std::uint8_t *_rbx{}; /* method pointer */
			std::uint8_t *_r15{}; /* java thread pointer */
			std::uint8_t *_rsp{}; /* return address pointer */

			inline constexpr i2cRegisterBackup( ) = default;

			inline void restore( ) {
				_rbx = nullptr;
				_r15 = nullptr;
				_rsp = nullptr;
			}
		};

		/* structure that stores register values that
			 might be overwritten by the hook function
			 when called from i2c trampoline. */
		struct i2iRegisterBackup {
			inline constexpr i2iRegisterBackup( ) = default;

			inline void restore( ) {
				
			}
		};

		/* generate an assembly trampoline to call our hook */
		static glob64Address make_i2i_trampoline( const JavaMethodHook &hook );
		static glob64Address make_i2c_trampoline( const JavaMethodHook &hook );
		static glob64Address make_c2i_trampoline( const JavaMethodHook &hook );

	private:
		struct entryBackup {
			glob64Address _i2i_backup{};
			glob64Address _i2c_backup{};
			glob64Address _c2i_backup{};

			inline constexpr entryBackup( ) = default;

			inline entryBackup( javaMethod *method )
				: _i2i_backup{ method->_i2i_entry }
				, _i2c_backup{ method->_i2c_entry }
				, _c2i_backup{ method->_c2i_entry }
			{}

			inline void restore( javaMethod *method ) {
				if ( !method )
					return;

				method->_i2i_entry = _i2i_backup;
				method->_i2c_entry = _i2c_backup;
				method->_c2i_entry = _c2i_backup;
			}
		};

		/* entry we're hooking in */
		entryType _entry_type{};

		/* backup registers */
		i2cRegisterBackup _i2c_reg_backup{};
		i2iRegisterBackup _i2i_reg_backup{};

		glob64Address _replace{};

		javaMethod *_target{};

		/* function trampolines */
		glob64Address _i2i_tramp{};
		glob64Address _i2c_tramp{};
		glob64Address _c2i_tramp{};

		entryBackup _orig_entries{};

	public:
		inline constexpr JavaMethodHook( ) = default;

		inline JavaMethodHook( javaMethod *target, const glob64Address replace )
			: _entry_type{ dispatch_target_entry( target ) }, _target{ target }, _replace{ replace }, _orig_entries{ target } {}

		inline JavaMethodHook( javaMethod *target, const glob64Address replace, const entryType entry_type )
			: _entry_type{ entry_type }, _target{ target }, _replace{ replace }, _orig_entries{ target } {}

		inline ~JavaMethodHook( ) {
			restore( );
		}

		inline bool init( ) {
			if ( _i2i_tramp._ptr || _i2c_tramp._ptr )
				return false;

			switch ( _entry_type ) {
			case entryType::kI2I:
				_i2i_tramp = make_i2i_trampoline( *this );
				
				return _i2i_tramp._ptr ? true : false;
			case entryType::kI2C:
				_i2c_tramp = make_i2c_trampoline( *this );

				return _i2c_tramp._ptr ? true : false;
			case entryType::kC2I2I:
				_i2i_tramp = make_i2i_trampoline( *this );
				_c2i_tramp = make_c2i_trampoline( *this );

				return ( _i2i_tramp._ptr && _c2i_tramp._ptr ) ? true : false;
			case entryType::kC2I2C:
				_i2c_tramp = make_i2c_trampoline( *this );
				_c2i_tramp = make_c2i_trampoline( *this );

				return ( _i2c_tramp._ptr && _c2i_tramp._ptr ) ? true : false;
			case entryType::kC2I:
				_c2i_tramp = make_c2i_trampoline( *this );

				return _c2i_tramp._ptr ? true : false;
			case entryType::kNone:
				return false;
			}

			return false;
		}

		inline bool enable( ) {
			switch ( _entry_type ) {
			case entryType::kI2I: {
				if ( !_i2i_tramp )
					return false;
				
				_target->_i2i_entry = _i2i_tramp;

				return true;
			}
			case entryType::kI2C: {
				if ( !_i2c_tramp )
					return false;

				_target->_i2c_entry = _i2c_tramp;

				return true;
			}
			case entryType::kC2I2C: {
				if ( !_i2c_tramp || !_c2i_tramp )
					return false;

				_target->_c2i_entry = _c2i_tramp;
				_target->_i2c_entry = _i2c_tramp;

				return true;
			}
			case entryType::kC2I2I: {
				if ( !_c2i_tramp || !_i2i_tramp )
					return false;

				_target->_c2i_entry = _c2i_tramp;
				_target->_i2i_entry = _i2i_tramp;

				return true;
			}
			case entryType::kC2I: {
				if ( !_c2i_tramp )
					return false;
				 
				_target->_c2i_entry = _c2i_tramp;

				return true;
			}
			case entryType::kNone:
				return false;
			}

			return false;
		}

		inline void restore( ) {
			_orig_entries.restore( _target );

			if ( _i2i_tramp._ptr )
				std::free( _i2i_tramp.as<void *>( ) );

			if ( _i2c_tramp._ptr )
				std::free( _i2c_tramp.as<void *>( ) );

			if ( _c2i_tramp._ptr )
				std::free( _c2i_tramp.as<void *>( ) );

			_i2i_reg_backup.restore( );
			_i2c_reg_backup.restore( );
		}
	};
}