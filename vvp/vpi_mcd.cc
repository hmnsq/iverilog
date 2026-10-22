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

خارجي FILE* vpi_trace;

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

تعريفنوع بنية mcd_entry {
	FILE *fp;
	حرف *filename;
} mcd_entry_s;
ساكن mcd_entry_s mcd_table[31];
ساكن mcd_entry_s *fd_table = عدم;
ساكن unsigned fd_table_len = 0;

ساكن FILE* logfile;

منطقي vpip_mcd0_disable = خطأ;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
فراغ vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*حجم(mcd_entry_s)));
      لكل (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = عدم;
	    fd_table[idx].filename = عدم;
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
فراغ vpi_mcd_delete(فراغ)
{
      حرر(mcd_table[0].filename);
      mcd_table[0].filename = عدم;
      mcd_table[0].fp = عدم;

      حرر(fd_table[0].filename);
      fd_table[0].filename = عدم;
      fd_table[0].fp = عدم;

      حرر(fd_table[1].filename);
      fd_table[1].filename = عدم;
      fd_table[1].fp = عدم;

      حرر(fd_table[2].filename);
      fd_table[2].filename = عدم;
      fd_table[2].fp = عدم;

      حرر(fd_table);
      fd_table = عدم;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
خارجي "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      صحيح rc = 0;

      𐡄𐡍 (IS_MCD(mcd)) {
	    𐡄𐡍 (mcd & 1) rc |= 1;
	    لكل(صحيح i = 1; i < 31; i++) {
		  𐡄𐡍 ((mcd>>i) & 1) {
			𐡄𐡍 (mcd_table[i].fp) {
			      𐡄𐡍 (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      حرر(mcd_table[i].filename);
			      mcd_table[i].fp = عدم;
			      mcd_table[i].filename = عدم;
			} وإلا {
			      rc |= 1<<i;
			}
		  }
	    }
      } وإلا {
	    unsigned idx = FD_IDX(mcd);
	    𐡄𐡍 (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  𐡄𐡍 (fclose(fd_table[idx].fp)) rc = mcd;
		  حرر(fd_table[idx].filename);
		  fd_table[idx].fp = عدم;
		  fd_table[idx].filename = عدم;
	    } وإلا rc = mcd;
      }
      أرجع rc;
}

خارجي "C" حرف *vpi_mcd_name(PLI_UINT32 mcd)
{
	𐡄𐡍 (IS_MCD(mcd)) {
		لكل(صحيح i = 0; i < 31; i++) {
			𐡄𐡍((mcd>>i) & 1)
				أرجع mcd_table[i].filename;
		}
	} وإلا {
		unsigned idx = FD_IDX(mcd);
		𐡄𐡍 (idx < fd_table_len)
			أرجع fd_table[idx].filename;
	}
	أرجع عدم;
}

خارجي "C" PLI_UINT32 vpi_mcd_open(حرف *name)
{
	صحيح i;

	لكل(i = 0; i < 31; i++) {
		𐡄𐡍(mcd_table[i].filename == عدم)
			اذهب got_entry;
	}
	أرجع 0;  /* too many open mcd's */

got_entry:
#𐡄𐡍 defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#وإلا
	𐡄𐡍 (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	وإلا
		mcd_table[i].fp = fopen("nul", "w");
#endif
	𐡄𐡍(mcd_table[i].fp == عدم)
		أرجع 0;
	mcd_table[i].filename = strdup(name);

	𐡄𐡍 (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	أرجع 1<<i;
}

خارجي "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ثابت حرف*fmt, va_list ap)
{
      حرف buffer[4096];
      حرف *buf_ptr = buffer;
      صحيح rc = 0;
      منطقي need_free = خطأ;
      va_list saved_ap;

      𐡄𐡍 (!IS_MCD(mcd)) أرجع EOF;

      𐡄𐡍 (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned صحيح)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, حجم buffer, fmt, ap);
      أكد(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      𐡄𐡍 ((unsigned) rc >= حجم buffer) {
	    buf_ptr = static_cast<حرف *>(malloc(rc + 1));
	    need_free = 𐡒𐡔𐡈;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      لكل(صحيح i = 0; i < 31; i++) {
	    𐡄𐡍((mcd>>i) & 1) {
		  𐡄𐡍(mcd_table[i].fp) {
			𐡄𐡍 (i == 0) {
			      𐡄𐡍 (logfile)
				    fputs(buf_ptr, logfile);
			      𐡄𐡍 (vpip_mcd0_disable)
				    استمر;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } وإلا {
			rc = EOF;
		  }
	    }
      }
      𐡄𐡍 (need_free) حرر(buf_ptr);

      أرجع rc;
}

خارجي "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ثابت حرف *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      صحيح r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      أرجع r;
}

خارجي "C" فراغ vpip_mcd_rawwrite(PLI_UINT32 mcd, ثابت حرف*buf, size_t cnt)
{
      𐡄𐡍 (!IS_MCD(mcd)) أرجع;

      لكل(صحيح idx = 0; idx < 31; idx += 1) {
	    𐡄𐡍 (((mcd>>idx) & 1) == 0)
		  استمر;

	    𐡄𐡍 (mcd_table[idx].fp == 0)
		  استمر;

	    𐡄𐡍 (idx == 0) {
		  𐡄𐡍 (logfile)
			fwrite(buf, 1, cnt, logfile);
		  𐡄𐡍 (vpip_mcd0_disable)
			استمر;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

خارجي "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	صحيح rc = 0;

	𐡄𐡍 (IS_MCD(mcd)) {
		لكل(صحيح i = 0; i < 31; i++) {
			𐡄𐡍((mcd>>i) & 1) {
				𐡄𐡍 (i == 0) {
				      𐡄𐡍 (logfile)
					    fflush(logfile);
				      𐡄𐡍 (vpip_mcd0_disable)
					    استمر;
				}
				𐡄𐡍 (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} وإلا {
		unsigned idx = FD_IDX(mcd);
		𐡄𐡍 (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	أرجع rc;
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
خارجي "C" PLI_INT32 vpi_fopen(ثابت حرف*name, ثابت حرف*mode)
{
      unsigned i;
      لكل (i = 0; i < fd_table_len; i += 1) {
	    𐡄𐡍 (fd_table[i].filename == عدم) اذهب got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      𐡄𐡍 (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    أرجع 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*حجم(mcd_entry_s)));
      لكل (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = عدم;
	    fd_table[idx].filename = عدم;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#وإلا // Changed for MSVC++ so vpi/pr723.v will pass.
	  𐡄𐡍(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  وإلا
		fd_table[i].fp = fopen("nul", mode);
#endif
      𐡄𐡍 (fd_table[i].fp == عدم) أرجع 0;
      fd_table[i].filename = strdup(name);
      أرجع ((1U<<31)|i);
}

خارجي "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      𐡄𐡍 (IS_MCD(fd)) أرجع عدم;

	// Only know about fd_table_len indices
      𐡄𐡍 (FD_IDX(fd) >= fd_table_len) أرجع عدم;

      أرجع fd_table[FD_IDX(fd)].fp;
}
