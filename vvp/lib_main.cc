ধ্রুবক অক্ষর COPYRIGHT[] =
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

#যদি defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#যদি defined(__MINGW32__)
# include  <windows.h>
#endif

#অন্তর্ভাব "libvvp.h"

using নামাকাশ std;

ofstream debug_file;

#যদি defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
বাহ্যিক "C" পূর্ণসংখ্যা getopt(পূর্ণসংখ্যা argc, অক্ষর*argv[], ধ্রুবক অক্ষর*fmt);
বাহ্যিক "C" পূর্ণসংখ্যা optind;
বাহ্যিক "C" ধ্রুবক অক্ষর*optarg;
#endif

যৌক্তিক verbose_flag = মিথ্যা;
স্থির পূর্ণসংখ্যা vvp_return_মান = 0;
স্থির পূর্ণসংখ্যা vvp_used = 0;

শূন্য vvp_set_stop_is_finish(যৌক্তিক flag, পূর্ণসংখ্যা exit_code)
{
      বাহ্যিক যৌক্তিক stop_is_finish;
      বাহ্যিক পূর্ণসংখ্যা  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

শূন্য vvp_set_quiet_flag(যৌক্তিক flag)
{
      vpip_mcd0_disable = flag;
}

শূন্য vvp_set_verbose_flag(যৌক্তিক flag)
{
      verbose_flag = flag;
}

শূন্য vpip_set_return_মান(পূর্ণসংখ্যা value)
{
      vvp_return_মান = value;
}

স্থির অক্ষর log_বাফার[4096];

#যদি defined(HAVE_SYS_RESOURCE_H)
স্থির শূন্য my_getrusage(কাঠামো rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     যদি defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_আকার = sysconf(_SC_PAGESIZE);
	    যদি (page_আকার==-1) page_আকার=0;
	    statm = fopen("/proc/self/statm", "r");
	    যদি (!statm) {
		  perror("/proc/self/statm");
		  ফেরত;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    যদি (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_আকার * siz;
		  a->ru_idrss  = page_আকার * rss;
		  a->ru_ixrss  = page_আকার * shd;
	    }
	    fclose(statm);
      }
#     endif
}

স্থির শূন্য print_rusage(কাঠামো rusage *a, কাঠামো rusage *b)
{
      দ্বিগুণ delta = a->ru_utime.tv_sec
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

#অন্যথা // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
কাঠামো rusage { পূর্ণসংখ্যা x; };
অন্তঃরেখ স্থির শূন্য my_getrusage(কাঠামো rusage *) { }
অন্তঃরেখ স্থির শূন্য print_rusage(কাঠামো rusage *, কাঠামো rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

স্থির যৌক্তিক have_ivl_version = মিথ্যা;
/*
 * Verify that the input file has a compatible version.
 */
শূন্য verify_version(অক্ষর*ivl_ver, অক্ষর*commit)
{
      have_ivl_version = সত্য;

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    যদি (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      পূর্ণসংখ্যা file_major, file_minor, file_minor2;
      অক্ষর file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      পূর্ণসংখ্যা rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      যদি (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    প্রতিজ্ঞা((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      যদি (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      যদি (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      যদি (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

পূর্ণসংখ্যা vpip_delay_selection = _vpiDelaySelTypical;
শূন্য set_delay_selection(ধ্রুবক অক্ষর* sel)
{
      যদি (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } অন্যথা যদি (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } অন্যথা যদি (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } অন্যথা {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

স্থির শূন্য final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      জন্য (vector<ধ্রুবক অক্ষর*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (শূন্য)need_ফলাফল_buf(0, RBUF_DEL);
      codespace_delete();
      মূল_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_স্তূপ_delete();
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

বাহ্যিক শূন্য vpip_mcd_init(FILE *log);
বাহ্যিক শূন্য vvp_vpi_init(শূন্য);

স্থির শূন্য report_used(শূন্য)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

শূন্য vvp_init(ধ্রুবক অক্ষর *logfile_name, পূর্ণসংখ্যা argc, অক্ষর*argv[])
{
      কাঠামো rusage cycle;
      FILE *logfile = 0x0;
      বাহ্যিক শূন্য vpi_set_vlog_info(পূর্ণসংখ্যা, অক্ষর**);

      যদি (vvp_used++) {
          report_used();
          ফেরত;
      }

      যদি( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          যৌক্তিক debugger_release = মিথ্যা;
          যখন( !debugger_release )  {
#যদি defined(__MINGW32__)
              Sleep(1000);
#অন্যথা
              সুষুপ্তি(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      যদি (ধ্রুবক অক্ষর*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      যদি (logfile_name) {
	    যদি (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    অন্যথা {
		  logfile = fopen(logfile_name, "w");
		  যদি (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_বাফার, _IOLBF, আকার(log_বাফার));
	    }
      }

      vpip_mcd_init(logfile);

      যদি (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

পূর্ণসংখ্যা vvp_run(ধ্রুবক অক্ষর *design_path)
{
      কাঠামো rusage cycles[3];
      পূর্ণসংখ্যা ret_cd;

      (শূন্য) COPYRIGHT;
      যদি (vvp_used++ != 1) {
          যদি (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          অন্যথা
              report_used();
          ফেরত 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      যদি (ret_cd) ফেরত ret_cd;

      যদি (!have_ivl_version) {
	    যদি (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      যদি (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    ফেরত compile_errors;
      }

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   গুনতি_functors, vvp_net_fun_t::heap_যোগ());
	    vpi_mcd_printf(1, "           %8lu logic\n",  গুনতি_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  গুনতি_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",গুনতি_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", গুনতি_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   গুনতি_filters, vvp_net_fil_t::heap_যোগ());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   গুনতি_opcodes, আকার_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     গুনতি_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   গুনতি_vvp_nets, আকার_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   গুনতি_net_arrays, গুনতি_net_array_শব্দগুলি);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   গুনতি_var_arrays+গুনতি_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   গুনতি_var_arrays, গুনতি_var_array_শব্দগুলি);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   গুনতি_real_arrays, গুনতি_real_array_শব্দগুলি);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   গুনতি_vpi_scopes);
      }

      যদি (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      যদি (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   গুনতি_time_events, গুনতি_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    গুনতি_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    গুনতি_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   গুনতি_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   গুনতি_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   গুনতি_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   গুনতি_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   গুনতি_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   গুনতি_gen_events, গুনতি_gen_pool());
      }

      final_cleanup();

      ফেরত vvp_return_মান;
}
