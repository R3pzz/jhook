#pragma once

namespace shared {
	class JavaMethodHook {
		friend glob64Address make_i2i_trampoline( const JavaMethodHook &hook );
		friend glob64Address make_i2c_trampoline( const JavaMethodHook &hook );
		friend glob64Address make_c2i_trampoline( const JavaMethodHook &hook );

	public:
		/* so:
		* 1. method is brand-new, called from and to the interpreter:
		*		- we have to hook i2i entry in the normal way
		*			through a trampoline.
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
		*			`result'(*)(JNIEnv *env, jobject object, `other args...')
		*			and call original method from there with the same one via
		*			a regular minhook.
		* 
		* PS: we'll need to handle situations when the method is initially
		*			interpreted, but gets compiled after a certain amount of
		*			iterations. as a result, all of our hooks (i2c and c2i basically)
		*			will get reset. we need to somehow rework this method so
		*			the hook gets changed even after compiling the method.
		*/
		enum struct entryType : std::uint32_t {
			kNone = 0u,

			/* interpreter-called method hooking sequence */
			kUncompiled = 2u,
			/* compiled method hooking sequence */
			kCompiled = 3u,

			/* left here for debugging purposes... */
			kI2CUncompiled = 4u,
		};

		inline static entryType dispatch_target_entry( const javaMethod *method ) {
			if ( !method )
				return entryType::kNone;

			/* lack of an nmethod means there
			   is no compiled code for it. */
			if ( !method->_nmethod )
				return entryType::kUncompiled;
			/* else, the method has been compiled and is ready
			   to be called from compiled and interpreted code. */
			else
				return entryType::kCompiled;

			return entryType::kNone;
		}

		/* structure that stores register values that
		   might be overwritten by the hook function
			 when called from i2c trampoline. */
		struct i2cRegisterBackup {
			glob64Address _rbx{}; /* method pointer */
			glob64Address _r15{}; /* java thread pointer */
			glob64Address _rsp{}; /* stack pointer */

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
			glob64Address _rbx{};
			glob64Address _r15{};
			glob64Address _rsp{};
			glob64Address _r13{};

			inline constexpr i2iRegisterBackup( ) = default;

			inline void restore( ) {
				_rbx = nullptr;
				_r15 = nullptr;
				_rsp = nullptr;
				_r13 = nullptr;
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
			/* this thing will hold an original c2i entry address at first,
			   but may get overwritten with code's original address if a
				 compiled method is hooked */
			glob64Address _c2i_backup{};

			inline constexpr entryBackup( ) = default;

			inline entryBackup( javaMethod *method )
				: _i2i_backup{ method ? method->_i2i_entry : nullptr }
				, _i2c_backup{ method ? method->_i2c_entry : nullptr }
				, _c2i_backup{ method ? method->_c2i_entry : nullptr }
			{}

			inline void store( javaMethod *method ) {
				if ( !method )
					return;

				_i2i_backup = method->_i2i_entry;
				_i2c_backup = method->_i2c_entry;
				_c2i_backup = method->_c2i_entry;
			}

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

		inline constexpr JavaMethodHook( const JavaMethodHook & ) = delete;

		inline constexpr JavaMethodHook &operator=( const JavaMethodHook & ) = delete;

		inline JavaMethodHook( javaMethod *target, const glob64Address replace )
			: _entry_type{ dispatch_target_entry( target ) }, _target{ target }, _replace{ replace }, _orig_entries{ target } {}

		inline JavaMethodHook( javaMethod *target, const glob64Address replace, const entryType entry_type )
			: _entry_type{ entry_type }, _target{ target }, _replace{ replace }, _orig_entries{ target } {}

		inline ~JavaMethodHook( ) {
			restore( );
		}

		bool init( javaMethod *target, const glob64Address replace, const entryType entry_type = entryType::kNone );

		bool enable( );

		inline void restore( ) {
			_orig_entries.restore( _target );

			if ( _i2i_tramp._ptr )
				VirtualFree( _i2i_tramp.as<void *>( ), 0u, MEM_DECOMMIT );

			if ( _i2c_tramp._ptr )
				VirtualFree( _i2c_tramp.as<void *>( ), 0u, MEM_DECOMMIT );

			if ( _c2i_tramp._ptr )
				VirtualFree( _c2i_tramp.as<void *>( ), 0u, MEM_DECOMMIT );

			_i2i_reg_backup.restore( );
			_i2c_reg_backup.restore( );
		}
		
		inline const i2iRegisterBackup &i2i_reg_backup( ) const {
			return _i2i_reg_backup;
		}
	};
}