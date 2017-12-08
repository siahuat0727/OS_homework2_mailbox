#include "slave.h"
//#define DEBUG
//#define DELAY

int main(int argc, char **argv)
{
#ifdef DEBUG
	puts("I am slave");
#endif

	// open mailbox
	int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR);
	if(sysfs_fd < 0) 
		error("slave open");

	// start until receive kill signal
	struct mail_t mail;
	while(1) {
		receive_until_success(sysfs_fd, &mail);
		count_query_word(&mail);
		send_until_success(sysfs_fd, &mail);
	}

	
	// will not be executed
	close(sysfs_fd);
#ifdef DEBUG
	puts("slave terminate");
#endif
	return 0;
}

void lower(char* str)
{
	int len = strlen(str);
	for(int i = 0; i < len; ++i)
		str[i] |= 1 << 5;
}

void count_query_word(struct mail_t * mail)
{
	// init
	char query_word[32];
	strcpy(query_word, mail->data.query_word);
	lower(query_word);
	mail->data.word_count = 0;

	// open file
	FILE* fp = fopen(mail->file_path, "r");
	if(fp == NULL)
		error("File open failed");

	// start counting
	char str[4096];
	while(fscanf(fp, "%s", str) != EOF) {
		lower(str);
		if(!strcmp(query_word, str))
			mail->data.word_count++;
	}

	// close file
	fclose(fp);
}


int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	lseek(sysfs_fd, 0, SEEK_SET);
	return write(sysfs_fd, (char*)mail, sizeof(struct mail_t));
}

int receive_from_fd(int sysfs_fd,
                    struct mail_t *mail)
{
	lseek(sysfs_fd, 0, SEEK_SET);
	return read(sysfs_fd, (char *)mail, sizeof(struct mail_t));
}

void send_until_success(int sysfs_fd, struct mail_t* mail)
{
	int ret_val;
	while((ret_val=send_to_fd(sysfs_fd, mail)) == ERR_FULL);
#ifdef DEBUG
	printf("Slave write successfully! : word_count = %d | path = %s | return value = %d\n",
	       mail->data.word_count, mail->file_path, ret_val);
#endif
}

void receive_until_success(int sysfs_fd, struct mail_t* mail)
{
	int ret_val;
	while((ret_val=receive_from_fd(sysfs_fd, mail)) == ERR_EMPTY);
#ifdef DEBUG
	printf("Slave read successfully! : query = %s | path = %s | return value = %d\n",
	       mail->data.query_word, mail->file_path, ret_val);
#endif
}
