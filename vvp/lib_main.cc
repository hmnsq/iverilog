ಧ್ರುವ ವರ್ಣ COPYRIGHT[] =
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

#ಯದಿ defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#ಯದಿ defined(__MINGW32__)
# include  <windows.h>
#endif

#ಅನ್ತರ್ಭಾವ "libvvp.h"

using ನಾಮಾಕಾಶ std;

ofstream debug_file;

#ಯದಿ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಙ್ಕ getopt(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ*argv[], ಧ್ರುವ ವರ್ಣ*fmt);
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಙ್ಕ optind;
ಬಾಹ್ಯ "C" ಧ್ರುವ ವರ್ಣ*optarg;
#endif

ತಾರ್ಕಿಕ verbose_flag = ಅಸತ್ಯ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vvp_return_ಮೌಲ್ಯ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vvp_used = 0;

ಶೂನ್ಯ vvp_set_stop_is_finish(ತಾರ್ಕಿಕ flag, ಪೂರ್ಣಾಙ್ಕ exit_code)
{
      ಬಾಹ್ಯ ತಾರ್ಕಿಕ stop_is_finish;
      ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

ಶೂನ್ಯ vvp_set_quiet_flag(ತಾರ್ಕಿಕ flag)
{
      vpip_mcd0_disable = flag;
}

ಶೂನ್ಯ vvp_set_verbose_flag(ತಾರ್ಕಿಕ flag)
{
      verbose_flag = flag;
}

ಶೂನ್ಯ vpip_set_return_ಮೌಲ್ಯ(ಪೂರ್ಣಾಙ್ಕ value)
{
      vvp_return_ಮೌಲ್ಯ = value;
}

ಸ್ಥಿರ ವರ್ಣ log_buffer[4096];

#ಯದಿ defined(HAVE_SYS_RESOURCE_H)
ಸ್ಥಿರ ಶೂನ್ಯ my_getrusage(ಸಂರಚನಾ rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     ಯದಿ defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    ಯದಿ (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    ಯದಿ (!statm) {
		  perror("/proc/self/statm");
		  ಪ್ರತ್ಯಾಗಮ;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    ಯದಿ (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ಸ್ಥಿರ ಶೂನ್ಯ print_rusage(ಸಂರಚನಾ rusage *a, ಸಂರಚನಾ rusage *b)
{
      ದ್ವಿಗುಣ delta = a->ru_utime.tv_sec
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

#ಅನ್ಯಥಾ // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ಸಂರಚನಾ rusage { ಪೂರ್ಣಾಙ್ಕ x; };
ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಶೂನ್ಯ my_getrusage(ಸಂರಚನಾ rusage *) { }
ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಶೂನ್ಯ print_rusage(ಸಂರಚನಾ rusage *, ಸಂರಚನಾ rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

ಸ್ಥಿರ ತಾರ್ಕಿಕ have_ivl_version = ಅಸತ್ಯ;
/*
 * Verify that the input file has a compatible version.
 */
ಶೂನ್ಯ verify_version(ವರ್ಣ*ivl_ver, ವರ್ಣ*commit)
{
      have_ivl_version = ಸತ್ಯ;

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    ಯದಿ (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      ಪೂರ್ಣಾಙ್ಕ file_major, file_minor, file_minor2;
      ವರ್ಣ file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      ಪೂರ್ಣಾಙ್ಕ rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      ಯದಿ (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    ಪ್ರತಿಜ್ಞಾ((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      ಯದಿ (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      ಯದಿ (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      ಯದಿ (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

ಪೂರ್ಣಾಙ್ಕ vpip_delay_selection = _vpiDelaySelTypical;
ಶೂನ್ಯ set_delay_selection(ಧ್ರುವ ವರ್ಣ* sel)
{
      ಯದಿ (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } ಅನ್ಯಥಾ ಯದಿ (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } ಅನ್ಯಥಾ ಯದಿ (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } ಅನ್ಯಥಾ {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

ಸ್ಥಿರ ಶೂನ್ಯ final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      ಪ್ರತಿ (vector<ಧ್ರುವ ವರ್ಣ*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (ಶೂನ್ಯ)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      ಬೇರು_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ರಾಶಿ_delete();
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

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_mcd_init(FILE *log);
ಬಾಹ್ಯ ಶೂನ್ಯ vvp_vpi_init(ಶೂನ್ಯ);

ಸ್ಥಿರ ಶೂನ್ಯ report_used(ಶೂನ್ಯ)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

ಶೂನ್ಯ vvp_init(ಧ್ರುವ ವರ್ಣ *logfile_name, ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ*argv[])
{
      ಸಂರಚನಾ rusage cycle;
      FILE *logfile = 0x0;
      ಬಾಹ್ಯ ಶೂನ್ಯ vpi_set_vlog_info(ಪೂರ್ಣಾಙ್ಕ, ವರ್ಣ**);

      ಯದಿ (vvp_used++) {
          report_used();
          ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          ತಾರ್ಕಿಕ debugger_release = ಅಸತ್ಯ;
          ಯಾವತ್( !debugger_release )  {
#ಯದಿ defined(__MINGW32__)
              Sleep(1000);
#ಅನ್ಯಥಾ
              ಸುಷುಪ್ತಿ(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      ಯದಿ (ಧ್ರುವ ವರ್ಣ*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      ಯದಿ (logfile_name) {
	    ಯದಿ (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    ಅನ್ಯಥಾ {
		  logfile = fopen(logfile_name, "w");
		  ಯದಿ (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, ಪರಿಮಾಣ(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      ಯದಿ (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

ಪೂರ್ಣಾಙ್ಕ vvp_run(ಧ್ರುವ ವರ್ಣ *design_path)
{
      ಸಂರಚನಾ rusage cycles[3];
      ಪೂರ್ಣಾಙ್ಕ ret_cd;

      (ಶೂನ್ಯ) COPYRIGHT;
      ಯದಿ (vvp_used++ != 1) {
          ಯದಿ (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          ಅನ್ಯಥಾ
              report_used();
          ಪ್ರತ್ಯಾಗಮ 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      ಯದಿ (ret_cd) ಪ್ರತ್ಯಾಗಮ ret_cd;

      ಯದಿ (!have_ivl_version) {
	    ಯದಿ (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      ಯದಿ (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    ಪ್ರತ್ಯಾಗಮ compile_errors;
      }

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   ಎಣಿಕೆ_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  ಎಣಿಕೆ_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  ಎಣಿಕೆ_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",ಎಣಿಕೆ_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", ಎಣಿಕೆ_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   ಎಣಿಕೆ_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   ಎಣಿಕೆ_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     ಎಣಿಕೆ_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   ಎಣಿಕೆ_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   ಎಣಿಕೆ_net_arrays, ಎಣಿಕೆ_net_array_ಪದಗಳು);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   ಎಣಿಕೆ_var_arrays+ಎಣಿಕೆ_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   ಎಣಿಕೆ_var_arrays, ಎಣಿಕೆ_var_array_ಪದಗಳು);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   ಎಣಿಕೆ_real_arrays, ಎಣಿಕೆ_real_array_ಪದಗಳು);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   ಎಣಿಕೆ_vpi_scopes);
      }

      ಯದಿ (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      ಯದಿ (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   ಎಣಿಕೆ_time_events, ಎಣಿಕೆ_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    ಎಣಿಕೆ_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    ಎಣಿಕೆ_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   ಎಣಿಕೆ_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   ಎಣಿಕೆ_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   ಎಣಿಕೆ_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   ಎಣಿಕೆ_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   ಎಣಿಕೆ_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   ಎಣಿಕೆ_gen_events, ಎಣಿಕೆ_gen_pool());
      }

      final_cleanup();

      ಪ್ರತ್ಯಾಗಮ vvp_return_ಮೌಲ್ಯ;
}
