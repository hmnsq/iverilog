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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cassert>
# include  <cstdarg>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  "ivl_alloc.h"

ಬಾಹ್ಯ FILE* vpi_trace;

/*
 * This table keeps track of the MCD files. Note that there may be
 * only 31 such files, and mcd bit0 (32'h00_00_00_01) is the special
 * standard output file, which may be replicated to a logfile and/or
 * suppressed depending on flags to the command line.
 */

/*
 * MCD/FD manipulation macros
 */
#ವ್ಯಾಖ್ಯಾನಿಸು IS_MCD(mcd)	!(static_cast<ಚಿಹ್ನೆರಹಿತ>(mcd)>>31&1)
#ವ್ಯಾಖ್ಯಾನಿಸು FD_IDX(fd)	((fd)&~(1U<<31))
#ವ್ಯಾಖ್ಯಾನಿಸು FD_INCR		32

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ mcd_ನಮೂದು {
	FILE *fp;
	ಅಕ್ಷರ *filename;
} mcd_ನಮೂದು_s;
ಸ್ಥಿರ mcd_ನಮೂದು_s mcd_ಕೋಷ್ಟಕ[31];
ಸ್ಥಿರ mcd_ನಮೂದು_s *fd_ಕೋಷ್ಟಕ = ಖಾಲಿ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ fd_ಕೋಷ್ಟಕ_len = 0;

ಸ್ಥಿರ FILE* logfile;

ತಾರ್ಕಿಕ vpip_mcd0_disable = ಸುಳ್ಳು;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
ಶೂನ್ಯ vpip_mcd_init(FILE *ಲಘುಗಣಕ)
{
      fd_ಕೋಷ್ಟಕ_len = FD_INCR;
      fd_ಕೋಷ್ಟಕ = static_cast<mcd_ನಮೂದು_s *>(ಸ್ಮರಣೆ_ಕೊಡು(fd_ಕೋಷ್ಟಕ_len*ಗಾತ್ರ(mcd_ನಮೂದು_s)));
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < fd_ಕೋಷ್ಟಕ_len; idx += 1) {
	    fd_ಕೋಷ್ಟಕ[idx].fp = ಖಾಲಿ;
	    fd_ಕೋಷ್ಟಕ[idx].filename = ಖಾಲಿ;
      }

      mcd_ಕೋಷ್ಟಕ[0].fp = stdout;
      mcd_ಕೋಷ್ಟಕ[0].filename = strdup("stdout");

      fd_ಕೋಷ್ಟಕ[0].fp = stdin;
      fd_ಕೋಷ್ಟಕ[0].filename = strdup("stdin");
      fd_ಕೋಷ್ಟಕ[1].fp = stdout;
      fd_ಕೋಷ್ಟಕ[1].filename = strdup("stdout");
      fd_ಕೋಷ್ಟಕ[2].fp = stderr;
      fd_ಕೋಷ್ಟಕ[2].filename = strdup("stderr");

      logfile = ಲಘುಗಣಕ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vpi_mcd_delete(ಶೂನ್ಯ)
{
      ಬಿಡುಗಡೆ(mcd_ಕೋಷ್ಟಕ[0].filename);
      mcd_ಕೋಷ್ಟಕ[0].filename = ಖಾಲಿ;
      mcd_ಕೋಷ್ಟಕ[0].fp = ಖಾಲಿ;

      ಬಿಡುಗಡೆ(fd_ಕೋಷ್ಟಕ[0].filename);
      fd_ಕೋಷ್ಟಕ[0].filename = ಖಾಲಿ;
      fd_ಕೋಷ್ಟಕ[0].fp = ಖಾಲಿ;

      ಬಿಡುಗಡೆ(fd_ಕೋಷ್ಟಕ[1].filename);
      fd_ಕೋಷ್ಟಕ[1].filename = ಖಾಲಿ;
      fd_ಕೋಷ್ಟಕ[1].fp = ಖಾಲಿ;

      ಬಿಡುಗಡೆ(fd_ಕೋಷ್ಟಕ[2].filename);
      fd_ಕೋಷ್ಟಕ[2].filename = ಖಾಲಿ;
      fd_ಕೋಷ್ಟಕ[2].fp = ಖಾಲಿ;

      ಬಿಡುಗಡೆ(fd_ಕೋಷ್ಟಕ);
      fd_ಕೋಷ್ಟಕ = ಖಾಲಿ;
      fd_ಕೋಷ್ಟಕ_len = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
ಬಾಹ್ಯ "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      ಪೂರ್ಣಾಂಕ rc = 0;

      ಆದರೆ (IS_MCD(mcd)) {
	    ಆದರೆ (mcd & 1) rc |= 1;
	    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 1; i < 31; i++) {
		  ಆದರೆ ((mcd>>i) & 1) {
			ಆದರೆ (mcd_ಕೋಷ್ಟಕ[i].fp) {
			      ಆದರೆ (fclose(mcd_ಕೋಷ್ಟಕ[i].fp)) rc |= 1<<i;
			      ಬಿಡುಗಡೆ(mcd_ಕೋಷ್ಟಕ[i].filename);
			      mcd_ಕೋಷ್ಟಕ[i].fp = ಖಾಲಿ;
			      mcd_ಕೋಷ್ಟಕ[i].filename = ಖಾಲಿ;
			} ಇಲ್ಲದಿದ್ದರೆ {
			      rc |= 1<<i;
			}
		  }
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ idx = FD_IDX(mcd);
	    ಆದರೆ (idx > 2 && idx < fd_ಕೋಷ್ಟಕ_len && fd_ಕೋಷ್ಟಕ[idx].fp) {
		  ಆದರೆ (fclose(fd_ಕೋಷ್ಟಕ[idx].fp)) rc = mcd;
		  ಬಿಡುಗಡೆ(fd_ಕೋಷ್ಟಕ[idx].filename);
		  fd_ಕೋಷ್ಟಕ[idx].fp = ಖಾಲಿ;
		  fd_ಕೋಷ್ಟಕ[idx].filename = ಖಾಲಿ;
	    } ಇಲ್ಲದಿದ್ದರೆ rc = mcd;
      }
      ಹಿಂತಿರುಗಿ rc;
}

