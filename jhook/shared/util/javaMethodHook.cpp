#include <shared/shared.hpp>

namespace shared {
	/*
	* rbx - method
	* r15 - thread
	* r13 - caller sp
	*/
	glob64Address JavaMethodHook::make_i2i_trampoline( const JavaMethodHook &hook ) {
		JIT_START;

#if 0
		auto method = x86::ptr( hook._i2i_reg_backup._rbx._ptr );
		auto thread = x86::ptr( hook._i2i_reg_backup._r15._ptr );
		auto callersp = x86::ptr( hook._i2i_reg_backup._r13._ptr );
		auto sp = x86::ptr( hook._i2i_reg_backup._rsp._ptr );

		auto replace = x86::ptr( hook._replace._ptr );
		auto target = x86::ptr( hook._i2i_reg_backup._rbx._ptr );

		/* push old args */
		__ mov( method, x86::rbx );
		__ mov( thread, x86::r15 );
		__ mov( callersp, x86::r13 );
		__ mov( sp, x86::rsp );

		/* push stack frame here mb?? cus args are passed thru
		   stack and we're messing around with it so ._. */

		/* shall we create a new frame here?? */
		__ push( x86::rbx );
		__ push( x86::r15 );

		/* call the replace */
		__ call( replace );

		/* free the stack */
		__ pop( x86::r15 );
		__ pop( x86::rbx );

		/* pop old args */
		__ mov( x86::rbx, method );
		__ mov( x86::r15, thread );
		__ mov( x86::r13, callersp );
		__ mov( x86::rsp, sp );

		__ call( x86::rbx );

		__ ret( );
#else
		/* a simple tester for the  */
		__ call( x86::rbx );
		__ ret( );
#endif
	}

	/*
	* r15 - thread
	* rsp - initial return address, preserve it as
	*				it further gets passed to rax in the trampoline.
	* rbx - method pointer, has to be preserved.
	* 
	* !! args get passed through a stack frame, just call   !!
	* !! original in a trampoline and everything will be ok !!
	*/
	glob64Address JavaMethodHook::make_i2c_trampoline( const JavaMethodHook &hook ) {
#if 0
		/*
		* hook._i2c_reg_backup._rbx - [r12]
		* hook._i2c_reg_backup._r15 - [r12+8h]
		* hook._i2c_reg_backup._rsp - [r14+10h]
		*/
		auto *regs = &hook._i2c_reg_backup;
		auto *replace = &hook._replace;
		auto *target = &hook._target;

		__asm {
			mov qword ptr [regs], rbx /* save the Method * */
			mov qword ptr [regs+8h], r15 /* save the JavaThread * */
			mov qword ptr [regs+10h], rsp /* save the return address */
			push rbx /* arg #1 */
			push r15 /* arg #2 */
			call [replace] /* call replace */
			pop r15
			pop rbx
			mov rbx, qword ptr [regs] /* restore rbx */
			mov r15, qword ptr [regs+8h] /* restore r15 */
			mov rsp, qword ptr [regs+10h] /* restore rsp */
			call [target] /* call the original trampoline */
			ret
		}
#endif

		return hook._replace;
	}

	glob64Address JavaMethodHook::make_c2i_trampoline( const JavaMethodHook &hook ) {
		return hook._replace;
	}
}