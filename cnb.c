#define WHISKER_NOPREFIX
#include "cnb.h"

int main(int argc, const char* argv[]) {
    bool rebuilt = getenv("CNB_REBUILT") != NULL;
    if (rebuilt) {
        unsetenv("CNB_REBUILT");
        printf("Build system was rebuilt, forcing full recompilation\n");
    }

    rebuild_build("cnb.c", argv);

    Whikser_Config config = {
        BUILD_SOURCES(
            "test/foo.c",
            "test/main.c"
        ),
        BUILD_FLAGS(
            "-O2",
            "-flto",
            "-march=native"
        ),
        .compiler = "clang",
        .build_dir = "build/",
        .output = "build/bin/main",
        .parallel_jobs = 12
    };

    if (build_project(&config, rebuilt)) {
        printf("Compiled :3\n");
        return 0;
    } else {
        printf("Good luck deubgging!\n");
        return 1;
    }
}
