#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

int maxInt(int a, int b) {
	return a < b ? b : a;
}

void freeList(List *l) {
	if (l->data == NULL) return;
	free(l->data);
	l->data = NULL;
	l->list_capacity = 0;
	l->list_length = 0;
}

void expand(List *l) {
	void* new_area;
	l->list_length++;

	if (l->list_length <= l->list_capacity) return;

	new_area = malloc(l->datum_size * maxInt(l->list_capacity, 1) * 2);
	memcpy(new_area, l->data, l->datum_size * l->list_capacity);
	free(l->data);
	l->data = new_area;
	l->list_capacity = maxInt(l->list_capacity, 1) * 2;
}

void moveout(List *src, List *dest) {
	assert(dest->datum_size == src->datum_size);
	freeList(dest);
	dest->list_length = src->list_length;
	dest->list_capacity = src->list_capacity;
	dest->data = src->data;
	src->list_length = 0;
	src->list_capacity = 0;
	src->data = NULL;
}
