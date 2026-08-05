#ifndef COMPILER_H
#define COMPILER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

static inline long long min(long long a, long long b) {
	return (a < b) ? a : b;
}
static inline long long max(long long a, long long b) {
	return (a > b) ? a : b;
}

#endif /* COMPILER_H */