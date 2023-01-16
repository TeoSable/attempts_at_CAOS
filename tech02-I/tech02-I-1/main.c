#include <stdio.h>
#include <stdlib.h>

extern sum(int x0, size_t N, int *X);

int main() {
	int N = 0;
	scanf("%d", &N);
	int *X = malloc(N*sizeof(int));
	for (int i = 0; i < N; ++i)
		scanf("%d", X+i);
	int x0 = 0;
	scanf("%d", &x0);
	printf("%d\n", sum(x0, N, X));
	return 0;
}
