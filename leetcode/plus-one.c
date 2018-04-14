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
// Created by mutter on 4/13/18.
//

/**
 * Given a non-negative integer represented as a non-empty array of digits, plus one to the integer.
 * You may assume the integer do not contain any leading zero, except the number 0 itself.
 * The digits are stored such that the most significant digit is at the head of the list.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_array(int array[], int size)
{
    printf("[");
    for (int i = 0; i < size; ++i) {
        printf("%d", array[i]);
        if (i == size-1) {
            printf("]");
        } else {
            printf(", ");
        }
    }
    putchar('\n');
}

static int* plus_one(int array[], int size, int* new_size)
{
    int carry = 0;
    for (int i = size-1; i >= 0; --i) {
        if (i == size-1) {
            array[i] += 1;
        }

        array[i] += carry;
        if (array[i] >= 10) {
            carry = 1;
            array[i] -= 10;
        } else {
            carry = 0;
        }
    }

    if (carry == 1) {
        int* a = malloc(sizeof(int)*(size+1));
        a[0] = 1;
        memcpy(a+1, array, sizeof(int)*size);
        if (new_size) {
            *new_size = size+1;
        }
        return a;
    }

    if (new_size) {
        *new_size = size;
    }

    return array;
}

static int* arrayfy(long number, int* size)
{
    int w = 0;
    long n = number;
    while (n > 0) {
        w++;
        n /= 10;
    }

    if (size) {
        *size = w;
    }

    int* array = malloc(sizeof(int)*w);
    n = number;
    for (int i = w-1; i >= 0; --i) {
        array[i] = (int)n % 10;
        n /= 10;
    }

    return array;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <non-negative-integer>\n", argv[0]);
        exit(0);
    }

    long number = strtol(argv[1], NULL, 10);
    printf("Plus one with built in arithmetic operation %ld\n", number+1);

    int size = 0;
    int* array = arrayfy(number, &size);
    printf("Arrayfy\n");
    print_array(array, size);

    printf("Plus one with array operation\n");
    int ns = 0;
    int* na = plus_one(array, size, &ns);
    print_array(na, ns);

    free(array);
    return 0;
}
