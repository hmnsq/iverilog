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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
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
#घोषणा IS_MCD(mcd)	!(static_cast<अचिन्हित>(mcd)>>31&1)
#घोषणा FD_IDX(fd)	((fd)&~(1U<<31))
#घोषणा FD_INCR		32

प्रकार काष्ठा mcd_entry {
	खाता *fp;
	अक्षर *filename;
} mcd_entry_s;
अटल mcd_entry_s mcd_table[31];
अटल mcd_entry_s *fd_table = शून्य;
अटल अचिन्हित fd_table_len = 0;

अटल खाता* logfile;

bool vpip_mcd0_disable = false;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
व्योम vpip_mcd_init(खाता *log)
{
      fd_table_len = FD_INCR;
      fd_table = static_cast<mcd_entry_s *>(दो_स्मृति(fd_table_len*माप(mcd_entry_s)));
      क्रम (अचिन्हित idx = 0; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = शून्य;
	    fd_table[idx].filename = शून्य;
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

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vpi_mcd_delete(व्योम)
{
      मुक्त(mcd_table[0].filename);
      mcd_table[0].filename = शून्य;
      mcd_table[0].fp = शून्य;

      मुक्त(fd_table[0].filename);
      fd_table[0].filename = शून्य;
      fd_table[0].fp = शून्य;

      मुक्त(fd_table[1].filename);
      fd_table[1].filename = शून्य;
      fd_table[1].fp = शून्य;

      मुक्त(fd_table[2].filename);
      fd_table[2].filename = शून्य;
      fd_table[2].fp = शून्य;

      मुक्त(fd_table);
      fd_table = शून्य;
      fd_table_len = 0;
}
#पूर्ण_अगर

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
बाह्य "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      पूर्णांक rc = 0;

      अगर (IS_MCD(mcd)) {
	    अगर (mcd & 1) rc |= 1;
	    क्रम(पूर्णांक i = 1; i < 31; i++) {
		  अगर ((mcd>>i) & 1) {
			अगर (mcd_table[i].fp) {
			      अगर (ख_बंद(mcd_table[i].fp)) rc |= 1<<i;
			      मुक्त(mcd_table[i].filename);
			      mcd_table[i].fp = शून्य;
			      mcd_table[i].filename = शून्य;
			} अन्यथा {
			      rc |= 1<<i;
			}
		  }
	    }
      } अन्यथा {
	    अचिन्हित idx = FD_IDX(mcd);
	    अगर (idx > 2 && idx < fd_table_len && fd_table[idx].fp) {
		  अगर (ख_बंद(fd_table[idx].fp)) rc = mcd;
		  मुक्त(fd_table[idx].filename);
		  fd_table[idx].fp = शून्य;
		  fd_table[idx].filename = शून्य;
	    } अन्यथा rc = mcd;
      }
      वापस rc;
}

बाह्य "C" अक्षर *vpi_mcd_name(PLI_UINT32 mcd)
{
	अगर (IS_MCD(mcd)) {
		क्रम(पूर्णांक i = 0; i < 31; i++) {
			अगर((mcd>>i) & 1)
				वापस mcd_table[i].filename;
		}
	} अन्यथा {
		अचिन्हित idx = FD_IDX(mcd);
		अगर (idx < fd_table_len)
			वापस fd_table[idx].filename;
	}
	वापस शून्य;
}

बाह्य "C" PLI_UINT32 vpi_mcd_open(अक्षर *name)
{
	पूर्णांक i;

	क्रम(i = 0; i < 31; i++) {
		अगर(mcd_table[i].filename == शून्य)
			जाओ got_entry;
	}
	वापस 0;  /* too many open mcd's */

got_entry:
#अगर defined(__GNUC__)
	mcd_table[i].fp = ख_खोलो(name, "w");
#अन्यथा
	अगर (म_भेद(name, "/dev/null") != 0)
		mcd_table[i].fp = ख_खोलो(name, "w");
	अन्यथा
		mcd_table[i].fp = ख_खोलो("nul", "w");
#पूर्ण_अगर
	अगर(mcd_table[i].fp == शून्य)
		वापस 0;
	mcd_table[i].filename = strdup(name);

	अगर (vpi_trace) {
	      ख_लिखो(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	वापस 1<<i;
}

बाह्य "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, स्थिर अक्षर*fmt, बहु_सूची ap)
{
      अक्षर बफर[4096];
      अक्षर *buf_ptr = बफर;
      पूर्णांक rc = 0;
      bool need_free = false;
      बहु_सूची saved_ap;

      अगर (!IS_MCD(mcd)) वापस खातापूर्ण;

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (अचिन्हित पूर्णांक)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(बफर, माप बफर, fmt, ap);
      निश्चित(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      अगर ((अचिन्हित) rc >= माप बफर) {
	    buf_ptr = static_cast<अक्षर *>(दो_स्मृति(rc + 1));
	    need_free = true;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      बहु_पूर्ण(saved_ap);

      क्रम(पूर्णांक i = 0; i < 31; i++) {
	    अगर((mcd>>i) & 1) {
		  अगर(mcd_table[i].fp) {
			अगर (i == 0) {
			      अगर (logfile)
				    ख_माला_दो(buf_ptr, logfile);
			      अगर (vpip_mcd0_disable)
				    जारी;
			}
			ख_माला_दो(buf_ptr, mcd_table[i].fp);
		  } अन्यथा {
			rc = खातापूर्ण;
		  }
	    }
      }
      अगर (need_free) मुक्त(buf_ptr);

      वापस rc;
}

बाह्य "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, स्थिर अक्षर *fmt, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, fmt);
      पूर्णांक r = vpi_mcd_vprintf(mcd,fmt,ap);
      बहु_पूर्ण(ap);
      वापस r;
}

बाह्य "C" व्योम vpip_mcd_rawwrite(PLI_UINT32 mcd, स्थिर अक्षर*buf, माप_प्रकार cnt)
{
      अगर (!IS_MCD(mcd)) वापस;

      क्रम(पूर्णांक idx = 0; idx < 31; idx += 1) {
	    अगर (((mcd>>idx) & 1) == 0)
		  जारी;

	    अगर (mcd_table[idx].fp == 0)
		  जारी;

	    अगर (idx == 0) {
		  अगर (logfile)
			ख_डालो(buf, 1, cnt, logfile);
		  अगर (vpip_mcd0_disable)
			जारी;
	    }
	    ख_डालो(buf, 1, cnt, mcd_table[idx].fp);
      }
}

बाह्य "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	पूर्णांक rc = 0;

	अगर (IS_MCD(mcd)) {
		क्रम(पूर्णांक i = 0; i < 31; i++) {
			अगर((mcd>>i) & 1) {
				अगर (i == 0) {
				      अगर (logfile)
					    ख_साफ(logfile);
				      अगर (vpip_mcd0_disable)
					    जारी;
				}
				अगर (ख_साफ(mcd_table[i].fp)) rc |= 1<<i;
			}
		}
	} अन्यथा {
		अचिन्हित idx = FD_IDX(mcd);
		अगर (idx < fd_table_len) rc = ख_साफ(fd_table[idx].fp);
	}
	वापस rc;
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
बाह्य "C" PLI_INT32 vpi_fopen(स्थिर अक्षर*name, स्थिर अक्षर*mode)
{
      अचिन्हित i;
      क्रम (i = 0; i < fd_table_len; i += 1) {
	    अगर (fd_table[i].filename == शून्य) जाओ got_entry;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      अगर (fd_table_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    वापस 0;
      }
      fd_table_len += FD_INCR;
      fd_table = static_cast<mcd_entry_s *>
                 (पुनः_स्मृति(fd_table, fd_table_len*माप(mcd_entry_s)));
      क्रम (अचिन्हित idx = i; idx < fd_table_len; idx += 1) {
	    fd_table[idx].fp = शून्य;
	    fd_table[idx].filename = शून्य;
      }

got_entry:
#अगर_अघोषित _MSC_VER
	  fd_table[i].fp = ख_खोलो(name, mode);
#अन्यथा // Changed for MSVC++ so vpi/pr723.v will pass.
	  अगर(म_भेद(name, "/dev/null") != 0)
		fd_table[i].fp = ख_खोलो(name, mode);
	  अन्यथा
		fd_table[i].fp = ख_खोलो("nul", mode);
#पूर्ण_अगर
      अगर (fd_table[i].fp == शून्य) वापस 0;
      fd_table[i].filename = strdup(name);
      वापस ((1U<<31)|i);
}

बाह्य "C" खाता *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      अगर (IS_MCD(fd)) वापस शून्य;

	// Only know about fd_table_len indices
      अगर (FD_IDX(fd) >= fd_table_len) वापस शून्य;

      वापस fd_table[FD_IDX(fd)].fp;
}
