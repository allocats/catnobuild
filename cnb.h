#ifndef CNB_H
#define CNB_H

#include <cstdarg>
#ifndef WHISKER_ASSERT
#include <assert.h>
#define WHISKER_ASSERT assert
#endif // WHISKER_ASSERT

#ifndef WHISKER_ALLOC
#define WHISKER_ALLOC malloc
#endif // WHISKER_ALLOC

#ifndef WHISKER_FREE
#define WHISKER_FREE free
#endif // WHISKER_FREE

#ifndef WHISKER_REALLOC
#define WHISKER_REALLOC realloc 
#endif // WHISKER_REALLOC

#ifdef WHISKER_NOPREFIX
    #define Cmd Whisker_Cmd
    #define cmd_append whisker_cmd_append
    #define cmd_execute whisker_cmd_execute
    #define cmd_reset whisker_cmd_reset
    #define cmd_destroy whisker_cmd_destroy
    #define should_rebuild whisker_should_rebuild
    #define rebuild_build whisker_rebuild_build
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    const char** items;
    size_t count;
    size_t capacity;
} Whisker_Cmd;

#define whisker_cmd_append_args(cmd, ...) \
    whisker_cmd_append_args_impl(cmd, \
            ((const char*[]){ __VA_ARGS__ }), \
            (sizeof((const char*[]){ __VA_ARGS__ }) / sizeof(const char*))) 

static void whisker_cmd_destroy(Whisker_Cmd* cmd) {
    WHISKER_ASSERT(cmd);

    WHISKER_FREE(cmd -> items);
    cmd -> items = NULL;
    cmd -> count = 0;
    cmd -> capacity= 0;
}

static void whisker_cmd_reset(Whisker_Cmd* cmd) {
    WHISKER_ASSERT(cmd);

    cmd -> count = 0;
    cmd -> capacity = 8;
    WHISKER_FREE(cmd -> items);

    cmd -> items = (const char**) WHISKER_ALLOC(sizeof(const char*) * cmd -> capacity);
    WHISKER_ASSERT(cmd -> items);
}

static void whisker_cmd_append(Whisker_Cmd* cmd, const char* arg) {
    WHISKER_ASSERT(cmd);

    if (cmd -> count >= cmd -> capacity) {
        const size_t new_capacity = cmd -> capacity == 0 ? 8 : cmd -> capacity * 2;
        WHISKER_ASSERT(new_capacity > cmd -> capacity || new_capacity < (SIZE_MAX / sizeof(const char*))); 

        const char** new_items = (const char**) WHISKER_REALLOC(cmd -> items, sizeof(const char*) * new_capacity);
        WHISKER_ASSERT(new_items);

        cmd -> items = new_items;
        cmd -> capacity = new_capacity;
    }

    cmd -> items[cmd -> count++] = arg;
}

static void whisker_cmd_append_args_impl(Whisker_Cmd* cmd, const char** args, const size_t n) {
    WHISKER_ASSERT(cmd);

    for (size_t i = 0; i < n; i++) {
        whisker_cmd_append(cmd, args[i]);
    }
}  

static bool whisker_cmd_execute(Whisker_Cmd* cmd) {
    WHISKER_ASSERT(cmd && cmd -> count > 0);
    
    if (cmd -> items[cmd -> count - 1] != NULL) {
        whisker_cmd_append(cmd, NULL);
    }

    pid_t pid = fork();
    if (pid < 0) {
        return false;
    }

    if (pid == 0) {
        execvp(cmd -> items[0], (char* const*) cmd -> items);
        fprintf(stderr, "execvp failed! %s\n", cmd -> items[0]);
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        return false;
    }

    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

static bool whisker_should_rebuild(const char* src, const char* bin) {
    struct stat src_stat;
    struct stat bin_stat;
    if (stat(src, &src_stat) < 0) return false;
    if (stat(bin, &bin_stat) < 0) return true;
    return src_stat.st_mtime > bin_stat.st_mtime;
}

static void whisker_rebuild_build(const char* path, const char* argv[]) {
    if (!whisker_should_rebuild(path, argv[0])) return;

    Whisker_Cmd cmd = {0};
    whisker_cmd_append(&cmd, "clang");
    whisker_cmd_append(&cmd, "-Wall");
    whisker_cmd_append(&cmd, path);
    whisker_cmd_append(&cmd, "-o");
    whisker_cmd_append(&cmd, argv[0]);

    if (!whisker_cmd_execute(&cmd)) {
        whisker_cmd_destroy(&cmd);
        fprintf(stderr, "Build failed\n");
        exit(1);
    }

    whisker_cmd_destroy(&cmd);
    execvp(argv[0], (char* const*) argv);
    fprintf(stderr, "Failed to reload\n");
    exit(1);
}

static void whisker_build_project(const char** srcs) {

}

#endif /* ifndef CNB_H */
