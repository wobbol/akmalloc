#ifndef ak_header_
#define ak_header_
struct block_t {
	struct block_t *next;
	bool free;
	size_t size;
	char mem[];
};

extern char all_k_mem[];

void init();
void split(struct block_t *in, size_t size);
void *mymalloc(size_t nbytes);
void joinFreeBlocks();
void myfree(void *ptr);
#endif
