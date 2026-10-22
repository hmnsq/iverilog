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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cassert>
# include  <cstdarg>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  "ivl_alloc.h"

బాహ్య FILE* vpi_trace;

/*
 * This table keeps track of the MCD files. Note that there may be
 * only 31 such files, and mcd bit0 (32'h00_00_00_01) is the special
 * standard output file, which may be replicated to a logfile and/or
 * suppressed depending on flags to the command line.
 */

/*
 * MCD/FD manipulation macros
 */
#నిర్వచించు IS_MCD(mcd)	!(static_cast<గుర్తులేని>(mcd)>>31&1)
#నిర్వచించు FD_IDX(fd)	((fd)&~(1U<<31))
#నిర్వచించు FD_INCR		32

రకనిర్వచనం నిర్మాణం mcd_నమోదు {
	FILE *fp;
	అక్షరం *filename;
} mcd_నమోదు_s;
స్థిర mcd_నమోదు_s mcd_పట్టిక[31];
స్థిర mcd_నమోదు_s *fd_పట్టిక = ఖాళీ;
స్థిర గుర్తులేని fd_పట్టిక_len = 0;

స్థిర FILE* logfile;

తార్కిక vpip_mcd0_disable = అబద్ధం;

/* Initialize mcd portion of vpi.  Must be called before
 * any vpi_mcd routines can be used.
 */
