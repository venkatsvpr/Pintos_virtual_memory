#include "page.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "hash.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"

bool add_stack(void *user_vaddr)
{
	size_t stack_addr = PHYS_BASE - pg_round_down(user_vaddr);


	if(stack_addr <= VALID_STACK_SIZE)
	{		

	struct suppl_page_table_info *spt_entry = malloc(sizeof(struct suppl_page_table_info));

	if(!spt_entry)
	{	
		return false;
	}
	spt_entry->is_page_accessed = true;
    spt_entry->uv_addr = pg_round_down(user_vaddr);
	spt_entry->is_writable = true;
	spt_entry->page_isloaded = true;
	
	uint8_t *stack_frame = FrameAlloc(PAL_USER);
	if(!stack_frame)
	{
		free(spt_entry);
		return false;

	}
	else
	{
				
			if(!install_page(spt_entry->uv_addr,stack_frame, spt_entry->is_writable))
			{
				FrameRemove(stack_frame);
				free(spt_entry);				
				return false;
			}

			if(intr_context())
				spt_entry->is_page_accessed = false;
	}


	struct hash_elem *success = hash_insert(&thread_current()->suppl_page_table, &spt_entry->hash_table_elem);
	if(success != NULL)
	{
		return false;
	}
		return true;
	}
	else
	{
		return false;
	}
}


struct suppl_page_table_info* find_spt(void *user_vaddr){
	struct suppl_page_table_info spt_entry;
	spt_entry.uv_addr = user_vaddr; 

	struct hash_iterator i;

	hash_first (&i, &thread_current()->suppl_page_table);
	while (hash_next (&i))
	{
	    struct suppl_page_table_info *f = hash_entry (hash_cur (&i), struct suppl_page_table_info, hash_table_elem);
	    if(f->uv_addr != spt_entry.uv_addr)
		{
	    	continue;
	    }
		else
		{
			return f;
		}	
	}
		
	return NULL;
}
#if 0
bool file_load(struct suppl_page_table_info *spt_entry)
{
	enum palloc_flags page_flags = PAL_USER;

	if (spt_entry->read_bytes == 0)
		page_flags = page_flags | PAL_USER;

	uint8_t *kpage = FrameAlloc(page_flags);
    if (!kpage)
        return false;

    if(spt_entry->read_bytes > 0){
    if (file_read_at(spt_entry->load_file, kpage, spt_entry->read_bytes, spt_entry->page_offset) != (int) spt_entry->read_bytes)
        {
        	FrameRemove (kpage);
        	return false; 
        }

	memset (kpage + spt_entry->read_bytes, 0, spt_entry->zero_bytes);
   }

    if (!install_page (spt_entry->uv_addr, kpage, spt_entry->is_writable)) 
    {
        FrameRemove (kpage);
        return false; 
    }
 	spt_entry->page_isloaded = true;
 	return true;
}
#endif
bool less_page (struct hash_elem *e1, struct hash_elem *e2,void *aux UNUSED){
  struct suppl_page_table_info *spt_entry1 = hash_entry(e1, struct suppl_page_table_info,hash_table_elem);
  struct suppl_page_table_info *spt_entry2 = hash_entry(e2, struct suppl_page_table_info,hash_table_elem);

  if (spt_entry1->uv_addr >= spt_entry2->uv_addr)
  {
	return false;
  }
  
  return true;
}

uint32_t hash_page (struct hash_elem *elem, void *aux UNUSED)
{
	struct suppl_page_table_info *spt_entry = hash_entry(elem, struct suppl_page_table_info, hash_table_elem);
	return hash_int((uint32_t) spt_entry->uv_addr);
}

void free_page(struct hash_elem *elem, void *aux UNUSED){
	struct suppl_page_table_info *spt_entry = hash_entry(elem, struct suppl_page_table_info,hash_table_elem);
	if(spt_entry->page_isloaded)
	{
		void *page = pagedir_get_page(thread_current()->pagedir, spt_entry->uv_addr);
		FrameRemove(page);
		pagedir_clear_page(thread_current()->pagedir, spt_entry->uv_addr);
	}
	
	free(spt_entry);
	return;
}


bool add_page (struct file *file_page, uint32_t zero_bytes, uint32_t read_bytes, 
						bool is_writable, uint32_t ofs,uint8_t *user_page, struct hash *spt){
	struct suppl_page_table_info *spt_entry = malloc(sizeof(struct suppl_page_table_info));
	if(!spt_entry)
	{ 
		 return false;
	}
	     spt_entry->is_writable = is_writable;
	     spt_entry->page_offset = ofs;
		 spt_entry->read_bytes = read_bytes;
		 spt_entry->uv_addr = user_page;
		 spt_entry->page_isloaded = false;
		 spt_entry->load_file = file_page;
		 spt_entry->is_page_accessed = false;
		 spt_entry->zero_bytes = zero_bytes;
		 
		 struct hash_elem *page_elem = hash_insert(spt, &spt_entry->hash_table_elem);
	 	 										
	 	 if(page_elem == NULL)
		 {
	 	 	return true;
	 	 }
	 	return false;
	 	 
}

