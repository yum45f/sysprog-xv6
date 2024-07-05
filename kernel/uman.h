#define MAX_USERNAME 32
#define MAX_PASSWORD 64
#define MAX_USERS 256

enum user_status
{
  USER_ACTIVE,
  USER_INACTIVE
};

struct user
{
  int uid;
  char username[MAX_USERNAME];
  char password[MAX_PASSWORD];
  enum user_status status;
};
