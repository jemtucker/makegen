#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>

#define PROGNAME "makegen"

#define STR2(s) #s
#define STR(s) STR2(s)
#define ERR(msg, __VA_ARGS__) printf(PROGNAME ":" __FILE__ ":" STR(__LINE__) ": " msg,  __VA_ARGS__)

/**
 * Create a directory 'dir' under parent 'root' with group and owner read/
 * write/execute permissions.
 * @param root: The parent directory
 * @param dir: The new directory name
 * @return: 0 on success else a non-zero errno error code.
 */
int mkdir_relative(const char* root, const char* dir) {
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
int make_directories(const char* root) {
    int result;

    result = mkdir_relative(root, "src");
    if (result != 0) {
        ERR("Error creating 'src' dir: %d\n", result);
        return -1;
    }
    
    result = mkdir_relative(root, "inc");
    if (result != 0) {
        ERR("Error creating 'inc' dir: %d\n", result);
        return -1;
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
    
    return 0;
}





















