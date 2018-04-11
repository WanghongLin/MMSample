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
// Created by mutter on 4/11/18.
//

/**
 * Given an array of strings, group anagrams together.
 *
 * For example, given: ["eat", "tea", "tan", "ate", "nat", "bat"],
 *
 * Return:
 * [
 * ["ate", "eat","tea"],
 * ["nat","tan"],
 * ["bat"]
 * ]
 *
 * Note: All inputs will be in lower-case.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct word {
    const char* s;
    struct word* next;
} word;

typedef struct word_group {
    word* head;
    int size;
} word_group;

word* new_word(const char* s)
{
    word* w = malloc(sizeof(word));
    w->next = NULL;
    w->s = strdup(s);
    return w;
}

word_group new_word_group(word* w)
{
    word_group wg;
    wg.head = w;
    wg.size = 1;
    return wg;
}

void print_word_group(word_group* wg)
{
    if (wg->size > 0) {
        word* h = wg->head;
        printf("[");
        while (h) {
            printf("%s", h->s);
            if (h->next) {
                printf(", ");
            } else {
                printf("]\n");
            }
            h = h->next;
        }
    }
}

void add_to_word_group(word* w, word_group* wg)
{
    word* tail = wg->head;
    while (tail->next) {
        tail = tail->next;
    }
    tail->next = w;
    wg->size++;
}

int is_anagrams(const char* check, const char* s)
{
    int found = 0;

    for (int i = 0; *(check+i); i++) {
        for (int j = 0; *(s+j); j++) {
            if (*(s+j) == *(check+i)) {
                found++;
                break;
            }
        }
    }

    if (found == strlen(check)) {
        return found;
    }
    return 0;
}

word_group* group_anagrams(char* words[], int n)
{
    word_group* wg = calloc((size_t)n+1, sizeof(word_group));

    int current_wg_index = 0;

    for (int i = 0; *(words+i); i++) {

        const char* current_word = *(words+i);

        int added = 0;
        for (int j = 0; (wg+j)->size != 0; j++) {

            const char* existing_word = (wg+j)->head->s;
            if (is_anagrams(current_word, existing_word)) {
                add_to_word_group(new_word(current_word), wg+j);
                added = 1;
                break;
            }
        }

        if (!added) {
            wg[current_wg_index++] = new_word_group(new_word(current_word));
        }
    }

    return wg;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <word-list>\n", argv[0]);
        exit(0);
    }

    word_group* result = group_anagrams(argv+1, argc-1);
    for (int i = 0; (result+i)->size != 0; i++) {
        print_word_group(result+i);
    }

    return 0;
}
