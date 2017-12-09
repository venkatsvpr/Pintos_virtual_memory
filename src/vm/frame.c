#include "vm/frame.h"
void *FrameAlloc (enum palloc_flags page_flags) 
{
	if ((page_flags & PAL_USER) == 0)
	{
		return NULL;
	}
	
	void *frame = palloc_get_page(page_flags);
	if(!frame)
	{		 
	 	PANIC("Ran out of frames");	 
		return frame;
	}
 
	struct frame_table_entry *ft_entry= malloc(sizeof(struct frame_table_entry));
	ft_entry->frame = frame;
	ft_entry->thread = thread_current();
	list_push_back(&frame_list, &ft_entry->frame_entry_elem);

	return frame;
}

void FrameRemove (void *allocated_frame)
{
	struct list_elem *elem;
	for(elem = list_begin(&frame_list); elem != list_end(&frame_list); elem = list_next(elem))
	{
		struct frame_table_entry *ft_entry = list_entry(elem, struct frame_table_entry, frame_entry_elem);
		
		if (ft_entry->frame != allocated_frame)
		continue;
	
		list_remove(elem);
		free(ft_entry);
		palloc_free_page(allocated_frame);
	}
}

