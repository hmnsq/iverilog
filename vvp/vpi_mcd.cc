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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
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
#تعریف_کړه IS_MCD(mcd)	!(static_cast<بینښه>(mcd)>>31&1)
#تعریف_کړه FD_IDX(fd)	((fd)&~(1U<<31))
#تعریف_کړه FD_INCR		32

تعريفنوع بنية mcd_ثبت {
	FILE *fp;
	حرف *filename;
} mcd_ثبت_s;
ساكن mcd_ثبت_s mcd_جدول[31];
ساكن mcd_ثبت_s *fd_جدول = عدم;
ساكن بینښه fd_جدول_len = 0;

ساكن FILE* logfile;

منطقي vpip_mcd0_disable = خطأ;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
فراغ vpip_mcd_init(FILE *لوګاریتم)
{
      fd_جدول_len = FD_INCR;
      fd_جدول = static_cast<mcd_ثبت_s *>(حافظه_ورکړه(fd_جدول_len*حجم(mcd_ثبت_s)));
      لكل (بینښه idx = 0; idx < fd_جدول_len; idx += 1) {
	    fd_جدول[idx].fp = عدم;
	    fd_جدول[idx].filename = عدم;
      }

      mcd_جدول[0].fp = stdout;
      mcd_جدول[0].filename = strdup("stdout");

      fd_جدول[0].fp = stdin;
      fd_جدول[0].filename = strdup("stdin");
      fd_جدول[1].fp = stdout;
      fd_جدول[1].filename = strdup("stdout");
      fd_جدول[2].fp = stderr;
      fd_جدول[2].filename = strdup("stderr");

      logfile = لوګاریتم;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vpi_mcd_delete(فراغ)
{
      حرر(mcd_جدول[0].filename);
      mcd_جدول[0].filename = عدم;
      mcd_جدول[0].fp = عدم;

      حرر(fd_جدول[0].filename);
      fd_جدول[0].filename = عدم;
      fd_جدول[0].fp = عدم;

      حرر(fd_جدول[1].filename);
      fd_جدول[1].filename = عدم;
      fd_جدول[1].fp = عدم;

      حرر(fd_جدول[2].filename);
      fd_جدول[2].filename = عدم;
      fd_جدول[2].fp = عدم;

      حرر(fd_جدول);
      fd_جدول = عدم;
      fd_جدول_len = 0;
}
#د_که_پای

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
خارجي "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      صحيح rc = 0;

      إذا (IS_MCD(mcd)) {
	    إذا (mcd & 1) rc |= 1;
	    لكل(صحيح i = 1; i < 31; i++) {
		  إذا ((mcd>>i) & 1) {
			إذا (mcd_جدول[i].fp) {
			      إذا (fclose(mcd_جدول[i].fp)) rc |= 1<<i;
			      حرر(mcd_جدول[i].filename);
			      mcd_جدول[i].fp = عدم;
			      mcd_جدول[i].filename = عدم;
			} وإلا {
			      rc |= 1<<i;
			}
		  }
	    }
      } وإلا {
	    بینښه idx = FD_IDX(mcd);
	    إذا (idx > 2 && idx < fd_جدول_len && fd_جدول[idx].fp) {
		  إذا (fclose(fd_جدول[idx].fp)) rc = mcd;
		  حرر(fd_جدول[idx].filename);
		  fd_جدول[idx].fp = عدم;
		  fd_جدول[idx].filename = عدم;
	    } وإلا rc = mcd;
      }
      أرجع rc;
}

خارجي "C" حرف *vpi_mcd_name(PLI_UINT32 mcd)
{
	إذا (IS_MCD(mcd)) {
		لكل(صحيح i = 0; i < 31; i++) {
			إذا((mcd>>i) & 1)
				أرجع mcd_جدول[i].filename;
		}
	} وإلا {
		بینښه idx = FD_IDX(mcd);
		إذا (idx < fd_جدول_len)
			أرجع fd_جدول[idx].filename;
	}
	أرجع عدم;
}

