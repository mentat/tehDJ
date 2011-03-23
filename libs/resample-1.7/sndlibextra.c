/* sndlibextra.c

   An interface to the low-level sndlib functions contained in sndlib/headers.c
   and sndlib/io.c, designed for use by the resample program.
   (sndlibextra.c is a stripped-down version of sndlibsupport.c in RTcmix.) 

   - John Gibson (jgg9c@virginia.edu)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
//#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include "sndlibextra.h"

/* #define NDEBUG */     /* define to disable asserts */
#if 0
static int open_rd_or_rdwr(char *, int);
#ifdef UNUSED
static int get_current_header_raw_comment(int, char **);
#endif



/* ---------------------------------------------------- sndlib_create --- */
/* Creates a new file and writes a header with the given characteristics.
   <type> is a sndlib constant for header type (e.g. MUS_AIFF).
   <format> is a sndlib constant for sound data format (e.g. snd_16_linear).
   (These constants are defined in sndlib.h.) Caller is responsible for
   checking that the header type and data format are compatible, and that
   the header type is one that sndlib can write (is WRITEABLE_HEADER_TYPE()).
   Writes no comment.

   NOTE: This will truncate an existing file with <sfname>, so check first!

   On success, returns a standard file descriptor, and leaves the file
   position pointer at the end of the header.
   On failure, returns -1. Caller can check errno then.
*/
#if 0
int
sndlib_create(char *sfname, int type, int format, int srate, int chans, 
	      char *comment)
{
   int  fd, loc;

   assert(sfname != NULL && strlen(sfname) <= FILENAME_MAX);

   /* make sure relevant parts of sndlib are initialized */
   mus_header_initialize();
   //   mus_create_descriptors();

   fd = open(sfname, O_RDWR | O_CREAT | O_TRUNC, 0666);
   if (fd == -1)
      return -1;

   if (sndlib_write_header(fd, 0, type, format, srate, chans,
                                                         comment, &loc) == -1)
      return -1;

   mus_file_open_descriptors(fd, format, mus_header_data_format_to_bytes_per_sample(),loc);

   return fd;
}
#endif


/* -------------------------------------------------- open_rd_or_rdwr --- */
/* Does the dirty work for sndlib_open_read and sndlib_open_write (below).
*/
#if 0
static int
open_rd_or_rdwr(char *sfname, int accesstype)
{
   int fd, format, loc;

   assert(sfname != NULL && strlen(sfname) <= FILENAME_MAX);

   //   mus_create_descriptors();

   fd = open(sfname, accesstype);
   if (fd == -1)
      return -1;

   if (sndlib_read_header(fd) == -1)
      return -1;

   format = mus_header_format();
   loc = mus_header_data_location();

   mus_file_open_descriptors(fd, format, mus_header_data_format_to_bytes_per_sample(),loc);

   if (lseek(fd, loc, SEEK_SET) == -1) {
      perror("open_rd_or_rdwr: lseek");
      return -1;
   }
   return fd;
}
#endif

/* ------------------------------------------------- sndlib_open_read --- */
/* Returns a new file descriptor for <sfname>, opened read-only, and reads
   its header into the sndlib header buffer. After this, the caller can
   use sndlib functions (like mus_header_type, mus_data_format, etc.)
   to gather information from the header.

   Leaves file position pointer at start of sound data.

   The caller should check that this is actually a sound file by using
   the NOT_A_SOUND_FILE macro (sndlibsupport.h).
   The caller might want to check first that this file exists.
*/
int
sndlib_open_read(char *sfname)
{
   return open_rd_or_rdwr(sfname, O_RDONLY);
}


/* ------------------------------------------------- sndlib_open_write --- */
/* Just like sndlib_open_read, except that it opens the file for read/write
   access. See the comments at sndlib_open_read for more.
*/
int
sndlib_open_write(char *sfname)
{
   return open_rd_or_rdwr(sfname, O_RDWR);
}


