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

ಬಾಹ್ಯ FILE* vpi_trace;

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ mcd_entry {
	FILE *fp;
	ವರ್ಣ *filename;
} mcd_entry_s;
ಸ್ಥಿರ mcd_entry_s mcd_table[31];
ಸ್ಥಿರ mcd_entry_s *fd_table = ರಿಕ್ತ;
ಸ್ಥಿರ unsigned fd_table_len = 0;

ಸ್ಥಿರ FILE* logfile;

ತಾರ್ಕಿಕ vpip_mcd0_disable = ಅಸತ್ಯ;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
ಶೂನ್ಯ vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*ಪರಿಮಾಣ(mcd_entry_s)));
      ಪ್ರತಿ (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ರಿಕ್ತ;
	    fd_table[idx].filename = ರಿಕ್ತ;
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
ಶೂನ್ಯ vpi_mcd_delete(ಶೂನ್ಯ)
{
      ಮುಕ್ತ(mcd_table[0].filename);
      mcd_table[0].filename = ರಿಕ್ತ;
      mcd_table[0].fp = ರಿಕ್ತ;

      ಮುಕ್ತ(fd_table[0].filename);
      fd_table[0].filename = ರಿಕ್ತ;
      fd_table[0].fp = ರಿಕ್ತ;

      ಮುಕ್ತ(fd_table[1].filename);
      fd_table[1].filename = ರಿಕ್ತ;
      fd_table[1].fp = ರಿಕ್ತ;

      ಮುಕ್ತ(fd_table[2].filename);
      fd_table[2].filename = ರಿಕ್ತ;
      fd_table[2].fp = ರಿಕ್ತ;

      ಮುಕ್ತ(fd_table);
      fd_table = ರಿಕ್ತ;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
ಬಾಹ್ಯ "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      ಪೂರ್ಣಾಙ್ಕ rc = 0;

      ಯದಿ (IS_MCD(mcd)) {
	    ಯದಿ (mcd & 1) rc |= 1;
	    ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 1; i < 31; i++) {
		  ಯದಿ ((mcd>>i) & 1) {
			ಯದಿ (mcd_table[i].fp) {
			      ಯದಿ (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      ಮುಕ್ತ(mcd_table[i].filename);
			      mcd_table[i].fp = ರಿಕ್ತ;
			      mcd_table[i].filename = ರಿಕ್ತ;
			} ಅನ್ಯಥಾ {
			      rc |= 1<<i;
			}
		  }
	    }
      } ಅನ್ಯಥಾ {
	    unsigned idx = FD_IDX(mcd);
	    ಯದಿ (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  ಯದಿ (fclose(fd_table[idx].fp)) rc = mcd;
		  ಮುಕ್ತ(fd_table[idx].filename);
		  fd_table[idx].fp = ರಿಕ್ತ;
		  fd_table[idx].filename = ರಿಕ್ತ;
	    } ಅನ್ಯಥಾ rc = mcd;
      }
      ಪ್ರತ್ಯಾಗಮ rc;
}

ಬಾಹ್ಯ "C" ವರ್ಣ *vpi_mcd_name(PLI_UINT32 mcd)
{
	ಯದಿ (IS_MCD(mcd)) {
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < 31; i++) {
			ಯದಿ((mcd>>i) & 1)
				ಪ್ರತ್ಯಾಗಮ mcd_table[i].filename;
		}
	} ಅನ್ಯಥಾ {
		unsigned idx = FD_IDX(mcd);
		ಯದಿ (idx < fd_table_len)
			ಪ್ರತ್ಯಾಗಮ fd_table[idx].filename;
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಬಾಹ್ಯ "C" PLI_UINT32 vpi_mcd_open(ವರ್ಣ *name)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ(i = 0; i < 31; i++) {
		ಯದಿ(mcd_table[i].filename == ರಿಕ್ತ)
			ಗಚ್ಛ got_entry;
	}
	ಪ್ರತ್ಯಾಗಮ 0;  /* too many open mcd's */

got_entry:
#ಯದಿ defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#ಅನ್ಯಥಾ
	ಯದಿ (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	ಅನ್ಯಥಾ
		mcd_table[i].fp = fopen("nul", "w");
#endif
	ಯದಿ(mcd_table[i].fp == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ 0;
	mcd_table[i].filename = strdup(name);

	ಯದಿ (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	ಪ್ರತ್ಯಾಗಮ 1<<i;
}

ಬಾಹ್ಯ "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ಧ್ರುವ ವರ್ಣ*fmt, va_list ap)
{
      ವರ್ಣ buffer[4096];
      ವರ್ಣ *buf_ptr = buffer;
      ಪೂರ್ಣಾಙ್ಕ rc = 0;
      ತಾರ್ಕಿಕ need_free = ಅಸತ್ಯ;
      va_list saved_ap;

      ಯದಿ (!IS_MCD(mcd)) ಪ್ರತ್ಯಾಗಮ EOF;

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned ಪೂರ್ಣಾಙ್ಕ)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, ಪರಿಮಾಣ buffer, fmt, ap);
      ಪ್ರತಿಜ್ಞಾ(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      ಯದಿ ((unsigned) rc >= ಪರಿಮಾಣ buffer) {
	    buf_ptr = static_cast<ವರ್ಣ *>(malloc(rc + 1));
	    need_free = ಸತ್ಯ;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < 31; i++) {
	    ಯದಿ((mcd>>i) & 1) {
		  ಯದಿ(mcd_table[i].fp) {
			ಯದಿ (i == 0) {
			      ಯದಿ (logfile)
				    fputs(buf_ptr, logfile);
			      ಯದಿ (vpip_mcd0_disable)
				    ಅನುವರ್ತ;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } ಅನ್ಯಥಾ {
			rc = EOF;
		  }
	    }
      }
      ಯದಿ (need_free) ಮುಕ್ತ(buf_ptr);

      ಪ್ರತ್ಯಾಗಮ rc;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ಪೂರ್ಣಾಙ್ಕ r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      ಪ್ರತ್ಯಾಗಮ r;
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_mcd_rawwrite(PLI_UINT32 mcd, ಧ್ರುವ ವರ್ಣ*buf, size_t cnt)
{
      ಯದಿ (!IS_MCD(mcd)) ಪ್ರತ್ಯಾಗಮ;

      ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < 31; idx += 1) {
	    ಯದಿ (((mcd>>idx) & 1) == 0)
		  ಅನುವರ್ತ;

	    ಯದಿ (mcd_table[idx].fp == 0)
		  ಅನುವರ್ತ;

	    ಯದಿ (idx == 0) {
		  ಯದಿ (logfile)
			fwrite(buf, 1, cnt, logfile);
		  ಯದಿ (vpip_mcd0_disable)
			ಅನುವರ್ತ;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	ಯದಿ (IS_MCD(mcd)) {
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < 31; i++) {
			ಯದಿ((mcd>>i) & 1) {
				ಯದಿ (i == 0) {
				      ಯದಿ (logfile)
					    fflush(logfile);
				      ಯದಿ (vpip_mcd0_disable)
					    ಅನುವರ್ತ;
				}
				ಯದಿ (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} ಅನ್ಯಥಾ {
		unsigned idx = FD_IDX(mcd);
		ಯದಿ (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
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
ಬಾಹ್ಯ "C" PLI_INT32 vpi_fopen(ಧ್ರುವ ವರ್ಣ*name, ಧ್ರುವ ವರ್ಣ*mode)
{
      unsigned i;
      ಪ್ರತಿ (i = 0; i < fd_table_len; i += 1) {
	    ಯದಿ (fd_table[i].filename == ರಿಕ್ತ) ಗಚ್ಛ got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      ಯದಿ (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*ಪರಿಮಾಣ(mcd_entry_s)));
      ಪ್ರತಿ (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ರಿಕ್ತ;
	    fd_table[idx].filename = ರಿಕ್ತ;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#ಅನ್ಯಥಾ // Changed for MSVC++ so vpi/pr723.v will pass.
	  ಯದಿ(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  ಅನ್ಯಥಾ
		fd_table[i].fp = fopen("nul", mode);
#endif
      ಯದಿ (fd_table[i].fp == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ 0;
      fd_table[i].filename = strdup(name);
      ಪ್ರತ್ಯಾಗಮ ((1U<<31)|i);
}

ಬಾಹ್ಯ "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      ಯದಿ (IS_MCD(fd)) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	// Only know about fd_table_len indices
      ಯದಿ (FD_IDX(fd) >= fd_table_len) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

      ಪ್ರತ್ಯಾಗಮ fd_table[FD_IDX(fd)].fp;
}
