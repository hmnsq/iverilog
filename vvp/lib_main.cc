ଅଚଳ ଅକ୍ଷର COPYRIGHT[] =
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

#ଯଦି defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#ଯଦି defined(__MINGW32__)
# include  <windows.h>
#endif

#ଅନ୍ତର୍ଭାଵ "libvvp.h"

using ନାମାକାଶ std;

ofstream debug_file;

#ଯଦି defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ବାହ୍ୟ "C" ପୂର୍ଣ୍ଣସଂଖ୍ୟା getopt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର*argv[], ଅଚଳ ଅକ୍ଷର*fmt);
ବାହ୍ୟ "C" ପୂର୍ଣ୍ଣସଂଖ୍ୟା optind;
ବାହ୍ୟ "C" ଅଚଳ ଅକ୍ଷର*optarg;
#endif

ତାର୍କିକ verbose_flag = ମିଥ୍ୟା;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vvp_return_ମୂଲ୍ୟ = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vvp_used = 0;

ଶୂନ୍ୟ vvp_set_stop_is_finish(ତାର୍କିକ flag, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exit_code)
{
      ବାହ୍ୟ ତାର୍କିକ stop_is_finish;
      ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

ଶୂନ୍ୟ vvp_set_quiet_flag(ତାର୍କିକ flag)
{
      vpip_mcd0_disable = flag;
}

ଶୂନ୍ୟ vvp_set_verbose_flag(ତାର୍କିକ flag)
{
      verbose_flag = flag;
}

ଶୂନ୍ୟ vpip_set_return_ମୂଲ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା value)
{
      vvp_return_ମୂଲ୍ୟ = value;
}

ସ୍ଥିର ଅକ୍ଷର log_buffer[4096];

#ଯଦି defined(HAVE_SYS_RESOURCE_H)
ସ୍ଥିର ଶୂନ୍ୟ my_getrusage(ସଂରଚନା rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     ଯଦି defined(LINUX)
      {
	    FILE *statm;
	    unsigned siz, rss, shd;
	    long page_size = sysconf(_SC_PAGESIZE);
	    ଯଦି (page_size==-1) page_size=0;
	    statm = fopen("/proc/self/statm", "r");
	    ଯଦି (!statm) {
		  perror("/proc/self/statm");
		  ଫେରନ୍ତୁ;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    ଯଦି (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ସ୍ଥିର ଶୂନ୍ୟ print_rusage(ସଂରଚନା rusage *a, ସଂରଚନା rusage *b)
{
      ଦ୍ୱିଗୁଣ delta = a->ru_utime.tv_sec
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

#ଅନ୍ୟଥା // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ସଂରଚନା rusage { ପୂର୍ଣ୍ଣସଂଖ୍ୟା x; };
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ my_getrusage(ସଂରଚନା rusage *) { }
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ print_rusage(ସଂରଚନା rusage *, ସଂରଚନା rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

ସ୍ଥିର ତାର୍କିକ have_ivl_version = ମିଥ୍ୟା;
/*
 * Verify that the input file has a compatible version.
 */
ଶୂନ୍ୟ verify_version(ଅକ୍ଷର*ivl_ver, ଅକ୍ଷର*commit)
{
      have_ivl_version = ସତ୍ୟ;

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    ଯଦି (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_major, file_minor, file_minor2;
      ଅକ୍ଷର file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sscanf(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      ଯଦି (rc == 2) {
	    file_extra[0] = 0;
	    rc = sscanf(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    ପ୍ରତିଜ୍ଞା((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      ଯଦି (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      ଯଦି (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    exit(1);
      }

      ଯଦି (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_delay_selection = _vpiDelaySelTypical;
ଶୂନ୍ୟ set_delay_selection(ଅଚଳ ଅକ୍ଷର* sel)
{
      ଯଦି (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } ଅନ୍ୟଥା ଯଦି (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } ଅନ୍ୟଥା ଯଦି (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } ଅନ୍ୟଥା {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    exit(1);
      }
      delete[] sel;
}

ସ୍ଥିର ଶୂନ୍ୟ final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      ପାଇଁ (vector<ଅଚଳ ଅକ୍ଷର*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (ଶୂନ୍ୟ)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      ମୂଳ_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ଗଦା_delete();
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

ବାହ୍ୟ ଶୂନ୍ୟ vpip_mcd_init(FILE *log);
ବାହ୍ୟ ଶୂନ୍ୟ vvp_vpi_init(ଶୂନ୍ୟ);

ସ୍ଥିର ଶୂନ୍ୟ report_used(ଶୂନ୍ୟ)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

ଶୂନ୍ୟ vvp_init(ଅଚଳ ଅକ୍ଷର *logfile_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର*argv[])
{
      ସଂରଚନା rusage cycle;
      FILE *logfile = 0x0;
      ବାହ୍ୟ ଶୂନ୍ୟ vpi_set_vlog_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର**);

      ଯଦି (vvp_used++) {
          report_used();
          ଫେରନ୍ତୁ;
      }

      ଯଦି( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          ତାର୍କିକ debugger_release = ମିଥ୍ୟା;
          ଯେତେବେଳେ( !debugger_release )  {
#ଯଦି defined(__MINGW32__)
              Sleep(1000);
#ଅନ୍ୟଥା
              ସୁଷୁପ୍ତି(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      ଯଦି (ଅଚଳ ଅକ୍ଷର*path = getenv("VVP_DEBUG")) {
	    debug_file.open(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      ଯଦି (logfile_name) {
	    ଯଦି (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    ଅନ୍ୟଥା {
		  logfile = fopen(logfile_name, "w");
		  ଯଦି (!logfile) {
		        perror(logfile_name);
		        exit(1);
		  }
		  setvbuf(logfile, log_buffer, _IOLBF, ଆକାର(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      ଯଦି (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vvp_run(ଅଚଳ ଅକ୍ଷର *design_path)
{
      ସଂରଚନା rusage cycles[3];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_cd;

      (ଶୂନ୍ୟ) COPYRIGHT;
      ଯଦି (vvp_used++ != 1) {
          ଯଦି (vvp_used == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          ଅନ୍ୟଥା
              report_used();
          ଫେରନ୍ତୁ 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      ଯଦି (ret_cd) ଫେରନ୍ତୁ ret_cd;

      ଯଦି (!have_ivl_version) {
	    ଯଦି (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      ଯଦି (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    ଫେରନ୍ତୁ compile_errors;
      }

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   ଗଣନା_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  ଗଣନା_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  ଗଣନା_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",ଗଣନା_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", ଗଣନା_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   ଗଣନା_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   ଗଣନା_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     ଗଣନା_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   ଗଣନା_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   ଗଣନା_net_arrays, ଗଣନା_net_array_ଶବ୍ଦଗୁଡ଼ିକ);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   ଗଣନା_var_arrays+ଗଣନା_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   ଗଣନା_var_arrays, ଗଣନା_var_array_ଶବ୍ଦଗୁଡ଼ିକ);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   ଗଣନା_real_arrays, ଗଣନା_real_array_ଶବ୍ଦଗୁଡ଼ିକ);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   ଗଣନା_vpi_scopes);
      }

      ଯଦି (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      ଯଦି (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   ଗଣନା_time_events, ଗଣନା_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    ଗଣନା_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    ଗଣନା_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   ଗଣନା_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   ଗଣନା_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   ଗଣନା_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   ଗଣନା_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   ଗଣନା_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   ଗଣନା_gen_events, ଗଣନା_gen_pool());
      }

      final_cleanup();

      ଫେରନ୍ତୁ vvp_return_ମୂଲ୍ୟ;
}
