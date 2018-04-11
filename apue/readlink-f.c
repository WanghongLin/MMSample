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
 * simulate readlink -f feature
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

static const char* abs_path(const char* path)
{
    if (path[0] == '/') {
        return path;
    }

    struct stat sbuf;
    if (lstat(path, &sbuf) != 0) {
    }

    return realpath(path, NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file-or-directory-name>\n", argv[0]);
        exit(0);
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    printf("%s\n", abs_path(argv[1]));

    return 0;
}
