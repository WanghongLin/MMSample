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
// Created by mutter on 4/15/18.
//

#include <stdio.h>
#include <stdlib.h>

static int _sqrt(int x)
{
    if (x == 0) {
        return 0;
    }

    int left = 1;
    int right = x;

    while (1) {
        int mid = left + (right-left) / 2;
        if ((mid*mid) > x) {
            right = mid-1;
        } else {
            if ((mid+1)*(mid+1) > x) {
                return mid;
            }
            left = mid+1;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <integer>\n", argv[0]);
        exit(0);
    }
    printf("sqrt %d\n", _sqrt((int)strtol(argv[1], NULL, 10)));
    return 0;
}
