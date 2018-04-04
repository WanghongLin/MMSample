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

#include <stdio.h>
#include <stdlib.h>

static double _pow(double x, int n)
{
    return (n == 1) ? x : x*_pow(x, n-1);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s x n\n", argv[0]);
        exit(0);
    }

    double x = strtod(argv[1], NULL);
    int n = (int)strtol(argv[2], NULL, 10);

    printf("%f pow %d is %f\n", x, n, _pow(x, n));
    return 0;
}