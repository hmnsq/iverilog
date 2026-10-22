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

ബാഹ്യ FILE* vpi_trace;

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

തരനിർവചനം ഘടന mcd_entry {
	FILE *fp;
	അക്ഷരം *filename;
} mcd_entry_s;
സ്ഥിതം mcd_entry_s mcd_table[31];
സ്ഥിതം mcd_entry_s *fd_table = ശൂന്യ;
സ്ഥിതം unsigned fd_table_len = 0;

സ്ഥിതം FILE* logfile;

തർക്കം vpip_mcd0_disable = അസത്യം;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
ശൂന്യം vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*വലുപ്പം(mcd_entry_s)));
      വേണ്ടി (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ശൂന്യ;
	    fd_table[idx].filename = ശൂന്യ;
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
ശൂന്യം vpi_mcd_delete(ശൂന്യം)
{
      സ്വതന്ത്രം(mcd_table[0].filename);
      mcd_table[0].filename = ശൂന്യ;
      mcd_table[0].fp = ശൂന്യ;

      സ്വതന്ത്രം(fd_table[0].filename);
      fd_table[0].filename = ശൂന്യ;
      fd_table[0].fp = ശൂന്യ;

      സ്വതന്ത്രം(fd_table[1].filename);
      fd_table[1].filename = ശൂന്യ;
      fd_table[1].fp = ശൂന്യ;

      സ്വതന്ത്രം(fd_table[2].filename);
      fd_table[2].filename = ശൂന്യ;
      fd_table[2].fp = ശൂന്യ;

      സ്വതന്ത്രം(fd_table);
      fd_table = ശൂന്യ;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
ബാഹ്യ "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      പൂർണ്ണസംഖ്യ rc = 0;

      എങ്കിൽ (IS_MCD(mcd)) {
	    എങ്കിൽ (mcd & 1) rc |= 1;
	    വേണ്ടി(പൂർണ്ണസംഖ്യ i = 1; i < 31; i++) {
		  എങ്കിൽ ((mcd>>i) & 1) {
			എങ്കിൽ (mcd_table[i].fp) {
			      എങ്കിൽ (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      സ്വതന്ത്രം(mcd_table[i].filename);
			      mcd_table[i].fp = ശൂന്യ;
			      mcd_table[i].filename = ശൂന്യ;
			} അല്ലെങ്കിൽ {
			      rc |= 1<<i;
			}
		  }
	    }
      } അല്ലെങ്കിൽ {
	    unsigned idx = FD_IDX(mcd);
	    എങ്കിൽ (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  എങ്കിൽ (fclose(fd_table[idx].fp)) rc = mcd;
		  സ്വതന്ത്രം(fd_table[idx].filename);
		  fd_table[idx].fp = ശൂന്യ;
		  fd_table[idx].filename = ശൂന്യ;
	    } അല്ലെങ്കിൽ rc = mcd;
      }
      തിരികെ rc;
}

ബാഹ്യ "C" അക്ഷരം *vpi_mcd_name(PLI_UINT32 mcd)
{
	എങ്കിൽ (IS_MCD(mcd)) {
		വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < 31; i++) {
			എങ്കിൽ((mcd>>i) & 1)
				തിരികെ mcd_table[i].filename;
		}
	} അല്ലെങ്കിൽ {
		unsigned idx = FD_IDX(mcd);
		എങ്കിൽ (idx < fd_table_len)
			തിരികെ fd_table[idx].filename;
	}
	തിരികെ ശൂന്യ;
}

ബാഹ്യ "C" PLI_UINT32 vpi_mcd_open(അക്ഷരം *name)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി(i = 0; i < 31; i++) {
		എങ്കിൽ(mcd_table[i].filename == ശൂന്യ)
			പോകുക got_entry;
	}
	തിരികെ 0;  /* too many open mcd's */

got_entry:
#എങ്കിൽ defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#അല്ലെങ്കിൽ
	എങ്കിൽ (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	അല്ലെങ്കിൽ
		mcd_table[i].fp = fopen("nul", "w");
#endif
	എങ്കിൽ(mcd_table[i].fp == ശൂന്യ)
		തിരികെ 0;
	mcd_table[i].filename = strdup(name);

	എങ്കിൽ (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	തിരികെ 1<<i;
}

ബാഹ്യ "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, സ്ഥിരം അക്ഷരം*fmt, va_list ap)
{
      അക്ഷരം buffer[4096];
      അക്ഷരം *buf_ptr = buffer;
      പൂർണ്ണസംഖ്യ rc = 0;
      തർക്കം need_free = അസത്യം;
      va_list saved_ap;

      എങ്കിൽ (!IS_MCD(mcd)) തിരികെ EOF;

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned പൂർണ്ണസംഖ്യ)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, വലുപ്പം buffer, fmt, ap);
      പ്രതിജ്ഞാ(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      എങ്കിൽ ((unsigned) rc >= വലുപ്പം buffer) {
	    buf_ptr = static_cast<അക്ഷരം *>(malloc(rc + 1));
	    need_free = സത്യം;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < 31; i++) {
	    എങ്കിൽ((mcd>>i) & 1) {
		  എങ്കിൽ(mcd_table[i].fp) {
			എങ്കിൽ (i == 0) {
			      എങ്കിൽ (logfile)
				    fputs(buf_ptr, logfile);
			      എങ്കിൽ (vpip_mcd0_disable)
				    തുടരുക;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } അല്ലെങ്കിൽ {
			rc = EOF;
		  }
	    }
      }
      എങ്കിൽ (need_free) സ്വതന്ത്രം(buf_ptr);

      തിരികെ rc;
}

ബാഹ്യ "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, സ്ഥിരം അക്ഷരം *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      പൂർണ്ണസംഖ്യ r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      തിരികെ r;
}

ബാഹ്യ "C" ശൂന്യം vpip_mcd_rawwrite(PLI_UINT32 mcd, സ്ഥിരം അക്ഷരം*buf, size_t cnt)
{
      എങ്കിൽ (!IS_MCD(mcd)) തിരികെ;

      വേണ്ടി(പൂർണ്ണസംഖ്യ idx = 0; idx < 31; idx += 1) {
	    എങ്കിൽ (((mcd>>idx) & 1) == 0)
		  തുടരുക;

	    എങ്കിൽ (mcd_table[idx].fp == 0)
		  തുടരുക;

	    എങ്കിൽ (idx == 0) {
		  എങ്കിൽ (logfile)
			fwrite(buf, 1, cnt, logfile);
		  എങ്കിൽ (vpip_mcd0_disable)
			തുടരുക;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

ബാഹ്യ "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	പൂർണ്ണസംഖ്യ rc = 0;

	എങ്കിൽ (IS_MCD(mcd)) {
		വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < 31; i++) {
			എങ്കിൽ((mcd>>i) & 1) {
				എങ്കിൽ (i == 0) {
				      എങ്കിൽ (logfile)
					    fflush(logfile);
				      എങ്കിൽ (vpip_mcd0_disable)
					    തുടരുക;
				}
				എങ്കിൽ (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} അല്ലെങ്കിൽ {
		unsigned idx = FD_IDX(mcd);
		എങ്കിൽ (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	തിരികെ rc;
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
ബാഹ്യ "C" PLI_INT32 vpi_fopen(സ്ഥിരം അക്ഷരം*name, സ്ഥിരം അക്ഷരം*mode)
{
      unsigned i;
      വേണ്ടി (i = 0; i < fd_table_len; i += 1) {
	    എങ്കിൽ (fd_table[i].filename == ശൂന്യ) പോകുക got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      എങ്കിൽ (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    തിരികെ 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*വലുപ്പം(mcd_entry_s)));
      വേണ്ടി (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ശൂന്യ;
	    fd_table[idx].filename = ശൂന്യ;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#അല്ലെങ്കിൽ // Changed for MSVC++ so vpi/pr723.v will pass.
	  എങ്കിൽ(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  അല്ലെങ്കിൽ
		fd_table[i].fp = fopen("nul", mode);
#endif
      എങ്കിൽ (fd_table[i].fp == ശൂന്യ) തിരികെ 0;
      fd_table[i].filename = strdup(name);
      തിരികെ ((1U<<31)|i);
}

ബാഹ്യ "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      എങ്കിൽ (IS_MCD(fd)) തിരികെ ശൂന്യ;

	// Only know about fd_table_len indices
      എങ്കിൽ (FD_IDX(fd) >= fd_table_len) തിരികെ ശൂന്യ;

      തിരികെ fd_table[FD_IDX(fd)].fp;
}
