#ifndef _CONSOLE_H
#define _CONSOLE_H

extern int quiet;

#define conprintf(...) if (!quiet) printf(__VA_ARGS__)
#define confprintf(...) if (!quiet) fprintf(__VA_ARGS__)

#endif
