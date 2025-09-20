#include <stdio.h>
#include <assert.h>

int assert_eq_double(double actual, double expected) {
    if (actual != expected) {
        printf("Expected %f but was %f\n", expected, actual);
        return 0;
    }

    return 1;
}

int assert_eq_int(int actual, int expected) {
    if (actual != expected) {
        printf("Expected %d but was %d\n", expected, actual);
        return 0;
    }

    return 1;
}