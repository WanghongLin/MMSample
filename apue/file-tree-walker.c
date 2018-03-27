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
#include <dirent.h>

typedef struct FileStatics {
    size_t n_regular;
    size_t n_block;
    size_t n_character;
    size_t n_fifo;
    size_t n_symbolic;
    size_t n_socket;
    size_t n_total;
} FileStatics;

/**
 * Add stat information to file statics
 * @param statbuf  the stat from lstat
 * @param fileStatics the static result to write to
 */
void stat_a_file(struct stat* statbuf, FileStatics* fileStatics) {
    if (statbuf && fileStatics) {
        switch (statbuf->st_mode & S_IFMT) {
            case S_IFREG: fileStatics->n_regular++; break;
            case S_IFBLK: fileStatics->n_block++; break;
            case S_IFCHR: fileStatics->n_character++; break;
            case S_IFIFO: fileStatics->n_fifo++; break;
            case S_IFLNK: fileStatics->n_symbolic++; break;
            case S_IFSOCK: fileStatics->n_socket++; break;
            default: err_msg("unknown file type");
        }
        fileStatics->n_total++;
    } else {
        if (!statbuf) {
            err_msg("invalid stat");
        }
        if (!fileStatics) {
            err_msg("invalid file statics");
        }
    }
}

FileStatics init_file_statics()
{
    FileStatics fs = {
            .n_socket = 0,
            .n_symbolic = 0,
            .n_fifo = 0,
            .n_character = 0,
            .n_block = 0,
            .n_total = 0,
            .n_regular = 0
    };
    return fs;
}

FileStatics sum_file_statics(FileStatics* fs1, FileStatics* fs2) {
    FileStatics fs;

    fs.n_total = fs1->n_total + fs2->n_total;
    fs.n_socket = fs1->n_socket + fs2->n_socket;
    fs.n_symbolic = fs1->n_symbolic + fs2->n_symbolic;
    fs.n_fifo = fs1->n_fifo + fs2->n_fifo;
    fs.n_character = fs1->n_character + fs2->n_character;
    fs.n_block = fs1->n_block + fs2->n_block;
    fs.n_regular = fs1->n_regular + fs2->n_regular;

    return fs;
}

void print_file_statics(FileStatics* fileStatics) {
    if (fileStatics && fileStatics->n_total > 0) {
        printf("regular file %ld %5.2f\n", fileStatics->n_regular, fileStatics->n_regular*100.0/fileStatics->n_total);
        printf("block file %ld %5.2f\n", fileStatics->n_block, fileStatics->n_block*100.0/fileStatics->n_total);
        printf("char file %ld %5.2f\n", fileStatics->n_character, fileStatics->n_character*100.0/fileStatics->n_total);
        printf("fifo file %ld %5.2f\n", fileStatics->n_fifo, fileStatics->n_fifo*100.0/fileStatics->n_total);
        printf("symbolic file %ld %5.2f\n", fileStatics->n_symbolic, fileStatics->n_symbolic*100.0/fileStatics->n_total);
        printf("socket file %ld %5.2f\n", fileStatics->n_socket, fileStatics->n_socket*100.0/fileStatics->n_total);
    }
}

FileStatics walker(const char* start_point) {

    FileStatics fileStatics = init_file_statics();

    struct stat buf;
    if ((lstat(start_point, &buf)) < 0) {
        err_sys("stat error for %s", start_point);
    }

    if (S_ISDIR(buf.st_mode)) {
        DIR* dirp;
        if ((dirp = opendir(start_point)) == NULL) {
            return fileStatics;
        }

        printf("walk into directory %s\n", start_point);

        struct dirent* dirent_p;
        while ((dirent_p = readdir(dirp))) {
            if (strcmp(dirent_p->d_name, ".") == 0 ||
                strcmp(dirent_p->d_name, "..") == 0) {
                continue;
            }

            size_t subpath_len = strlen(start_point) + strlen(dirent_p->d_name) + 2;
            char* subpath = malloc(subpath_len*sizeof(char));
            memset(subpath, 0, subpath_len);
            strcat(subpath, start_point);
            strcat(subpath, "/");
            strcat(subpath, dirent_p->d_name);

            FileStatics subsatics = walker(subpath);
            fileStatics = sum_file_statics(&fileStatics, &subsatics);
        }

        closedir(dirp);
    } else {
        stat_a_file(&buf, &fileStatics);
    }

    return fileStatics;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        err_quit("Usage: %s <file_start_point>", argv[0]);
    }

    FileStatics fileStatics = walker(argv[1]);
    print_file_statics(&fileStatics);
    return 0;
}
