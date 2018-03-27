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
// Created by mutter on 3/21/18.
//

#include "apue.h"
#include <sys/resource.h>

#define doit(name) pr_limits(#name, name)

static void pr_limits(char*, int);

int main(int argc, char *argv[])
{
    doit(RLIMIT_AS);
    doit(RLIMIT_CORE);
    doit(RLIMIT_CPU);
    doit(RLIMIT_DATA);
    doit(RLIMIT_FSIZE);
    doit(RLIMIT_NOFILE);
    doit(RLIMIT_STACK);
    doit(RLIMIT_LOCKS);
    doit(RLIMIT_MEMLOCK);
    doit(RLIMIT_MSGQUEUE);
    doit(RLIMIT_NICE);
    doit(RLIMIT_NPROC);
    doit(RLIMIT_OFILE);
    doit(RLIMIT_RSS);
    doit(RLIMIT_RTPRIO);
    doit(RLIMIT_RTTIME);
    doit(RLIMIT_SIGPENDING);
    return 0;
}

static void pr_limits(char* name, int resource)
{
    struct rlimit limit;
    unsigned long long lim;

    if (getrlimit(resource, &limit) < 0) {
        err_sys("getrlimit error for %s", name);
    }
    printf("%-14s    ", name);
    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("(infinite)  ");
    } else {
        lim = limit.rlim_cur;
        printf("%10lld    ", lim);
    }

    if (limit.rlim_max == RLIM_INFINITY) {
        printf("(infinite)");
    } else {
        lim = limit.rlim_max;
        printf("%10lld", lim);
    }

    putchar('\n');
}