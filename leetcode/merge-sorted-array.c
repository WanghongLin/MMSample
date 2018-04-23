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
// Created by mutter on 4/23/18.
//

/**
 * Given two sorted integer arrays nums1 and nums2, merge nums2 into nums1 as one sorted array.
 *
 * Note:
 *
 * The number of elements initialized in nums1 and nums2 are m and n respectively.
 * You may assume that nums1 has enough space (size that is greater or equal to m + n) to hold additional elements from nums2.
 *
 * Example:
 *
 * Input:
 * nums1 = [1,2,3,0,0,0], m = 3
 * nums2 = [2,5,6],       n = 3
 *
 * Output: [1,2,2,3,5,6]
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int compare(const void* lhs, const void* rhs)
{
    int i1 = *(int*)lhs;
    int i2 = *(int*)rhs;
    return i1-i2;
}

int* create_sorted_array(int total_size, int valid_size)
{
    srand(time(NULL));
    int* a = malloc(sizeof(int)*total_size);
    for (int i = 0; i < total_size; ++i) {
        if (i >= valid_size) {
            a[i] = 0;
        } else {
            a[i] = rand() % valid_size;
        }
    }

    qsort(a, valid_size, sizeof(int), compare);

    return a;
}

void merge_sorted_array(int a1[], int m, int a2[], int n)
{
    int* aa1 = malloc(sizeof(int)*m);
    memcpy(aa1, a1, sizeof(int)*m);

    int i = 0, j = 0, k = 0;
    for (i = 0, j = 0; i < m && j < n;) {
        if (aa1[i] <= a2[j]) {
            a1[k] = aa1[i];
            i++;
        } else {
            a1[k] = a2[j];
            j++;
        }
        k++;
    }

    if (i < m-1) {
        memcpy(a1+k, aa1+i, sizeof(int)*(m-i));
    }

    if (j < n-1) {
        memcpy(a1+k, a2+j, sizeof(int)*(n-j));
    }
}

void print_array(int array[], int size)
{
    putchar('[');
    for (int i = 0; i < size; ++i) {
        printf("%d", array[i]);
        if (i == size-1) {
            printf("]\n");
        } else {
            printf(", ");
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <first-array-size> <second-array-size>\n", argv[0]);
        exit(0);
    }

    int m = (int)strtol(argv[1], NULL, 10);
    int n = (int)strtol(argv[2], NULL, 10);

    int* a1 = create_sorted_array(m+n, m);
    int* a2 = create_sorted_array(n, n);

    print_array(a1, m+n);
    print_array(a2, n);

    merge_sorted_array(a1, m, a2, n);

    printf("after merge array\n");
    print_array(a1, m+n);

    return 0;
}
