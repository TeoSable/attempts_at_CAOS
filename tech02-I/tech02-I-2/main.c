#include <stdio.h>
#include <stdint.h>

extern solve(int64_t A, int B, int C, int D);

int main() {
	int64_t A = 0;
	int B = 0, C = 0, D = 0;
	scanf("%d %d %d %d", &A, &B, &C, &D);
	printf("%d %d %d %d\n", A, B, C, D);
	printf("%d\n", solve(A, B, C, D));
	return 0;
}
