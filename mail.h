#ifndef MAIL_H
#define MAIL_H

#include "module/def.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>


struct mail_t {
	union {
		char query_word[32];
		unsigned int word_count;
	} data;
	char file_path[4096];
};

int send_to_fd(int sysfs_fd, struct mail_t *mail);
int receive_from_fd(int sysfs_fd, struct mail_t *mail);

void error(const char *msg)
{
	perror(msg);
	exit(1);
}
#endif