శూన్య vpip_mcd_init(FILE *సంవర్గమానం)
{
      fd_పట్టిక_len = FD_INCR;
      fd_పట్టిక = static_cast<mcd_నమోదు_s *>(జ్ఞాపకం_ఇవ్వు(fd_పట్టిక_len*పరిమాణం(mcd_నమోదు_s)));
      కొరకు (గుర్తులేని idx = 0; idx < fd_పట్టిక_len; idx += 1) {
	    fd_పట్టిక[idx].fp = ఖాళీ;
	    fd_పట్టిక[idx].filename = ఖాళీ;
      }

      mcd_పట్టిక[0].fp = stdout;
      mcd_పట్టిక[0].filename = strdup("stdout");

      fd_పట్టిక[0].fp = stdin;
      fd_పట్టిక[0].filename = strdup("stdin");
      fd_పట్టిక[1].fp = stdout;
      fd_పట్టిక[1].filename = strdup("stdout");
      fd_పట్టిక[2].fp = stderr;
      fd_పట్టిక[2].filename = strdup("stderr");

      logfile = సంవర్గమానం;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vpi_mcd_delete(శూన్య)
{
      విడుదల(mcd_పట్టిక[0].filename);
      mcd_పట్టిక[0].filename = ఖాళీ;
      mcd_పట్టిక[0].fp = ఖాళీ;

      విడుదల(fd_పట్టిక[0].filename);
      fd_పట్టిక[0].filename = ఖాళీ;
      fd_పట్టిక[0].fp = ఖాళీ;

      విడుదల(fd_పట్టిక[1].filename);
      fd_పట్టిక[1].filename = ఖాళీ;
      fd_పట్టిక[1].fp = ఖాళీ;

      విడుదల(fd_పట్టిక[2].filename);
      fd_పట్టిక[2].filename = ఖాళీ;
      fd_పట్టిక[2].fp = ఖాళీ;

      విడుదల(fd_పట్టిక);
      fd_పట్టిక = ఖాళీ;
      fd_పట్టిక_len = 0;
}
#అయితే_ముగింపు

/*
 * Close one or more channels. We refuse to close the preopened ones.
 */
బాహ్య "C" PLI_UINT32 vpi_mcd_close(PLI_UINT32 mcd)
{
      పూర్ణాంకం rc = 0;

      ఒకవేళ (IS_MCD(mcd)) {
	    ఒకవేళ (mcd & 1) rc |= 1;
	    కొరకు(పూర్ణాంకం i = 1; i < 31; i++) {
		  ఒకవేళ ((mcd>>i) & 1) {
			ఒకవేళ (mcd_పట్టిక[i].fp) {
			      ఒకవేళ (fclose(mcd_పట్టిక[i].fp)) rc |= 1<<i;
			      విడుదల(mcd_పట్టిక[i].filename);
			      mcd_పట్టిక[i].fp = ఖాళీ;
			      mcd_పట్టిక[i].filename = ఖాళీ;
			} లేకపోతే {
			      rc |= 1<<i;
			}
		  }
	    }
      } లేకపోతే {
	    గుర్తులేని idx = FD_IDX(mcd);
	    ఒకవేళ (idx > 2 && idx < fd_పట్టిక_len && fd_పట్టిక[idx].fp) {
		  ఒకవేళ (fclose(fd_పట్టిక[idx].fp)) rc = mcd;
		  విడుదల(fd_పట్టిక[idx].filename);
		  fd_పట్టిక[idx].fp = ఖాళీ;
		  fd_పట్టిక[idx].filename = ఖాళీ;
	    } లేకపోతే rc = mcd;
      }
      తిరిగి rc;
}

బాహ్య "C" అక్షరం *vpi_mcd_name(PLI_UINT32 mcd)
{
	ఒకవేళ (IS_MCD(mcd)) {
		కొరకు(పూర్ణాంకం i = 0; i < 31; i++) {
			ఒకవేళ((mcd>>i) & 1)
				తిరిగి mcd_పట్టిక[i].filename;
		}
	} లేకపోతే {
		గుర్తులేని idx = FD_IDX(mcd);
		ఒకవేళ (idx < fd_పట్టిక_len)
			తిరిగి fd_పట్టిక[idx].filename;
	}
	తిరిగి ఖాళీ;
}

బాహ్య "C" PLI_UINT32 vpi_mcd_open(అక్షరం *name)
{
	పూర్ణాంకం i;

	కొరకు(i = 0; i < 31; i++) {
		ఒకవేళ(mcd_పట్టిక[i].filename == ఖాళీ)
			వెళ్ళు got_నమోదు;
	}
	తిరిగి 0;  /* too many open mcd's */

got_నమోదు:
#ఒకవేళ defined(__GNUC__)
	mcd_పట్టిక[i].fp = fopen(name, "w");
#లేకపోతే
	ఒకవేళ (strcmp(name, "/dev/null") != 0)
		mcd_పట్టిక[i].fp = fopen(name, "w");
	లేకపోతే
		mcd_పట్టిక[i].fp = fopen("nul", "w");
#అయితే_ముగింపు
	ఒకవేళ(mcd_పట్టిక[i].fp == ఖాళీ)
		తిరిగి 0;
	mcd_పట్టిక[i].filename = strdup(name);

	ఒకవేళ (vpi_trace) {
	      fprintf(vpi_trace, "vpi_mcd_open(%s) --> 0x%08x\n",
		      name, 1 << i);
	}

	తిరిగి 1<<i;
}

బాహ్య "C" PLI_INT32
vpi_mcd_vprintf(PLI_UINT32 mcd, స్థిరరాశి అక్షరం*fmt, va_list ap)
{
      అక్షరం బఫరు[4096];
      అక్షరం *buf_ptr = బఫరు;
      పూర్ణాంకం rc = 0;
      తార్కిక need_free = అబద్ధం;
      va_list saved_ap;

      ఒకవేళ (!IS_MCD(mcd)) తిరిగి EOF;

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_mcd_vprintf(0x%08x, %s, ...);\n",
		    (గుర్తులేని పూర్ణాంకం)mcd, fmt);
      }

      va_copy(saved_ap, ap);
      rc = vsnprintf(బఫరు, పరిమాణం బఫరు, fmt, ap);
      ప్రతిజ్ఞా(rc >= 0);
	/*
	 * If rc is greater than sizeof buffer then the result was truncated
	 * so the print needs to be redone with a larger buffer (very rare).
	 */
      ఒకవేళ ((గుర్తులేని) rc >= పరిమాణం బఫరు) {
	    buf_ptr = static_cast<అక్షరం *>(జ్ఞాపకం_ఇవ్వు(rc + 1));
	    need_free = నిజం;
	    rc = vsnprintf(buf_ptr, rc+1, fmt, saved_ap);
      }
      va_end(saved_ap);

      కొరకు(పూర్ణాంకం i = 0; i < 31; i++) {
	    ఒకవేళ((mcd>>i) & 1) {
		  ఒకవేళ(mcd_పట్టిక[i].fp) {
			ఒకవేళ (i == 0) {
			      ఒకవేళ (logfile)
				    fputs(buf_ptr, logfile);
			      ఒకవేళ (vpip_mcd0_disable)
				    కొనసాగు;
			}
			fputs(buf_ptr, mcd_పట్టిక[i].fp);
		  } లేకపోతే {
			rc = EOF;
		  }
	    }
      }
      ఒకవేళ (need_free) విడుదల(buf_ptr);

      తిరిగి rc;
}

