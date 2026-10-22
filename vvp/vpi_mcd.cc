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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cassert>
# include  <cstdarg>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  "ivl_alloc.h"

خارجی FILE* vpi_trace;

/*
 * This table keeps track of the MCD files. Note that there may be
 * only 31 such files, and mcd bit0 (32'h00_00_00_01) is the special
 * standard output file, which may be replicated to a logfile and/or
 * suppressed depending on flags to the command line.
 */

/*
 * MCD/FD manipulation macros
 */
#تعریف_کرو IS_MCD(mcd)	!(static_cast<غیرعلامتی>(mcd)>>31&1)
#تعریف_کرو FD_IDX(fd)	((fd)&~(1U<<31))
#تعریف_کرو FD_INCR		32

قسمتعریف ساخت mcd_اندراج {
	FILE *fp;
	حرف *filename;
} mcd_اندراج_s;
ساکن mcd_اندراج_s mcd_جدول[31];
ساکن mcd_اندراج_s *fd_جدول = تہی;
ساکن غیرعلامتی fd_جدول_len = 0;

ساکن FILE* logfile;

منطقی vpip_mcd0_disable = جھوٹ;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
خالی vpip_mcd_init(FILE *لوگارتھم)
{
      fd_جدول_len = FD_INCR;
      fd_جدول = static_cast<mcd_اندراج_s *>(حافظہ_دو(fd_جدول_len*حجم(mcd_اندراج_s)));
      برائے (غیرعلامتی idx = 0; idx < fd_جدول_len; idx += 1) {
	    fd_جدول[idx].fp = تہی;
	    fd_جدول[idx].filename = تہی;
      }

      mcd_جدول[0].fp = stdout;
      mcd_جدول[0].filename = strdup("stdout");

      fd_جدول[0].fp = stdin;
      fd_جدول[0].filename = strdup("stdin");
      fd_جدول[1].fp = stdout;
      fd_جدول[1].filename = strdup("stdout");
      fd_جدول[2].fp = stderr;
      fd_جدول[2].filename = strdup("stderr");

      logfile = لوگارتھم;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vpi_mcd_delete(خالی)
{
      آزاد(mcd_جدول[0].filename);
      mcd_جدول[0].filename = تہی;
      mcd_جدول[0].fp = تہی;

      آزاد(fd_جدول[0].filename);
      fd_جدول[0].filename = تہی;
      fd_جدول[0].fp = تہی;

      آزاد(fd_جدول[1].filename);
      fd_جدول[1].filename = تہی;
      fd_جدول[1].fp = تہی;

      آزاد(fd_جدول[2].filename);
      fd_جدول[2].filename = تہی;
      fd_جدول[2].fp = تہی;

      آزاد(fd_جدول);
      fd_جدول = تہی;
      fd_جدول_len = 0;
}
#اگر_ختم

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
خارجی "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      صحیح rc = 0;

      اگر (IS_MCD(mcd)) {
	    اگر (mcd & 1) rc |= 1;
	    برائے(صحیح i = 1; i < 31; i++) {
		  اگر ((mcd>>i) & 1) {
			اگر (mcd_جدول[i].fp) {
			      اگر (fclose(mcd_جدول[i].fp)) rc |= 1<<i;
			      آزاد(mcd_جدول[i].filename);
			      mcd_جدول[i].fp = تہی;
			      mcd_جدول[i].filename = تہی;
			} ورنہ {
			      rc |= 1<<i;
			}
		  }
	    }
      } ورنہ {
	    غیرعلامتی idx = FD_IDX(mcd);
	    اگر (idx > 2 && idx < fd_جدول_len && fd_جدول[idx].fp) {
		  اگر (fclose(fd_جدول[idx].fp)) rc = mcd;
		  آزاد(fd_جدول[idx].filename);
		  fd_جدول[idx].fp = تہی;
		  fd_جدول[idx].filename = تہی;
	    } ورنہ rc = mcd;
      }
      واپس rc;
}

خارجی "C" حرف *vpi_mcd_name(PLI_UINT32 mcd)
{
	اگر (IS_MCD(mcd)) {
		برائے(صحیح i = 0; i < 31; i++) {
			اگر((mcd>>i) & 1)
				واپس mcd_جدول[i].filename;
		}
	} ورنہ {
		غیرعلامتی idx = FD_IDX(mcd);
		اگر (idx < fd_جدول_len)
			واپس fd_جدول[idx].filename;
	}
	واپس تہی;
}

