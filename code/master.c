#include "master.h"
//#define DEBUG
#define CHECK_DUPLICATE(x) if(strlen(x) != 0) error("Duplicate input parameter")

int main(int argc, char **argv)
{
#ifdef DEBUG
	puts("I am master");
#endif
        // check number of parameter
	if(argc != 5 && argc != 7)
		error("Invalid parameter numbers");

        // init
	struct mail_t mail;
	int n_slave = 1;

        // get input
	for (int i = 1; i < argc; i += 2) {
		if (!strcmp("-q", argv[i])) {
			CHECK_DUPLICATE(mail.data.query_word);
			strcpy(mail.data.query_word, argv[i+1]);
		} else if (!strcmp("-d", argv[i])) {
			CHECK_DUPLICATE(mail.file_path);
			strcpy(mail.file_path, argv[i+1]);
		} else if (!strcmp("-s", argv[i])) {
			n_slave = atoi(argv[i+1]);
		} else
			error("Invalid parameter");
	}

        //return if input is invalid
	if(!strlen(mail.data.query_word) || !strlen(mail.file_path) || n_slave < 1)
		error("Invalid parameter");

	// master fork n_slave times
	int pid;
	while(n_slave-- && (pid=fork()) > 0); 

	// if slave
	if(pid == 0) {
		char* execv_str[] = {NULL};
		execv("slave", execv_str);
		perror("execv "); // will be exec iff execv failed
		exit(0);
	}

        // open mailbox
	int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR);
	if(sysfs_fd < 0)
		error("open");

        // start finding paths
	int n_path_found = 0;
	long long int total_word_count = 0;
	find_paths(sysfs_fd, &mail, &n_path_found, &total_word_count);

        // receive remainding mail
	while(n_path_found--)
		total_word_count += (long long int)receive_until_success(sysfs_fd);

	// close mailbox
	close(sysfs_fd);

	// print result
	printf("The total number of query word \"%s\" is %lld\n", mail.data.query_word,
	       total_word_count);

	// kill when done
	kill(0, 2);
	
	// will not be executed
#ifdef DEBUG
	puts("master terminate");
#endif
	return 0;
}

void find_paths(int sysfs_fd, struct mail_t *mail, int *n_path_found,
               long long int *total_word_count)
{
	// reset errno value
	errno = 0;

	DIR* dir = opendir(mail->file_path);
	if(errno == ENOENT)
		printf("%s does not exist\n", mail->file_path);
	else if(errno == ENOTDIR)
		send_until_success(sysfs_fd, mail, n_path_found, total_word_count);
	else {
		struct dirent *dp;
		char path[4096];
		strcpy(path, mail->file_path);
		while((dp = readdir(dir)) != NULL) {
			if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
				continue;
			strcpy(mail->file_path, path);
			strcat(mail->file_path, "/");
			strcat(mail->file_path, dp->d_name);
			find_paths(sysfs_fd, mail, n_path_found, total_word_count);
		}
		closedir(dir);
	}
}

int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	lseek(sysfs_fd, 0, SEEK_SET);
	return write(sysfs_fd, (char*)mail, sizeof(struct mail_t));
}

int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{
	lseek(sysfs_fd, 0, SEEK_SET);
	return read(sysfs_fd, (char *)mail, sizeof(struct mail_t));
}

void send_until_success(int sysfs_fd, struct mail_t *mail, int* n_path_found,
                        long long int* total_word_count)
{
	int ret_val;
	while((ret_val=send_to_fd(sysfs_fd, mail)) == ERR_FULL) {
		*total_word_count += (long long int)receive_until_success(sysfs_fd);
		(*n_path_found)--;
	}
	(*n_path_found)++;
#ifdef DEBUG
	printf("Master write successfully! : query = %s | path = %s | return value = %d\n",
	       mail->data.query_word, mail->file_path, ret_val);
#endif
}

unsigned int receive_until_success(int sysfs_fd)
{
	struct mail_t mail;
	int ret_val;
	while((ret_val=receive_from_fd(sysfs_fd, &mail)) == ERR_EMPTY);
#ifdef DEBUG
	printf("Master read successfully! : word_count = %d | path = %s | return value = %d\n",
	       mail.data.word_count, mail.file_path, ret_val);
#endif
	return mail.data.word_count;
}
