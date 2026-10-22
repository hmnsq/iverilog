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

வெளி FILE* vpi_trace;

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

வகைவரை கட்டமைப்பு mcd_entry {
	FILE *fp;
	எழுத்து *filename;
} mcd_entry_s;
நிலையான mcd_entry_s mcd_table[31];
நிலையான mcd_entry_s *fd_table = வெற்று;
நிலையான unsigned fd_table_len = 0;

நிலையான FILE* logfile;

தருக்க vpip_mcd0_disable = பொய்;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
வெறுமை vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*அளவு(mcd_entry_s)));
      க்கு (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = வெற்று;
	    fd_table[idx].filename = வெற்று;
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
வெறுமை vpi_mcd_delete(வெறுமை)
{
      விடுவி(mcd_table[0].filename);
      mcd_table[0].filename = வெற்று;
      mcd_table[0].fp = வெற்று;

      விடுவி(fd_table[0].filename);
      fd_table[0].filename = வெற்று;
      fd_table[0].fp = வெற்று;

      விடுவி(fd_table[1].filename);
      fd_table[1].filename = வெற்று;
      fd_table[1].fp = வெற்று;

      விடுவி(fd_table[2].filename);
      fd_table[2].filename = வெற்று;
      fd_table[2].fp = வெற்று;

      விடுவி(fd_table);
      fd_table = வெற்று;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
வெளி "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      முழு rc = 0;

      எனில் (IS_MCD(mcd)) {
	    எனில் (mcd & 1) rc |= 1;
	    க்கு(முழு i = 1; i < 31; i++) {
		  எனில் ((mcd>>i) & 1) {
			எனில் (mcd_table[i].fp) {
			      எனில் (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      விடுவி(mcd_table[i].filename);
			      mcd_table[i].fp = வெற்று;
			      mcd_table[i].filename = வெற்று;
			} இல்லையெனில் {
			      rc |= 1<<i;
			}
		  }
	    }
      } இல்லையெனில் {
	    unsigned idx = FD_IDX(mcd);
	    எனில் (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  எனில் (fclose(fd_table[idx].fp)) rc = mcd;
		  விடுவி(fd_table[idx].filename);
		  fd_table[idx].fp = வெற்று;
		  fd_table[idx].filename = வெற்று;
	    } இல்லையெனில் rc = mcd;
      }
      திரும்பு rc;
}

வெளி "C" எழுத்து *vpi_mcd_name(PLI_UINT32 mcd)
{
	எனில் (IS_MCD(mcd)) {
		க்கு(முழு i = 0; i < 31; i++) {
			எனில்((mcd>>i) & 1)
				திரும்பு mcd_table[i].filename;
		}
	} இல்லையெனில் {
		unsigned idx = FD_IDX(mcd);
		எனில் (idx < fd_table_len)
			திரும்பு fd_table[idx].filename;
	}
	திரும்பு வெற்று;
}

வெளி "C" PLI_UINT32 vpi_mcd_open(எழுத்து *name)
{
	முழு i;

	க்கு(i = 0; i < 31; i++) {
		எனில்(mcd_table[i].filename == வெற்று)
			செல் got_entry;
	}
	திரும்பு 0;  /* too many open mcd's */

got_entry:
#எனில் defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#இல்லையெனில்
	எனில் (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	இல்லையெனில்
		mcd_table[i].fp = fopen("nul", "w");
#endif
	எனில்(mcd_table[i].fp == வெற்று)
		திரும்பு 0;
	mcd_table[i].filename = strdup(name);

	எனில் (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	திரும்பு 1<<i;
}

வெளி "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, மாறிலி எழுத்து*fmt, va_list ap)
{
      எழுத்து buffer[4096];
      எழுத்து *buf_ptr = buffer;
      முழு rc = 0;
      தருக்க need_free = பொய்;
      va_list saved_ap;

      எனில் (!IS_MCD(mcd)) திரும்பு EOF;

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned முழு)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, அளவு buffer, fmt, ap);
      ப்ரதிஜ்ஞா(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      எனில் ((unsigned) rc >= அளவு buffer) {
	    buf_ptr = static_cast<எழுத்து *>(malloc(rc + 1));
	    need_free = உண்மை;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      க்கு(முழு i = 0; i < 31; i++) {
	    எனில்((mcd>>i) & 1) {
		  எனில்(mcd_table[i].fp) {
			எனில் (i == 0) {
			      எனில் (logfile)
				    fputs(buf_ptr, logfile);
			      எனில் (vpip_mcd0_disable)
				    தொடர்;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } இல்லையெனில் {
			rc = EOF;
		  }
	    }
      }
      எனில் (need_free) விடுவி(buf_ptr);

      திரும்பு rc;
}

வெளி "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, மாறிலி எழுத்து *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      முழு r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      திரும்பு r;
}

வெளி "C" வெறுமை vpip_mcd_rawwrite(PLI_UINT32 mcd, மாறிலி எழுத்து*buf, size_t cnt)
{
      எனில் (!IS_MCD(mcd)) திரும்பு;

      க்கு(முழு idx = 0; idx < 31; idx += 1) {
	    எனில் (((mcd>>idx) & 1) == 0)
		  தொடர்;

	    எனில் (mcd_table[idx].fp == 0)
		  தொடர்;

	    எனில் (idx == 0) {
		  எனில் (logfile)
			fwrite(buf, 1, cnt, logfile);
		  எனில் (vpip_mcd0_disable)
			தொடர்;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

வெளி "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	முழு rc = 0;

	எனில் (IS_MCD(mcd)) {
		க்கு(முழு i = 0; i < 31; i++) {
			எனில்((mcd>>i) & 1) {
				எனில் (i == 0) {
				      எனில் (logfile)
					    fflush(logfile);
				      எனில் (vpip_mcd0_disable)
					    தொடர்;
				}
				எனில் (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} இல்லையெனில் {
		unsigned idx = FD_IDX(mcd);
		எனில் (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	திரும்பு rc;
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
வெளி "C" PLI_INT32 vpi_fopen(மாறிலி எழுத்து*name, மாறிலி எழுத்து*mode)
{
      unsigned i;
      க்கு (i = 0; i < fd_table_len; i += 1) {
	    எனில் (fd_table[i].filename == வெற்று) செல் got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      எனில் (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    திரும்பு 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*அளவு(mcd_entry_s)));
      க்கு (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = வெற்று;
	    fd_table[idx].filename = வெற்று;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#இல்லையெனில் // Changed for MSVC++ so vpi/pr723.v will pass.
	  எனில்(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  இல்லையெனில்
		fd_table[i].fp = fopen("nul", mode);
#endif
      எனில் (fd_table[i].fp == வெற்று) திரும்பு 0;
      fd_table[i].filename = strdup(name);
      திரும்பு ((1U<<31)|i);
}

வெளி "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      எனில் (IS_MCD(fd)) திரும்பு வெற்று;

	// Only know about fd_table_len indices
      எனில் (FD_IDX(fd) >= fd_table_len) திரும்பு வெற்று;

      திரும்பு fd_table[FD_IDX(fd)].fp;
}
