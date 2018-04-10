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
// Created by mutter on 4/10/18.
//

/**
 * Merge k sorted list
 */

#include <stdlib.h>
#include <stdio.h>
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

Node* new_sorted_list(int size)
{
    Node* head = NULL;

    srand(time(NULL));
    for (int i = 0; i < size; ++i) {
        Node* node = new_node(rand() % size);

        if (head == NULL) {
            head = node;
        } else {
            Node* h = head;
            Node* prev = head;

            while (h && node->value > h->value) {
                prev = h;
                h = h->next;
            }

            if (prev == h) {
                node->next = h;
                head = node;
            } else {
                node->next = h;
                prev->next = node;
            }
        }

    }

    return head;
}

void print_list(Node* list)
{
    Node* l = list;
    while (l) {
        printf("%d", l->value);
        if (l->next) {
            printf(" -> ");
        }
        l = l->next;
    }
    putchar('\n');
}

Node* merge_two_sorted_list(Node* lhs, Node* rhs)
{
    Node* head = NULL;
    Node* tail = NULL;

    Node* first = lhs;
    Node* second = rhs;

    while (first && second) {
        Node* will_merged = NULL;

        if (first->value <= second->value) {
            will_merged = first;
            first = first->next;
        } else {
            will_merged = second;
            second = second->next;
        }

        if (head == NULL) {
            head = tail = will_merged;
        } else {
            tail->next = will_merged;
            tail = will_merged;
        }
    }

    if (first && tail) {
        tail->next = first;
    }
    if (second && tail) {
        tail->next = second;
    }

    return head;
}

Node* merge_k_sorted_list(Node* sorted_list[], int sorted_list_size)
{
    if (sorted_list_size == 2) {
        return merge_two_sorted_list(sorted_list[0], sorted_list[1]);
    }

    Node* first = sorted_list[0];
    Node* smaller_sorted_list = merge_k_sorted_list(sorted_list+1, sorted_list_size-1);

    return merge_two_sorted_list(first, smaller_sorted_list);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number-of-sorted-list> <size-of-per-sorted-list>\n", argv[0]);
        exit(0);
    }

    int n_sorted_list = (int)strtol(argv[1], NULL, 10);
    int size_of_sorted_list = (int)strtol(argv[2], NULL, 10);

    Node** sorted_list = malloc(sizeof(Node*) * n_sorted_list);

    for (int i = 0; i < n_sorted_list; ++i) {
        sorted_list[i] = new_sorted_list(size_of_sorted_list);
    }

    for (int j = 0; j < n_sorted_list; ++j) {
        print_list(sorted_list[j]);
    }

    printf("after merge done\n");

    print_list(merge_k_sorted_list(sorted_list, n_sorted_list));

    return 0;
}
