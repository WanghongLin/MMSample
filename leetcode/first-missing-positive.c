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
// Created by mutter on 4/4/18.
//

/**
 * Given an unsorted integer array, find the first missing positive integer.
 *
 * For example
 * Given [1,2,0] return 3,
 * and [3,4,-1,1] return 2.
 *
 * Your algorithm should run in O(n) time and uses constant space.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int* new_random_array(int size)
{
    int* a = malloc(sizeof(int) * size);

    srand((unsigned int) time(NULL));
    for (int i = 0; i < size; i++) {
        a[i] = rand() % size;
        if (rand() % 2) {
            a[i] = -a[i];
        }
    }

    return a;
}

static void print_array(int array[], int size)
{
    for (int i = 0; i < size; i++) {
        printf("%s%d", i==0 ? "[" : ", ", array[i]);
    }
    printf("]\n");
}

static int first_missing_positive(const int array[], int n)
{
    int result = 0x7fffffff;

    for (int i = 0; i < n; i++) {
        int v = array[i]+1;
        if (v > 0) {
            if (v < result) {
                result = v;
            }
        } else {
            result = 1;
        }
    }

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <size-of-array>\n", argv[0]);
        exit(0);
    }

    const int size = strtol(argv[1], NULL, 10);
    int* array = new_random_array(size);
    print_array(array, size);

    printf("first missing positive %d\n", first_missing_positive(array, size));
    return 0;
}
