स्थिर अक्षर COPYRIGHT[] =
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

#अगर defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#पूर्ण_अगर // defined(HAVE_SYS_RESOURCE_H)

#अगर defined(__MINGW32__)
# include  <windows.h>
#पूर्ण_अगर

#समावेश "libvvp.h"

using namespace std;

बख_प्रवाह debug_file;

#अगर defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णांक getopt(पूर्णांक argc, अक्षर*argv[], स्थिर अक्षर*fmt);
बाह्य "C" पूर्णांक optind;
बाह्य "C" स्थिर अक्षर*optarg;
#पूर्ण_अगर

bool verbose_flag = false;
अटल पूर्णांक vvp_return_मूल्य = 0;
अटल पूर्णांक vvp_used = 0;

व्योम vvp_set_stop_is_finish(bool flag, पूर्णांक exit_code)
{
      बाह्य bool stop_is_finish;
      बाह्य पूर्णांक  stop_is_finish_exit_code;

      stop_is_finish = flag;
      stop_is_finish_exit_code = exit_code;
}

व्योम vvp_set_quiet_flag(bool flag)
{
      vpip_mcd0_disable = flag;
}

व्योम vvp_set_verbose_flag(bool flag)
{
      verbose_flag = flag;
}

व्योम vpip_set_return_मूल्य(पूर्णांक value)
{
      vvp_return_मूल्य = value;
}

अटल अक्षर log_बफर[4096];

