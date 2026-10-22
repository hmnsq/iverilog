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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cassert>
# include  <cstdarg>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  "ivl_alloc.h"

ਬਾਹਰੀ FILE* vpi_trace;

/*
 * This table keeps track of the MCD files. Note that there may be
 * only 31 such files, and mcd bit0 (32'h00_00_00_01) is the special
 * standard output file, which may be replicated to a logfile and/or
 * suppressed depending on flags to the command line.
 */

/*
 * MCD/FD manipulation macros
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_MCD(mcd)	!(static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ>(mcd)>>31&1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FD_IDX(fd)	((fd)&~(1U<<31))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FD_INCR		32

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ mcd_ਇੰਦਰਾਜ {
	FILE *fp;
	ਅੱਖਰ *filename;
} mcd_ਇੰਦਰਾਜ_s;
ਸਥਿਰ mcd_ਇੰਦਰਾਜ_s mcd_ਸਾਰਣੀ[31];
ਸਥਿਰ mcd_ਇੰਦਰਾਜ_s *fd_ਸਾਰਣੀ = ਨਿਰਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ fd_ਸਾਰਣੀ_len = 0;

ਸਥਿਰ FILE* logfile;

ਤਾਰਕਿਕ vpip_mcd0_disable = ਝੂਠ;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
ਖਾਲੀ vpip_mcd_init(FILE *ਲਘੂਗਣਕ)
{
      fd_ਸਾਰਣੀ_len = FD_INCR;
      fd_ਸਾਰਣੀ = static_cast<mcd_ਇੰਦਰਾਜ_s *>(ਸਮ੍ਰਿਤੀ_ਦਿਓ(fd_ਸਾਰਣੀ_len*ਆਕਾਰ(mcd_ਇੰਦਰਾਜ_s)));
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < fd_ਸਾਰਣੀ_len; idx += 1) {
	    fd_ਸਾਰਣੀ[idx].fp = ਨਿਰਾ;
	    fd_ਸਾਰਣੀ[idx].filename = ਨਿਰਾ;
      }

      mcd_ਸਾਰਣੀ[0].fp = stdout;
      mcd_ਸਾਰਣੀ[0].filename = strdup("stdout");

      fd_ਸਾਰਣੀ[0].fp = stdin;
      fd_ਸਾਰਣੀ[0].filename = strdup("stdin");
      fd_ਸਾਰਣੀ[1].fp = stdout;
      fd_ਸਾਰਣੀ[1].filename = strdup("stdout");
      fd_ਸਾਰਣੀ[2].fp = stderr;
      fd_ਸਾਰਣੀ[2].filename = strdup("stderr");

      logfile = ਲਘੂਗਣਕ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vpi_mcd_delete(ਖਾਲੀ)
{
      ਮੁਕਤ(mcd_ਸਾਰਣੀ[0].filename);
      mcd_ਸਾਰਣੀ[0].filename = ਨਿਰਾ;
      mcd_ਸਾਰਣੀ[0].fp = ਨਿਰਾ;

      ਮੁਕਤ(fd_ਸਾਰਣੀ[0].filename);
      fd_ਸਾਰਣੀ[0].filename = ਨਿਰਾ;
      fd_ਸਾਰਣੀ[0].fp = ਨਿਰਾ;

      ਮੁਕਤ(fd_ਸਾਰਣੀ[1].filename);
      fd_ਸਾਰਣੀ[1].filename = ਨਿਰਾ;
      fd_ਸਾਰਣੀ[1].fp = ਨਿਰਾ;

      ਮੁਕਤ(fd_ਸਾਰਣੀ[2].filename);
      fd_ਸਾਰਣੀ[2].filename = ਨਿਰਾ;
      fd_ਸਾਰਣੀ[2].fp = ਨਿਰਾ;

      ਮੁਕਤ(fd_ਸਾਰਣੀ);
      fd_ਸਾਰਣੀ = ਨਿਰਾ;
      fd_ਸਾਰਣੀ_len = 0;
}
#ਜੇ_ਅੰਤ

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
ਬਾਹਰੀ "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      ਪੂਰਨ rc = 0;

      ਜੇ (IS_MCD(mcd)) {
	    ਜੇ (mcd & 1) rc |= 1;
	    ਲਈ(ਪੂਰਨ i = 1; i < 31; i++) {
		  ਜੇ ((mcd>>i) & 1) {
			ਜੇ (mcd_ਸਾਰਣੀ[i].fp) {
			      ਜੇ (fclose(mcd_ਸਾਰਣੀ[i].fp)) rc |= 1<<i;
			      ਮੁਕਤ(mcd_ਸਾਰਣੀ[i].filename);
			      mcd_ਸਾਰਣੀ[i].fp = ਨਿਰਾ;
			      mcd_ਸਾਰਣੀ[i].filename = ਨਿਰਾ;
			} ਨਹੀਂਤਾਂ {
			      rc |= 1<<i;
			}
		  }
	    }
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx = FD_IDX(mcd);
	    ਜੇ (idx > 2 && idx < fd_ਸਾਰਣੀ_len && fd_ਸਾਰਣੀ[idx].fp) {
		  ਜੇ (fclose(fd_ਸਾਰਣੀ[idx].fp)) rc = mcd;
		  ਮੁਕਤ(fd_ਸਾਰਣੀ[idx].filename);
		  fd_ਸਾਰਣੀ[idx].fp = ਨਿਰਾ;
		  fd_ਸਾਰਣੀ[idx].filename = ਨਿਰਾ;
	    } ਨਹੀਂਤਾਂ rc = mcd;
      }
      ਵਾਪਸ rc;
}

ਬਾਹਰੀ "C" ਅੱਖਰ *vpi_mcd_name(PLI_UINT32 mcd)
{
	ਜੇ (IS_MCD(mcd)) {
		ਲਈ(ਪੂਰਨ i = 0; i < 31; i++) {
			ਜੇ((mcd>>i) & 1)
				ਵਾਪਸ mcd_ਸਾਰਣੀ[i].filename;
		}
	} ਨਹੀਂਤਾਂ {
		ਬਿਨਾਨਿਸ਼ਾਨੀ idx = FD_IDX(mcd);
		ਜੇ (idx < fd_ਸਾਰਣੀ_len)
			ਵਾਪਸ fd_ਸਾਰਣੀ[idx].filename;
	}
	ਵਾਪਸ ਨਿਰਾ;
}

ਬਾਹਰੀ "C" PLI_UINT32 vpi_mcd_open(ਅੱਖਰ *name)
{
	ਪੂਰਨ i;

	ਲਈ(i = 0; i < 31; i++) {
		ਜੇ(mcd_ਸਾਰਣੀ[i].filename == ਨਿਰਾ)
			ਜਾਓ got_ਇੰਦਰਾਜ;
	}
	ਵਾਪਸ 0;  /* too many open mcd's */

