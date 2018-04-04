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
 *
 * Given an array of non-negative integers, you are initially positioned at the first index of the array.
 *
 * Each element in the array represents your maximum jump length at that position.
 *
 * Your goal is to reach the last index in the minimum number of jumps.
 *
 * For example:
 * Given array A = [2,3,1,1,4]
 * The minimum number of jumps to reach the last index is 2. (Jump 1 step from index 0 to 1, then 3 steps to the last index.)
 *
 * Note:
 * You can assume that you can always reach the last index.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct Index {
    int index;
    struct Index* next;
} Index;

Index* new_index_node(int index)
{
    Index* node = malloc(sizeof(Index));
    node->next = NULL;
    node->index = index;
    return node;
}

Index* find_jump_list(int array[], int reach_at)
{
    if (reach_at == 1) {
        return new_index_node(0);
    }

    int distance = 1;
    int reach_at_smaller = reach_at-1;
    for (int i = reach_at-1; i >= 0; i--) {
        if (array[i] >= distance) {
            reach_at_smaller = i;
        }
        distance++;
    }

    Index* smaller_jump_list = find_jump_list(array, reach_at_smaller);

    // append to end of list
    Index* end_of_list = smaller_jump_list;
    while (end_of_list->next) {
        end_of_list = end_of_list->next;
    }
    end_of_list->next = new_index_node(reach_at_smaller);

    return smaller_jump_list;
}

void print_index_list(Index* list)
{
    Index* l = list;
    while (l) {
        printf("%d", l->index);
        if (l->next) {
            printf(" -> ");
        }
        l = l->next;
    }
    putchar('\n');
}

int main(int argc, char *argv[])
{
    int array[] = { 2, 3, 1, 1, 4 };
    Index* list = find_jump_list(array, sizeof(array)/sizeof(array[0])-1);
    print_index_list(list);
    return 0;
}
