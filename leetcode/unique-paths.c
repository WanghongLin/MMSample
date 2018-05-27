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
// Created by mutter on 5/13/18.
//

/**
 * A robot is located at the top-left corner of a m x n grid (marked 'Start' in the diagram below).
 *
 * The robot can only move either down or right at any point in time. The robot is trying to reach the bottom-right corner of the grid (marked 'Finish' in the diagram below).
 *
 * How many possible unique paths are there?
 *
 * Note: m and n will be at most 100.
 *
 * Example 1:
 *
 * Input: m = 3, n = 2
 * Output: 3
 *
 * Explanation:
 * From the top-left corner, there are a total of 3 ways to reach the bottom-right corner:
 *
 * 1. Right -> Right -> Down
 * 2. Right -> Down -> Right
 * 3. Down -> Right -> Right
 *
 * Example 2:
 *
 * Input: m = 7, n = 3
 * Output: 28
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_paths(const char* paths)
{
    for (int i = 0; ; i++) {
        if (paths[i] == 'r') {
            printf("%s", "right");
		} else if (paths[i] == 'd') {
			printf("%s", "down");
		}

		if (paths[i+1] == '\0') {
			putchar('\n');
			break;
		} else {
			printf(" -> ");
		}
    }
}

static void find_unique_paths(const char* existed_paths, int rows, int columns,
                              const int total_rows, const int total_columns, int* total_paths)
{
    if (rows == (total_rows-1) && columns == (total_columns-1)) {
        print_paths(existed_paths);
        if (total_paths != NULL) {
			(*total_paths)++;
		}
        return;
    }

	size_t existed_size = strlen(existed_paths);
    // go to right
	if (columns+1 <= total_columns-1) {
		char* right = calloc(existed_size+1+1, sizeof(char));
		strcpy(right, existed_paths);
		right[existed_size] = 'r';
		find_unique_paths(right, rows, columns+1, total_rows, total_columns, total_paths);
	}

    // go to down
	if (rows+1 <= total_rows-1) {
		char* down = calloc(existed_size+1+1, sizeof(char));
		strcpy(down, existed_paths);
		down[existed_size] = 'd';
		find_unique_paths(down, rows+1, columns, total_rows, total_columns, total_paths);
	}

	if (strlen(existed_paths) > 0) {
		free(existed_paths);
	}
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <rows> <columns>\n", argv[0]);
        exit(0);
    }

    int rows = (int)strtol(argv[1], NULL, 10);
    int columns = (int)strtol(argv[2], NULL, 10);

	if (rows > 100 || columns > 100) {
		fprintf(stderr, "rows and columns will be at most 100\n");
		exit(0);
	}

	int total_paths = 0;
    find_unique_paths("", 0, 0, rows, columns, &total_paths);
	printf("total path number is %d\n", total_paths);
    return 0;
}
