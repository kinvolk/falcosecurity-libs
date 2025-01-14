#include "../../event_class/event_class.h"

#ifdef __NR_chdir
TEST(SyscallExit, chdirX)
{
	auto evt_test = new event_test(__NR_chdir, EXIT_EVENT);

	evt_test->enable_capture();

	/*=============================== TRIGGER SYSCALL  ===========================*/

	/* Syscall special notes:
	 * `chdir()` changes the current working directory of the calling
	 * process to the directory specified in path. Here we pass a mock dir
	 * so the call must fail.
	 */

	const char* new_dir = "mock_dir";
	assert_syscall_state(SYSCALL_FAILURE, "chdir", syscall(__NR_chdir, new_dir));
	int64_t errno_value = -errno;

	/*=============================== TRIGGER SYSCALL ===========================*/

	evt_test->disable_capture();

	evt_test->assert_event_presence();

	if(HasFatalFailure())
	{
		return;
	}

	evt_test->parse_event();

	evt_test->assert_header();

	/*=============================== ASSERT PARAMETERS  ===========================*/

	/* Parameter 1: res (type: PT_ERRNO)*/
	evt_test->assert_numeric_param(1, (int64_t)errno_value);

	/* Parameter 2: path (type: PT_CHARBUF) */
	evt_test->assert_charbuf_param(2, new_dir);

	/*=============================== ASSERT PARAMETERS  ===========================*/

	evt_test->assert_num_params_pushed(2);
}
#endif
