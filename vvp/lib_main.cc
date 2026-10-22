ثابت حرف COPYRIGHT[] =
  "Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)";
/*
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

# include  "version_base.h"
# include  "config.h"
# include  "compile.h"
# include  "schedule.h"
# include  "vpi_priv.h"
# include  "statistics.h"
# include  "vvp_cleanup.h"
# include  "vvp_object.h"
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <unistd.h>
# include  <cassert>

#إذا defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#جے_انت // defined(HAVE_SYS_RESOURCE_H)

#إذا defined(__MINGW32__)
# include  <windows.h>
#جے_انت

#ضمن "libvvp.h"

ورتوں نطاق std;

ofstream debug_file;

#إذا defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
خارجي "C" صحيح getopt(صحيح argc, حرف*argv[], ثابت حرف*fmt);
خارجي "C" صحيح optind;
خارجي "C" ثابت حرف*optarg;
#جے_انت

منطقي verbose_flag = خطأ;
ساكن صحيح vvp_return_قدر = 0;
ساكن صحيح vvp_ورتیا = 0;

فراغ vvp_سیٹ_stop_is_انت(منطقي flag, صحيح exit_کوڈ)
{
      خارجي منطقي stop_is_انت;
      خارجي صحيح  stop_is_انت_exit_کوڈ;

      stop_is_انت = flag;
      stop_is_انت_exit_کوڈ = exit_کوڈ;
}

فراغ vvp_سیٹ_quiet_flag(منطقي flag)
{
      vpip_mcd0_disable = flag;
}

فراغ vvp_سیٹ_verbose_flag(منطقي flag)
{
      verbose_flag = flag;
}

فراغ vpip_سیٹ_return_قدر(صحيح value)
{
      vvp_return_قدر = value;
}

ساكن حرف log_بفر[4096];

#إذا defined(HAVE_SYS_RESOURCE_H)
ساكن فراغ my_getrusage(بنية rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     إذا defined(LINUX)
      {
	    FILE *statm;
	    بنانشانی siz, rss, shd;
	    لما ورقہ_ناپ = sysconf(_SC_PAGESIZE);
	    إذا (ورقہ_ناپ==-1) ورقہ_ناپ=0;
	    statm = fopen("/proc/self/statm", "r");
	    إذا (!statm) {
		  perror("/proc/self/statm");
		  أرجع;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    إذا (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = ورقہ_ناپ * siz;
		  a->ru_idrss  = ورقہ_ناپ * rss;
		  a->ru_ixrss  = ورقہ_ناپ * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ساكن فراغ print_rusage(بنية rusage *a, بنية rusage *b)
{
      مزدوج delta = a->ru_utime.tv_sec
	    +        a->ru_utime.tv_usec/1E6
	    +        a->ru_stime.tv_sec
	    +        a->ru_stime.tv_usec/1E6
	    -        b->ru_utime.tv_sec
	    -        b->ru_utime.tv_usec/1E6
	    -        b->ru_stime.tv_sec
	    -        b->ru_stime.tv_usec/1E6
	    ;

      vpi_mcd_printf(1,
	      " ... %G seconds,"
	      " %.1f/%.1f/%.1f KBytes size/rss/shared\n",
	      delta,
	      a->ru_maxrss/1024.0,
	      (a->ru_idrss+a->ru_isrss)/1024.0,
	      a->ru_ixrss/1024.0 );
}

#وإلا // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
بنية rusage { صحيح x; };
مضمن ساكن فراغ my_getrusage(بنية rusage *) { }
مضمن ساكن فراغ print_rusage(بنية rusage *, بنية rusage *){};

#جے_انت // ! defined(HAVE_SYS_RESOURCE_H)

ساكن منطقي have_ivl_نسخہ = خطأ;
/*
 * Verify that the input file has a compatible version.
 */
