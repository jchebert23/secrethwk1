// farthing.h                                     Stan Eisenstat (08/20/18)
//   System header files and macros for farthing

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

// Write message to stderr using format FORMAT
#define WARN(format,...) fprintf (stderr, "farthing: " format "\n", __VA_ARGS__)

// Write message to stderr using format FORMAT and exit.
#define DIE(format,...)  WARN(format,__VA_ARGS__), exit (EXIT_FAILURE)

// Change allocated size of array X of unspecified type to S elements.
// X may be NULL; assumes that realloc() never fails.
#define REALLOC(x,s) x = realloc (x, (s) * sizeof(*(x)))
