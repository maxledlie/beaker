#include <stdlib.h>
#include <time.h>

/* Simple random number generation module, loosely based on Python's interface.
It is NOT threadsafe and NOT cryptographically secure and probably does NOT generate perfectly uniform distributions. */

static int _initialized = 0;

int _init() {
    if (!_initialized) {
        srand((unsigned int)time(NULL));
        _initialized = 1;
    }
    return 0;
}

/* Return the next random floating-point number in the range 0.0 <= X < 1.0 */
double random_double() {
    _init();
    return (double)rand() / RAND_MAX;
}

/* Returns a random floating-point number N such that a <= N <= b for a <= b and b <= N <= a for b < a.
The end-point value b may or may not be included in the range depending on floating-point rounding
in the expression a + (b-a) * random() */
double random_uniform(double a, double b) {
    double min = a < b ? a : b;
    double max = a < b ? b : a;
    return min + (max - min) * random_double();
}