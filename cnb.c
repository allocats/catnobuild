#define WHISKER_NOPREFIX
#include "cnb.h"

#include <string.h>

#define FLAGS "-O3", "-flto", "-march=native"

int main(int argc, char* argv[]) {
    const char* build_file = "cnb.c";
    const char** source_files = {
        "src/main.c"
    }

    if (argc > 1 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
        printf("Build loaded: %s\n", BUILD_TIMESTAMP);
        return 0;
    }

    if (should_rebuild(build_file, argv[0])) {
        Cmd cmd = {0};
        cmd_append(&cmd, "clang");
        cmd_append(&cmd, build_file);
        cmd_append(&cmd, "-o");
        cmd_append(&cmd, argv[0]);
        
        if (!cmd_execute(&cmd)) {
            cmd_destroy(&cmd);
            fprintf(stderr, "Build failed\n");
            return 1;
        }

        cmd_destroy(&cmd);
        execvp(argv[0], argv);
        fprintf(stderr, "Failed to reload\n");
        return 1;
    } 

    return 0;
}
