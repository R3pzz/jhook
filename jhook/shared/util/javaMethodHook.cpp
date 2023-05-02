#include <shared/shared.hpp>
#include <libs/asmjit/asmjit.h>

namespace shared {
	glob64Address JavaMethodHook::make_i2i_trampoline( const JavaMethodHook &hook ) {
		/* use asmjit here... */

		return hook._replace;

		//return nullptr;
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
			lea r12, regs
			mov qword ptr [r12], rbx /* save the Method * */
			mov qword ptr [r12+8h], r15 /* save the JavaThread * */
			mov qword ptr [r12+10h], rsp /* save the return address */
			push rbx /* arg #1 */
			push r15 /* arg #2 */
			call [replace] /* call replace */
			pop r15
			pop rbx
			mov rbx, qword ptr [r12] /* restore rbx */
			mov r15, qword ptr [r12+8h] /* restore r15 */
			mov rsp, qword ptr [r12+10h] /* restore rsp */
			call [target] /* call the original trampoline */
			ret
		}
#endif

		return hook._replace;
	}

	glob64Address JavaMethodHook::make_c2i_trampoline( const JavaMethodHook &hook ) {
		/* use asmjit here... */

		return hook._replace;

		//return nullptr;
	}
}