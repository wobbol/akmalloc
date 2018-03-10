//copyright katiereeves 2018
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static char all_k_mem[4096];

struct block_t {
	struct block_t *next;
	bool free;
	size_t size;
} *head = (void *)all_k_mem;

void init()
{
	head->size = (sizeof(all_k_mem) - sizeof(struct block_t));
	head->free = true;
	head->next = NULL;
}

void split(struct block_t *in, size_t nbytes) //brake a piece of all_k_mem, like the chocolate I shouldn't have eaten last night...
{
	struct block_t *b = (void*)((void*)in + nbytes + sizeof(struct block_t));
	b->size = in->size - sizeof(struct block_t) - nbytes;
	b->free = true;
	b->next = NULL;
	in->free = false;
	in->size = nbytes - sizeof(struct block_t);
	in->next = b;
}

void *mymalloc(size_t nbytes) //alocate a piece of all_k_mem, because memory leaks are not fun
{
	struct block_t *b;
	if(!head->size) init();
	b = head; //start at the start of the all_k_mem
	for(; b->size < nbytes || !b->free || b->size > nbytes; b = b->next){ //walk around the block.
		if(b->size == nbytes && b->free){ //fits perfectly!, unlike that top I bought.
			b->free = false;
			return (void*)b;
		}
		else if(b->size > nbytes && b->free){
			split(b, nbytes);
			return b;
		}
	}
	return NULL;
}

void joinFreeBlocks() //glue free blocks beside each other
{
	struct block_t *b = head;
	for(; b->next && b->next->next; b = b->next){
		if(b->free && b->next->free){
			b->size += (b->next->size)+sizeof(struct block_t);
			b->next = b->next->next;
		}
	}
}

void myfree(void *ptr)
{
	if((void*)all_k_mem <= ptr && ptr >= (void*)all_k_mem + sizeof(all_k_mem)){  //check if the pointer is in all_k_mem, something... something fishy.
		struct block_t *b = ptr;
		b->free = true;
		joinFreeBlocks();
	}
}
static void print_ruler(int n)
{
	const char hex[] = "123456789abcdef";
	for(int i = 0; i < n; ++i)
		printf("%c", hex[i%15]);
	printf("\n");
}
static void print_block(struct block_t *b)
{
	printf("%s ",b->free ? "freeee!":"notfree");
	printf("bytes: %lu ",b->size);
	printf("next: %p\n", b->next);
}

void main(void)
{
	const int n = 50;
	init();
	print_block(head);
	char *test = mymalloc(sizeof(*test) * (n + 1));
	print_block(head);
	int i = 0;
	while(i < n) {
		test[i] = 'a';
		++i;
	}
	test[i] = '\0';
	print_ruler(n);
	printf("%s\n", test);
}
