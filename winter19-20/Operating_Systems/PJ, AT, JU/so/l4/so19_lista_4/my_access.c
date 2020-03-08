#include <sys/stat.h>
#include <unistd.h>

int my_access(struct stat *statbuf, int mode) {
  const int MAX_GROUPS = 128;
  gid_t groups[MAX_GROUPS];
  int groupsNo = getgroups(MAX_GROUPS, groups);
  uid_t euid = geteuid();
  int inCorrectGroup = 0;
  for (int i = 0; i < groupsNo; i++) {
    if (groups[i] == statbuf->st_gid)
      inCorrectGroup = 1;
  }
  // I assume file always exists
  int fileMode = F_OK;
  
  if (statbuf->st_uid == euid) {
    if (statbuf->st_mode & S_IRUSR)
      fileMode |= R_OK;
    if (statbuf->st_mode & S_IWUSR)
      fileMode |= W_OK;
    if (statbuf->st_mode & S_IXUSR)
      fileMode |= X_OK;
  }
  
  if (inCorrectGroup) {
    if (statbuf->st_mode & S_IRGRP)
      fileMode |= R_OK;
    if (statbuf->st_mode & S_IWGRP)
      fileMode |= W_OK;
    if (statbuf->st_mode & S_IXGRP)
      fileMode |= X_OK;
  }

  if (statbuf->st_mode & S_IROTH)
    fileMode |= R_OK;
  if (statbuf->st_mode & S_IWOTH)
    fileMode |= W_OK;
  if (statbuf->st_mode & S_IXOTH)
    fileMode |= X_OK;

  return (fileMode & mode) == mode;
}
