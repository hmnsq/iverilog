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

બાહ્ય FILE* vpi_trace;

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

પ્રકારવ્યાખ્યા સંરચના mcd_entry {
	FILE *fp;
	અક્ષર *filename;
} mcd_entry_s;
સ્થિર mcd_entry_s mcd_table[31];
સ્થિર mcd_entry_s *fd_table = ખાલી;
સ્થિર unsigned fd_table_len = 0;

સ્થિર FILE* logfile;

તાર્કિક vpip_mcd0_disable = ખોટું;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
શૂન્ય vpip_mcd_init(FILE *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(malloc(fd_table_len*કદ(mcd_entry_s)));
      માટે (unsigned idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ખાલી;
	    fd_table[idx].filename = ખાલી;
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
શૂન્ય vpi_mcd_delete(શૂન્ય)
{
      મુક્ત(mcd_table[0].filename);
      mcd_table[0].filename = ખાલી;
      mcd_table[0].fp = ખાલી;

      મુક્ત(fd_table[0].filename);
      fd_table[0].filename = ખાલી;
      fd_table[0].fp = ખાલી;

      મુક્ત(fd_table[1].filename);
      fd_table[1].filename = ખાલી;
      fd_table[1].fp = ખાલી;

      મુક્ત(fd_table[2].filename);
      fd_table[2].filename = ખાલી;
      fd_table[2].fp = ખાલી;

      મુક્ત(fd_table);
      fd_table = ખાલી;
      fd_table_len = 0;
}
#endif

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
બાહ્ય "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      પૂર્ણાંક rc = 0;

      જો (IS_MCD(mcd)) {
	    જો (mcd & 1) rc |= 1;
	    માટે(પૂર્ણાંક i = 1; i < 31; i++) {
		  જો ((mcd>>i) & 1) {
			જો (mcd_table[i].fp) {
			      જો (fclose(mcd_table[i].fp)) rc |= 1<<i;
			      મુક્ત(mcd_table[i].filename);
			      mcd_table[i].fp = ખાલી;
			      mcd_table[i].filename = ખાલી;
			} નહીંતર {
			      rc |= 1<<i;
			}
		  }
	    }
      } નહીંતર {
	    unsigned idx = FD_IDX(mcd);
	    જો (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  જો (fclose(fd_table[idx].fp)) rc = mcd;
		  મુક્ત(fd_table[idx].filename);
		  fd_table[idx].fp = ખાલી;
		  fd_table[idx].filename = ખાલી;
	    } નહીંતર rc = mcd;
      }
      પાછા rc;
}

બાહ્ય "C" અક્ષર *vpi_mcd_name(PLI_UINT32 mcd)
{
	જો (IS_MCD(mcd)) {
		માટે(પૂર્ણાંક i = 0; i < 31; i++) {
			જો((mcd>>i) & 1)
				પાછા mcd_table[i].filename;
		}
	} નહીંતર {
		unsigned idx = FD_IDX(mcd);
		જો (idx < fd_table_len)
			પાછા fd_table[idx].filename;
	}
	પાછા ખાલી;
}

બાહ્ય "C" PLI_UINT32 vpi_mcd_open(અક્ષર *name)
{
	પૂર્ણાંક i;

	માટે(i = 0; i < 31; i++) {
		જો(mcd_table[i].filename == ખાલી)
			જાઓ got_entry;
	}
	પાછા 0;  /* too many open mcd's */

got_entry:
#જો defined(__GNUC__)
	mcd_table[i].fp = fopen(name, "w");
#નહીંતર
	જો (strcmp(name, "/dev/null") != 0)
		mcd_table[i].fp = fopen(name, "w");
	નહીંતર
		mcd_table[i].fp = fopen("nul", "w");
#endif
	જો(mcd_table[i].fp == ખાલી)
		પાછા 0;
	mcd_table[i].filename = strdup(name);

	જો (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	પાછા 1<<i;
}

બાહ્ય "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, અચલ અક્ષર*fmt, va_list ap)
{
      અક્ષર buffer[4096];
      અક્ષર *buf_ptr = buffer;
      પૂર્ણાંક rc = 0;
      તાર્કિક need_free = ખોટું;
      va_list saved_ap;

      જો (!IS_MCD(mcd)) પાછા EOF;

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (unsigned પૂર્ણાંક)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, કદ buffer, fmt, ap);
      પ્રતિજ્ઞા(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      જો ((unsigned) rc >= કદ buffer) {
	    buf_ptr = static_cast<અક્ષર *>(malloc(rc + 1));
	    need_free = સાચું;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      માટે(પૂર્ણાંક i = 0; i < 31; i++) {
	    જો((mcd>>i) & 1) {
		  જો(mcd_table[i].fp) {
			જો (i == 0) {
			      જો (logfile)
				    fputs(buf_ptr, logfile);
			      જો (vpip_mcd0_disable)
				    ચાલુ;
			}
			fputs(buf_ptr, mcd_table[i].fp);
		  } નહીંતર {
			rc = EOF;
		  }
	    }
      }
      જો (need_free) મુક્ત(buf_ptr);

      પાછા rc;
}

બાહ્ય "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, અચલ અક્ષર *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      પૂર્ણાંક r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      પાછા r;
}

બાહ્ય "C" શૂન્ય vpip_mcd_rawwrite(PLI_UINT32 mcd, અચલ અક્ષર*buf, size_t cnt)
{
      જો (!IS_MCD(mcd)) પાછા;

      માટે(પૂર્ણાંક idx = 0; idx < 31; idx += 1) {
	    જો (((mcd>>idx) & 1) == 0)
		  ચાલુ;

	    જો (mcd_table[idx].fp == 0)
		  ચાલુ;

	    જો (idx == 0) {
		  જો (logfile)
			fwrite(buf, 1, cnt, logfile);
		  જો (vpip_mcd0_disable)
			ચાલુ;
	    }
	    fwrite(buf, 1, cnt, mcd_table[idx].fp);
      }
}

બાહ્ય "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	પૂર્ણાંક rc = 0;

	જો (IS_MCD(mcd)) {
		માટે(પૂર્ણાંક i = 0; i < 31; i++) {
			જો((mcd>>i) & 1) {
				જો (i == 0) {
				      જો (logfile)
					    fflush(logfile);
				      જો (vpip_mcd0_disable)
					    ચાલુ;
				}
				જો (fflush(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} નહીંતર {
		unsigned idx = FD_IDX(mcd);
		જો (idx < fd_table_len) rc = fflush(fd_table[idx].fp);
	}
	પાછા rc;
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
બાહ્ય "C" PLI_INT32 vpi_fopen(અચલ અક્ષર*name, અચલ અક્ષર*mode)
{
      unsigned i;
      માટે (i = 0; i < fd_table_len; i += 1) {
	    જો (fd_table[i].filename == ખાલી) જાઓ got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      જો (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    પાછા 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (realloc(fd_table, fd_table_len*કદ(mcd_entry_s)));
      માટે (unsigned idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = ખાલી;
	    fd_table[idx].filename = ખાલી;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = fopen(name, mode);
#નહીંતર // Changed for MSVC++ so vpi/pr723.v will pass.
	  જો(strcmp(name, "/dev/null") != 0)
		fd_table[i].fp = fopen(name, mode);
	  નહીંતર
		fd_table[i].fp = fopen("nul", mode);
#endif
      જો (fd_table[i].fp == ખાલી) પાછા 0;
      fd_table[i].filename = strdup(name);
      પાછા ((1U<<31)|i);
}

બાહ્ય "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      જો (IS_MCD(fd)) પાછા ખાલી;

	// Only know about fd_table_len indices
      જો (FD_IDX(fd) >= fd_table_len) પાછા ખાલી;

      પાછા fd_table[FD_IDX(fd)].fp;
}
