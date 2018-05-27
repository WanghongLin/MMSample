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
// Created by mutter on 5/26/18.
//

/**
 * Given a mxn grid filled with non-negative numbers,
 * find a path from top left to bottom right which
 * minimizes the sum of all numbers along its path.
 *
 * Note: You can only move either down or right at any
 * point in time
 *
 * Example:
 * Input:
 * [
 *  [1, 3, 1],
 *  [1, 5, 1],
 *  [4, 2, 1]
 * ]
 *
 * Output: 7
 * Explanation: Because the path 1->3->1->1->1 minimizes the sum.
 */

#include <stdio.h>

static int given_input[3][3] = {
        {1, 3, 1},
        {1, 5, 1},
        {4, 2, 1}
};

static int sum = 0x7fffffff;

static void minimize_path_sum(int array[][3], int row, int column, int M, int N, int current_sum)
{
    if (row == (M-1) && column == (N-1)) {
        if (current_sum < sum) {
            sum = current_sum;
        }
        return;
    }

    // go right
    if (row <= M-2) {
        minimize_path_sum(array, row+1, column, M, N, current_sum+array[row+1][column]);
    }

    // go down
    if (column <= N-2) {
        minimize_path_sum(array, row, column+1, M, N, current_sum+array[row][column+1]);
    }
}

int main(int argc, char *argv[])
{
    minimize_path_sum(given_input, 0, 0, 3, 3, given_input[0][0]);
    printf("minimize sum %d\n", sum);
    return 0;
}

