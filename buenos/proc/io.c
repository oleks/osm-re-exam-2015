/*
 * Buenos userland I/O support.
 *
 * Copyright (C) 2015 OSM Course Team.
 *
 * Sandstone Basic License - Version 0.2 - June 16, 2014.
 *
 * Permission is hereby granted, free of charge, to any legal person obtaining
 * these artefacts, to do with the artefacts as they please, including, without
 * limitation, the rights to use, copy, modify, distribute, sublicense, and/or
 * sell these artefacts, and to permit legal persons, to whom the artefacts are
 * furnished, to do so as well, subject to the following conditions:
 *
 * The creators of these artefacts are properly attributed for their original
 * work, e.g. by including the above copyright notice and the below notice of
 * no warranty in any copy or derivation of the artefacts.
 *
 * THE ARTEFACTS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHENTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE OR NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE CREATORS OF THE ARTEFACTS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF, OR IN CONNECTION WITH THE ARTEFACTS, THE USE, OR OTHER
 * DEALINGS IN THE ARTEFACTS.
 *
 */

#include "drivers/gcd.h"
#include "fs/vfs.h"
#include "kernel/assert.h"
#include "proc/syscall.h"

#include "proc/io.h"

/** @name Userland I/O support.
 *
 * This module contains facilities for userland I/O.
 *
 * @{
 */

gcd_t* get_tty(void)
{
  device_t *dev;
  gcd_t *gcd;

  /* Find system console (first tty) */
  dev = device_get(YAMS_TYPECODE_TTY, 0);
  if (dev == NULL) {
    return NULL;
  }
  gcd = (gcd_t *)dev->generic_device;
  if (gcd == NULL) {
    return NULL;
  }
  return gcd;
}

int tty_read(void* buffer, int length)
{
  gcd_t *gcd;

  gcd = get_tty();
  if (gcd == NULL) {
    return VFS_ERROR;
  }
  return gcd->read(gcd, buffer, length);
}

int tty_write_stdout(void* buffer, int length)
{
  gcd_t *gcd;

  gcd = get_tty();
  if (gcd == NULL) {
    return VFS_ERROR;
  }
  return gcd->write(gcd, buffer, length);
}

/* Assumes gcd != NULL. */
int tty_set_red(gcd_t* gcd)
{
  static const char red[7] = "\x1b[31;1m";

  if (gcd->write(gcd, red, 7) != 7) {
    return VFS_ERROR;
  }
  return VFS_OK;
}

/* Assumes gcd != NULL. */
int tty_reset_red(gcd_t* gcd)
{
  static const char reset[4] = "\x1b[0m";

  if (gcd->write(gcd, reset, 4) != 4) {
    return VFS_ERROR;
  }
  return VFS_OK;
}

int tty_write_stderr(void* buffer, int length)
{
  gcd_t *gcd;
  int res;

  gcd = get_tty();
  if (gcd == NULL) {
    return VFS_ERROR;
  }

  if (tty_set_red(gcd) != VFS_OK) {
    return VFS_ERROR;
  }

  res = gcd->write(gcd, buffer, length);

  if (tty_reset_red(gcd) != VFS_OK) {
    return VFS_ERROR;
  }

  return res;
}

/**
 * Reads at most length bytes from the file identified by filehandle into the
 * specified buffer, starting at the current position, and advancing the file
 * position.
 *
 * @return The number of bytes actually read. It is invalid to read from
 * FILEHANDLE_STDOUT or FILEHANDLE_STDERR. In this case, VFS_INVALID_PARAMS is
 * returned.
 */
int io_read(openfile_t file, void* buffer, int length)
{
  int res;

  switch(file) {
  case FILEHANDLE_STDIN:
    res = tty_read(buffer, length);
    break;

  case FILEHANDLE_STDOUT:
  case FILEHANDLE_STDERR:
    res = VFS_INVALID_PARAMS;
    break;

  default:
    file -= 3;
    if (!process_has_open_file(file)) {
      res = VFS_NOT_OPEN_IN_PROCESS;
    } else {
      res = vfs_read(file, buffer, length);
    }
  }

  return res;
}

/**
 * Writes at most length bytes from the specified buffer to the file identified
 * by filehandle, starting at the current position, and advancing the position.
 *
 * @return The number of bytes actually written. It is invalid to write to
 * FILEHANDLE_STDIN. In this case, VFS_INVALID_PARAMS is returned.
 */
int io_write(int file, void* buffer, int length)
{
  int res;

  switch(file) {
  case FILEHANDLE_STDIN:
    res = VFS_INVALID_PARAMS;
    break;

  case FILEHANDLE_STDOUT:
    res = tty_write_stdout(buffer, length);
    break;

  case FILEHANDLE_STDERR:
    res = tty_write_stderr(buffer, length);
    break;

  default:
    file -= 3;
    if (!process_has_open_file(file)) {
      res = VFS_NOT_OPEN_IN_PROCESS;
    } else {
      res = vfs_write(file, buffer, length);
    }
  }

  return res;
}

openfile_t io_open(const char* pathname)
{
  openfile_t file = vfs_open(pathname);
  if (file < 0) {
    return file;
  }
  else {
    process_add_file(file);
    /* Don't return a value that can be mistaken for stdin, stdout, or
       stderr. */
    return file + 3;
  }
}

int io_close(openfile_t file) {
  file -= 3;
  if (!process_has_open_file(file)) return VFS_NOT_OPEN_IN_PROCESS;
  process_remove_file(file);
  return vfs_close(file);
}

int io_seek(openfile_t file, int offset) {
  file -= 3;
  if (!process_has_open_file(file)) return VFS_NOT_OPEN_IN_PROCESS;
  return vfs_seek(file, offset);
}

int io_tell(openfile_t file) {
  file -= 3;
  if (!process_has_open_file(file)) return VFS_NOT_OPEN_IN_PROCESS;
  return vfs_tell(file);
}

int io_create(const char* pathname, int size) {
  return vfs_create(pathname, size);
}

int io_remove(const char* pathname) {
  return vfs_remove(pathname);
}

/** @} */