got_ਇੰਦਰਾਜ:
#ਜੇ defined(__GNUC__)
	mcd_ਸਾਰਣੀ[i].fp = fopen(name, "w");
#ਨਹੀਂਤਾਂ
	ਜੇ (strcmp(name, "/dev/null") != 0)
		mcd_ਸਾਰਣੀ[i].fp = fopen(name, "w");
	ਨਹੀਂਤਾਂ
		mcd_ਸਾਰਣੀ[i].fp = fopen("nul", "w");
#ਜੇ_ਅੰਤ
	ਜੇ(mcd_ਸਾਰਣੀ[i].fp == ਨਿਰਾ)
		ਵਾਪਸ 0;
	mcd_ਸਾਰਣੀ[i].filename = strdup(name);

	ਜੇ (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	ਵਾਪਸ 1<<i;
}

ਬਾਹਰੀ "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ਅਚਲ ਅੱਖਰ*fmt, va_list ap)
{
      ਅੱਖਰ ਬਫਰ[4096];
      ਅੱਖਰ *buf_ptr = ਬਫਰ;
      ਪੂਰਨ rc = 0;
      ਤਾਰਕਿਕ need_free = ਝੂਠ;
      va_list saved_ap;

      ਜੇ (!IS_MCD(mcd)) ਵਾਪਸ EOF;

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(ਬਫਰ, ਆਕਾਰ ਬਫਰ, fmt, ap);
      ਪ੍ਰਤਿਜ੍ਞਾ(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ) rc >= ਆਕਾਰ ਬਫਰ) {
	    buf_ptr = static_cast<ਅੱਖਰ *>(ਸਮ੍ਰਿਤੀ_ਦਿਓ(rc + 1));
	    need_free = ਸੱਚ;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      ਲਈ(ਪੂਰਨ i = 0; i < 31; i++) {
	    ਜੇ((mcd>>i) & 1) {
		  ਜੇ(mcd_ਸਾਰਣੀ[i].fp) {
			ਜੇ (i == 0) {
			      ਜੇ (logfile)
				    fputs(buf_ptr, logfile);
			      ਜੇ (vpip_mcd0_disable)
				    ਜਾਰੀ;
			}
			fputs(buf_ptr, mcd_ਸਾਰਣੀ[i].fp);
		  } ਨਹੀਂਤਾਂ {
			rc = EOF;
		  }
	    }
      }
      ਜੇ (need_free) ਮੁਕਤ(buf_ptr);

      ਵਾਪਸ rc;
}

