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

#ܐܢ defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#ܐܢ defined(__MINGW32__)
# include  <windows.h>
#endif

#ضمن "libvvp.h"

using نطاق std;

ofstream debug_file;

#ܐܢ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
خارجي "C" صحيح getopt(صحيح argc, حرف*argv[], ثابت حرف*fmt);
خارجي "C" صحيح optind;
خارجي "C" ثابت حرف*optarg;
#endif

منطقي verbose_flag = ܕܓܠܐ;
ساكن صحيح vvp_return_value = 0;
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

فراغ vpip_set_return_value(صحيح value)
{
      vvp_return_value = value;
}

ساكن حرف log_buffer[4096];

#ܐܢ defined(HAVE_SYS_RESOURCE_H)
ساكن فراغ my_getrusage(بنية rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     ܐܢ defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    ܐܢ (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    ܐܢ (!statm) {
		  perror("/proc/self/statm");
		  ܦܢܐ;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    ܐܢ (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
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

#ܐܚܪܢܐ // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
بنية rusage { صحيح x; };
مضمن ساكن فراغ my_getrusage(بنية rusage *) { }
مضمن ساكن فراغ print_rusage(بنية rusage *, بنية rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

ساكن منطقي have_ivl_version = ܕܓܠܐ;
/*
 * Verify that the input file has a compatible version.
 */
فراغ verify_version(حرف*ivl_ver, حرف*commit)
{
      have_ivl_version = ܫܪܪܐ;

      ܐܢ (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    ܐܢ (commit) vpi_mcd_printf(1, " %s", commit);
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
      ܐܢ (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    أكد((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      ܐܢ (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      ܐܢ (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      ܐܢ (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

صحيح vpip_delay_selection = _vpiDelaySelTypical;
فراغ set_delay_selection(ثابت حرف* sel)
{
      ܐܢ (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } ܐܚܪܢܐ ܐܢ (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } ܐܚܪܢܐ ܐܢ (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } ܐܚܪܢܐ {
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
      root_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_stack_delete();
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

      ܐܢ (vvp_used++) {
          report_used();
          ܦܢܐ;
      }

      ܐܢ( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          منطقي debugger_release = ܕܓܠܐ;
          ܟܡܐ( !debugger_release )  {
#ܐܢ defined(__MINGW32__)
              Sleep(1000);
#ܐܚܪܢܐ
              نوم(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      ܐܢ (ثابت حرف*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      ܐܢ (logfile_name) {
	    ܐܢ (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    ܐܚܪܢܐ {
		  logfile = fopen(logfile_name, "w");
		  ܐܢ (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, حجم(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      ܐܢ (verbose_flag) {
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
      ܐܢ (vvp_used++ != 1) {
          ܐܢ (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          ܐܚܪܢܐ
              report_used();
          ܦܢܐ 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      ܐܢ (ret_cd) ܦܢܐ ret_cd;

      ܐܢ (!have_ivl_version) {
	    ܐܢ (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      ܐܢ (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      ܐܢ (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    ܦܢܐ compile_errors;
      }

      ܐܢ (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   count_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  count_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  count_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",count_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", count_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   count_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   count_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     count_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   count_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   count_net_arrays, count_net_array_words);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   count_var_arrays+count_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   count_var_arrays, count_var_array_words);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   count_real_arrays, count_real_array_words);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   count_vpi_scopes);
      }

      ܐܢ (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      ܐܢ (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   count_time_events, count_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    count_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    count_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   count_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   count_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   count_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   count_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   count_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   count_gen_events, count_gen_pool());
      }

      final_cleanup();

      ܦܢܐ vvp_return_value;
}
