#include "mailbox.h"
#define DEBUG
#define GET_S_M(x) (!strcmp("master", x) ? M : S)

MODULE_LICENSE("Dual BSD/GPL");

module_param(num_entry_max, int, S_IRUGO);

static void get_process_name(char *ouput_name)
{
	memcpy(ouput_name, current->comm, sizeof(current->comm));
}

#ifdef DEBUG
#define ENTRY_DATA ((mail_t *)(iter->data))

typedef struct {
	union {
		char query_word[32];
		unsigned int word_count;
	} data;
	char file_path[4096];
} mail_t;

void print_List(struct list_head *head)
{
	struct list_head *lisrptr;
	struct mailbox_entry_t *iter;
	printk("print list : %lld\n", LIST_COUNT);
	list_for_each(lisrptr, head) {
		iter = list_entry(lisrptr, struct mailbox_entry_t, entry);
		printk("%s | query = %s or word_count = %d | file_path = %s\n",
		       iter->s_m == S ? "slave " : "master", ENTRY_DATA->data.query_word,
		       ENTRY_DATA->data.word_count,
		       ENTRY_DATA->file_path);
	}
}
#endif

static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf)
{
	// init
	char process_name[100];
	struct list_head *lisrptr;
	struct mailbox_entry_t *iter;
	int to_find;
	int read_success;
	read_success = 0;
	get_process_name(process_name);

	// start finding data to read 
	to_find = -GET_S_M(process_name); // negative sign convert between S and M, since S == -M
	spin_lock_bh(&my_lock);
	list_for_each(lisrptr, &HEAD) {
		iter = list_entry(lisrptr, struct mailbox_entry_t, entry);
		if(iter->s_m == to_find) { // Found !
			printk("%s Read\n", process_name);
			memcpy(buf, iter->data, 32 + strlen(iter->data + 32) +
			       1); // 32 for query, strlen for file_path, 1 for '\0'
			list_del(&iter->entry);
			kfree(iter);
			LIST_COUNT--;
			read_success = 1;
#ifdef DEBUG
			print_List(&HEAD);
#endif
			break;
		}
	}
	spin_unlock_bh(&my_lock);

	//Don't know why this failed: if (iter == NULL) // kfreed -> read successfully
	if(read_success)
		return 32 + strlen(buf+32) + 1; 
	return ERR_EMPTY;
}

static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count)
{
	// init
	char process_name[100];
	int s_m;
	struct mailbox_entry_t *data_to_write;
	get_process_name(process_name);
	s_m = GET_S_M(process_name);

	// return if full
	spin_lock_bh(&my_lock);
	if((s_m == S && LIST_COUNT == num_entry_max) || (s_m == M
	        && LIST_COUNT >= num_entry_max - 1)) {
		spin_unlock_bh(&my_lock);
		return ERR_FULL;
	}

	// writing...
	printk("%s Write\n", process_name);
	data_to_write = (struct mailbox_entry_t *)kmalloc(
	                    sizeof(struct mailbox_entry_t), GFP_KERNEL);
	data_to_write->s_m = s_m;
	memcpy(data_to_write->data, buf, count);
	list_add_tail(&(data_to_write->entry), &HEAD);
	spin_unlock_bh(&my_lock);
	LIST_COUNT++;
#ifdef DEBUG
	print_List(&HEAD);
#endif
	return 32 + strlen(buf+32) + 1;
}

static int __init mailbox_init(void)
{
	printk("Insert\n");
	hw2_kobject = kobject_create_and_add("hw2", kernel_kobj);
	sysfs_create_file(hw2_kobject, &mailbox_attribute.attr);

	INIT_LIST_HEAD(&HEAD);
	spin_lock_init(&my_lock);
	return 0;
}

static void __exit mailbox_exit(void)
{
	printk("Remove\n");
	kobject_put(hw2_kobject);
}

module_init(mailbox_init);
module_exit(mailbox_exit);
