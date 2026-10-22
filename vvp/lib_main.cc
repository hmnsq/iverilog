ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ COPYRIGHT[] =
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

#ಆದರೆ defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#ಆದರೆ_ಅಂತ್ಯ // defined(HAVE_SYS_RESOURCE_H)

#ಆದರೆ defined(__MINGW32__)
# include  <windows.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "libvvp.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ofstream debug_file;

#ಆದರೆ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಂಕ getopt(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ*argv[], ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*fmt);
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಂಕ optind;
ಬಾಹ್ಯ "C" ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*optarg;
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ verbose_flag = ಸುಳ್ಳು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vvp_return_ಮೌಲ್ಯ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vvp_ಬಳಸಿದ = 0;

ಶೂನ್ಯ vvp_ಗಣ_stop_is_ಮುಕ್ತಾಯ(ತಾರ್ಕಿಕ flag, ಪೂರ್ಣಾಂಕ exit_ಸಂಕೇತ)
{
      ಬಾಹ್ಯ ತಾರ್ಕಿಕ stop_is_ಮುಕ್ತಾಯ;
      ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ  stop_is_ಮುಕ್ತಾಯ_exit_ಸಂಕೇತ;

      stop_is_ಮುಕ್ತಾಯ = flag;
      stop_is_ಮುಕ್ತಾಯ_exit_ಸಂಕೇತ = exit_ಸಂಕೇತ;
}

ಶೂನ್ಯ vvp_ಗಣ_quiet_flag(ತಾರ್ಕಿಕ flag)
{
      vpip_mcd0_disable = flag;
}

ಶೂನ್ಯ vvp_ಗಣ_verbose_flag(ತಾರ್ಕಿಕ flag)
{
      verbose_flag = flag;
}

ಶೂನ್ಯ vpip_ಗಣ_return_ಮೌಲ್ಯ(ಪೂರ್ಣಾಂಕ value)
{
      vvp_return_ಮೌಲ್ಯ = value;
}

ಸ್ಥಿರ ಅಕ್ಷರ log_ಬಫರ್[4096];

