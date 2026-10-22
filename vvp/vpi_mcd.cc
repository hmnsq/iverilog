/*
 * Copyright (c) 2000-2025 Stephen G. Tell <steve@telltronics.org>
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "vpi_priv.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cassert>
# include  <cstdarg>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  "ivl_alloc.h"

extern FILE* vpi_trace;

/*
 * This table keeps track of the MCD files. Note that there may be
 * only 31 such files, and mcd bit0 (32'h00_00_00_01) is the special
 * standard output file, which may be replicated to a logfile and/or
 * suppressed depending on flags to the command line.
 */

/*
 * MCD/FD manipulation macros
 */
#define IS_MCD(mcd)	!(static_cast<unsigned>(mcd)>>31&1)
#define FD_IDX(fd)	((fd)&~(1U<<31))
#define FD_INCR		32

typedef struct mcd_entry {
	FILE *fp;
	char *filename;
} mcd_entry_s;
static mcd_entry_s mcd_table[31];
static mcd_entry_s *fd_table = NULL;
static unsigned fd_table_len = 0;

static FILE* logfile;

bool vpip_mcd0_disable = false;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
void vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*sizeof(mcd_entry_s)));
      для (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = NULL;
	    fd_table[idx].filename = NULL;
      }

      mcd_table[0].fp = stdout;
      mcd_table[0].filename = strdup("stdout");

      fd_table[0].fp = stdin;
      fd_table[0].filename = strdup("stdin");
      fd_table[1].fp = stdout;
      fd_table[1].filename = strdup("stdout");
      fd_table[2].fp = stderr;
      fd_table[2].filename = strdup("stderr");

      logfile = log;
}

#ifdef CHECK_WITH_VALGRIND
void vpi_mcd_delete(void)
{
      free(mcd_table[0].filename);
      mcd_table[0].filename = NULL;
      mcd_table[0].fp = NULL;

      free(fd_table[0].filename);
      fd_table[0].filename = NULL;
      fd_table[0].fp = NULL;

      free(fd_table[1].filename);
      fd_table[1].filename = NULL;
      fd_table[1].fp = NULL;

      free(fd_table[2].filename);
      fd_table[2].filename = NULL;
      fd_table[2].fp = NULL;

      free(fd_table);
      fd_table = NULL;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
extern "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      ціле rc = 0;

      якщо (IS_MCD(mcd)) {
	    якщо (mcd & 1) rc |= 1;
	    для(ціле i = 1; i < 31; i++) {
		  якщо ((mcd>>i) & 1) {
			якщо (mcd_table[i].fp) {
			      якщо (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      free(mcd_table[i].filename);
			      mcd_table[i].fp = NULL;
			      mcd_table[i].filename = NULL;
			} інакше {
			      rc |= 1<<i;
			}
		  }
	    }
      } інакше {
	    unsigned idx = FD_IDX(mcd);
	    якщо (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  якщо (fclose(fd_table[idx].fp)) rc = mcd;
		  free(fd_table[idx].filename);
		  fd_table[idx].fp = NULL;
		  fd_table[idx].filename = NULL;
	    } інакше rc = mcd;
      }
      повернути rc;
}

extern "C" char *vpi_mcd_name(PLI_UINT32 mcd)
{
	якщо (IS_MCD(mcd)) {
		для(ціле i = 0; i < 31; i++) {
			якщо((mcd>>i) & 1)
				повернути mcd_table[i].filename;
		}
	} інакше {
		unsigned idx = FD_IDX(mcd);
		якщо (idx < fd_table_len)
			повернути fd_table[idx].filename;
	}
	повернути NULL;
}

extern "C" PLI_UINT32 vpi_mcd_open(char *name)
{
	ціле i;

	для(i = 0; i < 31; i++) {
		якщо(mcd_table[i].filename == NULL)
			goto got_entry;
	}
	повернути 0;  /* too many open mcd's */

got_entry:
#якщо defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#інакше
	якщо (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	інакше
		mcd_table[i].fp = fopen("nul", "w");
#endif
	якщо(mcd_table[i].fp == NULL)
		повернути 0;
	mcd_table[i].filename = strdup(name);

	якщо (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	повернути 1<<i;
}

extern "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, const char*fmt, va_list ap)
{
      char buffer[4096];
      char *buf_ptr = buffer;
      ціле rc = 0;
      bool need_free = false;
      va_list saved_ap;

      якщо (!IS_MCD(mcd)) повернути EOF;

      якщо (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned ціле)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, sizeof buffer, fmt, ap);
      assert(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      якщо ((unsigned) rc >= sizeof buffer) {
	    buf_ptr = static_cast<char *>(malloc(rc + 1));
	    need_free = true;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      для(ціле i = 0; i < 31; i++) {
	    якщо((mcd>>i) & 1) {
		  якщо(mcd_table[i].fp) {
			якщо (i == 0) {
			      якщо (logfile)
				    fputs(buf_ptr, logfile);
			      якщо (vpip_mcd0_disable)
				    continue;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } інакше {
			rc = EOF;
		  }
	    }
      }
      якщо (need_free) free(buf_ptr);

      повернути rc;
}

extern "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, const char *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ціле r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      повернути r;
}

extern "C" void vpip_mcd_rawwrite(PLI_UINT32 mcd, const char*buf, size_t cnt)
{
      якщо (!IS_MCD(mcd)) повернути;

      для(ціле idx = 0; idx < 31; idx += 1) {
	    якщо (((mcd>>idx) & 1) == 0)
		  continue;

	    якщо (mcd_table[idx].fp == 0)
		  continue;

	    якщо (idx == 0) {
		  якщо (logfile)
			fwrite(buf, 1, cnt, logfile);
		  якщо (vpip_mcd0_disable)
			continue;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

extern "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	ціле rc = 0;

	якщо (IS_MCD(mcd)) {
		для(ціле i = 0; i < 31; i++) {
			якщо((mcd>>i) & 1) {
				якщо (i == 0) {
				      якщо (logfile)
					    fflush(logfile);
				      якщо (vpip_mcd0_disable)
					    continue;
				}
				якщо (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} інакше {
		unsigned idx = FD_IDX(mcd);
		якщо (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	повернути rc;
}

/*
 * MCD/FD Extensions
 */

/*
 * The vpi_fopen function opens a file with the given path, and
 * returns a file descriptor that includes bit 31 set. This is to
 * differentiate the fd from a mcd descriptor. Note that these
 * descriptors are distinct from the mcd descriptors, so uses a
 * different fd table.
 */
extern "C" PLI_INT32 vpi_fopen(const char*name, const char*mode)
{
      unsigned i;
      для (i = 0; i < fd_table_len; i += 1) {
	    якщо (fd_table[i].filename == NULL) goto got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      якщо (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    повернути 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*sizeof(mcd_entry_s)));
      для (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = NULL;
	    fd_table[idx].filename = NULL;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#інакше // Changed for MSVC++ so vpi/pr723.v will pass.
	  якщо(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  інакше
		fd_table[i].fp = fopen("nul", mode);
#endif
      якщо (fd_table[i].fp == NULL) повернути 0;
      fd_table[i].filename = strdup(name);
      повернути ((1U<<31)|i);
}

extern "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      якщо (IS_MCD(fd)) повернути NULL;

	// Only know about fd_table_len indices
      якщо (FD_IDX(fd) >= fd_table_len) повернути NULL;

      повернути fd_table[FD_IDX(fd)].fp;
}
