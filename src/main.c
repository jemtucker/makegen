#include "log.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>

static const char CONTENT_MAKEFILE[] =                      \
"TARGET = hello\n"                                          \
"LIBS =\n"                                                  \
"CC = clang\n"                                              \
"CFLAGS = -g -Wall -Iinc/\n\n"                              \
".PHONY: default all clean\n\n"                             \
                                                            \
"default: $(TARGET)\n"                                      \
"all: default\n\n"                                          \
                                                            \
"OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))\n"     \
"HEADERS = $(wildcard src/*.h)\n\n"                         \
                                                            \
"%.o: %.c $(HEADERS)\n"                                     \
"\t$(CC) $(CFLAGS) -c $< -o $@\n\n"                         \
                                                            \
".PRECIOUS: $(TARGET) $(OBJECTS)\n\n"                       \
                                                            \
"$(TARGET): $(OBJECTS)\n"                                   \
"\t$(CC) $(OBJECTS) -Wall $(LIBS) -o $@\n\n"                \
                                                            \
"clean:\n"                                                  \
"\t-rm -f src/*.o\n"                                        \
"\t-rm -f $(TARGET)\n"                                      ;

static const char CONTENT_MAIN[] =                          \
"#include <stdio.h>\n\n"                                    \
                                                            \
"int main(int argc, const char *argv[]) {\n"                \
"    printf(\"Hello World\\n\");\n"                         \
"    return 0\n;"                                           \
"}\n"                                                       ;

/**
 * Create a directory 'dir' under parent 'root' with group and owner read/
 * write/execute permissions.
 * @param root: The parent directory
 * @param dir: The new directory name
 * @return: 0 on success else a non-zero errno error code.
 */
int mkdir_relative(const char *root, const char *dir) {
     char buf[255];
     int result;

     // Cat root/dir
     strcpy(buf, root);
     strcat(buf, dir);

     // Make directory with read/write/execute permissions for owner and group.
     result = mkdir(buf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
     if (result != 0) {
        return errno;
     } else {
        return 0;
     }
}

/**
 * Make all project directories under the passed root. Directories created are
 *      - src
 *      - inc
 * @param root: The root dir under which all project folders are created.
 * @return: 0 on success else -1
 */
int make_directories(const char *root) {
    int result;

    result = mkdir_relative(root, "src");
    if (result != 0 && result != EEXIST) {
        ERR_ERRNO("Error creating 'src' dir");
        return -1;
    }

    result = mkdir_relative(root, "inc");
    if (result != 0 && result != EEXIST) {
        ERR_ERRNO("Error creating 'inc' dir");
        return -1;
    }

    return 0;
}

/**
 * Write 'contents' to file at 'path', overwriting any exising file.
 * @param path: File to create
 * @param contents: Buffer containing data to write
 * @param contentsLen: Length of contents buffer
 * @return: 0 on success else error
 */
int make_file(const char *path, const char *contents, size_t contentsLen) {
    int fd, result;
    ssize_t len;

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        return errno;
    }

    // Write the contents to the new file
    len = write(fd, contents, contentsLen);
    if (len != contentsLen) {
        close(fd);
        return errno;
    }

    // Change permissions on the file to read/write owner and
    // read only for others
    result = fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (result != 0) {
        close(fd);
        return errno;
    }

    close(fd);
    return 0;
}

/**
 * Write a default Makefile to <root>/Makefile
 * @param root: Directory in which to write the file, including trailing /
 * @return: 0 on success else error
 */
int make_makefile(const char *root) {
    char buf[255];
    strcpy(buf, root);
    strcat(buf, "Makefile");

    // Write the data, without the null terminator
    return make_file(buf, CONTENT_MAKEFILE, sizeof(CONTENT_MAKEFILE) - 1);
}

/**
 * Write a 'Hello World' program to <root>/src/main.c
 * @param root: Directory in which to write the file, including trailing /
 * @return: 0 on success else errno error
 */
int make_main(const char *root) {
    char buf[255];
    strcpy(buf, root);
    strcat(buf, "src/main.c");

    // Write the data, without the null terminator
    return make_file(buf, CONTENT_MAIN, sizeof(CONTENT_MAIN) - 1);
}

int make_files(const char* root) {
    int result;
    result = make_makefile(root);
    if (result != 0) {
        ERR_ERRNO("Error creating Makefile");
        return result;
    }

    result = make_main(root);
    if (result != 0) {
        ERR_ERRNO("Error creating src/main.c");
        return result;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    int result;

    // Create all required directories
    result = make_directories("./test/");
    if (result != 0) {
        return result;
    }

    result = make_files("./test/");
    if (result != 0) {
        return result;
    }

    return 0;
}
