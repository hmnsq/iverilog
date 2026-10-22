ثابت نویسه COPYRIGHT[] =
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

#اگر defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#اگر defined(__MINGW32__)
# include  <windows.h>
#endif

#شامل "libvvp.h"

using فضاینام std;

ofstream debug_file;

#اگر defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
بیرونی "C" صحیح getopt(صحیح argc, نویسه*argv[], ثابت نویسه*fmt);
بیرونی "C" صحیح optind;
بیرونی "C" ثابت نویسه*optarg;
#endif

منطقی verbose_flag = نادرست;
ایستا صحیح vvp_return_مقدار = 0;
ایستا صحیح vvp_used = 0;

تهی vvp_set_stop_is_finish(منطقی flag, صحیح exit_code)
{
      بیرونی منطقی stop_is_finish;
      بیرونی صحیح  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

تهی vvp_set_quiet_flag(منطقی flag)
{
      vpip_mcd0_disable = flag;
}

تهی vvp_set_verbose_flag(منطقی flag)
{
      verbose_flag = flag;
}

تهی vpip_set_return_مقدار(صحیح value)
{
      vvp_return_مقدار = value;
}

ایستا نویسه log_buffer[4096];

#اگر defined(HAVE_SYS_RESOURCE_H)
ایستا تهی my_getrusage(ساختار rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     اگر defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    اگر (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    اگر (!statm) {
		  perror("/proc/self/statm");
		  بازگشت;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    اگر (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ایستا تهی print_rusage(ساختار rusage *a, ساختار rusage *b)
{
      دوگانه delta = a->ru_utime.tv_sec
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

#وگرنه // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ساختار rusage { صحیح x; };
درونخطی ایستا تهی my_getrusage(ساختار rusage *) { }
درونخطی ایستا تهی print_rusage(ساختار rusage *, ساختار rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

ایستا منطقی have_ivl_version = نادرست;
/*
 * Verify that the input file has a compatible version.
 */
تهی verify_version(نویسه*ivl_ver, نویسه*commit)
{
      have_ivl_version = درست;

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    اگر (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      صحیح file_major, file_minor, file_minor2;
      نویسه file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      صحیح rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      اگر (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    أكد((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      اگر (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      اگر (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      اگر (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

صحیح vpip_delay_selection = _vpiDelaySelTypical;
تهی set_delay_selection(ثابت نویسه* sel)
{
      اگر (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } وگرنه اگر (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } وگرنه اگر (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } وگرنه {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

ایستا تهی final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      برای (vector<ثابت نویسه*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (تهی)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      ریشه_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_پشته_delete();
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

بیرونی تهی vpip_mcd_init(FILE *log);
بیرونی تهی vvp_vpi_init(تهی);

ایستا تهی report_used(تهی)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

تهی vvp_init(ثابت نویسه *logfile_name, صحیح argc, نویسه*argv[])
{
      ساختار rusage cycle;
      FILE *logfile = 0x0;
      بیرونی تهی vpi_set_vlog_info(صحیح, نویسه**);

      اگر (vvp_used++) {
          report_used();
          بازگشت;
      }

      اگر( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          منطقی debugger_release = نادرست;
          وقتی( !debugger_release )  {
#اگر defined(__MINGW32__)
              Sleep(1000);
#وگرنه
              نوم(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      اگر (ثابت نویسه*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      اگر (logfile_name) {
	    اگر (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    وگرنه {
		  logfile = fopen(logfile_name, "w");
		  اگر (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, اندازه(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      اگر (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

صحیح vvp_run(ثابت نویسه *design_path)
{
      ساختار rusage cycles[3];
      صحیح ret_cd;

      (تهی) COPYRIGHT;
      اگر (vvp_used++ != 1) {
          اگر (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          وگرنه
              report_used();
          بازگشت 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      اگر (ret_cd) بازگشت ret_cd;

      اگر (!have_ivl_version) {
	    اگر (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      اگر (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    بازگشت compile_errors;
      }

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   شمار_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  شمار_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  شمار_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",شمار_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", شمار_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   شمار_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   شمار_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     شمار_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   شمار_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   شمار_net_arrays, شمار_net_array_واژگان);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   شمار_var_arrays+شمار_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   شمار_var_arrays, شمار_var_array_واژگان);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   شمار_real_arrays, شمار_real_array_واژگان);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   شمار_vpi_scopes);
      }

      اگر (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      اگر (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   شمار_time_events, شمار_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    شمار_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    شمار_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   شمار_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   شمار_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   شمار_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   شمار_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   شمار_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   شمار_gen_events, شمار_gen_pool());
      }

      final_cleanup();

      بازگشت vvp_return_مقدار;
}
