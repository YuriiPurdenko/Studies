#include "csapp.h"

static const char *uidname(uid_t uid) {
  /* TODO: Something is missing here! */
    return getpwuid(uid)->pw_name;
}

static const char *gidname(gid_t gid) {
  /* TODO: Something is missing here! */
    return getgrgid(gid)->gr_name;
}

static int getid(uid_t *uid_p, gid_t *gid_p, gid_t **gids_p) {
  gid_t *gids = NULL;
  int ngid = 2;
  int groups;

    for (groups = 1; ; ++groups) {
    gids = realloc(gids, sizeof(gid_t) * groups);
    ngid = getgroups(groups, gids);
    if (ngid != 0 && ngid != -1)
      break;
  }
    
  /* TODO: Something is missing here! */

  *gids_p = gids;
  return groups;
}

int main(void) {
  uid_t uid;
  gid_t *gids, gid;
  int groups = getid(&uid, &gid, &gids);

  printf("uid=%d(%s) gid=%d(%s) ", uid, uidname(uid), gid, gidname(gid));
  printf("groups=%d(%s)", gids[0], gidname(gids[0]));
  for (int i = 1; i < groups; i++)
    printf(",%d(%s)", gids[i], gidname(gids[i]));
  putchar('\n');

  free(gids);

  return 0;
}
