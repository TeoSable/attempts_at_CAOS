#include <stdio.h>

int main() {
	char c;
	do {
	c = getchar_unlocked();
	putchar_unlocked(c);
	} while (c != EOF); 
	return 0;
}
