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
// Created by mutter on 3/27/18.
//

#include "apue.h"
#include <sys/times.h>

static void pr_times(clock_t, struct tms*, struct tms*);
static void do_cmd(char *);

int main(int argc, char *argv[])
{
    int i;
    setbuf(stdout, NULL);
    for (i = 1; i < argc; i++) {
        do_cmd(argv[i]);
    }
    return 0;
}

static void do_cmd(char* cmd)
{
    struct tms tmsstart, tmsend;
    clock_t start, end;
    int status;

    printf("\ncommand: %s\n", cmd);
    if ((start = times(&tmsstart)) == -1) {
        err_sys("times error");
    }

    if ((status = system(cmd)) < 0) {
        err_sys("system() error");
    }

    if ((end = times(&tmsend)) == -1) {
        err_sys("times error");
    }

    pr_times(end-start, &tmsstart, &tmsend);
    pr_exit(status);
}

static void pr_times(clock_t real, struct tms* tmsstart, struct tms* tmsend)
{
    static long clktck = 0;
    if (clktck == 0) {
        if ((clktck = sysconf(_SC_CLK_TCK)) < 0) {
            err_sys("sysconf error");
        }
    }

    printf(" real: %7.2f\n", real / (double) clktck);
    printf(" user: %7.2f\n",
           (tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
    printf(" sys: %7.2f\n",
           (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
    printf(" child user: %7.2f\n",
           (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
    printf(" child sys: %7.2f\n",
           (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);
}
