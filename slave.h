#ifndef SLAVE_H
#define SLAVE_H

#include "mail.h"

void lower(char* str);
void count_query_word(struct mail_t* path);
void send_until_success(int sysfs_fd, struct mail_t *mail);
void receive_until_success(int sysfs_fd, struct mail_t *mail);
#endif
