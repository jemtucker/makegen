#include "log.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>

static const char CONTENT_MAKEFILE[] =                      \
"TARGET = makegen\n"                                        \
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
 * Write a default Makefile to <root>/Makefile
 * @param root: Directory in which to write the file
 * @return: 0 on success else error
 */
int make_makefile(const char *root) {
    int fd, result;
    ssize_t len;
    char buf[255];
    
    strcpy(buf, root);
    strcat(buf, "Makefile");
    
    fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        return errno;
    }
   
    // Write the contents to the new file 
    len = write(fd, CONTENT_MAKEFILE, sizeof(CONTENT_MAKEFILE));
    if (len != sizeof(CONTENT_MAKEFILE)) {
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
 * Write a 'Hello World' program to <root>/src/main.c
 * @param root: Directory in which to write the file
 * @return: 0 on success else errno error
 */
int make_main(const char *root){
    return 0;
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





















