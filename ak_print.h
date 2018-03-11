#include <stdbool.h>

union print_options_t {
	int flags;
	struct {
		bool block_f;   /* block formatted  */
		bool block_raw; /* block bytes      */
		bool mem_f;     /* memory formatted */
		bool mem_raw;   /* memory bytes     */
		bool child;     /* all children     */
		bool free;      /* free blocks      */
		bool used;      /* allocated blocks */
		bool unused3;
	};
};

void print_memory(void *ptr, size_t nbytes);
void print_string(void *ptr, size_t nbytes);
bool print_block(struct block_t *b, union print_options_t *const o);
void debug_mem_print(void *ptr, union print_options_t *const o, char *note);
