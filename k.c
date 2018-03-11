//copyright katiereeves 2018
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ak_print.h"

static char all_k_mem[4096];

struct block_t {
	struct block_t *next;
	bool free;
	size_t size;
	char mem[];
} *head = (void *)all_k_mem;

void init()
{
	head->size = (sizeof(all_k_mem) - sizeof(struct block_t));
	head->free = true;
	head->next = NULL;
}

/* size should be sizeof(*in) + bytes to allocate */
void split(struct block_t *in, size_t size) //brake a piece of all_k_mem, like the chocolate I shouldn't have eaten last night...
{
	struct block_t *new = (void *)in + size;

	new->size = in->size - size;
	new->free = true;
	new->next = in->next;

	in->free = false;
	in->size = size - sizeof(*in);
	in->next = new;
}

/* init must be run before first use of this! */
void *mymalloc(size_t nbytes) //alocate a piece of all_k_mem, because memory leaks are not fun
{
	struct block_t *b = head; //start at the start of the all_k_mem
	size_t split_size = nbytes + sizeof(*b);

	while(b) {
		while(b->free == false) {
			b = b->next;
			if(!b)
				return NULL;
		}
		if(b->size > split_size) {
			//gotta split
			split(b, split_size);
			return b->mem;
		} else if(b->size >= nbytes) {
			// Fits perfectly!, unlike that top I bought or I always need a 28" band, but sometimes settle for 30".
			b->free = false;
			return b->mem;
		}
		b = b->next; // This free block wasn't big enough.
	}
	return NULL; // TODO: Something like errno could be set here so the calling code can try to fix it.
}

void joinFreeBlocks() //glue free blocks beside each other
{
	struct block_t *b = head;
	for(; b->next && b->next->next; b = b->next){
		if(b->free && b->next->free){
			b->size += b->next->size + sizeof(*b);
			b->next = b->next->next;
		}
	}
}

void myfree(void *ptr)
{
	struct block_t *b = ptr - sizeof(struct block_t);
	if( // eh, don't know if I like this style
		(void *)b < (void *)all_k_mem ||
		(void *)b > (void *)(all_k_mem + sizeof(all_k_mem))
	) //check if the pointer is in all_k_mem, something... something fishy.
		return;

	b->free = true;
	joinFreeBlocks();
}

char *test(char ch, size_t len)
{
	char *test = mymalloc(sizeof(*test) * (len + 1));
	//print_all_child_block(head);
	int i = 0;
	while(i < len) {
		test[i] = ch;
		++i;
	}
	test[i] = '\0';
	//print_what(test, '\0', NULL);
	//print_ruler(len);
	//printf("%s\n", test);
	return test;
}


void main(void)
{
	const int t_a_bound = 50;
	init();
	union print_options_t option = { 
		.block_f = 1,
		.block_raw = 1,
		.mem_f = 1,
		.mem_raw = 1,
		.child = 1,
	};
	//print_all_child_block(head);
	char *a = test('a', 50);// print_what(a, "after a", &option);
	char *b = test('b', 53);// print_what(a,"after b",&option);
	//myfree(a); print_what(a,"after free(a)", &option);
	char *c = test('c', 20); debug_mem_print(a, &option, "after c");
	//char *d = test('d', 32);
	//print_what(NULL,"after d");
	//myfree(b);
	//print_what(NULL,"after free(b)");
	//char *e = test('e', 32);
	//print_what(NULL,"after e");
	//char *f = test('f', 32);
	//print_what(NULL,"after f");
	//print_memory(head,150);

}
