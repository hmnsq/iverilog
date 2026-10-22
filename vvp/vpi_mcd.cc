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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#यदि_अन्तः
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
#परिभाषय IS_MCD(mcd)	!(static_cast<अचिह्नित>(mcd)>>31&1)
#परिभाषय FD_IDX(fd)	((fd)&~(1U<<31))
#परिभाषय FD_INCR		32

प्रकारनिर्देश संरचना mcd_प्रविष्टिः {
	FILE *fp;
	वर्ण *filename;
} mcd_प्रविष्टिः_s;
स्थिर mcd_प्रविष्टिः_s mcd_सारणी[31];
स्थिर mcd_प्रविष्टिः_s *fd_सारणी = रिक्त;
स्थिर अचिह्नित fd_सारणी_len = 0;

स्थिर FILE* logfile;

तार्किक vpip_mcd0_disable = असत्य;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
शून्य vpip_mcd_init(FILE *लघुगणकः)
{
      fd_सारणी_len = FD_INCR;
      fd_सारणी = static_cast<mcd_प्रविष्टिः_s *>(स्मृतिं_देहि(fd_सारणी_len*परिमाण(mcd_प्रविष्टिः_s)));
      प्रति (अचिह्नित idx = 0; idx < fd_सारणी_len; idx += 1) {
	    fd_सारणी[idx].fp = रिक्त;
	    fd_सारणी[idx].filename = रिक्त;
      }

      mcd_सारणी[0].fp = stdout;
      mcd_सारणी[0].filename = strdup("stdout");

      fd_सारणी[0].fp = stdin;
      fd_सारणी[0].filename = strdup("stdin");
      fd_सारणी[1].fp = stdout;
      fd_सारणी[1].filename = strdup("stdout");
      fd_सारणी[2].fp = stderr;
      fd_सारणी[2].filename = strdup("stderr");

      logfile = लघुगणकः;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vpi_mcd_delete(शून्य)
{
      मुक्त(mcd_सारणी[0].filename);
      mcd_सारणी[0].filename = रिक्त;
      mcd_सारणी[0].fp = रिक्त;

      मुक्त(fd_सारणी[0].filename);
      fd_सारणी[0].filename = रिक्त;
      fd_सारणी[0].fp = रिक्त;

      मुक्त(fd_सारणी[1].filename);
      fd_सारणी[1].filename = रिक्त;
      fd_सारणी[1].fp = रिक्त;

      मुक्त(fd_सारणी[2].filename);
      fd_सारणी[2].filename = रिक्त;
      fd_सारणी[2].fp = रिक्त;

      मुक्त(fd_सारणी);
      fd_सारणी = रिक्त;
      fd_सारणी_len = 0;
}
#यदि_अन्तः

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
बाह्य "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      पूर्णाङ्क rc = 0;

      यदि (IS_MCD(mcd)) {
	    यदि (mcd & 1) rc |= 1;
	    प्रति(पूर्णाङ्क i = 1; i < 31; i++) {
		  यदि ((mcd>>i) & 1) {
			यदि (mcd_सारणी[i].fp) {
			      यदि (fclose(mcd_सारणी[i].fp)) rc |= 1<<i;
			      मुक्त(mcd_सारणी[i].filename);
			      mcd_सारणी[i].fp = रिक्त;
			      mcd_सारणी[i].filename = रिक्त;
			} अन्यथा {
			      rc |= 1<<i;
			}
		  }
	    }
      } अन्यथा {
	    अचिह्नित idx = FD_IDX(mcd);
	    यदि (idx > 2 && idx < fd_सारणी_len && fd_सारणी[idx].fp) {
		  यदि (fclose(fd_सारणी[idx].fp)) rc = mcd;
		  मुक्त(fd_सारणी[idx].filename);
		  fd_सारणी[idx].fp = रिक्त;
		  fd_सारणी[idx].filename = रिक्त;
	    } अन्यथा rc = mcd;
      }
      प्रत्यागम rc;
}

बाह्य "C" वर्ण *vpi_mcd_name(PLI_UINT32 mcd)
{
	यदि (IS_MCD(mcd)) {
		प्रति(पूर्णाङ्क i = 0; i < 31; i++) {
			यदि((mcd>>i) & 1)
				प्रत्यागम mcd_सारणी[i].filename;
		}
	} अन्यथा {
		अचिह्नित idx = FD_IDX(mcd);
		यदि (idx < fd_सारणी_len)
			प्रत्यागम fd_सारणी[idx].filename;
	}
	प्रत्यागम रिक्त;
}

