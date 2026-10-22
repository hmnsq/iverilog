અચલ અક્ષર COPYRIGHT[] =
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

#જો defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#જો defined(__MINGW32__)
# include  <windows.h>
#endif

#અન્તર્ભાવ "libvvp.h"

using નામાકાશ std;

ofstream debug_file;

#જો defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
બાહ્ય "C" પૂર્ણાંક getopt(પૂર્ણાંક argc, અક્ષર*argv[], અચલ અક્ષર*fmt);
બાહ્ય "C" પૂર્ણાંક optind;
બાહ્ય "C" અચલ અક્ષર*optarg;
#endif

તાર્કિક verbose_flag = ખોટું;
સ્થિર પૂર્ણાંક vvp_return_કિંમત = 0;
સ્થિર પૂર્ણાંક vvp_used = 0;

શૂન્ય vvp_set_stop_is_finish(તાર્કિક flag, પૂર્ણાંક exit_code)
{
      બાહ્ય તાર્કિક stop_is_finish;
      બાહ્ય પૂર્ણાંક  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

શૂન્ય vvp_set_quiet_flag(તાર્કિક flag)
{
      vpip_mcd0_disable = flag;
}

શૂન્ય vvp_set_verbose_flag(તાર્કિક flag)
{
      verbose_flag = flag;
}

શૂન્ય vpip_set_return_કિંમત(પૂર્ણાંક value)
{
      vvp_return_કિંમત = value;
}

સ્થિર અક્ષર log_buffer[4096];

#જો defined(HAVE_SYS_RESOURCE_H)
સ્થિર શૂન્ય my_getrusage(સંરચના rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     જો defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    જો (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    જો (!statm) {
		  perror("/proc/self/statm");
		  પાછા;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    જો (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    fclose(statm);
      }
#     endif
}

સ્થિર શૂન્ય print_rusage(સંરચના rusage *a, સંરચના rusage *b)
{
      દ્વિગુણ delta = a->ru_utime.tv_sec
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

#નહીંતર // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
સંરચના rusage { પૂર્ણાંક x; };
અંતઃરેખ સ્થિર શૂન્ય my_getrusage(સંરચના rusage *) { }
અંતઃરેખ સ્થિર શૂન્ય print_rusage(સંરચના rusage *, સંરચના rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

સ્થિર તાર્કિક have_ivl_version = ખોટું;
/*
 * Verify that the input file has a compatible version.
 */
શૂન્ય verify_version(અક્ષર*ivl_ver, અક્ષર*commit)
{
      have_ivl_version = સાચું;

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    જો (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      પૂર્ણાંક file_major, file_minor, file_minor2;
      અક્ષર file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      પૂર્ણાંક rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      જો (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    પ્રતિજ્ઞા((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      જો (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      જો (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      જો (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

પૂર્ણાંક vpip_delay_selection = _vpiDelaySelTypical;
શૂન્ય set_delay_selection(અચલ અક્ષર* sel)
{
      જો (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } નહીંતર જો (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } નહીંતર જો (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } નહીંતર {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

સ્થિર શૂન્ય final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      માટે (vector<અચલ અક્ષર*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (શૂન્ય)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      મૂળ_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ઢગલો_delete();
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

બાહ્ય શૂન્ય vpip_mcd_init(FILE *log);
બાહ્ય શૂન્ય vvp_vpi_init(શૂન્ય);

સ્થિર શૂન્ય report_used(શૂન્ય)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

શૂન્ય vvp_init(અચલ અક્ષર *logfile_name, પૂર્ણાંક argc, અક્ષર*argv[])
{
      સંરચના rusage cycle;
      FILE *logfile = 0x0;
      બાહ્ય શૂન્ય vpi_set_vlog_info(પૂર્ણાંક, અક્ષર**);

      જો (vvp_used++) {
          report_used();
          પાછા;
      }

      જો( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          તાર્કિક debugger_release = ખોટું;
          જ્યારે( !debugger_release )  {
#જો defined(__MINGW32__)
              Sleep(1000);
#નહીંતર
              સુષુપ્તિ(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      જો (અચલ અક્ષર*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      જો (logfile_name) {
	    જો (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    નહીંતર {
		  logfile = fopen(logfile_name, "w");
		  જો (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, કદ(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      જો (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

પૂર્ણાંક vvp_run(અચલ અક્ષર *design_path)
{
      સંરચના rusage cycles[3];
      પૂર્ણાંક ret_cd;

      (શૂન્ય) COPYRIGHT;
      જો (vvp_used++ != 1) {
          જો (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          નહીંતર
              report_used();
          પાછા 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      જો (ret_cd) પાછા ret_cd;

      જો (!have_ivl_version) {
	    જો (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      જો (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      જો (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    પાછા compile_errors;
      }

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   ગણતરી_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  ગણતરી_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  ગણતરી_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",ગણતરી_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", ગણતરી_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   ગણતરી_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   ગણતરી_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     ગણતરી_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   ગણતરી_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   ગણતરી_net_arrays, ગણતરી_net_array_શબ્દો);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   ગણતરી_var_arrays+ગણતરી_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   ગણતરી_var_arrays, ગણતરી_var_array_શબ્દો);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   ગણતરી_real_arrays, ગણતરી_real_array_શબ્દો);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   ગણતરી_vpi_scopes);
      }

      જો (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      જો (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   ગણતરી_time_events, ગણતરી_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    ગણતરી_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    ગણતરી_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   ગણતરી_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   ગણતરી_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   ગણતરી_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   ગણતરી_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   ગણતરી_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   ગણતરી_gen_events, ગણતરી_gen_pool());
      }

      final_cleanup();

      પાછા vvp_return_કિંમત;
}
