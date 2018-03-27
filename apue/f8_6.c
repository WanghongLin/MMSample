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
// Created by mutter on 3/24/18.
//

#include <sys/types.h>
#include <sys/wait.h>
#include "apue.h"

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        exit(7);
    }
    if (wait(&status) != pid) {
        err_sys("wait error");
    }
    pr_exit(status);

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        abort();
    }
    if (wait(&status) != pid) {
        err_sys("wait error");
    }
    pr_exit(status);

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        status /= 0;
    }
    if (wait(&status) != pid) {
        err_sys("wait error");
    }
    pr_exit(status);

    return 0;
}
