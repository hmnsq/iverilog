ਅਚਲ ਅੱਖਰ COPYRIGHT[] =
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

#ਜੇ defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#ਜੇ_ਅੰਤ // defined(HAVE_SYS_RESOURCE_H)

#ਜੇ defined(__MINGW32__)
# include  <windows.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "libvvp.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ofstream debug_file;

#ਜੇ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ਬਾਹਰੀ "C" ਪੂਰਨ getopt(ਪੂਰਨ argc, ਅੱਖਰ*argv[], ਅਚਲ ਅੱਖਰ*fmt);
ਬਾਹਰੀ "C" ਪੂਰਨ optind;
ਬਾਹਰੀ "C" ਅਚਲ ਅੱਖਰ*optarg;
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ verbose_flag = ਝੂਠ;
ਸਥਿਰ ਪੂਰਨ vvp_return_ਮੁੱਲ = 0;
ਸਥਿਰ ਪੂਰਨ vvp_ਵਰਤਿਆ = 0;

ਖਾਲੀ vvp_ਸੈੱਟ_stop_is_ਅੰਤ(ਤਾਰਕਿਕ flag, ਪੂਰਨ exit_ਕੋਡ)
{
      ਬਾਹਰੀ ਤਾਰਕਿਕ stop_is_ਅੰਤ;
      ਬਾਹਰੀ ਪੂਰਨ  stop_is_ਅੰਤ_exit_ਕੋਡ;

      stop_is_ਅੰਤ = flag;
      stop_is_ਅੰਤ_exit_ਕੋਡ = exit_ਕੋਡ;
}

ਖਾਲੀ vvp_ਸੈੱਟ_quiet_flag(ਤਾਰਕਿਕ flag)
{
      vpip_mcd0_disable = flag;
}

ਖਾਲੀ vvp_ਸੈੱਟ_verbose_flag(ਤਾਰਕਿਕ flag)
{
      verbose_flag = flag;
}

ਖਾਲੀ vpip_ਸੈੱਟ_return_ਮੁੱਲ(ਪੂਰਨ value)
{
      vvp_return_ਮੁੱਲ = value;
}

ਸਥਿਰ ਅੱਖਰ log_ਬਫਰ[4096];

