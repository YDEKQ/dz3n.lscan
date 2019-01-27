/* -------------------------------------------------------------------------- */

#ifndef __range_h__
#define __range_h__

/* -------------------------------------------------------------------------- */

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

/* -------------------------------------------------------------------------- */

#define RANGE_ADD		0
#define RANGE_REMOVE	1
#define RANGE_SET		2

/* -------------------------------------------------------------------------- */

typedef struct __range_elem_t
{
	unsigned int begin;
	unsigned int end;
	struct __range_elem_t *flink;
} range_elem_t;

/* -------------------------------------------------------------------------- */

typedef int (*range_callback_t)(void *parameter, range_elem_t *item, int mode);

/* -------------------------------------------------------------------------- */

typedef struct __range_t
{
	unsigned int size;
	struct __range_elem_t *items;
	range_callback_t cb;
	void *cb_param;
	HANDLE mutex;
	int locked;
} range_t;

/* -------------------------------------------------------------------------- */

range_t *range_new(range_callback_t cb, void *cb_param);
void range_free(range_t *range);

int range_lock(range_t *range);
int range_unlock(range_t *range);

int range_clear(range_t *range);
int range_merge(range_t *dest, range_t *source);

int range_add_single(range_t *range, unsigned int value);
int range_remove_single(range_t *range, unsigned int value);
unsigned int range_check_single(range_t *range, unsigned int value);

int range_add(range_t *range, unsigned int begin, unsigned int end);
int range_remove(range_t *range, unsigned int begin, unsigned int end);
unsigned int range_check(range_t *range, unsigned int begin, unsigned int end);

unsigned int range_get(range_t *range, unsigned int index, int remove);
unsigned int range_get_first(range_t *range, int remove);
unsigned int range_get_random(range_t *range, int remove);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
