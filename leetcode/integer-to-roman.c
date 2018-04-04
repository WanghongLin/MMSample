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
 * convert an integer to Roman number
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char roman_table[][3] = { {'I', 'V', 'X'},
                                       {'X', 'L', 'C'},
                                       {'C', 'D', 'M'},
                                       {'M'}};

static char* roman_for_a_number(int number, const char* table)
{
    char* s = NULL;
    switch (number) {
        case 0:
            return strdup("");
        case 1:
            s = calloc(2, sizeof(char));
            s[0] = table[0];
            break;
        case 2:
            s = calloc(3, sizeof(char));
            s[0] = s[1] = table[0];
            break;
        case 3:
            s = calloc(4, sizeof(char));
            s[0] = s[1] = s[2] = table[0];
            break;
        case 4:
            s = calloc(3, sizeof(char));
            s[0] = table[0];
            s[1] = table[1];
            break;
        case 5:
            s = calloc(2, sizeof(char));
            s[0] = table[1];
            break;
        case 6:
            s = calloc(3, sizeof(char));
            s[0] = table[1];
            s[1] = table[0];
            break;
        case 7:
            s = calloc(4, sizeof(char));
            s[0] = table[1];
            s[1] = s[2] = table[0];
            break;
        case 8:
            s = calloc(5, sizeof(char));
            s[0] = table[1];
            s[1] = s[2] = s[3] = table[0];
            break;
        case 9:
            s = calloc(3, sizeof(char));
            s[0] = table[0];
            s[1] = table[2];
            break;
        default:
            return strdup("");
    }

    return s;
}

static const char* integer_to_roman(int integer)
{
    char* vresult[5] = { 0 };
    int j = 0;
    while (integer > 0) {

        int i = integer % 10;
        vresult[j] = roman_for_a_number(i, roman_table[j]);

        integer /= 10;
        j++;
    }

    int total_length = 0;
    int nelems = 0;
    for (int i = 0; vresult[i] != 0; i++) {
        total_length += strlen(*(vresult+i));
        nelems++;
    }

    char* result = calloc(total_length + 1, sizeof(char));
    for (int i = nelems-1; i >= 0; i--) {
        strcat(result, vresult[i]);
    }

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input-number>\n", argv[0]);
        exit(0);
    }

    long number = strtol(argv[1], NULL, 10);
    if (number < 1 || number > 3999) {
        fprintf(stderr, "number must be within 1~3999\n");
        exit(0);
    }

    printf("convert to roman %s\n", integer_to_roman((int)number));
    return 0;
}

