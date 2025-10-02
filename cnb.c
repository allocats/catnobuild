#define WHISKER_NOPREFIX
#include "cnb.h"

#define FLAGS "-O3","-flto", "-march=native"

int main(int argc, const char* argv[]) {
    const char* build_file = "cnb.c";
    const char* source_files[] = {
        "src/main.c",
    };

    rebuild_build(build_file, argv);
    // build_project(source_files, FLAGS);

    return 0;
}
