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
#endif // defined(HAVE_SYS_RESOURCE_H)

#إذا defined(__MINGW32__)
# include  <windows.h>
#endif

#ضمن "libvvp.h"

using نطاق std;

ofstream debug_file;

#إذا defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
خارجي "C" صحيح getopt(صحيح argc, حرف*argv[], ثابت حرف*fmt);
خارجي "C" صحيح optind;
خارجي "C" ثابت حرف*optarg;
#endif

منطقي verbose_flag = خطأ;
ساكن صحيح vvp_return_نيلاي = 0;
ساكن صحيح vvp_used = 0;

فراغ vvp_set_stop_is_finish(منطقي flag, صحيح exit_code)
{
      خارجي منطقي stop_is_finish;
      خارجي صحيح  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

فراغ vvp_set_quiet_flag(منطقي flag)
{
      vpip_mcd0_disable = flag;
}

فراغ vvp_set_verbose_flag(منطقي flag)
{
      verbose_flag = flag;
}

فراغ vpip_set_return_نيلاي(صحيح value)
{
      vvp_return_نيلاي = value;
}

ساكن حرف log_buffer[4096];

#إذا defined(HAVE_SYS_RESOURCE_H)
ساكن فراغ my_getrusage(بنية rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     إذا defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    إذا (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    إذا (!statm) {
		  perror("/proc/self/statm");
		  أرجع;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    إذا (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
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

#endif // ! defined(HAVE_SYS_RESOURCE_H)

ساكن منطقي have_ivl_version = خطأ;
/*
 * Verify that the input file has a compatible version.
 */
فراغ verify_version(حرف*ivl_ver, حرف*commit)
{
      have_ivl_version = true;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    إذا (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      صحيح file_major, file_minor, file_minor2;
      حرف file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      صحيح rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      إذا (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    أكد((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      إذا (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      إذا (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      إذا (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

صحيح vpip_delay_selection = _vpiDelaySelTypical;
فراغ set_delay_selection(ثابت حرف* sel)
{
      إذا (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } وإلا إذا (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } وإلا إذا (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } وإلا {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

ساكن فراغ final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      لكل (vector<ثابت حرف*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (فراغ)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      اکر_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_تيندنن_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#endif
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      load_module_delete();

#ifdef CHECK_WITH_VALGRIND
      simulator_cb_delete();
#endif
}

خارجي فراغ vpip_mcd_init(FILE *log);
خارجي فراغ vvp_vpi_init(فراغ);

ساكن فراغ report_used(فراغ)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

فراغ vvp_init(ثابت حرف *logfile_name, صحيح argc, حرف*argv[])
{
      بنية rusage cycle;
      FILE *logfile = 0x0;
      خارجي فراغ vpi_set_vlog_info(صحيح, حرف**);

      إذا (vvp_used++) {
          report_used();
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
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      إذا (ثابت حرف*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
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
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, حجم(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      إذا (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

صحيح vvp_run(ثابت حرف *design_path)
{
      بنية rusage cycles[3];
      صحيح ret_cd;

      (فراغ) COPYRIGHT;
      إذا (vvp_used++ != 1) {
          إذا (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          وإلا
              report_used();
          أرجع 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      إذا (ret_cd) أرجع ret_cd;

      إذا (!have_ivl_version) {
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
		    design_path, compile_errors);
	    final_cleanup();
	    أرجع compile_errors;
      }

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   کيراءن_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  کيراءن_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  کيراءن_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",کيراءن_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", کيراءن_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   کيراءن_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   کيراءن_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     کيراءن_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   کيراءن_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   کيراءن_net_arrays, کيراءن_net_array_ڤرکاتاءن);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   کيراءن_var_arrays+کيراءن_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   کيراءن_var_arrays, کيراءن_var_array_ڤرکاتاءن);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   کيراءن_real_arrays, کيراءن_real_array_ڤرکاتاءن);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   کيراءن_vpi_scopes);
      }

      إذا (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      إذا (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   کيراءن_time_events, کيراءن_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    کيراءن_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    کيراءن_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   کيراءن_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   کيراءن_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   کيراءن_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   کيراءن_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   کيراءن_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   کيراءن_gen_events, کيراءن_gen_pool());
      }

      final_cleanup();

      أرجع vvp_return_نيلاي;
}
