//
//  Copyright (c) 2018 mutter
//
//  This file is part of MMSample.
//
//  MMSample is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MMSample is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MMSample.  If not, see <http://www.gnu.org/licenses/>.
//  
//
// Created by mutter on 4/12/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct sub_array {
    int start_index;
    int end_index;
    int max;
} sub_array;

static int sum(const int array[], int start, int end)
{
    int sum = 0;
    for (int i = start; i <= end; ++i) {
        sum += array[i];
    }
    return sum;
}

static sub_array maximum_sub_array(int array[], int n)
{
    sub_array sa;
    sa.max = 0;

    for (int i = 0; i < n-1; ++i) {
        for (int j = i+1; j < n; ++j) {
            int s = sum(array, i, j);
            if (s > sa.max) {
                sa.max = s;
                sa.start_index = i;
                sa.end_index = j;
            }
        }
    }

    return sa;
}

static int* new_random_array(int size)
{
    int* a = malloc(sizeof(int)*size);

    srand(time(NULL));
    for (int i = 0; i < size; ++i) {
        a[i] = rand() % size;

        if (rand() % 2) {
            a[i] = -a[i];
        }
    }

    return a;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <size-of-array>\n", argv[0]);
        exit(0);
    }

    int size = (int)strtol(argv[1], NULL, 10);
    int* array = new_random_array(size);

    sub_array sa = maximum_sub_array(array, size);

    printf("maximum sub array %d\n", sa.max);

    printf("[");
    for (int i = sa.start_index; i <= sa.end_index; ++i) {
        printf("%d", array[i]);
        if (i == sa.end_index) {
            printf("]\n");
        } else {
            printf(", ");
        }
    }

    free(array);
    return 0;
}
