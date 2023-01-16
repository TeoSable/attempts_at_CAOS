#include <stdio.h>

void normalize_path(char * path) {
  // Pointer cur indicates current position in path and will point to a 0 character when line ends.
  // Pointer cur2 will be used to offset forward from current position
  // i is an iterator for copying, d is a variable for the difference between cur2 and cur
	printf("\ncur = path\n");
  char* cur = path;
	printf("cur2 = cur\n");
  char* cur2 = cur;
  int i = 0;
  int d = 0;
  // Scan path until it ends
  while (*cur != '\0') {
    printf("%c", *cur);
    // Check for the beginning of a slash block
  	if (*cur == '/') {
      // Initialize second pointer and increment it until end of slash block
      cur2 = cur + 1;
      printf("%c", *cur2);
      while ((*cur2 == '/' || *cur2 == '.') && *cur2 != '\0') {
        // If dot is found, check whether it is:
        if (*cur2 == '.') {
          // a) one dot - just pass it
          if (*(cur + 1) == '/' || *(cur + 1) == '\0') {
            ++cur2;
            printf("%c", *cur2);
          } 
          // b) two dots - shift cur to previous slash or beginning of path
          else if (*(cur + 1) == '.' && (*(cur + 2) == '/' || *(cur + 2) == '\0')) {
            if (cur != path) {
              --cur;
              printf("%c", *cur);
              while (*cur != '/' && cur >= path) {
                --cur;
                printf("%c", *cur);
              }
            }
          }
        }
        // Otherwise, just increment cur2 (another slash was found)
        else {
          ++cur2;
          printf("%c", *cur2);
        }
      }
      if (cur2 - cur > 1) {
        // Move all chars starting from cur2 to (cur + 1)
        while (*(cur2 + i) != '\0') {
          *(cur + i + 1) = *(cur2 + i);
          printf("%c", *(cur + i + 1));
          ++i;
        }
        i = 0;
        // Clean the rest (cur2 - cur - 1) chars
        d = cur2 - cur - 1;
        while (*cur2 != '\0') {
          ++cur2;
        }
        --cur2;
        for (int j = 0; j < d; ++j) {
          *(cur - j) = '\0';
        }
      }
      
    }
    // Move to next cur position
    ++cur;
  }
	printf("\n");
  return;
}

int main() {
	printf("Initializing first path\n");
	char path1[] = "abrakadabra///abc";
	printf("Init path2\n");
	char path2[] = "/var/log/../lib/./ejexec";
	printf("normalize_path(path1)\n");
	normalize_path(path1);
	printf("%s\n", path1);
	normalize_path(path2);
	printf("%s\n", path2);
	return 0;
}
