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

বাহ্যিক FILE* vpi_trace;

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

প্রকারলেখ কাঠামো mcd_entry {
	FILE *fp;
	অক্ষর *filename;
} mcd_entry_s;
স্থির mcd_entry_s mcd_table[31];
স্থির mcd_entry_s *fd_table = ফাঁকা;
স্থির unsigned fd_table_len = 0;

স্থির FILE* logfile;

যৌক্তিক vpip_mcd0_disable = মিথ্যা;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
শূন্য vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*আকার(mcd_entry_s)));
      জন্য (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ফাঁকা;
	    fd_table[idx].filename = ফাঁকা;
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
শূন্য vpi_mcd_delete(শূন্য)
{
      মুক্ত(mcd_table[0].filename);
      mcd_table[0].filename = ফাঁকা;
      mcd_table[0].fp = ফাঁকা;

      মুক্ত(fd_table[0].filename);
      fd_table[0].filename = ফাঁকা;
      fd_table[0].fp = ফাঁকা;

      মুক্ত(fd_table[1].filename);
      fd_table[1].filename = ফাঁকা;
      fd_table[1].fp = ফাঁকা;

      মুক্ত(fd_table[2].filename);
      fd_table[2].filename = ফাঁকা;
      fd_table[2].fp = ফাঁকা;

      মুক্ত(fd_table);
      fd_table = ফাঁকা;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
বাহ্যিক "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      পূর্ণসংখ্যা rc = 0;

      যদি (IS_MCD(mcd)) {
	    যদি (mcd & 1) rc |= 1;
	    জন্য(পূর্ণসংখ্যা i = 1; i < 31; i++) {
		  যদি ((mcd>>i) & 1) {
			যদি (mcd_table[i].fp) {
			      যদি (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      মুক্ত(mcd_table[i].filename);
			      mcd_table[i].fp = ফাঁকা;
			      mcd_table[i].filename = ফাঁকা;
			} অন্যথা {
			      rc |= 1<<i;
			}
		  }
	    }
      } অন্যথা {
	    unsigned idx = FD_IDX(mcd);
	    যদি (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  যদি (fclose(fd_table[idx].fp)) rc = mcd;
		  মুক্ত(fd_table[idx].filename);
		  fd_table[idx].fp = ফাঁকা;
		  fd_table[idx].filename = ফাঁকা;
	    } অন্যথা rc = mcd;
      }
      ফেরত rc;
}

বাহ্যিক "C" অক্ষর *vpi_mcd_name(PLI_UINT32 mcd)
{
	যদি (IS_MCD(mcd)) {
		জন্য(পূর্ণসংখ্যা i = 0; i < 31; i++) {
			যদি((mcd>>i) & 1)
				ফেরত mcd_table[i].filename;
		}
	} অন্যথা {
		unsigned idx = FD_IDX(mcd);
		যদি (idx < fd_table_len)
			ফেরত fd_table[idx].filename;
	}
	ফেরত ফাঁকা;
}

বাহ্যিক "C" PLI_UINT32 vpi_mcd_open(অক্ষর *name)
{
	পূর্ণসংখ্যা i;

	জন্য(i = 0; i < 31; i++) {
		যদি(mcd_table[i].filename == ফাঁকা)
			যাও got_entry;
	}
	ফেরত 0;  /* too many open mcd's */

got_entry:
#যদি defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#অন্যথা
	যদি (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	অন্যথা
		mcd_table[i].fp = fopen("nul", "w");
#endif
	যদি(mcd_table[i].fp == ফাঁকা)
		ফেরত 0;
	mcd_table[i].filename = strdup(name);

	যদি (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	ফেরত 1<<i;
}

বাহ্যিক "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ধ্রুবক অক্ষর*fmt, va_list ap)
{
      অক্ষর বাফার[4096];
      অক্ষর *buf_ptr = বাফার;
      পূর্ণসংখ্যা rc = 0;
      যৌক্তিক need_free = মিথ্যা;
      va_list saved_ap;

      যদি (!IS_MCD(mcd)) ফেরত EOF;

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned পূর্ণসংখ্যা)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(বাফার, আকার বাফার, fmt, ap);
      প্রতিজ্ঞা(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      যদি ((unsigned) rc >= আকার বাফার) {
	    buf_ptr = static_cast<অক্ষর *>(malloc(rc + 1));
	    need_free = সত্য;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      জন্য(পূর্ণসংখ্যা i = 0; i < 31; i++) {
	    যদি((mcd>>i) & 1) {
		  যদি(mcd_table[i].fp) {
			যদি (i == 0) {
			      যদি (logfile)
				    fputs(buf_ptr, logfile);
			      যদি (vpip_mcd0_disable)
				    চালু;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } অন্যথা {
			rc = EOF;
		  }
	    }
      }
      যদি (need_free) মুক্ত(buf_ptr);

      ফেরত rc;
}

বাহ্যিক "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ধ্রুবক অক্ষর *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      পূর্ণসংখ্যা r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      ফেরত r;
}

বাহ্যিক "C" শূন্য vpip_mcd_rawwrite(PLI_UINT32 mcd, ধ্রুবক অক্ষর*buf, size_t cnt)
{
      যদি (!IS_MCD(mcd)) ফেরত;

      জন্য(পূর্ণসংখ্যা idx = 0; idx < 31; idx += 1) {
	    যদি (((mcd>>idx) & 1) == 0)
		  চালু;

	    যদি (mcd_table[idx].fp == 0)
		  চালু;

	    যদি (idx == 0) {
		  যদি (logfile)
			fwrite(buf, 1, cnt, logfile);
		  যদি (vpip_mcd0_disable)
			চালু;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

বাহ্যিক "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	পূর্ণসংখ্যা rc = 0;

	যদি (IS_MCD(mcd)) {
		জন্য(পূর্ণসংখ্যা i = 0; i < 31; i++) {
			যদি((mcd>>i) & 1) {
				যদি (i == 0) {
				      যদি (logfile)
					    fflush(logfile);
				      যদি (vpip_mcd0_disable)
					    চালু;
				}
				যদি (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} অন্যথা {
		unsigned idx = FD_IDX(mcd);
		যদি (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	ফেরত rc;
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
বাহ্যিক "C" PLI_INT32 vpi_fopen(ধ্রুবক অক্ষর*name, ধ্রুবক অক্ষর*mode)
{
      unsigned i;
      জন্য (i = 0; i < fd_table_len; i += 1) {
	    যদি (fd_table[i].filename == ফাঁকা) যাও got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      যদি (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    ফেরত 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*আকার(mcd_entry_s)));
      জন্য (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ফাঁকা;
	    fd_table[idx].filename = ফাঁকা;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#অন্যথা // Changed for MSVC++ so vpi/pr723.v will pass.
	  যদি(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  অন্যথা
		fd_table[i].fp = fopen("nul", mode);
#endif
      যদি (fd_table[i].fp == ফাঁকা) ফেরত 0;
      fd_table[i].filename = strdup(name);
      ফেরত ((1U<<31)|i);
}

বাহ্যিক "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      যদি (IS_MCD(fd)) ফেরত ফাঁকা;

	// Only know about fd_table_len indices
      যদি (FD_IDX(fd) >= fd_table_len) ফেরত ফাঁকা;

      ফেরত fd_table[FD_IDX(fd)].fp;
}