#अगर defined(HAVE_SYS_RESOURCE_H)
अटल व्योम my_getrusage(काष्ठा rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     अगर defined(LINUX)
      {
	    खाता *statm;
	    अचिन्हित siz, rss, shd;
	    दीर्घ page_आकार = sysconf(_SC_PAGESIZE);
	    अगर (page_आकार==-1) page_आकार=0;
	    statm = ख_खोलो("/proc/self/statm", "r");
	    अगर (!statm) {
		  लिखो_त्रुटि("/proc/self/statm");
		  वापस;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    अगर (3 == ख_पूछो(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = page_आकार * siz;
		  a->ru_idrss  = page_आकार * rss;
		  a->ru_ixrss  = page_आकार * shd;
	    }
	    ख_बंद(statm);
      }
#     endif
}

अटल व्योम print_rusage(काष्ठा rusage *a, काष्ठा rusage *b)
{
      द्विगुन delta = a->ru_utime.tv_sec
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
काष्ठा rusage { पूर्णांक x; };
अंतरभूत अटल व्योम my_getrusage(काष्ठा rusage *) { }
अंतरभूत अटल व्योम print_rusage(काष्ठा rusage *, काष्ठा rusage *){};

#पूर्ण_अगर // ! defined(HAVE_SYS_RESOURCE_H)

अटल bool have_ivl_version = false;
/*
 * Verify that the input file has a compatible version.
 */
व्योम verify_version(अक्षर*ivl_ver, अक्षर*commit)
{
      have_ivl_version = true;

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    अगर (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      मिटाओ[] commit;

      पूर्णांक file_major, file_minor, file_minor2;
      अक्षर file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      पूर्णांक rc = माला_पूछो(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      अगर (rc == 2) {
	    file_extra[0] = 0;
	    rc = माला_पूछो(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    निश्चित((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      मिटाओ[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      अगर (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      अगर (VERSION_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, VERSION);
	    निकास(1);
      }

      अगर (VERSION_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, VERSION);
      }
}

पूर्णांक vpip_delay_selection = _vpiDelaySelTypical;
व्योम set_delay_selection(स्थिर अक्षर* sel)
{
      अगर (म_भेद("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } अन्यथा अगर (म_भेद("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } अन्यथा अगर (म_भेद("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMaximum;
      } अन्यथा {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    निकास(1);
      }
      मिटाओ[] sel;
}

अटल व्योम final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#अगर_घोषित CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      क्रम (vector<स्थिर अक्षर*>::iterator cur = file_names.begin();
           cur != file_names.end() ; ++ cur ) {
	    मिटाओ[] *cur;
      }
	/* Clear the static result buffer. */
      (व्योम)need_परिणाम_buf(0, RBUF_DEL);
      codespace_delete();
      मूल_table_delete();
      def_table_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ढेर_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#पूर्ण_अगर
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      load_module_delete();

#अगर_घोषित CHECK_WITH_VALGRIND
      simulator_cb_delete();
#पूर्ण_अगर
}

बाह्य व्योम vpip_mcd_init(खाता *log);
बाह्य व्योम vvp_vpi_init(व्योम);

अटल व्योम report_used(व्योम)
{
  ख_लिखो(मानक_त्रुटि,
	  "This VVP simulation has already run and can not be reused\n");
}

व्योम vvp_init(स्थिर अक्षर *logfile_name, पूर्णांक argc, अक्षर*argv[])
{
      काष्ठा rusage cycle;
      खाता *logfile = 0x0;
      बाह्य व्योम vpi_set_vlog_info(पूर्णांक, अक्षर**);

      अगर (vvp_used++) {
          report_used();
          वापस;
      }

      अगर( ::दो_पर्या("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          ख_लिखो( मानक_त्रुटि, "Waiting for debugger...\n");
          bool debugger_release = false;
          जबतक( !debugger_release )  {
#अगर defined(__MINGW32__)
              Sleep(1000);
#अन्यथा
              sleep(1);
#पूर्ण_अगर
        }
      }

      vpip_add_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      अगर (स्थिर अक्षर*path = दो_पर्या("VVP_DEBUG")) {
	    debug_file.खोलो(path, पनप::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      अगर (logfile_name) {
	    अगर (!म_भेद(logfile_name, "-"))
		  logfile = मानक_त्रुटि;
	    अन्यथा {
		  logfile = ख_खोलो(logfile_name, "w");
		  अगर (!logfile) {
		        लिखो_त्रुटि(logfile_name);
		        निकास(1);
		  }
		  रखो_भबफ(logfile, log_बफर, _IOLBF, माप(log_बफर));
	    }
      }

      vpip_mcd_init(logfile);

      अगर (verbose_flag) {
	    my_getrusage(&cycle);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_set_vlog_info(argc, argv);

      compile_init();
}

पूर्णांक vvp_run(स्थिर अक्षर *design_path)
{
      काष्ठा rusage cycles[3];
      पूर्णांक ret_cd;

      (व्योम) COPYRIGHT;
      अगर (vvp_used++ != 1) {
          अगर (vvp_used == 1)
              ख_लिखो(मानक_त्रुटि, "vvp_init() has not been called\n");
          अन्यथा
              report_used();
          वापस 1;
      }
      ++vvp_used;

      ret_cd = compile_design(design_path);
      destroy_lexor();
      print_vpi_call_errors();
      अगर (ret_cd) वापस ret_cd;

      अगर (!have_ivl_version) {
	    अगर (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      अगर (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_path, compile_errors);
	    final_cleanup();
	    वापस compile_errors;
      }

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   गिनती_functors, vvp_net_fun_t::heap_योग());
	    vpi_mcd_printf(1, "           %8lu logic\n",  गिनती_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  गिनती_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",गिनती_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", गिनती_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   गिनती_filters, vvp_net_fil_t::heap_योग());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   गिनती_opcodes, आकार_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     गिनती_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   गिनती_vvp_nets, आकार_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   गिनती_net_arrays, गिनती_net_array_शब्दगण);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   गिनती_var_arrays+गिनती_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   गिनती_var_arrays, गिनती_var_array_शब्दगण);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   गिनती_real_arrays, गिनती_real_array_शब्दगण);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   गिनती_vpi_scopes);
      }

      अगर (verbose_flag) {
	    my_getrusage(cycles+1);
	    print_rusage(cycles+1, cycles+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      schedule_simulate();

      अगर (verbose_flag) {
	    my_getrusage(cycles+2);
	    print_rusage(cycles+2, cycles+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   गिनती_time_events, गिनती_time_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    गिनती_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    गिनती_assign_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   गिनती_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   गिनती_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   गिनती_assign_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   गिनती_assign_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   गिनती_assign_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   गिनती_gen_events, गिनती_gen_pool());
      }

      final_cleanup();

      वापस vvp_return_मूल्य;
}
