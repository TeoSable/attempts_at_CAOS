#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

// Function to get corresponding index in output file for table element in row j and column i
uint64_t ind(uint64_t i, uint64_t j, uint64_t N, uint64_t W) {
	return j * (N * W + 1) + i * W;
}

// Function to write table element in row j and column i to output file
void set_table_element(uint64_t i, uint64_t j, char* mem, uint64_t N, uint64_t W, uint32_t num) {
	// Initialize temporary string buffer
	char* str = (char*)calloc(W + 1, sizeof(char));
	str[W] = 0;
	// Calculate number of digits in num
	int num_ = num, log = 0;
	while (num_ > 0) {
		num_ /= 10;
		++log;
	}
	// Fill string buffer with number and spaces before it 
	memset(str, ' ', W);
	snprintf(str + W - log, log + 1, "%d", num);
	// Write end result to output file in its correct place
	snprintf(mem + ind(i, j, N, W), W + 1, "%s", str);
	// Delete string buffer from heap
	free(str);
	return;
}

// Command line arguments:
// 1 - input file
// 2 - N
// 3 - W
int main(int argc, char** argv) {
	uint64_t N = (uint64_t)atoi(argv[2]);
	uint64_t W = (uint64_t)atoi(argv[3]);
	// Open output file
  int fd = open(argv[1], O_RDWR | O_CREAT, 0666);
  if (-1 == fd) {
		perror("fd");
		_exit(1);
	}
	// Calculate file size
	uint64_t file_size = N * N * W + N;
	if (-1 == ftruncate(fd, file_size)) {
		perror("ftruncate");
		_exit(1);
	}
	// Map output file to memory
	char *mem = mmap(NULL, file_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, fd, 0);
	close(fd);
  if (MAP_FAILED == mem) {
    perror("mmap");
    _exit(1);
  }
	// Initialize buffer table to store spiral template
	uint32_t** table = (uint32_t**)malloc(N * sizeof(uint32_t*));
	for (uint64_t i = 0; i < N; ++i) {
		*(table + i) = (uint32_t*)malloc(N * sizeof(uint32_t));
	} 
	// Fill table with digits starting from 1 clockwise
	uint32_t num = 1;
	for (uint64_t k = 0; k < N / 2; ++k) {
		for (uint64_t i = k; i < N - k; ++i) {
			// printf("%ld %ld - %d\n", k, i, num);		DEBUG
			table[k][i] = num;
			++num;
		} 
		for (uint64_t j = k + 1; j < N - k; ++j) {
			// printf("%ld %ld - %d\n", j, N - k - 1, num);		DEBUG
			table[j][N - k - 1] = num;
			++num;
		}
		for (uint64_t i = N - k - 2; i > k; --i) {
			// printf("%ld %ld - %d\n", N - k - 1, i, num);		DEBUG
			table[N - k - 1][i] = num;
			++num;
		}
		for (uint64_t j = N - k - 1; j > k; --j) {
			// printf("%ld %ld - %d\n", j, k, num);		DEBUG
			table[j][k] = num;
			++num;
		}
	}
	// Check if center digit is needed and fill it if so
	if (N % 2 != 0) {
			// printf("%ld %ld - %d\n", N/2, N/2, num);		DEBUG
			table[N/2][N/2] = num;
	}
	// Fill output file with table contents
	for (uint64_t j = 0; j < N; ++j) {
		for (uint64_t i = 0; i < N; ++i) {
			set_table_element(i, j, mem, N, W, table[j][i]);
		}
		mem[(j + 1) * (N * W + 1) - 1] = '\n';
	}
	// Delete table from heap
	for (uint64_t i = 0; i < N; ++i) {
		free(table[i]);
	}
	free(table);
	// Unmap output file from memory
	munmap(mem, file_size);
  return 0;
}