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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _strlen(const char* string)
{
    int n = 0;
    for (int i = 0; *(string + i); ++i) {
        n++;
    }
    return n;
}

static char* _strstr(const char* haystack, const char* needle)
{
    int n = _strlen(haystack);
    int nn = _strlen(needle);

    int match = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < nn; ++j) {
            if (needle[j] == haystack[i]) {
                match++;
                i++;
            } else {
                break;
            }
        }

        if (match == nn) {
            return strndup(haystack+i-match, (size_t) match);
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <haystack> <needle>\n", argv[0]);
        exit(0);
    }

    char* found = _strstr(argv[1], argv[2]);
    if (found) {
        printf("%s\n", found);
        fflush(stdout);
        free(found);
    } else {
        printf("not found\n");
    }
    return 0;
}

