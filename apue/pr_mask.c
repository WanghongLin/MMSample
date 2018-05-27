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

typedef struct {
    int signo;
    const char* name;
} Signal;

static Signal signals[] = {
        {SIGINT, "SIGINT"},
        {SIGQUIT, "SIGQUIT"},
        {SIGALRM, "SIGALRM"},
        {SIGUSR1, "SIGUSR1"}
};

void pr_mask(const char* str)
{
    sigset_t sigset;
    int errno_save;

    errno_save = errno;

    if (sigprocmask(0, NULL, &sigset) < 0) {
        err_ret("sigprocmask error");
    } else {
        int number_of_signals = sizeof(signals) / sizeof(signals[0]);
        printf("%s ", str);
        for (int i = 0; i < number_of_signals; ++i) {
            if (sigismember(&sigset, signals[i].signo)) {
                printf("%s\n", signals[i].name);
            }
        }
        putchar('\n');
    }

    errno = errno_save;
}
