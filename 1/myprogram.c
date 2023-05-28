#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>


void throw_error(char *message)
{
	fprintf(stderr, message);
	fprintf(stderr, "\n");
	printf("errno=%s\n", strerror(errno));
	exit(1);
}

void process_file(char *input_name, char *output_name, int block_size)
{
	int input_file = 0;
	if (input_name != NULL)
	{
		input_file = open(input_name, O_RDONLY);
	}

	if (input_file == -1)
		throw_error("Open input file error");
		
	int output_file = open(output_name, O_WRONLY | O_CREAT, 0777);
	if (output_file == -1)
		{
			if (close(input_file) == -1)
				throw_error("Open output file error\nClose input file error");
			throw_error("Open output file error");
		}
	
	char *block = (char *) malloc(block_size);
	
	while(true)
	{
		ssize_t bytes_count = read(input_file, block, block_size);
		if (bytes_count == -1)
			throw_error("Read file error");
		if (bytes_count == 0)
			break;
			
		bool block_is_not_empty = false;
		char *temp_block = block;
		for(int i = 0; i < bytes_count; i++)
			block_is_not_empty = block_is_not_empty || *(temp_block++);
			
		if (block_is_not_empty)
		{
			if(write(output_file, block, bytes_count) != bytes_count)
				throw_error("Write file error");
		}
		else
		{
			if(lseek(output_file, bytes_count, SEEK_CUR) == -1)
				throw_error("Lseek error");
		}
	}
	
	if (close(input_file) == -1 || close(output_file) == -1)
		throw_error("Close file error");
}

void main(int argc, char *argv[])
{
	int block_size = 4096;
	int next_option;
	char *input_filename = NULL;
	char *output_filename = NULL;
	
	do
	{
		next_option = getopt(argc, argv, "b:");
		switch(next_option)
		{
			case 'b':
				block_size = atoi(optarg);
				break;
			default:
				break;
		}
	}
	while (next_option != -1);
	
	char *temp_filename = NULL;
	while (optind < argc) 
	{
		char *filename = (char *) malloc(strlen(argv[optind]) + 1);
		strcpy(filename, argv[optind]);
		optind++;
			
		if (temp_filename == NULL)
		{
			temp_filename = filename;
			continue;
		}
		
		
		if (input_filename == NULL)
		{
			input_filename = temp_filename;
			output_filename = filename;
			continue;
		}
		
		throw_error("Unexpected argument");
	}
		
	
	if (temp_filename == NULL)
	{
		throw_error("No output filename");
	}
	
	if (output_filename == NULL)
	{
		output_filename = temp_filename;
	}	
	
	process_file(input_filename, output_filename, block_size);
}
