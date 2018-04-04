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
 * Divide two integers without using multiplication, division and mod operator.
 * If it is overflow, return MAX_INT.
 */

#include <stdlib.h>
#include <stdio.h>

static int divide_two_integers(int dividend, int divisor)
{
    if (divisor == 0) {
        return 0x7fffffff;
    }

    int i = 0;
    while (dividend >= divisor) {
        dividend -= divisor;
        i++;
    }
    return i;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stdout, "Usage: %s <dividend> <divisor>\n", argv[0]);
        exit(0);
    }

    int dividend = (int)strtol(argv[1], NULL, 10);
    int divisor = (int)strtol(argv[2], NULL, 10);
    printf("%d divided by %d is %d\n", dividend, divisor, divide_two_integers(dividend, divisor));

    return 0;
}