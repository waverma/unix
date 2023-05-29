#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>


int lock_count;
char *locked_filename;


void throw_error(char *message)
{
	fprintf(stderr, message);
	fprintf(stderr, "\n");
	printf("errno=%s\n", strerror(errno));
	exit(1);
}

void lock_file(char *lock_filename)
{
	char pid_value[1024];
	sprintf(pid_value, "%d", getpid());
	int fd = -1;
	while (fd == -1)
		fd = open(lock_filename, O_CREAT | O_EXCL | O_RDWR, 0777);
		
	write(fd, pid_value, strlen(pid_value));
	close(fd);
	lock_count++;
}

void unlock_file(char *lock_filename)
{
	char *stringify_pid = (char *) malloc(1024);
	int fd = open(lock_filename, O_RDONLY);
	int size = read(fd, stringify_pid, 1024);
	if (fd == -1 || read(fd, stringify_pid, 1024) || atoi(stringify_pid) != getpid() || close(fd) == -1 || remove(lock_filename) == -1)
		throw_error("Unlock fail");
}

void write_with_lock(char *filename)
{
	char *lock_filename = strcat(filename, ".lck");
	lock_file(lock_filename);
	sleep(1);
	unlock_file(lock_filename);
}

void loop(char *filename)
{
	char *lock_filename = strcat(filename, ".lck");
	locked_filename = malloc(strlen(lock_filename) + 1);
	strcpy(locked_filename, filename);
	while (true) 
	{
		lock_file(lock_filename);
		sleep(1);
		unlock_file(lock_filename);
	}
}

void sigint_handler(int sig)
{
	int fd = open("result.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
	lseek(fd, 0, SEEK_END);
	char result_line[100];
	snprintf(result_line, 100, "pid: %d, lock_count: %d\n", getpid(), lock_count);
	write(fd, result_line, strlen(result_line));
	close(fd);
	remove(locked_filename);	
	exit(0);
}

void main(int argc, char *argv[])
{
	signal(SIGINT, sigint_handler);
	
	int next_option;
	bool start_loop = false;
	do
	{
		next_option = getopt(argc, argv, "l");
		switch(next_option)
		{
			case 'l':
				start_loop = true;
				break;
		}
	}
	while (next_option != -1);
	
	char *filename = NULL; 
	if (optind < argc)
	{
		char *rfilename = (char *) malloc(strlen(argv[optind]) + 1);
		strcpy(rfilename, argv[optind]);
		filename = rfilename;
	}
	
	
	if (filename == NULL)
	{
		throw_error("No filename");
	}
	
	if (start_loop)
	{
		loop(filename);
	}
	else
	{
		write_with_lock(filename);
	}	
}
