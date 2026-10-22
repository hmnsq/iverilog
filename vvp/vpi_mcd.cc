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

बाह्य FILE* vpi_trace;

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

प्रकारनिर्देश संरचना mcd_entry {
	FILE *fp;
	वर्ण *filename;
} mcd_entry_s;
स्थिर mcd_entry_s mcd_table[31];
स्थिर mcd_entry_s *fd_table = रिक्त;
स्थिर unsigned fd_table_len = 0;

स्थिर FILE* logfile;

तार्किक vpip_mcd0_disable = मिच्छा;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
शून्य vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*परिमाण(mcd_entry_s)));
      अत्थाय (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = रिक्त;
	    fd_table[idx].filename = रिक्त;
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
शून्य vpi_mcd_delete(शून्य)
{
      मुक्त(mcd_table[0].filename);
      mcd_table[0].filename = रिक्त;
      mcd_table[0].fp = रिक्त;

      मुक्त(fd_table[0].filename);
      fd_table[0].filename = रिक्त;
      fd_table[0].fp = रिक्त;

      मुक्त(fd_table[1].filename);
      fd_table[1].filename = रिक्त;
      fd_table[1].fp = रिक्त;

      मुक्त(fd_table[2].filename);
      fd_table[2].filename = रिक्त;
      fd_table[2].fp = रिक्त;

      मुक्त(fd_table);
      fd_table = रिक्त;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
बाह्य "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      पूर्णाङ्क rc = 0;

      सचे (IS_MCD(mcd)) {
	    सचे (mcd & 1) rc |= 1;
	    अत्थाय(पूर्णाङ्क i = 1; i < 31; i++) {
		  सचे ((mcd>>i) & 1) {
			सचे (mcd_table[i].fp) {
			      सचे (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      मुक्त(mcd_table[i].filename);
			      mcd_table[i].fp = रिक्त;
			      mcd_table[i].filename = रिक्त;
			} अञ्ञथा {
			      rc |= 1<<i;
			}
		  }
	    }
      } अञ्ञथा {
	    unsigned idx = FD_IDX(mcd);
	    सचे (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  सचे (fclose(fd_table[idx].fp)) rc = mcd;
		  मुक्त(fd_table[idx].filename);
		  fd_table[idx].fp = रिक्त;
		  fd_table[idx].filename = रिक्त;
	    } अञ्ञथा rc = mcd;
      }
      पटिनिवत्त rc;
}

बाह्य "C" वर्ण *vpi_mcd_name(PLI_UINT32 mcd)
{
	सचे (IS_MCD(mcd)) {
		अत्थाय(पूर्णाङ्क i = 0; i < 31; i++) {
			सचे((mcd>>i) & 1)
				पटिनिवत्त mcd_table[i].filename;
		}
	} अञ्ञथा {
		unsigned idx = FD_IDX(mcd);
		सचे (idx < fd_table_len)
			पटिनिवत्त fd_table[idx].filename;
	}
	पटिनिवत्त रिक्त;
}

बाह्य "C" PLI_UINT32 vpi_mcd_open(वर्ण *name)
{
	पूर्णाङ्क i;

	अत्थाय(i = 0; i < 31; i++) {
		सचे(mcd_table[i].filename == रिक्त)
			गच्छ got_entry;
	}
	पटिनिवत्त 0;  /* too many open mcd's */

got_entry:
#सचे defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#अञ्ञथा
	सचे (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	अञ्ञथा
		mcd_table[i].fp = fopen("nul", "w");
#endif
	सचे(mcd_table[i].fp == रिक्त)
		पटिनिवत्त 0;
	mcd_table[i].filename = strdup(name);

	सचे (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	पटिनिवत्त 1<<i;
}

बाह्य "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ध्रुव वर्ण*fmt, va_list ap)
{
      वर्ण buffer[4096];
      वर्ण *buf_ptr = buffer;
      पूर्णाङ्क rc = 0;
      तार्किक need_free = मिच्छा;
      va_list saved_ap;

      सचे (!IS_MCD(mcd)) पटिनिवत्त EOF;

      सचे (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned पूर्णाङ्क)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, परिमाण buffer, fmt, ap);
      प्रतिज्ञा(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      सचे ((unsigned) rc >= परिमाण buffer) {
	    buf_ptr = static_cast<वर्ण *>(malloc(rc + 1));
	    need_free = सच्च;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      अत्थाय(पूर्णाङ्क i = 0; i < 31; i++) {
	    सचे((mcd>>i) & 1) {
		  सचे(mcd_table[i].fp) {
			सचे (i == 0) {
			      सचे (logfile)
				    fputs(buf_ptr, logfile);
			      सचे (vpip_mcd0_disable)
				    अनुवर्त;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } अञ्ञथा {
			rc = EOF;
		  }
	    }
      }
      सचे (need_free) मुक्त(buf_ptr);

      पटिनिवत्त rc;
}

बाह्य "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ध्रुव वर्ण *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      पूर्णाङ्क r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      पटिनिवत्त r;
}

बाह्य "C" शून्य vpip_mcd_rawwrite(PLI_UINT32 mcd, ध्रुव वर्ण*buf, size_t cnt)
{
      सचे (!IS_MCD(mcd)) पटिनिवत्त;

      अत्थाय(पूर्णाङ्क idx = 0; idx < 31; idx += 1) {
	    सचे (((mcd>>idx) & 1) == 0)
		  अनुवर्त;

	    सचे (mcd_table[idx].fp == 0)
		  अनुवर्त;

	    सचे (idx == 0) {
		  सचे (logfile)
			fwrite(buf, 1, cnt, logfile);
		  सचे (vpip_mcd0_disable)
			अनुवर्त;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

बाह्य "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	पूर्णाङ्क rc = 0;

	सचे (IS_MCD(mcd)) {
		अत्थाय(पूर्णाङ्क i = 0; i < 31; i++) {
			सचे((mcd>>i) & 1) {
				सचे (i == 0) {
				      सचे (logfile)
					    fflush(logfile);
				      सचे (vpip_mcd0_disable)
					    अनुवर्त;
				}
				सचे (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} अञ्ञथा {
		unsigned idx = FD_IDX(mcd);
		सचे (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	पटिनिवत्त rc;
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
बाह्य "C" PLI_INT32 vpi_fopen(ध्रुव वर्ण*name, ध्रुव वर्ण*mode)
{
      unsigned i;
      अत्थाय (i = 0; i < fd_table_len; i += 1) {
	    सचे (fd_table[i].filename == रिक्त) गच्छ got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      सचे (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    पटिनिवत्त 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*परिमाण(mcd_entry_s)));
      अत्थाय (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = रिक्त;
	    fd_table[idx].filename = रिक्त;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#अञ्ञथा // Changed for MSVC++ so vpi/pr723.v will pass.
	  सचे(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  अञ्ञथा
		fd_table[i].fp = fopen("nul", mode);
#endif
      सचे (fd_table[i].fp == रिक्त) पटिनिवत्त 0;
      fd_table[i].filename = strdup(name);
      पटिनिवत्त ((1U<<31)|i);
}

बाह्य "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      सचे (IS_MCD(fd)) पटिनिवत्त रिक्त;

	// Only know about fd_table_len indices
      सचे (FD_IDX(fd) >= fd_table_len) पटिनिवत्त रिक्त;

      पटिनिवत्त fd_table[FD_IDX(fd)].fp;
}
