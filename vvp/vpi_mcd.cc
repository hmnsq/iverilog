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

ବାହ୍ୟ FILE* vpi_trace;

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା mcd_entry {
	FILE *fp;
	ଅକ୍ଷର *filename;
} mcd_entry_s;
ସ୍ଥିର mcd_entry_s mcd_table[31];
ସ୍ଥିର mcd_entry_s *fd_table = ଖାଲି;
ସ୍ଥିର unsigned fd_table_len = 0;

ସ୍ଥିର FILE* logfile;

ତାର୍କିକ vpip_mcd0_disable = ମିଥ୍ୟା;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
ଶୂନ୍ୟ vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*ଆକାର(mcd_entry_s)));
      ପାଇଁ (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ଖାଲି;
	    fd_table[idx].filename = ଖାଲି;
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
ଶୂନ୍ୟ vpi_mcd_delete(ଶୂନ୍ୟ)
{
      ମୁକ୍ତ(mcd_table[0].filename);
      mcd_table[0].filename = ଖାଲି;
      mcd_table[0].fp = ଖାଲି;

      ମୁକ୍ତ(fd_table[0].filename);
      fd_table[0].filename = ଖାଲି;
      fd_table[0].fp = ଖାଲି;

      ମୁକ୍ତ(fd_table[1].filename);
      fd_table[1].filename = ଖାଲି;
      fd_table[1].fp = ଖାଲି;

      ମୁକ୍ତ(fd_table[2].filename);
      fd_table[2].filename = ଖାଲି;
      fd_table[2].fp = ଖାଲି;

      ମୁକ୍ତ(fd_table);
      fd_table = ଖାଲି;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
ବାହ୍ୟ "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

      ଯଦି (IS_MCD(mcd)) {
	    ଯଦି (mcd & 1) rc |= 1;
	    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < 31; i++) {
		  ଯଦି ((mcd>>i) & 1) {
			ଯଦି (mcd_table[i].fp) {
			      ଯଦି (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      ମୁକ୍ତ(mcd_table[i].filename);
			      mcd_table[i].fp = ଖାଲି;
			      mcd_table[i].filename = ଖାଲି;
			} ଅନ୍ୟଥା {
			      rc |= 1<<i;
			}
		  }
	    }
      } ଅନ୍ୟଥା {
	    unsigned idx = FD_IDX(mcd);
	    ଯଦି (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  ଯଦି (fclose(fd_table[idx].fp)) rc = mcd;
		  ମୁକ୍ତ(fd_table[idx].filename);
		  fd_table[idx].fp = ଖାଲି;
		  fd_table[idx].filename = ଖାଲି;
	    } ଅନ୍ୟଥା rc = mcd;
      }
      ଫେରନ୍ତୁ rc;
}

ବାହ୍ୟ "C" ଅକ୍ଷର *vpi_mcd_name(PLI_UINT32 mcd)
{
	ଯଦି (IS_MCD(mcd)) {
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 31; i++) {
			ଯଦି((mcd>>i) & 1)
				ଫେରନ୍ତୁ mcd_table[i].filename;
		}
	} ଅନ୍ୟଥା {
		unsigned idx = FD_IDX(mcd);
		ଯଦି (idx < fd_table_len)
			ଫେରନ୍ତୁ fd_table[idx].filename;
	}
	ଫେରନ୍ତୁ ଖାଲି;
}

ବାହ୍ୟ "C" PLI_UINT32 vpi_mcd_open(ଅକ୍ଷର *name)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ(i = 0; i < 31; i++) {
		ଯଦି(mcd_table[i].filename == ଖାଲି)
			ଯାଅ got_entry;
	}
	ଫେରନ୍ତୁ 0;  /* too many open mcd's */