#ಆದರೆ defined(HAVE_SYS_RESOURCE_H)
ಸ್ಥಿರ ಶೂನ್ಯ my_getrusage(ರಚನೆ rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     ಆದರೆ defined(LINUX)
      {
	    FILE *statm;
	    ಚಿಹ್ನೆರಹಿತ siz, rss, shd;
	    ದೀರ್ಘ ಪುಟ_ಗಾತ್ರ = sysconf(_SC_PAGESIZE);
	    ಆದರೆ (ಪುಟ_ಗಾತ್ರ==-1) ಪುಟ_ಗಾತ್ರ=0;
	    statm = fopen("/proc/self/statm", "r");
	    ಆದರೆ (!statm) {
		  perror("/proc/self/statm");
		  ಹಿಂತಿರುಗಿ;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    ಆದರೆ (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = ಪುಟ_ಗಾತ್ರ * siz;
		  a->ru_idrss  = ಪುಟ_ಗಾತ್ರ * rss;
		  a->ru_ixrss  = ಪುಟ_ಗಾತ್ರ * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ಸ್ಥಿರ ಶೂನ್ಯ print_rusage(ರಚನೆ rusage *a, ರಚನೆ rusage *b)
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

#ಇಲ್ಲದಿದ್ದರೆ // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ರಚನೆ rusage { ಪೂರ್ಣಾಂಕ x; };
ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ my_getrusage(ರಚನೆ rusage *) { }
ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ print_rusage(ರಚನೆ rusage *, ರಚನೆ rusage *){};

#ಆದರೆ_ಅಂತ್ಯ // ! defined(HAVE_SYS_RESOURCE_H)

ಸ್ಥಿರ ತಾರ್ಕಿಕ have_ivl_ಆವೃತ್ತಿ = ಸುಳ್ಳು;
/*
 * Verify that the input file has a compatible version.
 */
ಶೂನ್ಯ verify_ಆವೃತ್ತಿ(ಅಕ್ಷರ*ivl_ver, ಅಕ್ಷರ*commit)
{
      have_ivl_ಆವೃತ್ತಿ = ನಿಜ;

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    ಆದರೆ (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      ಅಳಿಸು[] commit;

      ಪೂರ್ಣಾಂಕ file_major, file_minor, file_minor2;
      ಅಕ್ಷರ file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      ಪೂರ್ಣಾಂಕ rc = ಅಕ್ಷರಸರಣಿ_ಓದು(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      ಆದರೆ (rc == 2) {
	    file_extra[0] = 0;
	    rc = ಅಕ್ಷರಸರಣಿ_ಓದು(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    ಪ್ರತಿಜ್ಞಾ((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      ಅಳಿಸು[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      ಆದರೆ (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      ಆದರೆ (ಆವೃತ್ತಿ_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, ಆವೃತ್ತಿ);
	    ನಿರ್ಗಮಿಸು(1);
      }

      ಆದರೆ (ಆವೃತ್ತಿ_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, ಆವೃತ್ತಿ);
      }
}

ಪೂರ್ಣಾಂಕ vpip_delay_selection = _vpiDelaySelTypical;
ಶೂನ್ಯ ಗಣ_delay_selection(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sel)
{
      ಆದರೆ (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = vpi_Delay_Sel_ಗರಿಷ್ಠ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    ನಿರ್ಗಮಿಸು(1);
      }
      ಅಳಿಸು[] sel;
}

ಸ್ಥಿರ ಶೂನ್ಯ final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      ಗಾಗಿ (vector<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>::iterator cur = file_ಹೆಸರುಗಳು.begin();
           cur != file_ಹೆಸರುಗಳು.ಕೊನೆ() ; ++ cur ) {
	    ಅಳಿಸು[] *cur;
      }
	/* Clear the static result buffer. */
      (ಶೂನ್ಯ)need_ಫಲಿತಾಂಶ_buf(0, RBUF_DEL);
      codespace_delete();
      ಬೇರು_ಕೋಷ್ಟಕ_delete();
      def_ಕೋಷ್ಟಕ_delete();
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
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      ತುಂಬು_module_delete();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      simulator_cb_delete();
#ಆದರೆ_ಅಂತ್ಯ
}

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_mcd_init(FILE *ಲಘುಗಣಕ);
ಬಾಹ್ಯ ಶೂನ್ಯ vvp_vpi_init(ಶೂನ್ಯ);

ಸ್ಥಿರ ಶೂನ್ಯ report_ಬಳಸಿದ(ಶೂನ್ಯ)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

ಶೂನ್ಯ vvp_init(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *logfile_name, ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ*argv[])
{
      ರಚನೆ rusage ಚಕ್ರ;
      FILE *logfile = 0x0;
      ಬಾಹ್ಯ ಶೂನ್ಯ vpi_ಗಣ_vlog_info(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ**);

      ಆದರೆ (vvp_ಬಳಸಿದ++) {
          report_ಬಳಸಿದ();
          ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          ತಾರ್ಕಿಕ debugger_release = ಸುಳ್ಳು;
          ಆಗ( !debugger_release )  {
#ಆದರೆ defined(__MINGW32__)
              Sleep(1000);
#ಇಲ್ಲದಿದ್ದರೆ
              ಸುಷುಪ್ತಿ(1);
#ಆದರೆ_ಅಂತ್ಯ
        }
      }

      vpip_ಕೂಡಿಸು_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      ಆದರೆ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಮಾರ್ಗ = getenv("VVP_DEBUG")) {
	    debug_file.open(ಮಾರ್ಗ, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      ಆದರೆ (logfile_name) {
	    ಆದರೆ (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    ಇಲ್ಲದಿದ್ದರೆ {
		  logfile = fopen(logfile_name, "w");
		  ಆದರೆ (!logfile) {
		        perror(logfile_name);
		        ನಿರ್ಗಮಿಸು(1);
		  }
		  setvbuf(logfile, log_ಬಫರ್, _IOLBF, ಗಾತ್ರ(log_ಬಫರ್));
	    }
      }

      vpip_mcd_init(logfile);

      ಆದರೆ (verbose_flag) {
	    my_getrusage(&ಚಕ್ರ);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_ಗಣ_vlog_info(argc, argv);

      compile_init();
}

ಪೂರ್ಣಾಂಕ vvp_ಸತತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *design_ಮಾರ್ಗ)
{
      ರಚನೆ rusage ಚಕ್ರಗಳು[3];
      ಪೂರ್ಣಾಂಕ ret_cd;

      (ಶೂನ್ಯ) COPYRIGHT;
      ಆದರೆ (vvp_ಬಳಸಿದ++ != 1) {
          ಆದರೆ (vvp_ಬಳಸಿದ == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          ಇಲ್ಲದಿದ್ದರೆ
              report_ಬಳಸಿದ();
          ಹಿಂತಿರುಗಿ 1;
      }
      ++vvp_ಬಳಸಿದ;

      ret_cd = compile_design(design_ಮಾರ್ಗ);
      destroy_lexor();
      print_vpi_call_errors();
      ಆದರೆ (ret_cd) ಹಿಂತಿರುಗಿ ret_cd;

      ಆದರೆ (!have_ivl_ಆವೃತ್ತಿ) {
	    ಆದರೆ (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      ಆದರೆ (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_ಮಾರ್ಗ, compile_errors);
	    final_cleanup();
	    ಹಿಂತಿರುಗಿ compile_errors;
      }

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   ಎಣಿಕೆ_functors, vvp_net_fun_t::ರಾಶಿ_ಒಟ್ಟು());
	    vpi_mcd_printf(1, "           %8lu logic\n",  ಎಣಿಕೆ_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  ಎಣಿಕೆ_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",ಎಣಿಕೆ_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", ಎಣಿಕೆ_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   ಎಣಿಕೆ_filters, vvp_net_fil_t::ರಾಶಿ_ಒಟ್ಟು());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   ಎಣಿಕೆ_opcodes, ಗಾತ್ರ_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     ಎಣಿಕೆ_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   ಎಣಿಕೆ_vvp_nets, ಗಾತ್ರ_vvp_nets);
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

      ಆದರೆ (verbose_flag) {
	    my_getrusage(ಚಕ್ರಗಳು+1);
	    print_rusage(ಚಕ್ರಗಳು+1, ಚಕ್ರಗಳು+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      ವೇಳಾಪಟ್ಟಿ_simulate();

      ಆದರೆ (verbose_flag) {
	    my_getrusage(ಚಕ್ರಗಳು+2);
	    print_rusage(ಚಕ್ರಗಳು+2, ಚಕ್ರಗಳು+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   ಎಣಿಕೆ_ಸಮಯ_events, ಎಣಿಕೆ_ಸಮಯ_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    ಎಣಿಕೆ_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   ಎಣಿಕೆ_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   ಎಣಿಕೆ_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   ಎಣಿಕೆ_ನಿಯೋಜಿಸು_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   ಎಣಿಕೆ_ನಿಯೋಜಿಸು_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   ಎಣಿಕೆ_ನಿಯೋಜಿಸು_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   ಎಣಿಕೆ_gen_events, ಎಣಿಕೆ_gen_pool());
      }

      final_cleanup();

      ಹಿಂತಿರುಗಿ vvp_return_ಮೌಲ್ಯ;
}
