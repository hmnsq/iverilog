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

बाह्य खाता* vpi_trace;

/*
 * This table keeps track of the MCD files. Note that there may be
 * only 31 such files, and mcd bit0 (32'h00_00_00_01) is the special
 * standard output file, which may be replicated to a logfile and/or
 * suppressed depending on flags to the command line.
 */

/*
 * MCD/FD manipulation macros
 */
#define IS_MCD(mcd)	!(static_cast<अचिन्हित>(mcd)>>31&1)
#define FD_IDX(fd)	((fd)&~(1U<<31))
#define FD_INCR		32

प्रकारनिर्देश संरचना mcd_entry {
	खाता *fp;
	वर्ण *filename;
} mcd_entry_s;
स्थिर mcd_entry_s mcd_table[31];
स्थिर mcd_entry_s *fd_table = रिक्त;
स्थिर अचिन्हित fd_table_len = 0;

स्थिर खाता* logfile;

तार्किक vpip_mcd0_disable = असत्य;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
शून्य vpip_mcd_init(खाता *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(दो_स्मृति(fd_table_len*परिमाण(mcd_entry_s)));
      प्रति (अचिन्हित idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = रिक्त;
	    fd_table[idx].filename = रिक्त;
      }

      mcd_table[0].fp = मानक_निकास;
      mcd_table[0].filename = strdup("stdout");

      fd_table[0].fp = मानक_निवेश;
      fd_table[0].filename = strdup("stdin");
      fd_table[1].fp = मानक_निकास;
      fd_table[1].filename = strdup("stdout");
      fd_table[2].fp = मानक_त्रुटि;
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

      यदि (IS_MCD(mcd)) {
	    यदि (mcd & 1) rc |= 1;
	    प्रति(पूर्णाङ्क i = 1; i < 31; i++) {
		  यदि ((mcd>>i) & 1) {
			यदि (mcd_table[i].fp) {
			      यदि (ख_बंद(mcd_table[i].fp)) rc |= 1<<i;
			      मुक्त(mcd_table[i].filename);
			      mcd_table[i].fp = रिक्त;
			      mcd_table[i].filename = रिक्त;
			} अन्यथा {
			      rc |= 1<<i;
			}
		  }
	    }
      } अन्यथा {
	    अचिन्हित idx = FD_IDX(mcd);
	    यदि (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  यदि (ख_बंद(fd_table[idx].fp)) rc = mcd;
		  मुक्त(fd_table[idx].filename);
		  fd_table[idx].fp = रिक्त;
		  fd_table[idx].filename = रिक्त;
	    } अन्यथा rc = mcd;
      }
      प्रत्यागम rc;
}

बाह्य "C" वर्ण *vpi_mcd_name(PLI_UINT32 mcd)
{
	यदि (IS_MCD(mcd)) {
		प्रति(पूर्णाङ्क i = 0; i < 31; i++) {
			यदि((mcd>>i) & 1)
				प्रत्यागम mcd_table[i].filename;
		}
	} अन्यथा {
		अचिन्हित idx = FD_IDX(mcd);
		यदि (idx < fd_table_len)
			प्रत्यागम fd_table[idx].filename;
	}
	प्रत्यागम रिक्त;
}

