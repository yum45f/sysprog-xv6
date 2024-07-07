#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200
#define O_TRUNC 0x400

#define S_IRUSR 0b1
#define S_IWUSR 0b10
#define S_IXUSR 0b100
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
