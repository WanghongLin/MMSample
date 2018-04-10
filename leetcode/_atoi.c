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
 * string to integer
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * Convert a string to integer
 *
 * The overflow situation is not under consideration currently
 * @param string a input string
 * @return a integer
 */
static int _atoi(const char* string)
{
    int result = 0;
    for (int i = 0; *(string + i) != '\0'; i++) {
        result *= 10;
        result += *(string + i) - '0';
    }
    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input-string>\n", argv[0]);
        exit(0);
    }
    printf("convert to int %d\n", _atoi(argv[1]));
    return 0;
}
