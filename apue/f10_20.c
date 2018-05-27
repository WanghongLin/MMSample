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
// Created by mutter on 5/5/18.
//

#include "apue.h"
#include <setjmp.h>
#include <time.h>

static void sig_usr1(int);
static void sig_alrm(int);
static sigjmp_buf jmpbuf;
static volatile sig_atomic_t canjump;

int main(int argc, char *argv[])
{
    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        err_sys("signal(SIGUSR1) error");
    }

    if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
        err_sys("signal(SIGALRM) error");
    }

    pr_mask("starting main: ");

    if (sigsetjmp(jmpbuf, 1)) {
        pr_mask("ending main: ");
        exit(0);
    }

    canjump = 1;

    for (;;) {
        pause();
    }

    return 0;
}

static void sig_usr1(int signo)
{
    time_t starttime;
    if (canjump == 0) {
        return;
    }

    pr_mask("starting sig_usr1: ");
    alarm(3);

    starttime = time(NULL);

    for (;;) {
        if (time(NULL) > starttime)
            break;
    }

    pr_mask("finishing sig_usr1: ");

    canjump = 0;

    siglongjmp(jmpbuf, 1);
}

static void sig_alrm(int signo)
{
    pr_mask("in sig_alrm: ");
}

