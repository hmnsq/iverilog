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

بیرونی FILE* vpi_trace;

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

تعریفنوع ساختار mcd_entry {
	FILE *fp;
	نویسه *filename;
} mcd_entry_s;
ایستا mcd_entry_s mcd_table[31];
ایستا mcd_entry_s *fd_table = پوچ;
ایستا unsigned fd_table_len = 0;

ایستا FILE* logfile;

منطقی vpip_mcd0_disable = نادرست;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
تهی vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*اندازه(mcd_entry_s)));
      برای (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = پوچ;
	    fd_table[idx].filename = پوچ;
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
تهی vpi_mcd_delete(تهی)
{
      آزاد(mcd_table[0].filename);
      mcd_table[0].filename = پوچ;
      mcd_table[0].fp = پوچ;

      آزاد(fd_table[0].filename);
      fd_table[0].filename = پوچ;
      fd_table[0].fp = پوچ;

      آزاد(fd_table[1].filename);
      fd_table[1].filename = پوچ;
      fd_table[1].fp = پوچ;

      آزاد(fd_table[2].filename);
      fd_table[2].filename = پوچ;
      fd_table[2].fp = پوچ;

      آزاد(fd_table);
      fd_table = پوچ;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
بیرونی "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      صحیح rc = 0;

      اگر (IS_MCD(mcd)) {
	    اگر (mcd & 1) rc |= 1;
	    برای(صحیح i = 1; i < 31; i++) {
		  اگر ((mcd>>i) & 1) {
			اگر (mcd_table[i].fp) {
			      اگر (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      آزاد(mcd_table[i].filename);
			      mcd_table[i].fp = پوچ;
			      mcd_table[i].filename = پوچ;
			} وگرنه {
			      rc |= 1<<i;
			}
		  }
	    }
      } وگرنه {
	    unsigned idx = FD_IDX(mcd);
	    اگر (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  اگر (fclose(fd_table[idx].fp)) rc = mcd;
		  آزاد(fd_table[idx].filename);
		  fd_table[idx].fp = پوچ;
		  fd_table[idx].filename = پوچ;
	    } وگرنه rc = mcd;
      }
      بازگشت rc;
}

بیرونی "C" نویسه *vpi_mcd_name(PLI_UINT32 mcd)
{
	اگر (IS_MCD(mcd)) {
		برای(صحیح i = 0; i < 31; i++) {
			اگر((mcd>>i) & 1)
				بازگشت mcd_table[i].filename;
		}
	} وگرنه {
		unsigned idx = FD_IDX(mcd);
		اگر (idx < fd_table_len)
			بازگشت fd_table[idx].filename;
	}
	بازگشت پوچ;
}

بیرونی "C" PLI_UINT32 vpi_mcd_open(نویسه *name)
{
	صحیح i;

	برای(i = 0; i < 31; i++) {
		اگر(mcd_table[i].filename == پوچ)
			برو got_entry;
	}
	بازگشت 0;  /* too many open mcd's */

got_entry:
#اگر defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#وگرنه
	اگر (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	وگرنه
		mcd_table[i].fp = fopen("nul", "w");
#endif
	اگر(mcd_table[i].fp == پوچ)
		بازگشت 0;
	mcd_table[i].filename = strdup(name);

	اگر (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	بازگشت 1<<i;
}

بیرونی "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ثابت نویسه*fmt, va_list ap)
{
      نویسه buffer[4096];
      نویسه *buf_ptr = buffer;
      صحیح rc = 0;
      منطقی need_free = نادرست;
      va_list saved_ap;

      اگر (!IS_MCD(mcd)) بازگشت EOF;

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned صحیح)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, اندازه buffer, fmt, ap);
      أكد(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      اگر ((unsigned) rc >= اندازه buffer) {
	    buf_ptr = static_cast<نویسه *>(malloc(rc + 1));
	    need_free = درست;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      برای(صحیح i = 0; i < 31; i++) {
	    اگر((mcd>>i) & 1) {
		  اگر(mcd_table[i].fp) {
			اگر (i == 0) {
			      اگر (logfile)
				    fputs(buf_ptr, logfile);
			      اگر (vpip_mcd0_disable)
				    ادامه;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } وگرنه {
			rc = EOF;
		  }
	    }
      }
      اگر (need_free) آزاد(buf_ptr);

      بازگشت rc;
}

بیرونی "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ثابت نویسه *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      صحیح r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      بازگشت r;
}

بیرونی "C" تهی vpip_mcd_rawwrite(PLI_UINT32 mcd, ثابت نویسه*buf, size_t cnt)
{
      اگر (!IS_MCD(mcd)) بازگشت;

      برای(صحیح idx = 0; idx < 31; idx += 1) {
	    اگر (((mcd>>idx) & 1) == 0)
		  ادامه;

	    اگر (mcd_table[idx].fp == 0)
		  ادامه;

	    اگر (idx == 0) {
		  اگر (logfile)
			fwrite(buf, 1, cnt, logfile);
		  اگر (vpip_mcd0_disable)
			ادامه;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

بیرونی "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	صحیح rc = 0;

	اگر (IS_MCD(mcd)) {
		برای(صحیح i = 0; i < 31; i++) {
			اگر((mcd>>i) & 1) {
				اگر (i == 0) {
				      اگر (logfile)
					    fflush(logfile);
				      اگر (vpip_mcd0_disable)
					    ادامه;
				}
				اگر (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} وگرنه {
		unsigned idx = FD_IDX(mcd);
		اگر (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	بازگشت rc;
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
بیرونی "C" PLI_INT32 vpi_fopen(ثابت نویسه*name, ثابت نویسه*mode)
{
      unsigned i;
      برای (i = 0; i < fd_table_len; i += 1) {
	    اگر (fd_table[i].filename == پوچ) برو got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      اگر (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    بازگشت 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*اندازه(mcd_entry_s)));
      برای (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = پوچ;
	    fd_table[idx].filename = پوچ;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#وگرنه // Changed for MSVC++ so vpi/pr723.v will pass.
	  اگر(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  وگرنه
		fd_table[i].fp = fopen("nul", mode);
#endif
      اگر (fd_table[i].fp == پوچ) بازگشت 0;
      fd_table[i].filename = strdup(name);
      بازگشت ((1U<<31)|i);
}

بیرونی "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      اگر (IS_MCD(fd)) بازگشت پوچ;

	// Only know about fd_table_len indices
      اگر (FD_IDX(fd) >= fd_table_len) بازگشت پوچ;

      بازگشت fd_table[FD_IDX(fd)].fp;
}