बाह्य "C" PLI_UINT32 vpi_mcd_open(वर्ण *name)
{
	पूर्णाङ्क i;

	प्रति(i = 0; i < 31; i++) {
		यदि(mcd_सारणी[i].filename == रिक्त)
			गच्छ got_प्रविष्टिः;
	}
	प्रत्यागम 0;  /* too many open mcd's */

got_प्रविष्टिः:
#यदि defined(__GNUC__)
	mcd_सारणी[i].fp = fopen(name, "w");
#अन्यथा
	यदि (वाक्यं_तुलय(name, "/dev/null") != 0)
		mcd_सारणी[i].fp = fopen(name, "w");
	अन्यथा
		mcd_सारणी[i].fp = fopen("nul", "w");
#यदि_अन्तः
	यदि(mcd_सारणी[i].fp == रिक्त)
		प्रत्यागम 0;
	mcd_सारणी[i].filename = strdup(name);

	यदि (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	प्रत्यागम 1<<i;
}

बाह्य "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ध्रुव वर्ण*fmt, va_list ap)
{
      वर्ण बफर[4096];
      वर्ण *buf_ptr = बफर;
      पूर्णाङ्क rc = 0;
      तार्किक need_free = असत्य;
      va_list saved_ap;

      यदि (!IS_MCD(mcd)) प्रत्यागम EOF;

      यदि (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (अचिह्नित पूर्णाङ्क)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(बफर, परिमाण बफर, fmt, ap);
      प्रतिज्ञा(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      यदि ((अचिह्नित) rc >= परिमाण बफर) {
	    buf_ptr = static_cast<वर्ण *>(स्मृतिं_देहि(rc + 1));
	    need_free = सत्य;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      प्रति(पूर्णाङ्क i = 0; i < 31; i++) {
	    यदि((mcd>>i) & 1) {
		  यदि(mcd_सारणी[i].fp) {
			यदि (i == 0) {
			      यदि (logfile)
				    fputs(buf_ptr, logfile);
			      यदि (vpip_mcd0_disable)
				    अनुवर्त;
			}
			fputs(buf_ptr, mcd_सारणी[i].fp);
		  } अन्यथा {
			rc = EOF;
		  }
	    }
      }
      यदि (need_free) मुक्त(buf_ptr);

      प्रत्यागम rc;
}

बाह्य "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ध्रुव वर्ण *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      पूर्णाङ्क r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      प्रत्यागम r;
}

बाह्य "C" शून्य vpip_mcd_rawwrite(PLI_UINT32 mcd, ध्रुव वर्ण*buf, size_t cnt)
{
      यदि (!IS_MCD(mcd)) प्रत्यागम;

      प्रति(पूर्णाङ्क idx = 0; idx < 31; idx += 1) {
	    यदि (((mcd>>idx) & 1) == 0)
		  अनुवर्त;

	    यदि (mcd_सारणी[idx].fp == 0)
		  अनुवर्त;

	    यदि (idx == 0) {
		  यदि (logfile)
			fwrite(buf, 1, cnt, logfile);
		  यदि (vpip_mcd0_disable)
			अनुवर्त;
	    }
	    fwrite(buf, 1, cnt, mcd_सारणी[idx].fp);
      }
}

बाह्य "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	पूर्णाङ्क rc = 0;

	यदि (IS_MCD(mcd)) {
		प्रति(पूर्णाङ्क i = 0; i < 31; i++) {
			यदि((mcd>>i) & 1) {
				यदि (i == 0) {
				      यदि (logfile)
					    fflush(logfile);
				      यदि (vpip_mcd0_disable)
					    अनुवर्त;
				}
				यदि (fflush(mcd_सारणी[i].fp)) rc |= 1<<i;
			}
		}
	} अन्यथा {
		अचिह्नित idx = FD_IDX(mcd);
		यदि (idx < fd_सारणी_len) rc = fflush(fd_सारणी[idx].fp);
	}
	प्रत्यागम rc;
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
बाह्य "C" PLI_INT32 vpi_fopen(ध्रुव वर्ण*name, ध्रुव वर्ण*बहुलकम्)
{
      अचिह्नित i;
      प्रति (i = 0; i < fd_सारणी_len; i += 1) {
	    यदि (fd_सारणी[i].filename == रिक्त) गच्छ got_प्रविष्टिः;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      यदि (fd_सारणी_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    प्रत्यागम 0;
      }
      fd_सारणी_len += FD_INCR;
      fd_सारणी = static_cast<mcd_प्रविष्टिः_s *>
                 (realloc(fd_सारणी, fd_सारणी_len*परिमाण(mcd_प्रविष्टिः_s)));
      प्रति (अचिह्नित idx = i; idx < fd_सारणी_len; idx += 1) {
	    fd_सारणी[idx].fp = रिक्त;
	    fd_सारणी[idx].filename = रिक्त;
      }

got_प्रविष्टिः:
#यदि_अपरिभाषितम् _MSC_VER
	  fd_सारणी[i].fp = fopen(name, बहुलकम्);
#अन्यथा // Changed for MSVC++ so vpi/pr723.v will pass.
	  यदि(वाक्यं_तुलय(name, "/dev/null") != 0)
		fd_सारणी[i].fp = fopen(name, बहुलकम्);
	  अन्यथा
		fd_सारणी[i].fp = fopen("nul", बहुलकम्);
#यदि_अन्तः
      यदि (fd_सारणी[i].fp == रिक्त) प्रत्यागम 0;
      fd_सारणी[i].filename = strdup(name);
      प्रत्यागम ((1U<<31)|i);
}

बाह्य "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      यदि (IS_MCD(fd)) प्रत्यागम रिक्त;

	// Only know about fd_table_len indices
      यदि (FD_IDX(fd) >= fd_सारणी_len) प्रत्यागम रिक्त;

      प्रत्यागम fd_सारणी[FD_IDX(fd)].fp;
}
