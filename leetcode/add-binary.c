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
// Created by mutter on 4/13/18.
//

/**
 * Given two binary strings, return their sum (also a binary string).
 *
 * For example,
 *
 * a = "11"
 * b = "1"
 *
 * Return "100".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* add_two_binary(const char* b1, const char* b2)
{
    size_t l1 = strlen(b1);
    size_t l2 = strlen(b2);
    size_t l = l1 > l2 ? l1 : l2;

    char* result = calloc((l+2), sizeof(char));

    int carry = 0;
    for (int i = l1-1, j = l2-1, k = 0, n = l; k < l; --i, --j, ++k, --n) {
        int sum = 0;
        if (i >= 0) {
            sum += (*(b1+i)-'0');
        }

        if (j >= 0) {
            sum += (*(b2+j)-'0');
        }

        sum += carry;
        if (sum >= 2) {
            sum -= 2;
            carry = 1;
        } else {
            carry = 0;
        }

        result[n] = (char)('0' + sum);
    }

    if (carry) {
        result[0] = '1';
    }

    if (result[0] == '\0') {
        char* r = calloc(l+1, sizeof(char));
        memcpy(r, result, sizeof(char)*l);
        free(result);
        return r;
    }

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <binary-1> <binary-2>\n", argv[0]);
        exit(0);
    }

    printf("Add two binary %s\n", add_two_binary(argv[1], argv[2]));
    return 0;
}
