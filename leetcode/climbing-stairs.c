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
// Created by mutter on 4/15/18.
//

/**
 *
 * You are climbing a stair case. It takes n steps to reach to the top.
 * Each time you can either climb 1 or 2 steps. In how many distinct ways can you climb to the top?
 *
 * Note: Given n will be a positive integer.
 *
 * Example 1:
 * Input: 2
 * Output:  2
 *
 * Explanation:  There are two ways to climb to the top.
 *
 * 1. 1 step + 1 step
 * 2. 2 steps
 *
 * Example 2:
 * Input: 3
 * Output:  3
 *
 * Explanation:  There are three ways to climb to the top.
 *
 * 1. 1 step + 1 step + 1 step
 * 2. 1 step + 2 steps
 * 3. 2 steps + 1 step
 */

#include <stdio.h>
#include <stdlib.h>

static void set_one(int* array, int size)
{
    for (int i = 0; i < size; ++i) {
        array[i] = 1;
    }
}

static int** climbing_stairs(int step)
{
    int** steps = malloc(sizeof(int*) * step);
    steps[0] = malloc(sizeof(int) * step);
    set_one(steps[0], step);

    for (int i = 1, j = 0; i < step; ++i, ++j) {

        steps[i] = malloc(sizeof(int)*step);
        set_one(steps[i], step);

        steps[i][step-1] = 0;
        steps[i][j] = 2;
    }

    return steps;
}

static void print_stairs(int** stairs, int step)
{
    for (int i = 0; i < step; ++i) {
        printf("[");
        for (int j = 0; j < step; ++j) {
            if (stairs[i][j] != 0) {
                printf("%d", stairs[i][j]);
            }
            if (j == step-1) {
                printf("]\n");
            } else {
                printf(", ");
            }
        }
    }

    putchar('\n');
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number-of-stairs>\n", argv[0]);
        exit(0);
    }

    print_stairs(climbing_stairs((int)strtol(argv[1], NULL, 10)), (int)strtol(argv[1], NULL, 10));
    return 0;
}