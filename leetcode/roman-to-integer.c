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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char* roman_table[][2] = { {"I", "1"},
                                        {"II", "2"},
                                        {"III", "3"},
                                        {"IV", "4"},
                                        {"V", "5"},
                                        {"VI", "6"},
                                        {"VII", "7"},
                                        {"VIII", "8"},
                                        {"IX", "9"},
                                        {"X", "10"},
                                        {"XX", "20"},
                                        {"XXX", "30"},
                                        {"XL", "40"},
                                        {"L", "50"},
                                        {"LX", "60"},
                                        {"LXX", "70"},
                                        {"LXXX", "80"},
                                        {"XC", "90"},
                                        {"C", "100"},
                                        {"CC", "200"},
                                        {"CCC", "300"},
                                        {"CD", "400"},
                                        {"D", "500"},
                                        {"DC", "600"},
                                        {"DCC", "700"},
                                        {"DCCC", "800"},
                                        {"CM", "900"},
                                        {"M", "1000"}};

/**
 * Convert roman number to integer
 */
static int roman_to_integer(const char* roman)
{
    int result = 0;

    for (int i = 0; *(roman+i) != '\0';) {
        int longest = 0;
        const char* value = NULL;
        for (int j = 0; j < sizeof(roman_table)/sizeof(roman_table[0]); j++) {
            const char* key = roman_table[j][0];
            const char* p = roman+i;

            if (strstr(p, key) == p && longest < strlen(key)) {
                longest = strlen(key);
                value = roman_table[j][1];
            }
        }

        i += longest;
        result += (int)strtol(value, NULL, 10);
    }

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <roman>\n", argv[0]);
        exit(0);
    }

    printf("convert roman to integer %d\n", roman_to_integer(argv[1]));
    return 0;
}

