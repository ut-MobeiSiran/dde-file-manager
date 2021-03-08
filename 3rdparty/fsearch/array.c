/**
 * Copyright (C) 2017 Uniontech Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include <string.h>
#include <sys/param.h>
#include <assert.h>
#include "array.h"

struct _DynamicArray {
    // number of items in array
    uint32_t num_items;
    // total size of array
    uint32_t max_items;
    // data
    void **data;
};

void
darray_clear (DynamicArray *array)
{
    assert (array != NULL);
    if (array->num_items > 0) {
        for (uint32_t i = 0; i < array->max_items; i++) {
            if(*(array->data) != NULL)
                array->data[i] = NULL;
            else {
                break;
            }
        }
    }
}

void
darray_free (DynamicArray *array)
{
    if (array == NULL) {
        return;
    }

    darray_clear (array);
    if (array->data) {
        free (array->data);
        array->data = NULL;
    }
    free (array);
    array = NULL;
}

DynamicArray *
darray_new (size_t num_items)
{
    DynamicArray *new = calloc (1, sizeof (DynamicArray));
    assert (new != NULL);

    new->max_items = num_items;
    new->num_items = 0;

    new->data = calloc (num_items, sizeof (void *));
    assert (new->data != NULL);

    return new;
}

static void
darray_expand (DynamicArray *array, size_t min)
{
    assert (array != NULL);
    assert (array->data != NULL);

    size_t old_max_items = array->max_items;
    size_t expand_rate = MAX (array->max_items/2, min - old_max_items);
    array->max_items += expand_rate;

    void *new_data = realloc (array->data, array->max_items * sizeof (void *));
    assert (new_data != NULL);
    array->data = new_data;
    memset (array->data + old_max_items, 0, expand_rate + 1);
}

void
darray_set_item (DynamicArray *array, void *data, uint32_t idx)
{
    assert (array != NULL);
    assert (array->data != NULL);

    if (idx >= array->max_items) {
        darray_expand (array, idx + 1);
    }

    array->data[idx] = data;
    if (data != NULL) {
        array->num_items++;
    }
}

void
darray_remove_item (DynamicArray *array, uint32_t idx)
{
    assert (array != NULL);
    assert (array->data != NULL);

    if (idx >= array->max_items) {
        return;
    }

    array->data[idx] = NULL;
    array->num_items--;
}

void *
darray_get_item (DynamicArray *array, uint32_t idx)
{
//    assert (array != NULL);
//    assert (array->data != NULL);
    if(array==NULL)
    {
        return NULL;
    }
    if(array->data==NULL)
    {
        return NULL;
    }
    if (idx >= array->max_items) {
        return NULL;
    }

    return array->data[idx];
}

uint32_t
darray_get_num_items (DynamicArray *array)
{
    assert (array != NULL);
    assert (array->data != NULL);

    return array->num_items;
}

uint32_t
darray_get_size (DynamicArray *array)
{
    assert (array != NULL);
    assert (array->data != NULL);

    return array->max_items;
}

void
darray_sort (DynamicArray *array, int (*comp_func)(const void *, const void *))
{
    assert (array != NULL);
    assert (array->data != NULL);
    assert (comp_func != NULL);

    qsort (array->data, array->num_items, sizeof (void *), comp_func);
}
