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
// Created by mutter on 3/16/18.
//

#include "apue.h"
#include <dirent.h>

int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;

    if (argc != 2) {
        err_quit("usage: ls directory_name");
    }

    if ((dp = opendir(argv[1])) == NULL) {
        err_sys("can't open %s", argv[1]);
    }

    while ((dirp = readdir(dp))) {
        printf("%s\n", dirp->d_name);
    }

    closedir(dp);
    exit(0);
}
