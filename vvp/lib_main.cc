स्थिरांक अक्षर COPYRIGHT[] =
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

#जर defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

#जर defined(__MINGW32__)
# include  <windows.h>
#endif

#अन्तर्भाव "libvvp.h"

using नामाकाश std;

ofstream debug_file;

#जर defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णांक getopt(पूर्णांक argc, अक्षर*argv[], स्थिरांक अक्षर*fmt);
बाह्य "C" पूर्णांक optind;
बाह्य "C" स्थिरांक अक्षर*optarg;
#endif

तार्किक verbose_flag = असत्य;
स्थिर पूर्णांक vvp_return_मूल्य = 0;
स्थिर पूर्णांक vvp_used = 0;

शून्य vvp_set_stop_is_finish(तार्किक flag, पूर्णांक exit_code)
{
      बाह्य तार्किक stop_is_finish;
      बाह्य पूर्णांक  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

शून्य vvp_set_quiet_flag(तार्किक flag)
{
      vpip_mcd0_disable = flag;
}

शून्य vvp_set_verbose_flag(तार्किक flag)
{
      verbose_flag = flag;
}

शून्य vpip_set_return_मूल्य(पूर्णांक value)
{
      vvp_return_मूल्य = value;
}

स्थिर अक्षर log_buffer[4096];

#जर defined(HAVE_SYS_RESOURCE_H)
स्थिर शून्य my_getrusage(संरचना rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     जर defined(LINUX)
      {
	    खाता *statm;
	    अचिन्हित siz, rss, shd;
	    दीर्घ page_size = sysconf(_SC_PAGESIZE);
	    जर (page_size==-1) page_size=0;
	    statm = ख_खोलो("/proc/self/statm", "r");
	    जर (!statm) {
		  लिखो_त्रुटि("/proc/self/statm");
		  परत;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    जर (3 == ख_पूछो(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_size * siz;
		  a->ru_idrss  = page_size * rss;
		  a->ru_ixrss  = page_size * shd;
	    }
	    ख_बंद(statm);
      }
#     endif
}

स्थिर शून्य print_rusage(संरचना rusage *a, संरचना rusage *b)
{
      द्विगुण delta = a->ru_utime.tv_sec
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

#अन्यथा // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
संरचना rusage { पूर्णांक x; };
ओळीत स्थिर शून्य my_getrusage(संरचना rusage *) { }
ओळीत स्थिर शून्य print_rusage(संरचना rusage *, संरचना rusage *){};

#endif // ! defined(HAVE_SYS_RESOURCE_H)

स्थिर तार्किक have_ivl_version = असत्य;
/*
 * Verify that the input file has a compatible version.
 */
शून्य verify_version(अक्षर*ivl_ver, अक्षर*commit)
{
      have_ivl_version = सत्य;

      जर (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    जर (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      delete[] commit;

      पूर्णांक file_major, file_minor, file_minor2;
      अक्षर file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      पूर्णांक rc = माला_पूछो(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      जर (rc == 2) {
	    file_extra[0] = 0;
	    rc = माला_पूछो(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    प्रतिज्ञा((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      delete[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      जर (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      जर (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    निकास(1);
      }

      जर (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

पूर्णांक vpip_delay_selection = _vpiDelaySelTypical;
शून्य set_delay_selection(स्थिरांक अक्षर* sel)
{
      जर (म_भेद("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } अन्यथा जर (म_भेद("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } अन्यथा जर (म_भेद("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } अन्यथा {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    निकास(1);
      }
      delete[] sel;
}

स्थिर शून्य final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ifdef CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      साठी (vector<स्थिरांक अक्षर*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    delete[] *cur;
      }
	/* Clear the static result buffer. */
      (शून्य)need_result_buf(0, RBUF_DEL);
      codespace_delete();
      मूळ_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_चळत_delete();
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

बाह्य शून्य vpip_mcd_init(खाता *log);
बाह्य शून्य vvp_vpi_init(शून्य);

स्थिर शून्य report_used(शून्य)
{
  ख_लिखो(मानक_त्रुटि,
	  "This VVP simulation has already run and can not be reused\n");
}

शून्य vvp_init(स्थिरांक अक्षर *logfile_name, पूर्णांक argc, अक्षर*argv[])
{
      संरचना rusage cycle;
      खाता *logfile = 0x0;
      बाह्य शून्य vpi_set_vlog_info(पूर्णांक, अक्षर**);

      जर (vvp_used++) {
          report_used();
          परत;
      }

      जर( ::दो_पर्या("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          ख_लिखो( मानक_त्रुटि, "Waiting for debugger...\n");
          तार्किक debugger_release = असत्य;
          पर्यंत( !debugger_release )  {
#जर defined(__MINGW32__)
              Sleep(1000);
#अन्यथा
              सुषुप्ति(1);
#endif
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      जर (स्थिरांक अक्षर*path = दो_पर्या("VVP_DEBUG")) {
	    debug_file.खोलो(path, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      जर (logfile_name) {
	    जर (!म_भेद(logfile_name, "-"))
		  logfile = मानक_त्रुटि;
	    अन्यथा {
		  logfile = ख_खोलो(logfile_name, "w");
		  जर (!logfile) {
		        लिखो_त्रुटि(logfile_name);
		        निकास(1);
		  }
		  रखो_भबफ(logfile, log_buffer, _IOLBF, आकार(log_buffer));
	    }
      }

      vpip_mcd_init(logfile);

      जर (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

पूर्णांक vvp_run(स्थिरांक अक्षर *design_path)
{
      संरचना rusage cycles[3];
      पूर्णांक ret_cd;

      (शून्य) COPYRIGHT;
      जर (vvp_used++ != 1) {
          जर (vvp_used == 1)
              ख_लिखो(मानक_त्रुटि, "vvp_init() has not been called\n");
          अन्यथा
              report_used();
          परत 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      जर (ret_cd) परत ret_cd;

      जर (!have_ivl_version) {
	    जर (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      जर (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      जर (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    परत compile_errors;
      }

      जर (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   मोजणी_functors, vvp_net_fun_t::heap_total());
	    vpi_mcd_printf(1, "           %8lu logic\n",  मोजणी_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  मोजणी_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",मोजणी_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", मोजणी_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   मोजणी_filters, vvp_net_fil_t::heap_total());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   मोजणी_opcodes, size_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     मोजणी_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   मोजणी_vvp_nets, size_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   मोजणी_net_arrays, मोजणी_net_array_शब्द);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   मोजणी_var_arrays+मोजणी_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   मोजणी_var_arrays, मोजणी_var_array_शब्द);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   मोजणी_real_arrays, मोजणी_real_array_शब्द);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   मोजणी_vpi_scopes);
      }

      जर (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      जर (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   मोजणी_time_events, मोजणी_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    मोजणी_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    मोजणी_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   मोजणी_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   मोजणी_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   मोजणी_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   मोजणी_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   मोजणी_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   मोजणी_gen_events, मोजणी_gen_pool());
      }

      final_cleanup();

      परत vvp_return_मूल्य;
}
