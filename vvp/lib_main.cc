ध्रुव वर्ण COPYRIGHT[] =
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

#यदि defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#यदि_अन्तः // defined(HAVE_SYS_RESOURCE_H)

#यदि defined(__MINGW32__)
# include  <windows.h>
#यदि_अन्तः

#अन्तर्भाव "libvvp.h"

उपयुज्य नामाकाश std;

ofstream debug_file;

#यदि defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णाङ्क getopt(पूर्णाङ्क argc, वर्ण*argv[], ध्रुव वर्ण*fmt);
बाह्य "C" पूर्णाङ्क optind;
बाह्य "C" ध्रुव वर्ण*optarg;
#यदि_अन्तः

तार्किक verbose_flag = असत्य;
स्थिर पूर्णाङ्क vvp_return_मानम् = 0;
स्थिर पूर्णाङ्क vvp_प्रयुक्तम् = 0;

शून्य vvp_समुच्चयः_stop_is_finish(तार्किक flag, पूर्णाङ्क exit_संहिता)
{
      बाह्य तार्किक stop_is_finish;
      बाह्य पूर्णाङ्क  stop_is_finish_exit_संहिता;

      stop_is_finish = flag;
      stop_is_finish_exit_संहिता = exit_संहिता;
}

शून्य vvp_समुच्चयः_quiet_flag(तार्किक flag)
{
      vpip_mcd0_disable = flag;
}

शून्य vvp_समुच्चयः_verbose_flag(तार्किक flag)
{
      verbose_flag = flag;
}

शून्य vpip_समुच्चयः_return_मानम्(पूर्णाङ्क value)
{
      vvp_return_मानम् = value;
}

स्थिर वर्ण log_बफर[4096];