got_entry:
#ଯଦି defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#ଅନ୍ୟଥା
	ଯଦି (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	ଅନ୍ୟଥା
		mcd_table[i].fp = fopen("nul", "w");
#endif
	ଯଦି(mcd_table[i].fp == ଖାଲି)
		ଫେରନ୍ତୁ 0;
	mcd_table[i].filename = strdup(name);

	ଯଦି (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	ଫେରନ୍ତୁ 1<<i;
}

ବାହ୍ୟ "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ଅଚଳ ଅକ୍ଷର*fmt, va_list ap)
{
      ଅକ୍ଷର buffer[4096];
      ଅକ୍ଷର *buf_ptr = buffer;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
      ତାର୍କିକ need_free = ମିଥ୍ୟା;
      va_list saved_ap;

      ଯଦି (!IS_MCD(mcd)) ଫେରନ୍ତୁ EOF;

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, ଆକାର buffer, fmt, ap);
      ପ୍ରତିଜ୍ଞା(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      ଯଦି ((unsigned) rc >= ଆକାର buffer) {
	    buf_ptr = static_cast<ଅକ୍ଷର *>(malloc(rc + 1));
	    need_free = ସତ୍ୟ;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 31; i++) {
	    ଯଦି((mcd>>i) & 1) {
		  ଯଦି(mcd_table[i].fp) {
			ଯଦି (i == 0) {
			      ଯଦି (logfile)
				    fputs(buf_ptr, logfile);
			      ଯଦି (vpip_mcd0_disable)
				    ଜାରି;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } ଅନ୍ୟଥା {
			rc = EOF;
		  }
	    }
      }
      ଯଦି (need_free) ମୁକ୍ତ(buf_ptr);

      ଫେରନ୍ତୁ rc;
}

ବାହ୍ୟ "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      ଫେରନ୍ତୁ r;
}

ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_mcd_rawwrite(PLI_UINT32 mcd, ଅଚଳ ଅକ୍ଷର*buf, size_t cnt)
{
      ଯଦି (!IS_MCD(mcd)) ଫେରନ୍ତୁ;

      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < 31; idx += 1) {
	    ଯଦି (((mcd>>idx) & 1) == 0)
		  ଜାରି;

	    ଯଦି (mcd_table[idx].fp == 0)
		  ଜାରି;

	    ଯଦି (idx == 0) {
		  ଯଦି (logfile)
			fwrite(buf, 1, cnt, logfile);
		  ଯଦି (vpip_mcd0_disable)
			ଜାରି;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

ବାହ୍ୟ "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	ଯଦି (IS_MCD(mcd)) {
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 31; i++) {
			ଯଦି((mcd>>i) & 1) {
				ଯଦି (i == 0) {
				      ଯଦି (logfile)
					    fflush(logfile);
				      ଯଦି (vpip_mcd0_disable)
					    ଜାରି;
				}
				ଯଦି (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} ଅନ୍ୟଥା {
		unsigned idx = FD_IDX(mcd);
		ଯଦି (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	ଫେରନ୍ତୁ rc;
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
ବାହ୍ୟ "C" PLI_INT32 vpi_fopen(ଅଚଳ ଅକ୍ଷର*name, ଅଚଳ ଅକ୍ଷର*mode)
{
      unsigned i;
      ପାଇଁ (i = 0; i < fd_table_len; i += 1) {
	    ଯଦି (fd_table[i].filename == ଖାଲି) ଯାଅ got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      ଯଦି (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    ଫେରନ୍ତୁ 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*ଆକାର(mcd_entry_s)));
      ପାଇଁ (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ଖାଲି;
	    fd_table[idx].filename = ଖାଲି;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#ଅନ୍ୟଥା // Changed for MSVC++ so vpi/pr723.v will pass.
	  ଯଦି(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  ଅନ୍ୟଥା
		fd_table[i].fp = fopen("nul", mode);
#endif
      ଯଦି (fd_table[i].fp == ଖାଲି) ଫେରନ୍ତୁ 0;
      fd_table[i].filename = strdup(name);
      ଫେରନ୍ତୁ ((1U<<31)|i);
}

ବାହ୍ୟ "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      ଯଦି (IS_MCD(fd)) ଫେରନ୍ତୁ ଖାଲି;

	// Only know about fd_table_len indices
      ଯଦି (FD_IDX(fd) >= fd_table_len) ଫେରନ୍ତୁ ଖାଲି;

      ଫେରନ୍ତୁ fd_table[FD_IDX(fd)].fp;
}
