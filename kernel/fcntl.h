#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200
#define O_TRUNC 0x400

#define S_IRUSR 01 // Readable by owner
#define S_IWUSR 02 // Writable by owner
#define S_IXUSR 04 // Executable by owner
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
