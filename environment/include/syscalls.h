/*
 * syscalls.h
 *
 *  Created on: Nov 4, 2016
 *      Author: nenad
 */

#ifndef APPLICATION_ENVIRONMENT_SYSCALLS_H_
#define APPLICATION_ENVIRONMENT_SYSCALLS_H_

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>

caddr_t _sbrk(int incr);
int _gettimeofday (struct timeval * tp, struct timezone * tzp);
void initialise_monitor_handles(void);
int _getpid(void);
int _kill(int pid, int sig);
void _exit (int status);
int _write(int file, char *ptr, int len);
int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
int _open(char *path, int flags, ...);
int _wait(int *status);
int _unlink(char *name);
int _times(struct tms *buf);
int _stat(char *file, struct stat *st);
int _link(char *old, char *new);
int _fork(void);
int _execve(char *name, char **argv, char **env);

#endif /* APPLICATION_ENVIRONMENT_SYSCALLS_H_ */
