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
// Created by mutter on 4/10/18.
//

#include "apue.h"
#include <sys/wait.h>

static void sig_cld(int);

int main(int argc, char *argv[])
{
    pid_t pid;

    if (signal(SIGCLD, sig_cld) == SIG_ERR) {
        perror("signal error");
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
    } else if (pid == 0) {
        sleep(2);
        _exit(0);
    }

    pause();
    return 0;
}

static void sig_cld(int signo)
{
    pid_t pid;
    int status;

    printf("SIGCLD received\n");

    if (signal(SIGCLD, sig_cld) == SIG_ERR) {
        perror("signal error");
    }

    if ((pid = wait(&status)) < 0) {
        perror("wait error");
    }

    printf("pid = %d\n", pid);
}
