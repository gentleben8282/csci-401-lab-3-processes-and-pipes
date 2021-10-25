/* CSCI 401: Lab #3, Part 1
 * Programmer: Ben Corriette (@02956064)
 * Last modified: 10/21/2021
 * 
 * Summary: C program to demonstrate use of fork() and pipe()
 * 
 * Reference: Linux Programmer's Manual
 */ 

#include <errno.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <sys/wait.h>

// Prints error from the exception 
int print_error(int errnum, char errmsg[]) {
	fprintf(stderr, errmsg, errnum); 
	exit(EXIT_FAILURE); 
}
  
int main() 
{ 
	int fd1[2];  // Used to store two ends of first pipe 
	int fd2[2];  // Used to store two ends of second pipe
	int errsv;   // Used to store error number of exception

	char url_to_append[] = "howard.edu";
	char url_to_append2[] = "gobison.org";
	char input_str[100];
	char input_str2[100];
	char concat_str[100];
	char concat_str2[100];
	char pipe_creation_error_message[] = "Pipe creation failure caused by Error # %d";
	char fork_creation_error_message[] = "Fork creation failure caused by Error # %d";
	char fd_close_error_message[] = "Closing the file descriptor failed because of Error # %d";
	char fd_read_error_message[] = "Reading the file descriptor failed because of Error # %d";
	char fd_write_error_message[] = "Writing to the file descriptor failed because of Error # %d";
	char scan_error_message[] = "Scanning the user input failed because of Error # %d";
	char wait_error_message[] = "Waiting for the child process failed because of Error # %d";

	pid_t process_id; 

	if (pipe(fd1) == -1) { 
		errsv = errno;
		print_error(errsv, pipe_creation_error_message);
	} 
	if (pipe(fd2) == -1) { 
		errsv = errno;
		print_error(errsv, pipe_creation_error_message);
	} 

	printf("Enter a string to concatenate:");
	if (scanf("%s", input_str) == EOF) {
			errsv = errno;
			print_error(errsv, scan_error_message);
	}
	process_id = fork(); 

	if (process_id == -1) { 
		errsv = errno;
		print_error(errsv, fork_creation_error_message);
	} else if (process_id > 0) {
		// Close reading end of first pipe   
		if(close(fd1[0]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		// Write input string
		if (write(fd1[1], input_str, strlen(input_str)+1) == -1) {
			errsv = errno;
			print_error(errsv, fd_write_error_message);
		}
		// Wait for child to send a string   
		if (waitpid(process_id, NULL, 0) == -1) {
			errsv = errno;
			print_error(errsv, wait_error_message);
		}
		// Read concatenated string using second pipe 
		if (read(fd2[0], concat_str, 100) == -1) {
			errsv = errno;
			print_error(errsv, fd_read_error_message);
		}
		// Append “gobison.org” before printing to stdout
		int k = strlen(concat_str); 
		int i; 
		for (i=0; i<strlen(url_to_append2); i++) {
			concat_str[k++] = url_to_append2[i];
		}         
		concat_str[k] = '\0';   // string ends with '\0' 
		printf("Concatenated string: %s\n", concat_str);

		// Close writing end of both pipes 
		if (close(fd2[1]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		if (close(fd1[1]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		// Close reading end of first pipe
		if (close(fd2[0]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
  } else { 	
		// Close writing end of first pipe   
		if (close(fd1[1]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		// Read a string using first pipe
		if (read(fd1[0], concat_str2, 100) == -1) {
			errsv = errno;
			print_error(errsv, fd_read_error_message);
		}
		// Append “howard.edu” before printing to stdout
		int k = strlen(concat_str2); 
		int i; 
		for (i=0; i<strlen(url_to_append); i++) {
			concat_str2[k++] = url_to_append[i];
		}         
		concat_str2[k] = '\0';   // string ends with '\0' 
		printf("Concatenated string: %s\n", concat_str2);

		// Prompt for second input string
		printf("Enter another string to concatenate:");
		if (scanf("%s", input_str2) == EOF) {
			errsv = errno;
			print_error(errsv, scan_error_message);
		}
		// Write input string to second pipe
		if (write(fd2[1], input_str2, strlen(input_str2)+1) == -1) {
			errsv = errno;
			print_error(errsv, fd_write_error_message);
		}
		// Close reading ends of both pipes
		if (close(fd1[0]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		if (close(fd2[0]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		// Close writing end of second pipe   
		if (close(fd2[1]) == -1) {
			errsv = errno;
			print_error(errsv, fd_close_error_message);
		}
		exit(EXIT_SUCCESS);
	} 
} 