خارجی "C" PLI_UINT32 vpi_mcd_open(حرف *name)
{
	صحیح i;

	برائے(i = 0; i < 31; i++) {
		اگر(mcd_جدول[i].filename == تہی)
			جاؤ got_اندراج;
	}
	واپس 0;  /* too many open mcd's */

got_اندراج:
#اگر defined(__GNUC__)
	mcd_جدول[i].fp = fopen(name, "w");
#ورنہ
	اگر (strcmp(name, "/dev/null") != 0)
		mcd_جدول[i].fp = fopen(name, "w");
	ورنہ
		mcd_جدول[i].fp = fopen("nul", "w");
#اگر_ختم
	اگر(mcd_جدول[i].fp == تہی)
		واپس 0;
	mcd_جدول[i].filename = strdup(name);

	اگر (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	واپس 1<<i;
}

خارجی "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ثابت حرف*fmt, va_list ap)
{
      حرف بفر[4096];
      حرف *buf_ptr = بفر;
      صحیح rc = 0;
      منطقی need_free = جھوٹ;
      va_list saved_ap;

      اگر (!IS_MCD(mcd)) واپس EOF;

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (غیرعلامتی صحیح)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(بفر, حجم بفر, fmt, ap);
      أكد(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      اگر ((غیرعلامتی) rc >= حجم بفر) {
	    buf_ptr = static_cast<حرف *>(حافظہ_دو(rc + 1));
	    need_free = سچ;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      برائے(صحیح i = 0; i < 31; i++) {
	    اگر((mcd>>i) & 1) {
		  اگر(mcd_جدول[i].fp) {
			اگر (i == 0) {
			      اگر (logfile)
				    fputs(buf_ptr, logfile);
			      اگر (vpip_mcd0_disable)
				    جاری;
			}
			fputs(buf_ptr, mcd_جدول[i].fp);
		  } ورنہ {
			rc = EOF;
		  }
	    }
      }
      اگر (need_free) آزاد(buf_ptr);

      واپس rc;
}

خارجی "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ثابت حرف *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      صحیح r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      واپس r;
}

خارجی "C" خالی vpip_mcd_rawwrite(PLI_UINT32 mcd, ثابت حرف*buf, size_t cnt)
{
      اگر (!IS_MCD(mcd)) واپس;

      برائے(صحیح idx = 0; idx < 31; idx += 1) {
	    اگر (((mcd>>idx) & 1) == 0)
		  جاری;

	    اگر (mcd_جدول[idx].fp == 0)
		  جاری;

	    اگر (idx == 0) {
		  اگر (logfile)
			fwrite(buf, 1, cnt, logfile);
		  اگر (vpip_mcd0_disable)
			جاری;
	    }
	    fwrite(buf, 1, cnt, mcd_جدول[idx].fp);
      }
}

خارجی "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	صحیح rc = 0;

	اگر (IS_MCD(mcd)) {
		برائے(صحیح i = 0; i < 31; i++) {
			اگر((mcd>>i) & 1) {
				اگر (i == 0) {
				      اگر (logfile)
					    fflush(logfile);
				      اگر (vpip_mcd0_disable)
					    جاری;
				}
				اگر (fflush(mcd_جدول[i].fp)) rc |= 1<<i;
			}
		}
	} ورنہ {
		غیرعلامتی idx = FD_IDX(mcd);
		اگر (idx < fd_جدول_len) rc = fflush(fd_جدول[idx].fp);
	}
	واپس rc;
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
خارجی "C" PLI_INT32 vpi_fopen(ثابت حرف*name, ثابت حرف*منوال)
{
      غیرعلامتی i;
      برائے (i = 0; i < fd_جدول_len; i += 1) {
	    اگر (fd_جدول[i].filename == تہی) جاؤ got_اندراج;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      اگر (fd_جدول_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    واپس 0;
      }
      fd_جدول_len += FD_INCR;
      fd_جدول = static_cast<mcd_اندراج_s *>
                 (realloc(fd_جدول, fd_جدول_len*حجم(mcd_اندراج_s)));
      برائے (غیرعلامتی idx = i; idx < fd_جدول_len; idx += 1) {
	    fd_جدول[idx].fp = تہی;
	    fd_جدول[idx].filename = تہی;
      }

got_اندراج:
#اگر_غیر_تعریف_شدہ _MSC_VER
	  fd_جدول[i].fp = fopen(name, منوال);
#ورنہ // Changed for MSVC++ so vpi/pr723.v will pass.
	  اگر(strcmp(name, "/dev/null") != 0)
		fd_جدول[i].fp = fopen(name, منوال);
	  ورنہ
		fd_جدول[i].fp = fopen("nul", منوال);
#اگر_ختم
      اگر (fd_جدول[i].fp == تہی) واپس 0;
      fd_جدول[i].filename = strdup(name);
      واپس ((1U<<31)|i);
}

خارجی "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      اگر (IS_MCD(fd)) واپس تہی;

	// Only know about fd_table_len indices
      اگر (FD_IDX(fd) >= fd_جدول_len) واپس تہی;

      واپس fd_جدول[FD_IDX(fd)].fp;
}
