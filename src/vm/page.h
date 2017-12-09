#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "hash.h"
#include "vm/frame.h"
#include "filesys/file.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "hash.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"


#define VALID_STACK_SIZE (8 *(1 << 20))



bool add_page (struct file *file_page, uint32_t zero_bytes, uint32_t read_bytes, 
						bool is_writable, uint32_t page_offset,uint8_t *user_page, struct hash *spt);

void free_page(struct hash_elem *elem, void *aux UNUSED);						
bool add_stack(void *user_vaddr);
uint32_t hash_page (struct hash_elem *elem, void *aux UNUSED);
struct suppl_page_table_info* find_spt(void *user_vaddr);
bool less_page (struct hash_elem *e1, struct hash_elem *e2,void *aux UNUSED);

struct suppl_page_table_info{

	void *uv_addr;
	struct thread *td;
	struct hash_elem hash_table_elem;
	struct file *load_file;
	size_t page_offset;
	size_t dirty;
	size_t zero_bytes;
	size_t read_bytes;
	size_t magic;
	bool is_writable;
	bool page_isloaded;
	bool is_page_accessed;
};


#endif