ਬਾਹਰੀ "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ਪੂਰਨ r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      ਵਾਪਸ r;
}

ਬਾਹਰੀ "C" ਖਾਲੀ vpip_mcd_rawwrite(PLI_UINT32 mcd, ਅਚਲ ਅੱਖਰ*buf, size_t cnt)
{
      ਜੇ (!IS_MCD(mcd)) ਵਾਪਸ;

      ਲਈ(ਪੂਰਨ idx = 0; idx < 31; idx += 1) {
	    ਜੇ (((mcd>>idx) & 1) == 0)
		  ਜਾਰੀ;

	    ਜੇ (mcd_ਸਾਰਣੀ[idx].fp == 0)
		  ਜਾਰੀ;

	    ਜੇ (idx == 0) {
		  ਜੇ (logfile)
			fwrite(buf, 1, cnt, logfile);
		  ਜੇ (vpip_mcd0_disable)
			ਜਾਰੀ;
	    }
	    fwrite(buf, 1, cnt, mcd_ਸਾਰਣੀ[idx].fp);
      }
}

ਬਾਹਰੀ "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	ਪੂਰਨ rc = 0;

	ਜੇ (IS_MCD(mcd)) {
		ਲਈ(ਪੂਰਨ i = 0; i < 31; i++) {
			ਜੇ((mcd>>i) & 1) {
				ਜੇ (i == 0) {
				      ਜੇ (logfile)
					    fflush(logfile);
				      ਜੇ (vpip_mcd0_disable)
					    ਜਾਰੀ;
				}
				ਜੇ (fflush(mcd_ਸਾਰਣੀ[i].fp)) rc |= 1<<i;
			}
		}
	} ਨਹੀਂਤਾਂ {
		ਬਿਨਾਨਿਸ਼ਾਨੀ idx = FD_IDX(mcd);
		ਜੇ (idx < fd_ਸਾਰਣੀ_len) rc = fflush(fd_ਸਾਰਣੀ[idx].fp);
	}
	ਵਾਪਸ rc;
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
ਬਾਹਰੀ "C" PLI_INT32 vpi_fopen(ਅਚਲ ਅੱਖਰ*name, ਅਚਲ ਅੱਖਰ*ਬਹੁਲਕ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ i;
      ਲਈ (i = 0; i < fd_ਸਾਰਣੀ_len; i += 1) {
	    ਜੇ (fd_ਸਾਰਣੀ[i].filename == ਨਿਰਾ) ਜਾਓ got_ਇੰਦਰਾਜ;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      ਜੇ (fd_ਸਾਰਣੀ_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    ਵਾਪਸ 0;
      }
      fd_ਸਾਰਣੀ_len += FD_INCR;
      fd_ਸਾਰਣੀ = static_cast<mcd_ਇੰਦਰਾਜ_s *>
                 (realloc(fd_ਸਾਰਣੀ, fd_ਸਾਰਣੀ_len*ਆਕਾਰ(mcd_ਇੰਦਰਾਜ_s)));
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = i; idx < fd_ਸਾਰਣੀ_len; idx += 1) {
	    fd_ਸਾਰਣੀ[idx].fp = ਨਿਰਾ;
	    fd_ਸਾਰਣੀ[idx].filename = ਨਿਰਾ;
      }

got_ਇੰਦਰਾਜ:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _MSC_VER
	  fd_ਸਾਰਣੀ[i].fp = fopen(name, ਬਹੁਲਕ);
#ਨਹੀਂਤਾਂ // Changed for MSVC++ so vpi/pr723.v will pass.
	  ਜੇ(strcmp(name, "/dev/null") != 0)
		fd_ਸਾਰਣੀ[i].fp = fopen(name, ਬਹੁਲਕ);
	  ਨਹੀਂਤਾਂ
		fd_ਸਾਰਣੀ[i].fp = fopen("nul", ਬਹੁਲਕ);
#ਜੇ_ਅੰਤ
      ਜੇ (fd_ਸਾਰਣੀ[i].fp == ਨਿਰਾ) ਵਾਪਸ 0;
      fd_ਸਾਰਣੀ[i].filename = strdup(name);
      ਵਾਪਸ ((1U<<31)|i);
}

ਬਾਹਰੀ "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      ਜੇ (IS_MCD(fd)) ਵਾਪਸ ਨਿਰਾ;

	// Only know about fd_table_len indices
      ਜੇ (FD_IDX(fd) >= fd_ਸਾਰਣੀ_len) ਵਾਪਸ ਨਿਰਾ;

      ਵਾਪਸ fd_ਸਾਰਣੀ[FD_IDX(fd)].fp;
}
