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
 * Given two non-negative integers num1 and num2 represented as strings, return the product of num1 and num2.
 * Note:
 * The length of both num1 and num2 is < 110.
 * Both num1 and num2 contains only digits 0-9.
 * Both num1 and num2 does not contain any leading zero.
 * You must not use any built-in BigInteger library or convert the inputs to integer directly.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static char* multiply_strings(const char* lhs, const char* rhs)
{
    char result[220] = { 0 };
    int result_index = 0;

    for (int i = strlen(rhs)-1, k = 0; i >= 0; i--, k++) {
        int carry = 0;
        result_index = k;

        for (int j = strlen(lhs)-1; j >= 0; j--) {
            int m1 = rhs[i] - '0';
            int m2 = lhs[j] - '0';
            int v = m1 * m2 + carry;

            char old = result[result_index];
            if (old) {
                v += (old - '0');
            }

            result[result_index++] = '0' + (v % 10);
            carry = v / 10;
        }

        if (carry) {
            result[result_index] = '0' + carry;
        }
    }

    char* reversed_result = calloc(strlen(result)+1, sizeof(char));
    for (int i = strlen(result)-1, j = 0; i >= 0; i--, j++) {
        reversed_result[j] = result[i];
    }

    return reversed_result;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stdout, "Usage: %s <num1> <num2>\n", argv[0]);
        exit(0);
    }

    printf("perform string multiply\n");
    printf("%sx%s = %s\n", argv[1], argv[2], multiply_strings(argv[1], argv[2]));

    printf("perform arithmetic multiply\n");
    printf("%sx%s = %ld\n", argv[1], argv[2], strtol(argv[1], NULL, 10)*strtol(argv[2], NULL, 10));
    return 0;
}