बाह्य "C" PLI_UINT32 vpi_mcd_open(वर्ण *name)
{
	पूर्णाङ्क i;

	प्रति(i = 0; i < 31; i++) {
		यदि(mcd_table[i].filename == रिक्त)
			गच्छ got_entry;
	}
	प्रत्यागम 0;  /* too many open mcd's */

got_entry:
#यदि defined(__GNUC__)
	mcd_table[i].fp = ख_खोलो(name, "w");
#अन्यथा
	यदि (म_भेद(name, "/dev/null") != 0)
		mcd_table[i].fp = ख_खोलो(name, "w");
	अन्यथा
		mcd_table[i].fp = ख_खोलो("nul", "w");
#endif
	यदि(mcd_table[i].fp == रिक्त)
		प्रत्यागम 0;
	mcd_table[i].filename = strdup(name);

	यदि (vpi_trace) {
	      ख_लिखो(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	प्रत्यागम 1<<i;
}

बाह्य "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, ध्रुव वर्ण*fmt, बहु_सूची ap)
{
      वर्ण buffer[4096];
      वर्ण *buf_ptr = buffer;
      पूर्णाङ्क rc = 0;
      तार्किक need_free = असत्य;
      बहु_सूची saved_ap;

      यदि (!IS_MCD(mcd)) प्रत्यागम खातापूर्ण;

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (अचिन्हित पूर्णाङ्क)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(buffer, परिमाण buffer, fmt, ap);
      प्रतिज्ञा(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      यदि ((अचिन्हित) rc >= परिमाण buffer) {
	    buf_ptr = static_cast<वर्ण *>(दो_स्मृति(rc + 1));
	    need_free = सत्य;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      बहु_पूर्ण(saved_ap);

      प्रति(पूर्णाङ्क i = 0; i < 31; i++) {
	    यदि((mcd>>i) & 1) {
		  यदि(mcd_table[i].fp) {
			यदि (i == 0) {
			      यदि (logfile)
				    ख_माला_दो(buf_ptr, logfile);
			      यदि (vpip_mcd0_disable)
				    अनुवर्त;
			}
			ख_माला_दो(buf_ptr, mcd_table[i].fp);
		  } अन्यथा {
			rc = खातापूर्ण;
		  }
	    }
      }
      यदि (need_free) मुक्त(buf_ptr);

      प्रत्यागम rc;
}

बाह्य "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, ध्रुव वर्ण *fmt, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, fmt);
      पूर्णाङ्क r = vpi_mcd_vprintf(mcd,fmt,ap);
      बहु_पूर्ण(ap);
      प्रत्यागम r;
}

बाह्य "C" शून्य vpip_mcd_rawwrite(PLI_UINT32 mcd, ध्रुव वर्ण*buf, माप_प्रकार cnt)
{
      यदि (!IS_MCD(mcd)) प्रत्यागम;

      प्रति(पूर्णाङ्क idx = 0; idx < 31; idx += 1) {
	    यदि (((mcd>>idx) & 1) == 0)
		  अनुवर्त;

	    यदि (mcd_table[idx].fp == 0)
		  अनुवर्त;

	    यदि (idx == 0) {
		  यदि (logfile)
			ख_डालो(buf, 1, cnt, logfile);
		  यदि (vpip_mcd0_disable)
			अनुवर्त;
	    }
	    ख_डालो(buf, 1, cnt, mcd_table[idx].fp);
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
					    ख_साफ(logfile);
				      यदि (vpip_mcd0_disable)
					    अनुवर्त;
				}
				यदि (ख_साफ(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} अन्यथा {
		अचिन्हित idx = FD_IDX(mcd);
		यदि (idx < fd_table_len) rc = ख_साफ(fd_table[idx].fp);
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
बाह्य "C" PLI_INT32 vpi_fopen(ध्रुव वर्ण*name, ध्रुव वर्ण*mode)
{
      अचिन्हित i;
      प्रति (i = 0; i < fd_table_len; i += 1) {
	    यदि (fd_table[i].filename == रिक्त) गच्छ got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      यदि (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    प्रत्यागम 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (पुनः_स्मृति(fd_table, fd_table_len*परिमाण(mcd_entry_s)));
      प्रति (अचिन्हित idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = रिक्त;
	    fd_table[idx].filename = रिक्त;
      }

got_entry:
#ifndef _MSC_VER
	  fd_table[i].fp = ख_खोलो(name, mode);
#अन्यथा // Changed for MSVC++ so vpi/pr723.v will pass.
	  यदि(म_भेद(name, "/dev/null") != 0)
		fd_table[i].fp = ख_खोलो(name, mode);
	  अन्यथा
		fd_table[i].fp = ख_खोलो("nul", mode);
#endif
      यदि (fd_table[i].fp == रिक्त) प्रत्यागम 0;
      fd_table[i].filename = strdup(name);
      प्रत्यागम ((1U<<31)|i);
}

बाह्य "C" खाता *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      यदि (IS_MCD(fd)) प्रत्यागम रिक्त;

	// Only know about fd_table_len indices
      यदि (FD_IDX(fd) >= fd_table_len) प्रत्यागम रिक्त;

      प्रत्यागम fd_table[FD_IDX(fd)].fp;
}
