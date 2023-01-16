#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>

// Function to compare integers. Needed for qsort
int cmp(const void* a, const void* b) {
    if (*(const int*)a < *(const int*)b) return -1;
    if (*(const int*)a > *(const int*)b) return 1;
    return 0;
}

// Function to convert integers to strings that can be used as file names
char* int_to_str(int num) {
	int length = snprintf( NULL, 0, "%d", num);
	char* str = (char*)malloc((length + 1) * sizeof(char));
	snprintf(str, length + 1, "%d", num);
	return str;
}

// Function that merges two pre-sorted files together
void merge(int in_num1, int in_num2, int out_num/*, FILE* log*/) { //	log is for DEBUG
	// Open/create input and output files with corresponding number
	char* f_name1 = int_to_str(in_num1);
	char* f_name2 = int_to_str(in_num2);
	int f_in1 = open(f_name1, O_RDONLY);
	// fprintf(log, "Opened file %s for input\n", f_name1); //	DEBUG
	if (-1 == f_in1) {
		perror("open");
		_exit(1);
	}
	int f_in2 = open(f_name2, O_RDONLY);
	// fprintf(log, "Opened file %s for input\n", f_name2); //	DEBUG
	if (-1 == f_in2) {
		perror("open");
		_exit(1);
	}
	char* f_name3 = int_to_str(out_num);
	int f_out = open(f_name3, O_WRONLY | O_CREAT, 0666);
	// fprintf(log, "Opened file %s for output\n", f_name3); //	DEBUG
	if (-1 == f_out) {
		perror("open");
		_exit(1);
	}
	// Output file name can be deleted right away. Input file names will be needed later to delete input files
	free(f_name3);
	// Create buffers for input and output
	const int buff_size = 1024;
	int buff_in1[buff_size], buff_in2[buff_size], buff_out[2 * buff_size];
	// Variables: buffer input lengths (in bytes), buffer lengths (in integer sizes)
	int buff_in_len1 = 0, buff_in_len2 = 0, buff_len1 = 0, buff_len2 = 0;
	// Counters for output buffers and input buffers
	int count = 0, count1 = 0, count2 = 0;
	// Merge values from both input files to output files:
	// Read first blocks from input files
	buff_in_len1 = read(f_in1, buff_in1, buff_size * sizeof(int));
	buff_in_len2 = read(f_in2, buff_in2, buff_size * sizeof(int));
	// Loop until there is nothing to read from one of the input files
	while (buff_in_len1 != 0 && buff_in_len2 != 0) {
		// Calculate buffer lengths according to input sizes
		buff_len1 = buff_in_len1 / sizeof(int);
		buff_len2 = buff_in_len2 / sizeof(int);
		// fprintf(log, "First input buffer:\n");  			//
		// for (int i = 0; i < buff_len1; ++i) {      	//
		// 	fprintf(log, "%d ", buff_in1[i]); 					//	
		// }									  												//	
		// fprintf(log, "\n");					  							//	DEBUG
		// fprintf(log, "Second input buffer:\n");			//
		// for (int i = 0; i < buff_len2; ++i) {	  		//	
		// 	fprintf(log, "%d ", buff_in2[i]); 					//
		// }									  												//
		// fprintf(log, "\n"); 				  								//
		// Write min number from input buffers to output buffer until one of the buffers is empty
		while (count1 < buff_len1 && count2 < buff_len2) {
			if (buff_in1[count1] < buff_in2[count2]) {
				buff_out[count] = buff_in1[count1];
				++count1; ++count;
			} else {
				buff_out[count] = buff_in2[count2];
				++count2; ++count;
			} 
		}
		// fprintf(log, "count = %d, count1 = %d, count2 = %d\n", count, count1, count2); //	DEBUG
		// Reset whichever buffer is full and read next block to it
		if (count1 >= buff_len1) {
			count1 = 0;
			buff_in_len1 = read(f_in1, buff_in1, buff_size * sizeof(int));
			// fprintf(log, "Reset first input buffer\n"); //	DEBUG
		}
		if (count2 >= buff_len2) {
			count2 = 0;
			buff_in_len2 = read(f_in2, buff_in2, buff_size * sizeof(int));
			// fprintf(log, "Reset second input buffer\n"); //	DEBUG
		}
		// fprintf(log, "Output buffer:\n");			//
		// for (int i = 0; i < count; ++i) {			//
		// 	fprintf(log, "%d ", buff_out[i]);			//	DEBUG
		// } 																			//
		// fprintf(log, "\n");										//
		// Write output buffer to output file. Reset counter
		if (-1 == write(f_out, buff_out, count * sizeof(int))) {
			perror("write");
			_exit(1);
		}
		count = 0;
	}
	if (buff_in_len1 == -1 || buff_in_len2 == -1) {
		perror("read");
		_exit(1);
	}
	// Write the buffer that is not yet empty to file
	if (buff_in_len1 != 0) {
		// fprintf(log, "Buffer 1 not fully printed. Left values:\n");	//
		// for (int i = count1; i < buff_in_len1 / sizeof(int); ++i) {	//
		// 	fprintf(log, "%d ", buff_in1[i]);														//	DEBUG
		// }																														//
		// fprintf(log, "\n");																					//
		if (-1 == write(f_out, buff_in1 + count1, buff_in_len1 - count1 * sizeof(int))) {
			perror("write");
			_exit(1);
		}
	}
	if (buff_in_len2 != 0) {
		// fprintf(log, "Buffer 2 not fully printed. Left values:\n");	//
		// for (int i = count2; i < buff_in_len2 / sizeof(int); ++i) {	//
		// 	fprintf(log, "%d ", buff_in2[i]);														//	DEBUG
		// }																														//
		// fprintf(log, "\n");																					//
		if (-1 == write(f_out, buff_in2 + count2, buff_in_len2 - count2 * sizeof(int))) {
			perror("write");
			_exit(1);
		}
	}
	// If either of input files is not fully read, rewrite rest of values in it to output file
	while (buff_in_len1 = read(f_in1, buff_in1, buff_size * sizeof(int))) {
		// fprintf(log, "Writing the rest from file 1:");					//
		// for (int i = 0; i < buff_in_len1 / sizeof(int); ++i) { //	
		// 	fprintf(log, "%d ", buff_in1[i]);											//	DEBUG
		// }																											//
		// fprintf(log, "\n");																		//		
		if (-1 == write(f_out, buff_in1, buff_in_len1)) {
			perror("write");
			_exit(1);
		}
	}
	if (buff_in_len1 == -1) {
		perror("read");
		_exit(1);
	}
	while (buff_in_len2 = read(f_in2, buff_in2, buff_size * sizeof(int))) {
		// fprintf(log, "Writing the rest from file 2:");					//
		// for (int i = 0; i < buff_in_len2 / sizeof(int); ++i) {	//
		// 	fprintf(log, "%d ", buff_in2[i]);											//
		// }																											//
		// fprintf(log, "\n");																		//
		if (-1 == write(f_out, buff_in2, buff_in_len2)) {
			perror("write");
			_exit(1);
		}
	}
	if (buff_in_len1 == -1) {
		perror("read");
		_exit(1);
	}
	// Close all files
	if (-1 == close(f_in1) || -1 == close(f_in2) || -1 == close(f_out)) {
		perror("close");
		_exit(1);
	}
	// Delete input files to save memory
	if (-1 == unlink(f_name1) || -1 == unlink(f_name2)) {
		perror("unlink");
		_exit(1);
	}
	// Delete file names
	free(f_name1);
	free(f_name2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

// Command line arguments:
// 1 - input file
int main(int argc, char** argv) {
	// FILE* log;										//	DEBUG
	// log = fopen("log.txt", "w");	//
	// Open input file and create & open output files
  int f_in = open(argv[1], O_RDONLY);
  if (-1 == f_in) {
		perror("open");
		_exit(1);
	}
	// Initialize buffer for input
	const int buff_size = 1024;
  int buff_in[buff_size];
  // Check if file is empty, close file and end program if so
  if (0 == read(f_in, buff_in, sizeof(int))) {
  	close(f_in);
  	return 0;
  }
  // Move offset back to 0
  if (-1 == lseek(f_in, 0, SEEK_SET)) {
  	perror("lseek");
  	_exit(1);
  }  
  // Initialize big_buffer from which values will be written to separate files
  const uint32_t big_buff_size = buff_size * 500;
  int* big_buff = (int*)malloc(big_buff_size * sizeof(int));
  // Initialize array of file descriptors for output files
  const int max_files = 512;
  int f_out[max_files];
	// Define maximum output size
	const uint32_t max_out = 1024;
  // Variables: input buffer length and buffer length, counter for inputs written to big buffer, output file counter, char pointer for future file names
  int buff_in_len = 0;
  int buff_len = 0;
  int in_counter = 0;
	uint32_t out_counter = 0;
  int f_num = 0;
  char* f_name = NULL;
  
  // Read values until end of file
 	while (buff_in_len = read(f_in, buff_in, buff_size * sizeof(int))) {
		// Calculate buffer length
 		buff_len = buff_in_len / sizeof(int);
 		// Copy values from input buffer to big buffer
		// fprintf(log, "Input buffer:\n");					//														
		// for (int i = 0; i < buff_len; ++i) {			//	
		// 	fprintf(log, "%d ", buff_in[i]);				//	DEBUG
		// }																				//
		// fprintf(log, "\n");											//
		// Copy values to big buffer
    for (int i = 0; i < buff_len; ++i) {
    	big_buff[i + in_counter * buff_size] = buff_in[i];
    }
    ++in_counter;
    // Check if big buffer is full
    if (in_counter == big_buff_size / buff_size) {
			// fprintf(log, "Big buffer full.\n");	 															//
			// fprintf(log, "Unsorted:\n");																				//	
			// for (int i = 0; i < big_buff_size - buff_size + buff_len; ++i) {		//	DEBUG		
			// 	fprintf(log, "%d ", big_buff[i]);																	//
			// }																																	//
			// fprintf(log, "\n");																								//
			// Quick-sort array in big buffer
    	qsort(big_buff, big_buff_size - buff_size + buff_len, sizeof(int), cmp);
			// fprintf(log, "Sorted:\n");																					//	
			// for (int i = 0; i < big_buff_size - buff_size + buff_len; ++i) {		//		
			// 	fprintf(log, "%d ", big_buff[i]);																	//	DEBUG
			// }																																	//
			// fprintf(log, "\n");																								//		
      // Generate new file name from its number
      f_name = int_to_str(f_num);
      // Open new file and save its descriptor 
      f_out[f_num] = open(f_name, O_WRONLY | O_CREAT, 0666);
  		// Delete generated file name from heap
  		free(f_name);
			// Write big buffer to new file
			// fprintf(log, "%d < %d ?\n", (out_counter + 1) * max_out, big_buff_size - buff_size + buff_len); //	DEBUG
			while ((out_counter + 1) * max_out < big_buff_size - buff_size + buff_len) {
				// fprintf(log, "Writing to buffer:\n");																				//	
				// for (int i = out_counter * max_out; i < (out_counter + 1) * max_out; ++i) {	//		
				// 	fprintf(log, "%d ", big_buff[i]);																						//	DEBUG
				// }																																						//
				// fprintf(log, "\n");																													//
				if (-1 == write(f_out[f_num], big_buff + out_counter * max_out, max_out * sizeof(int))) {
       		perror("write");
       		_exit(3);
      	}
				++out_counter;
				// fprintf(log, "%d < %d ?\n", (out_counter + 1) * max_out, big_buff_size - buff_size + buff_len);	//	DEBUG
			}
			// Write last block (may not be of max length)
			// fprintf(log, "Last but not least: write %d symbols from big buffer starting at position %d\n",	//
			//  big_buff_size - buff_size + buff_len - (max_out * out_counter), out_counter * max_out);				//
			// fprintf(log, "Writing to buffer:\n");																													//	
			// for (int i = out_counter * max_out; i < big_buff_size - buff_size + buff_len; ++i) {						//	DEBUG		
			// 	fprintf(log, "%d ", big_buff[i]);																															//
			// }																																															//
			// fprintf(log, "\n");																																						//									
			if (-1 == write(f_out[f_num], big_buff + out_counter * max_out, (big_buff_size - buff_size + buff_len - (max_out * out_counter)) * sizeof(int))) {
				perror("write");
				_exit(1);
			}
			out_counter = 0;
     
      // Close file so that too much files don't end up opened at the same time
      if (-1 == close(f_out[f_num])) {
      	perror("close");
      	_exit(1);
      }
      ++f_num;
      in_counter = 0;
    }
  }
  if (buff_in_len == -1) {
  	perror("read");
  	_exit(1);
  }
  
  // Check if any output files were written; sort and output big buffer right away if not
  if (f_num == 0) {
		// fprintf(log, "No files created. Big buffer contents\n");	 						//
		// fprintf(log, "Unsorted:\n");																					//	
		// for (int i = 0; i < buff_size * (in_counter - 1) + buff_len; ++i) {	//	DEBUG		
		// 	fprintf(log, "%d ", big_buff[i]);																		//
		// }																																		//
		// fprintf(log, "\n");																									//
  	qsort(big_buff, buff_size * (in_counter - 1) + buff_len, sizeof(int), cmp);
		// fprintf(log, "Sorted:\n");																						//	
		// for (int i = 0; i < buff_size * (in_counter - 1) + buff_len; ++i) {	//		
		// 	fprintf(log, "%d ", big_buff[i]);																		//	DEBUG
		// }																																		//
		// fprintf(log, "\n");																									//
  	for (int i = 0; i < buff_size * (in_counter - 1) + buff_len; ++i) {
  		printf("%d\n", big_buff[i]);
  	}
		return 0;
  }
  else {
  	// Check if something is left in big buffer, sort it and write to output file if so
  	if (in_counter != 0) {
			// fprintf(log, "Rest of big buffer contents\nUnsorted:\n");						//							
			// for (int i = 0; i < buff_size * (in_counter - 1) + buff_len; ++i) {	//		
			// 	fprintf(log, "%d ", big_buff[i]);																		//	DEBUG
			// }																																		//
			// fprintf(log, "\n");																									//
    	qsort(big_buff, buff_size * (in_counter - 1) + buff_len, sizeof(int), cmp);
			// fprintf(log, "Sorted:\n");																						//	
			// for (int i = 0; i < buff_size * (in_counter - 1) + buff_len; ++i) {	//		
			// 	fprintf(log, "%d ", big_buff[i]);																		//	DEBUG
			// }																																		//
			// fprintf(log, "\n");																									//
	    f_name = int_to_str(f_num);
  	  f_out[f_num] = open(f_name, O_WRONLY | O_CREAT, 0666);
  		free(f_name);
			while (out_counter * max_out < buff_size * (in_counter - 1) + buff_len) {
				if (-1 == write(f_out[f_num], big_buff + out_counter * max_out, sizeof(int) * (
					(out_counter + 1) * max_out < buff_size * (in_counter - 1) + buff_len ? max_out : buff_size * (in_counter - 1) + buff_len - max_out * out_counter))) {
       		perror("write");
       		_exit(3);
      	}
				++out_counter;
			}
			out_counter = 0;
  	  // Close file so that too much files don't end up opened at the same time
  	  if (-1 == close(f_out[f_num])) {
  	  	perror("close");
  	   	_exit(1);
  	  }
  	  ++f_num;
  	}
  }
	// Delete big buffer from heap as it was dynamically allocated
  free(big_buff);
  // Close input file
  if (-1 == close(f_in)) {
  	perror("close");
  	_exit(1);
  }
  
  // Merge files into one non-recursively
  int in_num = 0, out_num = f_num;
  while (out_num - in_num > 1) {
  	merge(in_num, in_num + 1, out_num/*, log*/);
		// fprintf(log, "Merged files %d and %d into %d\n", in_num, in_num + 1, out_num);	//	DEBUG
  	in_num += 2; ++out_num;
  }
	// Open result file
  char* f_name_last = int_to_str(out_num - 1);
  int f_result = open(f_name_last, O_RDONLY);
  if (f_result == -1) {
  	perror("open");
  	_exit(1);
  }
	// fprintf(log, "Result file:\n");	//	DEBUG
	// Write result file contents to standard output
  while (buff_in_len = read(f_result, buff_in, buff_size * sizeof(int))) {
  	for (int i = 0; i < buff_in_len / sizeof(int); ++i) {
  		printf("%d ", buff_in[i]);
			// fprintf(log, "%d ", buff_in[i]);	//	DEBUG
  	}	
  }
  if (buff_in_len == -1) {
  	perror("read");
  	_exit(1);
  } 
  
  // Close final file
  if (-1 == close(f_result)) {
  	perror("close");
  	_exit(1);
  }  
	// fclose(log);	//	DEBUG
  return 0;
}