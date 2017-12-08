#ifndef MASTER_H
#define MASTER_H

#include "mail.h"
void find_paths(int sysfs_fd, struct mail_t *mail, int *n_path_found,
               long long int* total_word_count);
void send_until_success(int sysfs_fd, struct mail_t *mail, int* n_path_found,
                        long long int* total_word_count);
unsigned int receive_until_success(int sysfs_fd);


#endif
