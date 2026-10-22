സ്ഥിരം അക്ഷരം COPYRIGHT[] =
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

#എങ്കിൽ defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#എങ്കിൽ defined(__MINGW32__)
# include  <windows.h>
#endif

#അന്തര്ഭാവ "libvvp.h"

using നാമാകാശ std;

ofstream debug_file;

#എങ്കിൽ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ബാഹ്യ "C" പൂർണ്ണസംഖ്യ getopt(പൂർണ്ണസംഖ്യ argc, അക്ഷരം*argv[], സ്ഥിരം അക്ഷരം*fmt);
ബാഹ്യ "C" പൂർണ്ണസംഖ്യ optind;
ബാഹ്യ "C" സ്ഥിരം അക്ഷരം*optarg;
#endif

തർക്കം verbose_flag = അസത്യം;
സ്ഥിതം പൂർണ്ണസംഖ്യ vvp_return_മൂല്യം = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ vvp_used = 0;

ശൂന്യം vvp_set_stop_is_finish(തർക്കം flag, പൂർണ്ണസംഖ്യ exit_code)
{
      ബാഹ്യ തർക്കം stop_is_finish;
      ബാഹ്യ പൂർണ്ണസംഖ്യ  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

ശൂന്യം vvp_set_quiet_flag(തർക്കം flag)
{
      vpip_mcd0_disable = flag;
}

ശൂന്യം vvp_set_verbose_flag(തർക്കം flag)
{
      verbose_flag = flag;
}

ശൂന്യം vpip_set_return_മൂല്യം(പൂർണ്ണസംഖ്യ value)
{
      vvp_return_മൂല്യം = value;
}

സ്ഥിതം അക്ഷരം log_buffer[4096];

#എങ്കിൽ defined(HAVE_SYS_RESOURCE_H)
സ്ഥിതം ശൂന്യം my_getrusage(ഘടന rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     എങ്കിൽ defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    എങ്കിൽ (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    എങ്കിൽ (!statm) {
		  perror("/proc/self/statm");
		  തിരികെ;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    എങ്കിൽ (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    fclose(statm);
      }
#     endif
}

സ്ഥിതം ശൂന്യം print_rusage(ഘടന rusage *a, ഘടന rusage *b)
{
      ഇരട്ട delta = a->ru_utime.tv_sec
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

#അല്ലെങ്കിൽ // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ഘടന rusage { പൂർണ്ണസംഖ്യ x; };
വരിയിൽ സ്ഥിതം ശൂന്യം my_getrusage(ഘടന rusage *) { }
വരിയിൽ സ്ഥിതം ശൂന്യം print_rusage(ഘടന rusage *, ഘടന rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

സ്ഥിതം തർക്കം have_ivl_version = അസത്യം;
/*
 * Verify that the input file has a compatible version.
 */
ശൂന്യം verify_version(അക്ഷരം*ivl_ver, അക്ഷരം*commit)
{
      have_ivl_version = സത്യം;

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    എങ്കിൽ (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      പൂർണ്ണസംഖ്യ file_major, file_minor, file_minor2;
      അക്ഷരം file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      പൂർണ്ണസംഖ്യ rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      എങ്കിൽ (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    പ്രതിജ്ഞാ((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      എങ്കിൽ (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      എങ്കിൽ (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      എങ്കിൽ (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

പൂർണ്ണസംഖ്യ vpip_delay_selection = _vpiDelaySelTypical;
ശൂന്യം set_delay_selection(സ്ഥിരം അക്ഷരം* sel)
{
      എങ്കിൽ (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } അല്ലെങ്കിൽ {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

സ്ഥിതം ശൂന്യം final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      വേണ്ടി (vector<സ്ഥിരം അക്ഷരം*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (ശൂന്യം)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      വേര്_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_അട്ടി_delete();
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

ബാഹ്യ ശൂന്യം vpip_mcd_init(FILE *log);
ബാഹ്യ ശൂന്യം vvp_vpi_init(ശൂന്യം);

സ്ഥിതം ശൂന്യം report_used(ശൂന്യം)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

ശൂന്യം vvp_init(സ്ഥിരം അക്ഷരം *logfile_name, പൂർണ്ണസംഖ്യ argc, അക്ഷരം*argv[])
{
      ഘടന rusage cycle;
      FILE *logfile = 0x0;
      ബാഹ്യ ശൂന്യം vpi_set_vlog_info(പൂർണ്ണസംഖ്യ, അക്ഷരം**);

      എങ്കിൽ (vvp_used++) {
          report_used();
          തിരികെ;
      }

      എങ്കിൽ( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          തർക്കം debugger_release = അസത്യം;
          ആകുമ്പോൾ( !debugger_release )  {
#എങ്കിൽ defined(__MINGW32__)
              Sleep(1000);
#അല്ലെങ്കിൽ
              സുഷുപ്തി(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      എങ്കിൽ (സ്ഥിരം അക്ഷരം*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      എങ്കിൽ (logfile_name) {
	    എങ്കിൽ (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    അല്ലെങ്കിൽ {
		  logfile = fopen(logfile_name, "w");
		  എങ്കിൽ (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, വലുപ്പം(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      എങ്കിൽ (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

പൂർണ്ണസംഖ്യ vvp_run(സ്ഥിരം അക്ഷരം *design_path)
{
      ഘടന rusage cycles[3];
      പൂർണ്ണസംഖ്യ ret_cd;

      (ശൂന്യം) COPYRIGHT;
      എങ്കിൽ (vvp_used++ != 1) {
          എങ്കിൽ (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          അല്ലെങ്കിൽ
              report_used();
          തിരികെ 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      എങ്കിൽ (ret_cd) തിരികെ ret_cd;

      എങ്കിൽ (!have_ivl_version) {
	    എങ്കിൽ (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      എങ്കിൽ (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    തിരികെ compile_errors;
      }

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   എണ്ണം_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  എണ്ണം_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  എണ്ണം_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",എണ്ണം_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", എണ്ണം_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   എണ്ണം_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   എണ്ണം_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     എണ്ണം_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   എണ്ണം_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   എണ്ണം_net_arrays, എണ്ണം_net_array_വാക്കുകൾ);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   എണ്ണം_var_arrays+എണ്ണം_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   എണ്ണം_var_arrays, എണ്ണം_var_array_വാക്കുകൾ);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   എണ്ണം_real_arrays, എണ്ണം_real_array_വാക്കുകൾ);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   എണ്ണം_vpi_scopes);
      }

      എങ്കിൽ (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      എങ്കിൽ (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   എണ്ണം_time_events, എണ്ണം_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    എണ്ണം_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    എണ്ണം_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   എണ്ണം_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   എണ്ണം_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   എണ്ണം_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   എണ്ണം_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   എണ്ണം_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   എണ്ണം_gen_events, എണ്ണം_gen_pool());
      }

      final_cleanup();

      തിരികെ vvp_return_മൂല്യം;
}
