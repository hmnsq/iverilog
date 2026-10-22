மாறிலி எழுத்து COPYRIGHT[] =
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

#எனில் defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#எனில் defined(__MINGW32__)
# include  <windows.h>
#endif

#அந்தர்பாவ "libvvp.h"

using நாமாகாஶ std;

ofstream debug_file;

#எனில் defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
வெளி "C" முழு getopt(முழு argc, எழுத்து*argv[], மாறிலி எழுத்து*fmt);
வெளி "C" முழு optind;
வெளி "C" மாறிலி எழுத்து*optarg;
#endif

தருக்க verbose_flag = பொய்;
நிலையான முழு vvp_return_மதிப்பு = 0;
நிலையான முழு vvp_used = 0;

வெறுமை vvp_set_stop_is_finish(தருக்க flag, முழு exit_code)
{
      வெளி தருக்க stop_is_finish;
      வெளி முழு  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

வெறுமை vvp_set_quiet_flag(தருக்க flag)
{
      vpip_mcd0_disable = flag;
}

வெறுமை vvp_set_verbose_flag(தருக்க flag)
{
      verbose_flag = flag;
}

வெறுமை vpip_set_return_மதிப்பு(முழு value)
{
      vvp_return_மதிப்பு = value;
}

நிலையான எழுத்து log_buffer[4096];

#எனில் defined(HAVE_SYS_RESOURCE_H)
நிலையான வெறுமை my_getrusage(கட்டமைப்பு rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     எனில் defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    எனில் (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    எனில் (!statm) {
		  perror("/proc/self/statm");
		  திரும்பு;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    எனில் (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    fclose(statm);
      }
#     endif
}

நிலையான வெறுமை print_rusage(கட்டமைப்பு rusage *a, கட்டமைப்பு rusage *b)
{
      இரட்டை delta = a->ru_utime.tv_sec
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

#இல்லையெனில் // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
கட்டமைப்பு rusage { முழு x; };
உள்வரி நிலையான வெறுமை my_getrusage(கட்டமைப்பு rusage *) { }
உள்வரி நிலையான வெறுமை print_rusage(கட்டமைப்பு rusage *, கட்டமைப்பு rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

நிலையான தருக்க have_ivl_version = பொய்;
/*
 * Verify that the input file has a compatible version.
 */
வெறுமை verify_version(எழுத்து*ivl_ver, எழுத்து*commit)
{
      have_ivl_version = உண்மை;

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    எனில் (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      முழு file_major, file_minor, file_minor2;
      எழுத்து file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      முழு rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      எனில் (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    ப்ரதிஜ்ஞா((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      எனில் (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      எனில் (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      எனில் (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

முழு vpip_delay_selection = _vpiDelaySelTypical;
வெறுமை set_delay_selection(மாறிலி எழுத்து* sel)
{
      எனில் (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } இல்லையெனில் எனில் (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } இல்லையெனில் எனில் (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } இல்லையெனில் {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

நிலையான வெறுமை final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      க்கு (vector<மாறிலி எழுத்து*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (வெறுமை)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      வேர்_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_அடுக்கு_delete();
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

வெளி வெறுமை vpip_mcd_init(FILE *log);
வெளி வெறுமை vvp_vpi_init(வெறுமை);

நிலையான வெறுமை report_used(வெறுமை)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

வெறுமை vvp_init(மாறிலி எழுத்து *logfile_name, முழு argc, எழுத்து*argv[])
{
      கட்டமைப்பு rusage cycle;
      FILE *logfile = 0x0;
      வெளி வெறுமை vpi_set_vlog_info(முழு, எழுத்து**);

      எனில் (vvp_used++) {
          report_used();
          திரும்பு;
      }

      எனில்( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          தருக்க debugger_release = பொய்;
          போது( !debugger_release )  {
#எனில் defined(__MINGW32__)
              Sleep(1000);
#இல்லையெனில்
              ஸுஷுப்தி(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      எனில் (மாறிலி எழுத்து*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      எனில் (logfile_name) {
	    எனில் (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    இல்லையெனில் {
		  logfile = fopen(logfile_name, "w");
		  எனில் (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, அளவு(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      எனில் (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

முழு vvp_run(மாறிலி எழுத்து *design_path)
{
      கட்டமைப்பு rusage cycles[3];
      முழு ret_cd;

      (வெறுமை) COPYRIGHT;
      எனில் (vvp_used++ != 1) {
          எனில் (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          இல்லையெனில்
              report_used();
          திரும்பு 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      எனில் (ret_cd) திரும்பு ret_cd;

      எனில் (!have_ivl_version) {
	    எனில் (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      எனில் (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    திரும்பு compile_errors;
      }

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   மொத்தம்_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  மொத்தம்_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  மொத்தம்_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",மொத்தம்_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", மொத்தம்_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   மொத்தம்_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   மொத்தம்_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     மொத்தம்_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   மொத்தம்_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   மொத்தம்_net_arrays, மொத்தம்_net_array_சொற்கள்);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   மொத்தம்_var_arrays+மொத்தம்_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   மொத்தம்_var_arrays, மொத்தம்_var_array_சொற்கள்);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   மொத்தம்_real_arrays, மொத்தம்_real_array_சொற்கள்);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   மொத்தம்_vpi_scopes);
      }

      எனில் (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      எனில் (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   மொத்தம்_time_events, மொத்தம்_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    மொத்தம்_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    மொத்தம்_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   மொத்தம்_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   மொத்தம்_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   மொத்தம்_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   மொத்தம்_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   மொத்தம்_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   மொத்தம்_gen_events, மொத்தம்_gen_pool());
      }

      final_cleanup();

      திரும்பு vvp_return_மதிப்பு;
}