/* ----------------------------------------------------- sndlib_close --- */
/* Closes the file descriptor <fd> and, if <update> is true, updates its
   header with the number of samples (not frames) contained in the file.
   <type> is the type of header; <format> is the data format of the samples.
   If <update> is false, caller can pass any value for the last 3 args.

   Returns 0 on success; -1 on failure (check errno for reason).
*/
#if 0
int
sndlib_close(int fd, int update, int type, int format, int nsamps)
{
   assert(fd >= 0);

   if (update) {
      int sound_bytes;

      assert(format > 0 && WRITEABLE_HEADER_TYPE(type));

      sound_bytes = nsamps * mus_data_format_to_bytes_per_sample(format);

      /* keep going even if this fails */
      sndlib_set_header_data_size(fd, type, sound_bytes);
   }

   mus_file_close_descriptors(fd);

   return close(fd);
}
#endif


/* ----------------------------------------------- sndlib_read_header --- */
/* Reads the header of <fd> into the sndlib header buffer, so that we then
   can call sndlib functions (like mus_header_type, mus_data_format, etc.)
   to gather information from the header.

   When a header has been read into the header buffer, we call it the
   "current header". Any function in this file with "current_header" as
   part of its name depends on a previous call to sndlib_read_header.
*/
#if 0
int
sndlib_read_header(int fd)
{
   assert(fd >= 0);

   /* Init the part of sndlib we need -- only inits once. */
   mus_header_initialize();

   if (lseek(fd, 0, SEEK_SET) == -1)
      return -1;

   mus_header_read_with_fd(fd);      /* NOTE: sndlib doesn't return status! */

   return 0;
}
#endif


/* ---------------------------------------------- sndlib_write_header --- */
/* (a wrapper for c_write_header_with_fd in sndlib/headers.c)

   Writes a header to the newly created sound file opened as <fd>. If
   <data_location> is not NULL, sets the value it references to the offset
   of the start of sound data in the file. (This is computed by sndlib
   during the header write.)

   Assumes that <header_type> is one writeable by sndlib. (Find out in advance
   by using the WRITEABLE_HEADER_TYPE macro.) Caller is responsible for knowing
   if <header_type> supports <data_format> (including endianness concerns). If
   AIFF header type, make sure sndlib knows whether it's writing AIFF or AIFC
   (by calling sndlib_current_header_match_aiff_flavor (below) or the sndlib
   set_aifc_header function directly).

   CAUTION: This will write over the header of an existing sound file!
   This would almost certainly change the sound data start location, which
   might cause sound data loss or corruption (e.g., channel swapping or bad
   sample word alignment).

   IMPORTANT: Before closing this sound file, you must update the header's
   idea of how much sound data is in the file, by calling
   sndlib_set_header_data_size.

   On success, leaves file position pointer at end of header and returns 0;
   Otherwise, returns -1.
*/
#if 0
int
sndlib_write_header(int  fd,
                    int  header_exists,      /* unused in this version */
                    int  header_type,
                    int  data_format,
                    int  srate,
                    int  chans,
                    char *comment,
                    int  *data_location)
{
   int  result, comment_len = 0;

   assert(fd >= 0 && header_type > -1 && data_format > 0);

   if (data_location)
      *data_location = 0;

   if (comment)
      comment_len = strlen(comment);

   if (lseek(fd, 0, SEEK_SET) == -1) {
      perror("sndlib_write_header: lseek");
      return -1;
   }

   mus_header_initialize();             /* in case this hasn't been done yet */

   /* Hopefully this succeeds, because sndlib doesn't really tell us! */
   result = mus_header_write_with_fd(fd, header_type, srate, chans, 0, 0,
                                   data_format, comment ? comment : "",
                                   comment_len);
   if (result == -1) {     /* as of sndlib-5.5, only if header_type is wrong */
      fprintf(stderr, "sndlib_write_header: Can't write header\n");
      return -1;
   }

   /* Data location computed during header write. */
   if (data_location)
      *data_location = mus_header_data_location();

   return 0;
}
#endif


/* -------------------------------------- sndlib_set_header_data_size --- */
/* Update the header of file opened as <fd>, of type <header_type>, with
   the number of bytes of sound data written to the file, <sound_bytes).
*/
int
sndlib_set_header_data_size(int fd, int header_type, int sound_bytes)
{
   assert(fd >= 0 && header_type >= 0 && sound_bytes >= 0);

   /* Note: as of sndlib-5.5, this doesn't require us to make sure
      file's header is in sndlib header buffer. Nor do we have to
      seek to the beginning of the file first.
   */
   return mus_header_update_with_fd(fd, header_type, sound_bytes);
}


