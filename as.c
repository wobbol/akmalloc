#include <stdio.h>
#include <string.h>


struct free_list_t {
	void *p;
	int next :1;
	int free :1;
} *free_list;

struct {
	size_t count; /* number of pages in system memory */
	size_t size;  /* size in bytes of a page */
	void *start;  /* where the first page of memory starts */
} page_data;

void init_malloc(size_t sz, size_t count, void *start)
{
	page_data.size = sz;
	page_data.count = count;
	page_data.start = start;
	free_data.list = kmalloc(sizeof(*free_list) * count);
	for(int i = 0; i < count; ++i) {
		free_list[i].p = start + (i * sz); /* function p(i) */
		free_list[i].free = 1;
		free_list[i].next = 0;
	}
}

void free(void *p)
{
	/* function p^(-1)(i) */
	struct free_list_t *f = free_list[(p - page_start) / page_size];

	if(f->p != p) {
		printf("invalid pointer");
		return;
	}
	f->free = 1;
	while(f->next) {
		f->next = 0;
		f->free = 1;
		++f;
	}
}

void *malloc(size_t bytes)
{
	if(!bytes)
		return NULL;
	size_t pages = 0;
	while((++pages * page_data.size) <= bytes)
		;
	struct free_list_t *f = free_list;
	void *ret = NULL;
	for(int i = 0; i < page_data.count; ++i, ++f) {
		int j;
		for(j = 0; j < pages && f[j]->free && ((j + i) < page_data.count); ++j)
			; /* j pages are free and memory bounds haven't been over-run */
		if(j == pages) {
			ret = f->p;
			break;
		}
	}
	if(!ret)
		return ret;
	f->free = 0;
	for(int i = 1; i < pages; ++i) {
		f->next = 1;
		++f;
		f->free = 0;
	}
	return ret;
	}
}

void main(void)
{
	init_malloc(8, 3, 2);
	printf("%d %d %d %p", page_size, page_count, page_start);
}