فراغ verify_نسخہ(حرف*ivl_ver, حرف*commit)
{
      have_ivl_نسخہ = سچ;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    إذا (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      مٹاؤ[] commit;

      صحيح file_major, file_minor, file_minor2;
      حرف file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      صحيح rc = لڑی_توں_پڑھو(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      إذا (rc == 2) {
	    file_extra[0] = 0;
	    rc = لڑی_توں_پڑھو(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    أكد((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      مٹاؤ[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      إذا (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      إذا (نسخہ_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, نسخہ);
	    باہر_نکلو(1);
      }

      إذا (نسخہ_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, نسخہ);
      }
}

صحيح vpip_delay_selection = _vpiDelaySelTypical;
فراغ سیٹ_delay_selection(ثابت حرف* sel)
{
      إذا (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } وإلا إذا (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } وإلا إذا (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = vpi_Delay_Sel_ودھ_توں_ودھ;
      } وإلا {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    باہر_نکلو(1);
      }
      مٹاؤ[] sel;
}

ساكن فراغ final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      لكل (vector<ثابت حرف*>::iterator cur = file_ناں.begin();
           cur != file_ناں.سرا() ; ++ cur ) {
	    مٹاؤ[] *cur;
      }
	/* Clear the static result buffer. */
      (فراغ)need_نتیجہ_buf(0, RBUF_DEL);
      codespace_delete();
      جڑ_جدول_delete();
      def_جدول_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ڈھیر_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#جے_انت
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      لدو_module_delete();

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      simulator_cb_delete();
#جے_انت
}

خارجي فراغ vpip_mcd_init(FILE *لوگارتھم);
خارجي فراغ vvp_vpi_init(فراغ);

ساكن فراغ report_ورتیا(فراغ)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

فراغ vvp_init(ثابت حرف *logfile_name, صحيح argc, حرف*argv[])
{
      بنية rusage چکر;
      FILE *logfile = 0x0;
      خارجي فراغ vpi_سیٹ_vlog_info(صحيح, حرف**);

      إذا (vvp_ورتیا++) {
          report_ورتیا();
          أرجع;
      }

      إذا( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          منطقي debugger_release = خطأ;
          طالما( !debugger_release )  {
#إذا defined(__MINGW32__)
              Sleep(1000);
#وإلا
              نوم(1);
#جے_انت
        }
      }

      vpip_جوڑو_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      إذا (ثابت حرف*رستہ = getenv("VVP_DEBUG")) {
	    debug_file.open(رستہ, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      إذا (logfile_name) {
	    إذا (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    وإلا {
		  logfile = fopen(logfile_name, "w");
		  إذا (!logfile) {
		        perror(logfile_name);
		        باہر_نکلو(1);
		  }
		  setvbuf(logfile, log_بفر, _IOLBF, حجم(log_بفر));
	    }
      }

      vpip_mcd_init(logfile);

      إذا (verbose_flag) {
	    my_getrusage(&چکر);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_سیٹ_vlog_info(argc, argv);

      compile_init();
}

صحيح vvp_لگاتار(ثابت حرف *design_رستہ)
{
      بنية rusage چکراں[3];
      صحيح ret_cd;

      (فراغ) COPYRIGHT;
      إذا (vvp_ورتیا++ != 1) {
          إذا (vvp_ورتیا == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          وإلا
              report_ورتیا();
          أرجع 1;
      }
      ++vvp_ورتیا;

      ret_cd = compile_design(design_رستہ);
      destroy_lexor();
      print_vpi_call_errors();
      إذا (ret_cd) أرجع ret_cd;

      إذا (!have_ivl_نسخہ) {
	    إذا (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      إذا (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_رستہ, compile_errors);
	    final_cleanup();
	    أرجع compile_errors;
      }

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   گنتی_functors, vvp_net_fun_t::ڈھیر_کل());
	    vpi_mcd_printf(1, "           %8lu logic\n",  گنتی_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  گنتی_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",گنتی_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", گنتی_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   گنتی_filters, vvp_net_fil_t::ڈھیر_کل());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   گنتی_opcodes, ناپ_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     گنتی_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   گنتی_vvp_nets, ناپ_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   گنتی_net_arrays, گنتی_net_array_الفاظ);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   گنتی_var_arrays+گنتی_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   گنتی_var_arrays, گنتی_var_array_الفاظ);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   گنتی_real_arrays, گنتی_real_array_الفاظ);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   گنتی_vpi_scopes);
      }

      إذا (verbose_flag) {
	    my_getrusage(چکراں+1);
	    print_rusage(چکراں+1, چکراں+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      ویلا_سارنی_simulate();

      إذا (verbose_flag) {
	    my_getrusage(چکراں+2);
	    print_rusage(چکراں+2, چکراں+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   گنتی_ویلا_events, گنتی_ویلا_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    گنتی_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    گنتی_سونپو_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   گنتی_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   گنتی_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   گنتی_سونپو_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   گنتی_سونپو_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   گنتی_سونپو_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   گنتی_gen_events, گنتی_gen_pool());
      }

      final_cleanup();

      أرجع vvp_return_قدر;
}
