#include <stdbool.h>
#include <stdio.h>
#include "ak_print.h"
#include "ak.h"

static void print_ruler(int n)
{
	const char hex[] = "123456789abcdef";
	for(int i = 0; i < n; ++i)
		printf("%c", hex[i%15]);
	printf("\n");
}

void print_memory(void *ptr, size_t nbytes)
{
	printf("0x");
	unsigned char *u = ptr;
	for(int i = 0; i < nbytes; ++i) {
		printf("%X",u[i]);
	}
}
void print_string(void *ptr, size_t nbytes)
{
	char *s = ptr;
	for(int i = 0; i < nbytes; ++i)
		printf("%c", s[i]);
}

/*
 * Ignores child option. Use debug_mem_print for that.
 *
 * return - weather or not the child block would be
 * printed given the options in `o`
 */
bool print_block(struct block_t *b, union print_options_t *const o)
{
	if((o->free && b->free) || (o->used && !b->free)) {
		if(o->block_f) {
			printf("fblock: ");
			printf("%s ",b->free ? "free":"used");
			printf("bytes: %5.lu ",b->size);
			printf("next: %p", b->next);
			printf("\n");
		}

		if(o->block_raw) {
			printf("rblock: ");
			print_memory(b, sizeof(*b));
			printf("\n");
		}

		if(o->mem_f) {
			printf("fmem:   ");
			print_string(b->mem, b->size);
			printf("\n");
		}

		if(o->mem_raw) {
			printf("rmem:   ");
			print_memory(b->mem, b->size);
			printf("\n");
		}
	}
	if(!b->next)
		return false;

	return (o->free && b->next->free) || (o->used && !b->next->free);
}

void debug_mem_print(void *ptr, union print_options_t *const o, char *note)
{
	if(!(o->free || o->used))
		o->used = true; /* TODO: Is this a sane default? */

	printf("\nnote: %s\n", note);
	struct block_t *b =  ptr - sizeof(*b);
	while(1) {
		bool print_next = print_block(b, o);
		if(!o->child)
			break;

		b = b->next;
		if(print_next) {
			printf("\nchild:\n");
		} else {
			break;
		}
	}
	printf("done.\n");
}

