#include <stdio.h>

#include "foo.h"

void bar() {
    printf("bar");
}

int main(void) {
    printf("Helo :3!");
    foo();
    bar();
    return 0;
}
