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
// Created by mutter on 4/12/18.
//

#include <stdio.h>
#include <stdlib.h>

static int length_of_last_word(const char* s)
{
    int length = 0;

    for (int i = 0; *(s+i) != '\0'; i++) {
        if (*(s+i) == ' ') {
            length = 0;
        } else {
            length++;
        }
    }

    return length;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input-string>\n", argv[0]);
        exit(0);
    }

    printf("length of last word %d\n", length_of_last_word(argv[1]));
    return 0;
}