#ifdef UNUSED
/* ----------------------------------- get_current_header_raw_comment --- */
/* Reads the raw header comment string from the file opened as <fd>.
   Passes back the malloc'd comment in <rawcomment>. Caller should free.

   Returns the length (in bytes) of the comment space allocated in the
   header, or -1 if error. Note that the allocated space may be larger
   than the strlen length of the comment string. <rawcomment> is
   allocated len + 1 bytes;

   Assumes the header we want has already been read into the sndlib
   header buffer (sndlib_read_header).

   Leaves the file position pointer at the end of the comment.
*/
static int
get_current_header_raw_comment(int fd, char **rawcomment)
{
   int   start, end, len, bytes;

   assert(fd >= 0 && rawcomment != NULL);

   *rawcomment = NULL;

   start = mus_header_comment_start();
   assert(start >= 0 && start < 3000);

   end = mus_header_comment_end();
   assert(end >= start);

   len = end - start;

   if (len > 0)
      len++;
   else
      len = 0;

   if (len > 0) {
      *rawcomment = (char *)calloc(len + 1, sizeof(char)); /* incl term NULL */
      if (*rawcomment == NULL) {
         perror("get_current_header_raw_comment: calloc");
         return -1;
      }

      if (lseek(fd, start, SEEK_SET) == -1) {
         perror("get_current_header_raw_comment: lseek");
         return -1;
      }

      bytes = read(fd, *rawcomment, len);

      /* we should've read the amount sndlib said is there */
      if (bytes < len) {
         fprintf(stderr, "get_current_header_raw_comment: Read failed\n");
         return -1;
      }
   }
   /* else no comment text at all (not an error) */

   return len;
}
#endif /* UNUSED */


/* -------------------------- sndlib_current_header_match_aiff_flavor --- */
/* Sndlib doesn't make it easy to distinguish between AIFF and AIFC flavors
   of header type AIFF_sound_file. We can set the one we want by using
   set_aifc_header(), but what if we're using an existing header?

   sndlib_current_header_match_aiff_flavor finds out which flavor the current
   header is, and calls set_aifc_header so that a subsequent call to one of
   the sndlib header-writing functions will keep the same flavor.
 
   Assumes the header we want has already been read into the sndlib
   header buffer (sndlib_read_header), and that we know this
   header to be of type AIFF_sound_file.
*/
void
sndlib_current_header_match_aiff_flavor()
{
   unsigned char aifc[4] = {'A','I','F','C'};

   /* This is as complicated as it is, because it needs to work
      regardless of host byte-order.
   */
   int is_aifc = (mus_header_type_specifier()
		  == mus_char_to_uninterpreted_int((unsigned char *)aifc));
   mus_header_set_aifc(is_aifc);
}


/* ------------------------------------ sndlib_current_header_is_aifc --- */
/* Is the sound file header currently in sndlib's header buffer of type AIFC?
*/
int
sndlib_current_header_is_aifc()
{
   unsigned char aifc[4] = {'A','I','F','C'};

   return ( (mus_header_type() == MUS_AIFF)
         && (mus_header_type_specifier()
                          == mus_char_to_uninterpreted_int((unsigned char *)aifc)) );
}


/* ------------------------------------------ sndlib_allocate_buffers --- */
/* Allocate the multi-dimensional array required by sndlib I/O functions.
   Returns an array of <nchans> arrays of <nframes> integers. The memory
   is cleared. If the return value is NULL, check errno.
*/
#endif
int **
sndlib_allocate_buffers(int nchans, int nframes)
{
   int **bufs, n;

   assert(nchans > 0 && nframes > 0);

   bufs = (int **)calloc(nchans, sizeof(int *));
   if (bufs == NULL)
      return NULL;

   for (n = 0; n < nchans; n++) {
      bufs[n] = (int *)calloc(nframes, sizeof(int));
      if (bufs[n] == NULL) {
         sndlib_free_buffers(bufs, nchans);
         return NULL;
      }
   }

   return bufs;
}


/* ---------------------------------------------- sndlib_free_buffers --- */
/* Free the multi-dimensional array <bufs> with <nchans> elements.
*/
void
sndlib_free_buffers(int **bufs, int nchans)
{
   int n;

   assert(bufs != NULL);

   for (n = 0; n < nchans; n++)
      if (bufs[n])
         free(bufs[n]);
   free(bufs);
}


