#ifndef LIST_H
#define LIST_H
#include <stddef.h>

#define APPEND( TYPE, VAR, ELEM ) \
	do { \
		expand(&(VAR)); \
		((TYPE*) (VAR).data) [(VAR).list_length-1] = (ELEM); \
	} while (0);

typedef struct {
	size_t datum_size, list_length, list_capacity;
	void* data;
} List;

void expand(List *l);
void freeList(List *l);
void moveout(List *src, List *dest);
#endif
