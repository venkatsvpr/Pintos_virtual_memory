#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdint.h>
#include <stdbool.h>
#include "threads/thread.h"
#include <list.h>
#include "threads/synch.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"



struct frame_table_entry{

	struct thread *thread;
	struct list_elem frame_entry_elem;
	void *frame;
	struct suppl_page_table_info *spte;
};

struct list frame_list;
void *FrameAlloc (enum palloc_flags page_flags);
void FrameRemove (void *allocated_frame);

#endif
