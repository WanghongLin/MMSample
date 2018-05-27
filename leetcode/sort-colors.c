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
// Created by mutter on 5/5/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int* random_colors(int size)
{
    srand(time(NULL));

    int* colors = malloc(sizeof(int)*size);

    for (int i = 0; i < size; ++i) {
        colors[i] = rand() % 3;
    }

    return colors;
}

void sort_colors_deprecated(int colors[], int size)
{
    int numbers[3] = { 0 };

    for (int i = 0; i < size; ++i) {
        numbers[colors[i]]++;
    }

    int k = 0;
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < numbers[j]; ++i) {
            colors[k++] = j;
        }
    }
}

void sort_colors(int colors[], int size)
{
    int r = -1, g = -1, b = -1;

    for (int i = 0; i < size; ++i) {
        if (colors[i] == 0) {
            colors[++b] = 2;
            colors[++g] = 1;
            colors[++r] = 0;
        } else if (colors[i] == 1) {
            colors[++b] = 2;
            colors[++g] = 1;
        } else {
            colors[++b] = 2;
        }
    }
}

void print_colors(int colors[], int size)
{
    printf("[");
    for (int i = 0; i < size; ++i) {
        printf("%d", colors[i]);
        if (i == size-1) {
            printf("]\n");
        } else {
            printf(", ");
        }
    }
    putchar('\n');
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number-of-colors>\n", argv[0]);
        exit(0);
    }

    int size = (int)strtol(argv[1], NULL, 10);
    int* colors = random_colors(size);
    sort_colors(colors, size);
    print_colors(colors, size);

    return 0;
}
