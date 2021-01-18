/**
 * Copyright (C) 2017 Uniontech Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct _DynamicArray DynamicArray;

void
darray_sort (DynamicArray *array, int (*comp_func)(const void *, const void *));

uint32_t
darray_get_size (DynamicArray *array);

uint32_t
darray_get_num_items (DynamicArray *array);

void *
darray_get_item (DynamicArray *array, uint32_t idx);

void
darray_remove_item (DynamicArray *array, uint32_t idx);

void
darray_set_item (DynamicArray *array, void *data, uint32_t idx);

DynamicArray *
darray_new (size_t num_items);

void
darray_free (DynamicArray *array);

void
darray_clear (DynamicArray *array);
