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
// Created by mutter on 5/6/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct path_segment {
    const char* path_name;
    struct path_segment* next;
} path_segment;

static path_segment* new_path_segment(const char* name)
{
    path_segment* ps = malloc(sizeof(path_segment));
    ps->next = NULL;
    ps->path_name = name;
    return ps;
}

static void print_path_segment(path_segment* list)
{
    path_segment* head = list;
    while (head) {
        printf("%s", head->path_name);

        if (head->next) {
            printf(" -> ");
        }

        head = head->next;
    }
    putchar('\n');
}

char* simplify_path(const char* path)
{
    path_segment* list = NULL;
    path_segment* tail = NULL;

    int start = -1;
    for (int i = 0; ;++i) {
        if (path[i] == '/' || (*(path + i) == '\0' && i > 0 && *(path+i-1) != '/')) {

            if (start != -1) {
                path_segment* ps = new_path_segment(strndup(path+start, i-start));

                if (list == NULL) {
                    list = ps;
                    tail = ps;
                } else {
                    tail->next = ps;
                    tail = ps;
                }
            }

            start = i+1;
        }

        if (*(path+i) == '\0') {
            break;
        }
    }

    print_path_segment(list);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path-to-simplify>\n", argv[0]);
        exit(0);
    }

    simplify_path(argv[1]);
    return 0;
}