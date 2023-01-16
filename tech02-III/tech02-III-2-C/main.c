#include <sys/syscall.h>
#include <unistd.h>
#include <wchar.h>

extern long syscall(long number, ...);
// Define consts
enum CONSTS{
  MAX_TEXT_SIZE=4096000,
};

void _start() {
  // Get current heap break point
  void* brk_point = (void*)syscall(SYS_brk, -1);
	// Allocate char array for text
	wchar_t* text = (wchar_t*)brk_point;
	int brk_code = syscall(SYS_brk, text + MAX_TEXT_SIZE);
	if (brk_code == -1)
		syscall(SYS_exit, 1);
	
	// Read text from standard input
	ssize_t bytes_read = syscall(SYS_read, STDIN_FILENO, text, MAX_TEXT_SIZE);
	if (bytes_read == -1)
		syscall(SYS_exit, 2);
	
	// Declare variable for number of written bytes
	ssize_t bytes_written = 0;	
	// Declare variables responsible for current position in text and current line end. Start from end of text
	ssize_t cur_pos = bytes_read / 4 - 2, end_pos = bytes_read / 4 - 1;
	// While in text range, search for previous line end and then print line from start to end; then move to next line
	while (cur_pos >= 0) {
		while (cur_pos >= 0) {
			if (text[cur_pos] != '\n')
				cur_pos -= 4;
			else
				break;
		}
		// Exit cycle if current position is out of text range
		if (cur_pos < 0)
			break;
		bytes_written = syscall(SYS_write, STDOUT_FILENO, text + cur_pos + 1, (end_pos - cur_pos) * 4);
		if (bytes_written == -1)
			syscall(SYS_exit, 3);
		end_pos = cur_pos; 
		--cur_pos; 
	}
	// Write first line
	bytes_written = syscall(SYS_write, STDOUT_FILENO, text, end_pos * 4);
	if (bytes_written == -1)
		syscall(SYS_exit, 3);

  // Free heap. If error occurs, exit with code 4
  int brk_code_2 = syscall(SYS_brk, brk_point);
  if (brk_code_2 == -1)
    syscall(SYS_exit, 4);

  // Exit with code 0 upon success
  syscall(SYS_exit, 0);
}
