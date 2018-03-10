//copyright katiereeves 2018
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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
	struct block_t *new = in + size;

	new->size = in->size - size;
	new->free = true;
	new->next = NULL;

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
	if( // eh, don't know if I like this style
		ptr < (void *)all_k_mem ||
		ptr > (void *)(all_k_mem + sizeof(all_k_mem))
	) //check if the pointer is in all_k_mem, something... something fishy.
		return;

	struct block_t *b = ptr;
	b->free = true;
	joinFreeBlocks();
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

static void print_all_child_block(struct block_t *b)
{
	while(b) {
		print_block(b);
		b = b->next;
	}
}

void main(void)
{
	const int n = 50;
	init();
	print_all_child_block(head);
	printf("\n");
	char *test = mymalloc(sizeof(*test) * (n + 1));
	print_all_child_block(head);
	int i = 0;
	while(i < n) {
		test[i] = 'a';
		++i;
	}
	test[i] = '\0';
	print_ruler(n);
	printf("%s\n", test);
}
