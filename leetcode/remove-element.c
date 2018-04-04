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
 * Given an array and a value, remove all instances of that value in-place and return the new length.
 *
 * Do not allocate extra space for another array, you must do this by modifying the input array in-place with O(1) extra memory.
 *
 * The order of elements can be changed. It doesn't matter what you leave beyond the new length.
 *
 * Example:
 *
 * Given nums = [3,2,2,3], val = 3,
 *
 * Your function should return length = 2, with the first two elements of nums being 2.
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int remove_element(int array[], int n, int to_removed)
{
    int size = n;

    for (int i = 0; i < size;)  {
        if (array[i] == to_removed) {
            array[i] = array[size-1];
            size--;
        } else {
            i++;
        }
    }

    return size;
}

int* random_array(int size)
{
    int* a = malloc(sizeof(int)*size);

    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        a[i] = rand() % size;
    }

    return a;
}

void print_array(int* array, int dimension)
{
    for (int i = 0; i < dimension; i++) {
        printf("%s %d", i==0 ? "[" : ", ", array[i]);
    }
    printf("]\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <dimension-of-array>\n", argv[0]);
        exit(0);
    }

    int size = (int)strtol(argv[1], NULL, 10);
    int* array = random_array(size);
    int to_removed = rand() % size;

    printf("before remove element\n");
    print_array(array, size);

    printf("pick %d to remove\n", to_removed);
    size = remove_element(array, size, to_removed);

    printf("after remove element\n");
    print_array(array, size);

    return 0;
}