خارجي "C" PLI_UINT32 vpi_mcd_open(حرف *name)
{
	صحيح i;

	لكل(i = 0; i < 31; i++) {
		إذا(mcd_جدول[i].filename == عدم)
			اذهب got_ثبت;
	}
	أرجع 0;  /* too many open mcd's */

got_ثبت:
#إذا defined(__GNUC__)
	mcd_جدول[i].fp = fopen(name, "w");
#وإلا
	إذا (strcmp(name, "/dev/null") != 0)
		mcd_جدول[i].fp = fopen(name, "w");
	وإلا
		mcd_جدول[i].fp = fopen("nul", "w");
#د_که_پای
	إذا(mcd_جدول[i].fp == عدم)
		أرجع 0;
	mcd_جدول[i].filename = strdup(name);

	إذا (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	أرجع 1<<i;
}

خارجي "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ثابت حرف*fmt, va_list ap)
{
      حرف بفر[4096];
      حرف *buf_ptr = بفر;
      صحيح rc = 0;
      منطقي need_free = خطأ;
      va_list saved_ap;

      إذا (!IS_MCD(mcd)) أرجع EOF;

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (بینښه صحيح)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(بفر, حجم بفر, fmt, ap);
      أكد(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      إذا ((بینښه) rc >= حجم بفر) {
	    buf_ptr = static_cast<حرف *>(حافظه_ورکړه(rc + 1));
	    need_free = ریښتیا;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      لكل(صحيح i = 0; i < 31; i++) {
	    إذا((mcd>>i) & 1) {
		  إذا(mcd_جدول[i].fp) {
			إذا (i == 0) {
			      إذا (logfile)
				    fputs(buf_ptr, logfile);
			      إذا (vpip_mcd0_disable)
				    استمر;
			}
			fputs(buf_ptr, mcd_جدول[i].fp);
		  } وإلا {
			rc = EOF;
		  }
	    }
      }
      إذا (need_free) حرر(buf_ptr);

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
      إذا (!IS_MCD(mcd)) أرجع;

      لكل(صحيح idx = 0; idx < 31; idx += 1) {
	    إذا (((mcd>>idx) & 1) == 0)
		  استمر;

	    إذا (mcd_جدول[idx].fp == 0)
		  استمر;

	    إذا (idx == 0) {
		  إذا (logfile)
			fwrite(buf, 1, cnt, logfile);
		  إذا (vpip_mcd0_disable)
			استمر;
	    }
	    fwrite(buf, 1, cnt, mcd_جدول[idx].fp);
      }
}

خارجي "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	صحيح rc = 0;

	إذا (IS_MCD(mcd)) {
		لكل(صحيح i = 0; i < 31; i++) {
			إذا((mcd>>i) & 1) {
				إذا (i == 0) {
				      إذا (logfile)
					    fflush(logfile);
				      إذا (vpip_mcd0_disable)
					    استمر;
				}
				إذا (fflush(mcd_جدول[i].fp)) rc |= 1<<i;
			}
		}
	} وإلا {
		بینښه idx = FD_IDX(mcd);
		إذا (idx < fd_جدول_len) rc = fflush(fd_جدول[idx].fp);
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
خارجي "C" PLI_INT32 vpi_fopen(ثابت حرف*name, ثابت حرف*ډېرپېښ)
{
      بینښه i;
      لكل (i = 0; i < fd_جدول_len; i += 1) {
	    إذا (fd_جدول[i].filename == عدم) اذهب got_ثبت;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      إذا (fd_جدول_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    أرجع 0;
      }
      fd_جدول_len += FD_INCR;
      fd_جدول = static_cast<mcd_ثبت_s *>
                 (realloc(fd_جدول, fd_جدول_len*حجم(mcd_ثبت_s)));
      لكل (بینښه idx = i; idx < fd_جدول_len; idx += 1) {
	    fd_جدول[idx].fp = عدم;
	    fd_جدول[idx].filename = عدم;
      }

got_ثبت:
#که_نه_تعریف_شوی _MSC_VER
	  fd_جدول[i].fp = fopen(name, ډېرپېښ);
#وإلا // Changed for MSVC++ so vpi/pr723.v will pass.
	  إذا(strcmp(name, "/dev/null") != 0)
		fd_جدول[i].fp = fopen(name, ډېرپېښ);
	  وإلا
		fd_جدول[i].fp = fopen("nul", ډېرپېښ);
#د_که_پای
      إذا (fd_جدول[i].fp == عدم) أرجع 0;
      fd_جدول[i].filename = strdup(name);
      أرجع ((1U<<31)|i);
}

خارجي "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      إذا (IS_MCD(fd)) أرجع عدم;

	// Only know about fd_table_len indices
      إذا (FD_IDX(fd) >= fd_جدول_len) أرجع عدم;

      أرجع fd_جدول[FD_IDX(fd)].fp;
}