బాహ్య "C" PLI_INT32 vpi_mcd_printf(PLI_UINT32 mcd, స్థిరరాశి అక్షరం *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      పూర్ణాంకం r = vpi_mcd_vprintf(mcd,fmt,ap);
      va_end(ap);
      తిరిగి r;
}

బాహ్య "C" శూన్య vpip_mcd_rawwrite(PLI_UINT32 mcd, స్థిరరాశి అక్షరం*buf, size_t cnt)
{
      ఒకవేళ (!IS_MCD(mcd)) తిరిగి;

      కొరకు(పూర్ణాంకం idx = 0; idx < 31; idx += 1) {
	    ఒకవేళ (((mcd>>idx) & 1) == 0)
		  కొనసాగు;

	    ఒకవేళ (mcd_పట్టిక[idx].fp == 0)
		  కొనసాగు;

	    ఒకవేళ (idx == 0) {
		  ఒకవేళ (logfile)
			fwrite(buf, 1, cnt, logfile);
		  ఒకవేళ (vpip_mcd0_disable)
			కొనసాగు;
	    }
	    fwrite(buf, 1, cnt, mcd_పట్టిక[idx].fp);
      }
}

బాహ్య "C" PLI_INT32 vpi_mcd_flush(PLI_UINT32 mcd)
{
	పూర్ణాంకం rc = 0;

	ఒకవేళ (IS_MCD(mcd)) {
		కొరకు(పూర్ణాంకం i = 0; i < 31; i++) {
			ఒకవేళ((mcd>>i) & 1) {
				ఒకవేళ (i == 0) {
				      ఒకవేళ (logfile)
					    fflush(logfile);
				      ఒకవేళ (vpip_mcd0_disable)
					    కొనసాగు;
				}
				ఒకవేళ (fflush(mcd_పట్టిక[i].fp)) rc |= 1<<i;
			}
		}
	} లేకపోతే {
		గుర్తులేని idx = FD_IDX(mcd);
		ఒకవేళ (idx < fd_పట్టిక_len) rc = fflush(fd_పట్టిక[idx].fp);
	}
	తిరిగి rc;
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
బాహ్య "C" PLI_INT32 vpi_fopen(స్థిరరాశి అక్షరం*name, స్థిరరాశి అక్షరం*బహుళకం)
{
      గుర్తులేని i;
      కొరకు (i = 0; i < fd_పట్టిక_len; i += 1) {
	    ఒకవేళ (fd_పట్టిక[i].filename == ఖాళీ) వెళ్ళు got_నమోదు;
      }
	/* We need to allocate more table entries, but to keep things */
	/* sane we'll hard limit this to 1024 file descriptors total. */
      ఒకవేళ (fd_పట్టిక_len >= 1024) {
	    vpi_printf("WARNING: Icarus only supports 1024 open files!\n");
	    తిరిగి 0;
      }
      fd_పట్టిక_len += FD_INCR;
      fd_పట్టిక = static_cast<mcd_నమోదు_s *>
                 (realloc(fd_పట్టిక, fd_పట్టిక_len*పరిమాణం(mcd_నమోదు_s)));
      కొరకు (గుర్తులేని idx = i; idx < fd_పట్టిక_len; idx += 1) {
	    fd_పట్టిక[idx].fp = ఖాళీ;
	    fd_పట్టిక[idx].filename = ఖాళీ;
      }

got_నమోదు:
#నిర్వచితం_కాకపోతే _MSC_VER
	  fd_పట్టిక[i].fp = fopen(name, బహుళకం);
#లేకపోతే // Changed for MSVC++ so vpi/pr723.v will pass.
	  ఒకవేళ(strcmp(name, "/dev/null") != 0)
		fd_పట్టిక[i].fp = fopen(name, బహుళకం);
	  లేకపోతే
		fd_పట్టిక[i].fp = fopen("nul", బహుళకం);
#అయితే_ముగింపు
      ఒకవేళ (fd_పట్టిక[i].fp == ఖాళీ) తిరిగి 0;
      fd_పట్టిక[i].filename = strdup(name);
      తిరిగి ((1U<<31)|i);
}

బాహ్య "C" FILE *vpi_get_file(PLI_INT32 fd)
{
	// Only deal with FD's
      ఒకవేళ (IS_MCD(fd)) తిరిగి ఖాళీ;

	// Only know about fd_table_len indices
      ఒకవేళ (FD_IDX(fd) >= fd_పట్టిక_len) తిరిగి ఖాళీ;

      తిరిగి fd_పట్టిక[FD_IDX(fd)].fp;
}
