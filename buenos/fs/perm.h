#ifndef BUENOS_FS_PERM_H
#define BUENOS_FS_PERM_H

#define FS_PERM_NONE      (0)

#define FS_PERM_ALL       (-1)

#define FS_PERM_OWNER_R   (1 << 1)
#define FS_PERM_OWNER_W   (1 << 2)
#define FS_PERM_OWNER_X   (1 << 3)
#define FS_PERM_OWNER_RW  (FS_PERM_OWNER_R | FS_PERM_OWNER_W)
#define FS_PERM_OWNER_RWX (FS_PERM_OWNER_RW | FS_PERM_OWNER_X)

#define FS_PERM_OTHER_R   (1 << 4)
#define FS_PERM_OTHER_W   (1 << 5)
#define FS_PERM_OTHER_X   (1 << 6)
#define FS_PERM_OTHER_RW  (FS_PERM_OTHER_R | FS_PERM_OTHER_W)
#define FS_PERM_OTHER_RWX (FS_PERM_OTHER_RW | FS_PERM_OTHER_X)

#endif // BUENOS_FS_PERM_H
