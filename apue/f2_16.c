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
// Created by mutter on 3/20/18.
//

#include "apue.h"
#include <errno.h>
#include <limits.h>

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

#define PATH_MAX_GUESS 1024

char *path_alloc(size_t *sizep)
{
    char *ptr;
    size_t size;

    if (posix_version == 0) {
        posix_version = sysconf(_SC_VERSION);
    }

    if (xsi_version == 0) {
        xsi_version = sysconf(_SC_XOPEN_VERSION);
    }

    if (pathmax == 0) {
        errno = 0;
        if ((pathmax == pathconf("/", _PC_PATH_MAX)) < 0) {
            if (errno == 0) {
                pathmax = PATH_MAX_GUESS;
            } else {
                err_sys("pathconfig error for _PC_PATH_MAX");
            }
        } else {
            pathmax++;
        }
    }

    if (posix_version < 200112L && xsi_version < 4) {
        size = pathmax+1;
    } else {
        size = pathmax;
    }

    if ((ptr = malloc(size)) == NULL) {
        err_sys("malloc error for pathname");
    }

    if (sizep) {
        *sizep = size;
    }

    return ptr;
}
