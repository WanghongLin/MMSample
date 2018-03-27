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
// Created by mutter on 3/19/18.
//

#include "apue.h"
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        err_quit("usage: %s <pathname>", argv[0]);
    }

    if (access(argv[1], R_OK) < 0) {
        err_ret("access error for %s", argv[1]);
    } else {
        printf("read access OK\n");
    }

    if (open(argv[1], O_RDONLY) < 0) {
        err_ret("open error for %s", argv[1]);
    } else {
        printf("open for reading OK\n");
    }
    return 0;
}