#ਜੇ defined(HAVE_SYS_RESOURCE_H)
ਸਥਿਰ ਖਾਲੀ my_getrusage(ਬਣਤਰ rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     ਜੇ defined(LINUX)
      {
	    FILE *statm;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ siz, rss, shd;
	    ਲੰਮਾ ਪੰਨਾ_ਮਾਪ = sysconf(_SC_PAGESIZE);
	    ਜੇ (ਪੰਨਾ_ਮਾਪ==-1) ਪੰਨਾ_ਮਾਪ=0;
	    statm = fopen("/proc/self/statm", "r");
	    ਜੇ (!statm) {
		  perror("/proc/self/statm");
		  ਵਾਪਸ;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    ਜੇ (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = ਪੰਨਾ_ਮਾਪ * siz;
		  a->ru_idrss  = ਪੰਨਾ_ਮਾਪ * rss;
		  a->ru_ixrss  = ਪੰਨਾ_ਮਾਪ * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ਸਥਿਰ ਖਾਲੀ print_rusage(ਬਣਤਰ rusage *a, ਬਣਤਰ rusage *b)
{
      ਦੁੱਗਣਾ delta = a->ru_utime.tv_sec
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

#ਨਹੀਂਤਾਂ // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ਬਣਤਰ rusage { ਪੂਰਨ x; };
ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ my_getrusage(ਬਣਤਰ rusage *) { }
ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ print_rusage(ਬਣਤਰ rusage *, ਬਣਤਰ rusage *){};

#ਜੇ_ਅੰਤ // ! defined(HAVE_SYS_RESOURCE_H)

ਸਥਿਰ ਤਾਰਕਿਕ have_ivl_ਸੰਸਕਰਣ = ਝੂਠ;
/*
 * Verify that the input file has a compatible version.
 */
ਖਾਲੀ verify_ਸੰਸਕਰਣ(ਅੱਖਰ*ivl_ver, ਅੱਖਰ*commit)
{
      have_ivl_ਸੰਸਕਰਣ = ਸੱਚ;

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    ਜੇ (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      ਮਿਟਾਓ[] commit;

      ਪੂਰਨ file_major, file_minor, file_minor2;
      ਅੱਖਰ file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      ਪੂਰਨ rc = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      ਜੇ (rc == 2) {
	    file_extra[0] = 0;
	    rc = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    ਪ੍ਰਤਿਜ੍ਞਾ((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      ਮਿਟਾਓ[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      ਜੇ (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      ਜੇ (ਸੰਸਕਰਣ_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, ਸੰਸਕਰਣ);
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
      }

      ਜੇ (ਸੰਸਕਰਣ_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, ਸੰਸਕਰਣ);
      }
}

ਪੂਰਨ vpip_delay_selection = _vpiDelaySelTypical;
ਖਾਲੀ ਸੈੱਟ_delay_selection(ਅਚਲ ਅੱਖਰ* sel)
{
      ਜੇ (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = vpi_Delay_Sel_ਵੱਧ_ਤੋਂ_ਵੱਧ;
      } ਨਹੀਂਤਾਂ {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਮਿਟਾਓ[] sel;
}

ਸਥਿਰ ਖਾਲੀ final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      ਲਈ (vector<ਅਚਲ ਅੱਖਰ*>::iterator cur = file_ਨਾਂ.begin();
           cur != file_ਨਾਂ.ਸਿਰਾ() ; ++ cur ) {
	    ਮਿਟਾਓ[] *cur;
      }
	/* Clear the static result buffer. */
      (ਖਾਲੀ)need_ਨਤੀਜਾ_buf(0, RBUF_DEL);
      codespace_delete();
      ਜੜ੍ਹ_ਸਾਰਣੀ_delete();
      def_ਸਾਰਣੀ_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ਢੇਰ_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#ਜੇ_ਅੰਤ
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      ਲੱਦੋ_module_delete();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      simulator_cb_delete();
#ਜੇ_ਅੰਤ
}

ਬਾਹਰੀ ਖਾਲੀ vpip_mcd_init(FILE *ਲਘੂਗਣਕ);
ਬਾਹਰੀ ਖਾਲੀ vvp_vpi_init(ਖਾਲੀ);

ਸਥਿਰ ਖਾਲੀ report_ਵਰਤਿਆ(ਖਾਲੀ)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

ਖਾਲੀ vvp_init(ਅਚਲ ਅੱਖਰ *logfile_name, ਪੂਰਨ argc, ਅੱਖਰ*argv[])
{
      ਬਣਤਰ rusage ਚੱਕਰ;
      FILE *logfile = 0x0;
      ਬਾਹਰੀ ਖਾਲੀ vpi_ਸੈੱਟ_vlog_info(ਪੂਰਨ, ਅੱਖਰ**);

      ਜੇ (vvp_ਵਰਤਿਆ++) {
          report_ਵਰਤਿਆ();
          ਵਾਪਸ;
      }

      ਜੇ( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          ਤਾਰਕਿਕ debugger_release = ਝੂਠ;
          ਜਦੋਂ( !debugger_release )  {
#ਜੇ defined(__MINGW32__)
              Sleep(1000);
#ਨਹੀਂਤਾਂ
              ਸੁਸ਼ੁਪ੍ਤਿ(1);
#ਜੇ_ਅੰਤ
        }
      }

      vpip_ਜੋੜੋ_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      ਜੇ (ਅਚਲ ਅੱਖਰ*ਰਸਤਾ = getenv("VVP_DEBUG")) {
	    debug_file.open(ਰਸਤਾ, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      ਜੇ (logfile_name) {
	    ਜੇ (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    ਨਹੀਂਤਾਂ {
		  logfile = fopen(logfile_name, "w");
		  ਜੇ (!logfile) {
		        perror(logfile_name);
		        ਬਾਹਰ_ਨਿਕਲੋ(1);
		  }
		  setvbuf(logfile, log_ਬਫਰ, _IOLBF, ਆਕਾਰ(log_ਬਫਰ));
	    }
      }

      vpip_mcd_init(logfile);

      ਜੇ (verbose_flag) {
	    my_getrusage(&ਚੱਕਰ);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_ਸੈੱਟ_vlog_info(argc, argv);

      compile_init();
}

ਪੂਰਨ vvp_ਲੜੀਵਾਰ(ਅਚਲ ਅੱਖਰ *design_ਰਸਤਾ)
{
      ਬਣਤਰ rusage ਚੱਕਰਾਂ[3];
      ਪੂਰਨ ret_cd;

      (ਖਾਲੀ) COPYRIGHT;
      ਜੇ (vvp_ਵਰਤਿਆ++ != 1) {
          ਜੇ (vvp_ਵਰਤਿਆ == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          ਨਹੀਂਤਾਂ
              report_ਵਰਤਿਆ();
          ਵਾਪਸ 1;
      }
      ++vvp_ਵਰਤਿਆ;

      ret_cd = compile_design(design_ਰਸਤਾ);
      destroy_lexor();
      print_vpi_call_errors();
      ਜੇ (ret_cd) ਵਾਪਸ ret_cd;

      ਜੇ (!have_ivl_ਸੰਸਕਰਣ) {
	    ਜੇ (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      ਜੇ (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_ਰਸਤਾ, compile_errors);
	    final_cleanup();
	    ਵਾਪਸ compile_errors;
      }

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   ਗਿਣਤੀ_functors, vvp_net_fun_t::ਢੇਰ_ਕੁੱਲ());
	    vpi_mcd_printf(1, "           %8lu logic\n",  ਗਿਣਤੀ_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  ਗਿਣਤੀ_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",ਗਿਣਤੀ_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", ਗਿਣਤੀ_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   ਗਿਣਤੀ_filters, vvp_net_fil_t::ਢੇਰ_ਕੁੱਲ());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   ਗਿਣਤੀ_opcodes, ਮਾਪ_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     ਗਿਣਤੀ_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   ਗਿਣਤੀ_vvp_nets, ਮਾਪ_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   ਗਿਣਤੀ_net_arrays, ਗਿਣਤੀ_net_array_ਸ਼ਬਦ);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   ਗਿਣਤੀ_var_arrays+ਗਿਣਤੀ_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   ਗਿਣਤੀ_var_arrays, ਗਿਣਤੀ_var_array_ਸ਼ਬਦ);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   ਗਿਣਤੀ_real_arrays, ਗਿਣਤੀ_real_array_ਸ਼ਬਦ);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   ਗਿਣਤੀ_vpi_scopes);
      }

      ਜੇ (verbose_flag) {
	    my_getrusage(ਚੱਕਰਾਂ+1);
	    print_rusage(ਚੱਕਰਾਂ+1, ਚੱਕਰਾਂ+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      ਸਮਾਂ_ਸਾਰਣੀ_simulate();

      ਜੇ (verbose_flag) {
	    my_getrusage(ਚੱਕਰਾਂ+2);
	    print_rusage(ਚੱਕਰਾਂ+2, ਚੱਕਰਾਂ+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   ਗਿਣਤੀ_ਸਮਾਂ_events, ਗਿਣਤੀ_ਸਮਾਂ_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    ਗਿਣਤੀ_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    ਗਿਣਤੀ_ਸੌਂਪੋ_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   ਗਿਣਤੀ_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   ਗਿਣਤੀ_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   ਗਿਣਤੀ_ਸੌਂਪੋ_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   ਗਿਣਤੀ_ਸੌਂਪੋ_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   ਗਿਣਤੀ_ਸੌਂਪੋ_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   ਗਿਣਤੀ_gen_events, ਗਿਣਤੀ_gen_pool());
      }

      final_cleanup();

      ਵਾਪਸ vvp_return_ਮੁੱਲ;
}