#यदि defined(HAVE_SYS_RESOURCE_H)
स्थिर शून्य my_getrusage(संरचना rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     यदि defined(LINUX)
      {
	    FILE *statm;
	    अचिह्नित siz, rss, shd;
	    दीर्घ page_आकार = sysconf(_SC_PAGESIZE);
	    यदि (page_आकार==-1) page_आकार=0;
	    statm = fopen("/proc/self/statm", "r");
	    यदि (!statm) {
		  perror("/proc/self/statm");
		  प्रत्यागम;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    यदि (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_आकार * siz;
		  a->ru_idrss  = page_आकार * rss;
		  a->ru_ixrss  = page_आकार * shd;
	    }
	    fclose(statm);
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
संरचना rusage { पूर्णाङ्क x; };
अन्तःस्थ स्थिर शून्य my_getrusage(संरचना rusage *) { }
अन्तःस्थ स्थिर शून्य print_rusage(संरचना rusage *, संरचना rusage *){};

#यदि_अन्तः // ! defined(HAVE_SYS_RESOURCE_H)

स्थिर तार्किक have_ivl_संस्करणम् = असत्य;
/*
 * Verify that the input file has a compatible version.
 */
शून्य verify_संस्करणम्(वर्ण*ivl_ver, वर्ण*commit)
{
      have_ivl_संस्करणम् = सत्य;

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    यदि (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      विलोपय[] commit;

      पूर्णाङ्क file_major, file_minor, file_minor2;
      वर्ण file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      पूर्णाङ्क rc = वाक्यात्_पठ(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      यदि (rc == 2) {
	    file_extra[0] = 0;
	    rc = वाक्यात्_पठ(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    प्रतिज्ञा((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      विलोपय[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      यदि (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      यदि (संस्करणम्_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, संस्करणम्);
	    निर्गच्छ(1);
      }

      यदि (संस्करणम्_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, संस्करणम्);
      }
}

पूर्णाङ्क vpip_delay_selection = _vpiDelaySelTypical;
शून्य समुच्चयः_delay_selection(ध्रुव वर्ण* sel)
{
      यदि (वाक्यं_तुलय("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } अन्यथा यदि (वाक्यं_तुलय("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } अन्यथा यदि (वाक्यं_तुलय("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = vpi_Delay_Sel_अधिकतमम्;
      } अन्यथा {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    निर्गच्छ(1);
      }
      विलोपय[] sel;
}

स्थिर शून्य final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      प्रति (vector<ध्रुव वर्ण*>::iterator cur = file_नामानि.begin();
           cur != file_नामानि.अन्तः() ; ++ cur ) {
	    विलोपय[] *cur;
      }
	/* Clear the static result buffer. */
      (शून्य)need_परिणाम_buf(0, RBUF_DEL);
      codespace_delete();
      मूलम्_सारणी_delete();
      def_सारणी_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_राशिः_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#यदि_अन्तः
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      आरोपय_module_delete();

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      simulator_cb_delete();
#यदि_अन्तः
}

बाह्य शून्य vpip_mcd_init(FILE *लघुगणकः);
बाह्य शून्य vvp_vpi_init(शून्य);

स्थिर शून्य report_प्रयुक्तम्(शून्य)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

शून्य vvp_init(ध्रुव वर्ण *logfile_name, पूर्णाङ्क argc, वर्ण*argv[])
{
      संरचना rusage आवृत्तिः;
      FILE *logfile = 0x0;
      बाह्य शून्य vpi_समुच्चयः_vlog_info(पूर्णाङ्क, वर्ण**);

      यदि (vvp_प्रयुक्तम्++) {
          report_प्रयुक्तम्();
          प्रत्यागम;
      }

      यदि( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          तार्किक debugger_release = असत्य;
          यावत्( !debugger_release )  {
#यदि defined(__MINGW32__)
              Sleep(1000);
#अन्यथा
              सुषुप्ति(1);
#यदि_अन्तः
        }
      }

      vpip_सङ्कलय_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      यदि (ध्रुव वर्ण*मार्गः = getenv("VVP_DEBUG")) {
	    debug_file.उद्घाटय(मार्गः, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      यदि (logfile_name) {
	    यदि (!वाक्यं_तुलय(logfile_name, "-"))
		  logfile = stderr;
	    अन्यथा {
		  logfile = fopen(logfile_name, "w");
		  यदि (!logfile) {
		        perror(logfile_name);
		        निर्गच्छ(1);
		  }
		  setvbuf(logfile, log_बफर, _IOLBF, परिमाण(log_बफर));
	    }
      }

      vpip_mcd_init(logfile);

      यदि (verbose_flag) {
	    my_getrusage(&आवृत्तिः);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_समुच्चयः_vlog_info(argc, argv);

      compile_init();
}

पूर्णाङ्क vvp_सततम्(ध्रुव वर्ण *design_मार्गः)
{
      संरचना rusage आवृत्तयः[3];
      पूर्णाङ्क ret_cd;

      (शून्य) COPYRIGHT;
      यदि (vvp_प्रयुक्तम्++ != 1) {
          यदि (vvp_प्रयुक्तम् == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          अन्यथा
              report_प्रयुक्तम्();
          प्रत्यागम 1;
      }
      ++vvp_प्रयुक्तम्;

      ret_cd = compile_design(design_मार्गः);
      destroy_lexor();
      print_vpi_call_errors();
      यदि (ret_cd) प्रत्यागम ret_cd;

      यदि (!have_ivl_संस्करणम्) {
	    यदि (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      यदि (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_मार्गः, compile_errors);
	    final_cleanup();
	    प्रत्यागम compile_errors;
      }

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   गणनम्_functors, vvp_net_fun_t::heap_योग());
	    vpi_mcd_printf(1, "           %8lu logic\n",  गणनम्_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  गणनम्_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",गणनम्_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", गणनम्_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   गणनम्_filters, vvp_net_fil_t::heap_योग());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   गणनम्_opcodes, आकार_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     गणनम्_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   गणनम्_vvp_nets, आकार_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   गणनम्_net_arrays, गणनम्_net_array_शब्दाः);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   गणनम्_var_arrays+गणनम्_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   गणनम्_var_arrays, गणनम्_var_array_शब्दाः);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   गणनम्_real_arrays, गणनम्_real_array_शब्दाः);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   गणनम्_vpi_scopes);
      }

      यदि (verbose_flag) {
	    my_getrusage(आवृत्तयः+1);
	    print_rusage(आवृत्तयः+1, आवृत्तयः+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      कालसारणी_simulate();

      यदि (verbose_flag) {
	    my_getrusage(आवृत्तयः+2);
	    print_rusage(आवृत्तयः+2, आवृत्तयः+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   गणनम्_time_events, गणनम्_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    गणनम्_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    गणनम्_नियोजय_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   गणनम्_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   गणनम्_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   गणनम्_नियोजय_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   गणनम्_नियोजय_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   गणनम्_नियोजय_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   गणनम्_gen_events, गणनम्_gen_pool());
      }

      final_cleanup();

      प्रत्यागम vvp_return_मानम्;
}
