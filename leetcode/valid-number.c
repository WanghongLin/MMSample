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
// Created by mutter on 5/26/18.
//

#include <stdio.h>

static const char* string_numbers[] = {
        "0",
        " 0.1 ",
        "abc",
        "1 a",
        "2e10",
        NULL
};

static int valid_number(const char* string)
{
    int non_space_char_count = 0;
    int is_scientific_or_float = 0;
    int has_number_record = 0;
    int should_end_with_all_space = 0;

    char previous_char = ' ';

    for (int i = 0; *(string + i) != '\0'; ++i) {
        const char c = *(string+i);

        if (should_end_with_all_space && c != ' ') {
            return 0;
        }

        if (c == ' ') {
            if (has_number_record) {
                should_end_with_all_space = 1;
            }

            if (is_scientific_or_float && (previous_char == '.' || previous_char == 'e')) {
                return 0;
            }

            non_space_char_count++;
            previous_char = c;
            continue;
        }

        const int nc = c - '0';
        if (c == 'e' || c == '.') {
            if (previous_char == ' ') {
                return 0;
            }
            is_scientific_or_float = 1;
        } else if (nc >= 0 && nc <= 9) {
            has_number_record = 1;
        } else {
            return 0;
        }

        previous_char = c;
    }

    if (non_space_char_count == 0 && !has_number_record) {
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    for (int i = 0; *(string_numbers + i) != NULL; ++i) {
        printf("%s => %s\n", string_numbers[i], valid_number(string_numbers[i]) ? "true" : "false");
    }
    return 0;
}