ಬಾಹ್ಯ "C" ಅಕ್ಷರ *vpi_mcd_name(PLI_UINT32 mcd)
{
	ಆದರೆ (IS_MCD(mcd)) {
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < 31; i++) {
			ಆದರೆ((mcd>>i) & 1)
				ಹಿಂತಿರುಗಿ mcd_ಕೋಷ್ಟಕ[i].filename;
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಚಿಹ್ನೆರಹಿತ idx = FD_IDX(mcd);
		ಆದರೆ (idx < fd_ಕೋಷ್ಟಕ_len)
			ಹಿಂತಿರುಗಿ fd_ಕೋಷ್ಟಕ[idx].filename;
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಬಾಹ್ಯ "C" PLI_UINT32 vpi_mcd_open(ಅಕ್ಷರ *name)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ(i = 0; i < 31; i++) {
		ಆದರೆ(mcd_ಕೋಷ್ಟಕ[i].filename == ಖಾಲಿ)
			ಹೋಗು got_ನಮೂದು;
	}
	ಹಿಂತಿರುಗಿ 0;  /* too many open mcd's */

got_ನಮೂದು:
#ಆದರೆ defined(__GNUC__)
	mcd_ಕೋಷ್ಟಕ[i].fp = fopen(name, "w");
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (strcmp(name, "/dev/null") != 0)
		mcd_ಕೋಷ್ಟಕ[i].fp = fopen(name, "w");
	ಇಲ್ಲದಿದ್ದರೆ
		mcd_ಕೋಷ್ಟಕ[i].fp = fopen("nul", "w");
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(mcd_ಕೋಷ್ಟಕ[i].fp == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ 0;
	mcd_ಕೋಷ್ಟಕ[i].filename = strdup(name);

	ಆದರೆ (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	ಹಿಂತಿರುಗಿ 1<<i;
}

ಬಾಹ್ಯ "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*fmt, va_list ap)
{
      ಅಕ್ಷರ ಬಫರ್[4096];
      ಅಕ್ಷರ *buf_ptr = ಬಫರ್;
      ಪೂರ್ಣಾಂಕ rc = 0;
      ತಾರ್ಕಿಕ need_free = ಸುಳ್ಳು;
      va_list saved_ap;

      ಆದರೆ (!IS_MCD(mcd)) ಹಿಂತಿರುಗಿ EOF;

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(ಬಫರ್, ಗಾತ್ರ ಬಫರ್, fmt, ap);
      ಪ್ರತಿಜ್ಞಾ(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ) rc >= ಗಾತ್ರ ಬಫರ್) {
	    buf_ptr = static_cast<ಅಕ್ಷರ *>(ಸ್ಮರಣೆ_ಕೊಡು(rc + 1));
	    need_free = ನಿಜ;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < 31; i++) {
	    ಆದರೆ((mcd>>i) & 1) {
		  ಆದರೆ(mcd_ಕೋಷ್ಟಕ[i].fp) {
			ಆದರೆ (i == 0) {
			      ಆದರೆ (logfile)
				    fputs(buf_ptr, logfile);
			      ಆದರೆ (vpip_mcd0_disable)
				    ಮುಂದುವರಿ;
			}
			fputs(buf_ptr, mcd_ಕೋಷ್ಟಕ[i].fp);
		  } ಇಲ್ಲದಿದ್ದರೆ {
			rc = EOF;
		  }
	    }
      }
      ಆದರೆ (need_free) ಬಿಡುಗಡೆ(buf_ptr);

      ಹಿಂತಿರುಗಿ rc;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ಪೂರ್ಣಾಂಕ r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      ಹಿಂತಿರುಗಿ r;
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_mcd_rawwrite(PLI_UINT32 mcd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*buf, size_t cnt)
{
      ಆದರೆ (!IS_MCD(mcd)) ಹಿಂತಿರುಗಿ;

      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ idx = 0; idx < 31; idx += 1) {
	    ಆದರೆ (((mcd>>idx) & 1) == 0)
		  ಮುಂದುವರಿ;

	    ಆದರೆ (mcd_ಕೋಷ್ಟಕ[idx].fp == 0)
		  ಮುಂದುವರಿ;

	    ಆದರೆ (idx == 0) {
		  ಆದರೆ (logfile)
			fwrite(buf, 1, cnt, logfile);
		  ಆದರೆ (vpip_mcd0_disable)
			ಮುಂದುವರಿ;
	    }
	    fwrite(buf, 1, cnt, mcd_ಕೋಷ್ಟಕ[idx].fp);
      }
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	ಪೂರ್ಣಾಂಕ rc = 0;

	ಆದರೆ (IS_MCD(mcd)) {
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < 31; i++) {
			ಆದರೆ((mcd>>i) & 1) {
				ಆದರೆ (i == 0) {
				      ಆದರೆ (logfile)
					    fflush(logfile);
				      ಆದರೆ (vpip_mcd0_disable)
					    ಮುಂದುವರಿ;
				}
				ಆದರೆ (fflush(mcd_ಕೋಷ್ಟಕ[i].fp)) rc |= 1<<i;
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಚಿಹ್ನೆರಹಿತ idx = FD_IDX(mcd);
		ಆದರೆ (idx < fd_ಕೋಷ್ಟಕ_len) rc = fflush(fd_ಕೋಷ್ಟಕ[idx].fp);
	}
	ಹಿಂತಿರುಗಿ rc;
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
ಬಾಹ್ಯ "C" PLI_INT32 vpi_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಬಹುಲಕ)
{
      ಚಿಹ್ನೆರಹಿತ i;
      ಗಾಗಿ (i = 0; i < fd_ಕೋಷ್ಟಕ_len; i += 1) {
	    ಆದರೆ (fd_ಕೋಷ್ಟಕ[i].filename == ಖಾಲಿ) ಹೋಗು got_ನಮೂದು;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      ಆದರೆ (fd_ಕೋಷ್ಟಕ_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    ಹಿಂತಿರುಗಿ 0;
      }
      fd_ಕೋಷ್ಟಕ_len += FD_INCR;
      fd_ಕೋಷ್ಟಕ = static_cast<mcd_ನಮೂದು_s *>
                 (realloc(fd_ಕೋಷ್ಟಕ, fd_ಕೋಷ್ಟಕ_len*ಗಾತ್ರ(mcd_ನಮೂದು_s)));
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = i; idx < fd_ಕೋಷ್ಟಕ_len; idx += 1) {
	    fd_ಕೋಷ್ಟಕ[idx].fp = ಖಾಲಿ;
	    fd_ಕೋಷ್ಟಕ[idx].filename = ಖಾಲಿ;
      }

got_ನಮೂದು:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _MSC_VER
	  fd_ಕೋಷ್ಟಕ[i].fp = fopen(name, ಬಹುಲಕ);
#ಇಲ್ಲದಿದ್ದರೆ // Changed for MSVC++ so vpi/pr723.v will pass.
	  ಆದರೆ(strcmp(name, "/dev/null") != 0)
		fd_ಕೋಷ್ಟಕ[i].fp = fopen(name, ಬಹುಲಕ);
	  ಇಲ್ಲದಿದ್ದರೆ
		fd_ಕೋಷ್ಟಕ[i].fp = fopen("nul", ಬಹುಲಕ);
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ (fd_ಕೋಷ್ಟಕ[i].fp == ಖಾಲಿ) ಹಿಂತಿರುಗಿ 0;
      fd_ಕೋಷ್ಟಕ[i].filename = strdup(name);
      ಹಿಂತಿರುಗಿ ((1U<<31)|i);
}

ಬಾಹ್ಯ "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      ಆದರೆ (IS_MCD(fd)) ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// Only know about fd_table_len indices
      ಆದರೆ (FD_IDX(fd) >= fd_ಕೋಷ್ಟಕ_len) ಹಿಂತಿರುಗಿ ಖಾಲಿ;

      ಹಿಂತಿರುಗಿ fd_ಕೋಷ್ಟಕ[FD_IDX(fd)].fp;
}
