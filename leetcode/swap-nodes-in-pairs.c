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
// Created by mutter on 4/3/18.
//

/**
 * Given a linked list, swap every two adjacent nodes and return its head.
 *
 * For example,
 * Given 1->2->3->4, you should return the list as 2->1->4->3.
 *
 * Your algorithm should use only constant space.
 * You may not modify the values in the list, only nodes itself can be changed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

Node* new_node(int value)
{
    Node* n = malloc(sizeof(Node));
    n->value = value;
    n->next = NULL;
    return n;
}

Node* new_random_linked_list(int size)
{
    srand(time(NULL));

    Node* head = NULL;
    Node* tail = NULL;
    for (int i = 0; i < size; ++i) {
        Node* node = new_node(rand() % size);
        if (head == NULL) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }

    return head;
}

void print_linked_list(Node* list)
{
    Node* i = list;
    while (i) {
        printf("%d", i->value);
        if (i->next) {
            printf(" -> ");
        }
        i = i->next;
    }
    putchar('\n');
}

void swap(Node* lhs, Node* rhs)
{
    int t = lhs->value;
    lhs->value = rhs->value;
    rhs->value = t;
}

Node* swap_nodes_in_pairs(Node* list)
{
    if (list->next == NULL) {
        return list;
    }

    Node* l = list;
    while (l && l->next) {
        swap(l, l->next);
        l = l->next->next;
    }

    return list;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <size-of-linked-list>\n", argv[0]);
        exit(0);
    }

    const int size = (int)strtol(argv[1], NULL, 10);
    Node* list = new_random_linked_list(size);
    printf("before swap\n");
    print_linked_list(list);

    swap_nodes_in_pairs(list);
    printf("after swap\n");
    print_linked_list(list);

    return 0;
}
