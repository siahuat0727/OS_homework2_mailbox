#ifndef MAILBOX_H
#define MAILBOX_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/spinlock_types.h>
#include "def.h"

#define M -1
#define S 1

#define HEAD (HEAD__.head)
#define LIST_COUNT (HEAD__.count)

struct mailbox_head_t {
	long long int count;
	struct list_head head;
};

struct mailbox_entry_t {
	int s_m;
	char data[4128]; // 32 + 4096
	struct list_head entry;
};

struct mailbox_head_t HEAD__;

static void get_process_name(char *ouput_name);
static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf);
static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobject *hw2_kobject;
static struct kobj_attribute mailbox_attribute
    = __ATTR(mailbox, 0660, mailbox_read, mailbox_write);

static int num_entry_max = 2;
spinlock_t my_lock;

#endif
