స్థిరరాశి అక్షరం COPYRIGHT[] =
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

#ఒకవేళ defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#అయితే_ముగింపు // defined(HAVE_SYS_RESOURCE_H)

#ఒకవేళ defined(__MINGW32__)
# include  <windows.h>
#అయితే_ముగింపు

#అన్తర్భావ "libvvp.h"

ఉపయోగించి నామాకాశ std;

ofstream debug_file;

#ఒకవేళ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
బాహ్య "C" పూర్ణాంకం getopt(పూర్ణాంకం argc, అక్షరం*argv[], స్థిరరాశి అక్షరం*fmt);
బాహ్య "C" పూర్ణాంకం optind;
బాహ్య "C" స్థిరరాశి అక్షరం*optarg;
#అయితే_ముగింపు

తార్కిక verbose_flag = అబద్ధం;
స్థిర పూర్ణాంకం vvp_return_విలువ = 0;
స్థిర పూర్ణాంకం vvp_వాడిన = 0;

శూన్య vvp_సమితి_stop_is_ముగింపు(తార్కిక flag, పూర్ణాంకం exit_సంకేతం)
{
      బాహ్య తార్కిక stop_is_ముగింపు;
      బాహ్య పూర్ణాంకం  stop_is_ముగింపు_exit_సంకేతం;

      stop_is_ముగింపు = flag;
      stop_is_ముగింపు_exit_సంకేతం = exit_సంకేతం;
}

శూన్య vvp_సమితి_quiet_flag(తార్కిక flag)
{
      vpip_mcd0_disable = flag;
}

శూన్య vvp_సమితి_verbose_flag(తార్కిక flag)
{
      verbose_flag = flag;
}

శూన్య vpip_సమితి_return_విలువ(పూర్ణాంకం value)
{
      vvp_return_విలువ = value;
}

స్థిర అక్షరం log_బఫరు[4096];

#ఒకవేళ defined(HAVE_SYS_RESOURCE_H)
స్థిర శూన్య my_getrusage(నిర్మాణం rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     ఒకవేళ defined(LINUX)
      {
	    FILE *statm;
	    గుర్తులేని siz, rss, shd;
	    దీర్ఘ పుట_కొలత = sysconf(_SC_PAGESIZE);
	    ఒకవేళ (పుట_కొలత==-1) పుట_కొలత=0;
	    statm = fopen("/proc/self/statm", "r");
	    ఒకవేళ (!statm) {
		  perror("/proc/self/statm");
		  తిరిగి;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    ఒకవేళ (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = పుట_కొలత * siz;
		  a->ru_idrss  = పుట_కొలత * rss;
		  a->ru_ixrss  = పుట_కొలత * shd;
	    }
	    fclose(statm);
      }
#     endif
}

స్థిర శూన్య print_rusage(నిర్మాణం rusage *a, నిర్మాణం rusage *b)
{
      ద్విగుణ delta = a->ru_utime.tv_sec
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

#లేకపోతే // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
నిర్మాణం rusage { పూర్ణాంకం x; };
అంతఃవరుస స్థిర శూన్య my_getrusage(నిర్మాణం rusage *) { }
అంతఃవరుస స్థిర శూన్య print_rusage(నిర్మాణం rusage *, నిర్మాణం rusage *){};

#అయితే_ముగింపు // ! defined(HAVE_SYS_RESOURCE_H)

స్థిర తార్కిక have_ivl_సంస్కరణ = అబద్ధం;
/*
 * Verify that the input file has a compatible version.
 */
శూన్య verify_సంస్కరణ(అక్షరం*ivl_ver, అక్షరం*commit)
{
      have_ivl_సంస్కరణ = నిజం;

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    ఒకవేళ (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      తొలగించు[] commit;

      పూర్ణాంకం file_major, file_minor, file_minor2;
      అక్షరం file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      పూర్ణాంకం rc = అక్షరశ్రేణి_చదువు(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      ఒకవేళ (rc == 2) {
	    file_extra[0] = 0;
	    rc = అక్షరశ్రేణి_చదువు(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    ప్రతిజ్ఞా((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      తొలగించు[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      ఒకవేళ (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      ఒకవేళ (సంస్కరణ_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, సంస్కరణ);
	    నిష్క్రమించు(1);
      }

      ఒకవేళ (సంస్కరణ_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, సంస్కరణ);
      }
}

పూర్ణాంకం vpip_delay_selection = _vpiDelaySelTypical;
శూన్య సమితి_delay_selection(స్థిరరాశి అక్షరం* sel)
{
      ఒకవేళ (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } లేకపోతే ఒకవేళ (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } లేకపోతే ఒకవేళ (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = vpi_Delay_Sel_గరిష్ఠ;
      } లేకపోతే {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    నిష్క్రమించు(1);
      }
      తొలగించు[] sel;
}

స్థిర శూన్య final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      కొరకు (vector<స్థిరరాశి అక్షరం*>::iterator cur = file_పేర్లు.begin();
           cur != file_పేర్లు.చివర() ; ++ cur ) {
	    తొలగించు[] *cur;
      }
	/* Clear the static result buffer. */
      (శూన్య)need_ఫలితం_buf(0, RBUF_DEL);
      codespace_delete();
      మూలం_పట్టిక_delete();
      def_పట్టిక_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_దొంతర_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#అయితే_ముగింపు
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      ఎక్కించు_module_delete();

#నిర్వచితమైతే CHECK_WITH_VALGRIND
      simulator_cb_delete();
#అయితే_ముగింపు
}

బాహ్య శూన్య vpip_mcd_init(FILE *సంవర్గమానం);
బాహ్య శూన్య vvp_vpi_init(శూన్య);

స్థిర శూన్య report_వాడిన(శూన్య)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

శూన్య vvp_init(స్థిరరాశి అక్షరం *logfile_name, పూర్ణాంకం argc, అక్షరం*argv[])
{
      నిర్మాణం rusage చక్రం;
      FILE *logfile = 0x0;
      బాహ్య శూన్య vpi_సమితి_vlog_info(పూర్ణాంకం, అక్షరం**);

      ఒకవేళ (vvp_వాడిన++) {
          report_వాడిన();
          తిరిగి;
      }

      ఒకవేళ( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          తార్కిక debugger_release = అబద్ధం;
          అయితే( !debugger_release )  {
#ఒకవేళ defined(__MINGW32__)
              Sleep(1000);
#లేకపోతే
              సుషుప్తి(1);
#అయితే_ముగింపు
        }
      }

      vpip_కూడు_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      ఒకవేళ (స్థిరరాశి అక్షరం*మార్గం = getenv("VVP_DEBUG")) {
	    debug_file.open(మార్గం, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      ఒకవేళ (logfile_name) {
	    ఒకవేళ (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    లేకపోతే {
		  logfile = fopen(logfile_name, "w");
		  ఒకవేళ (!logfile) {
		        perror(logfile_name);
		        నిష్క్రమించు(1);
		  }
		  setvbuf(logfile, log_బఫరు, _IOLBF, పరిమాణం(log_బఫరు));
	    }
      }

      vpip_mcd_init(logfile);

      ఒకవేళ (verbose_flag) {
	    my_getrusage(&చక్రం);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_సమితి_vlog_info(argc, argv);

      compile_init();
}

పూర్ణాంకం vvp_నిరంతరం(స్థిరరాశి అక్షరం *design_మార్గం)
{
      నిర్మాణం rusage చక్రాలు[3];
      పూర్ణాంకం ret_cd;

      (శూన్య) COPYRIGHT;
      ఒకవేళ (vvp_వాడిన++ != 1) {
          ఒకవేళ (vvp_వాడిన == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          లేకపోతే
              report_వాడిన();
          తిరిగి 1;
      }
      ++vvp_వాడిన;

      ret_cd = compile_design(design_మార్గం);
      destroy_lexor();
      print_vpi_call_errors();
      ఒకవేళ (ret_cd) తిరిగి ret_cd;

      ఒకవేళ (!have_ivl_సంస్కరణ) {
	    ఒకవేళ (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      ఒకవేళ (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_మార్గం, compile_errors);
	    final_cleanup();
	    తిరిగి compile_errors;
      }

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   లెక్క_functors, vvp_net_fun_t::కుప్ప_మొత్తం());
	    vpi_mcd_printf(1, "           %8lu logic\n",  లెక్క_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  లెక్క_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",లెక్క_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", లెక్క_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   లెక్క_filters, vvp_net_fil_t::కుప్ప_మొత్తం());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   లెక్క_opcodes, కొలత_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     లెక్క_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   లెక్క_vvp_nets, కొలత_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   లెక్క_net_arrays, లెక్క_net_array_పదాలు);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   లెక్క_var_arrays+లెక్క_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   లెక్క_var_arrays, లెక్క_var_array_పదాలు);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   లెక్క_real_arrays, లెక్క_real_array_పదాలు);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   లెక్క_vpi_scopes);
      }

      ఒకవేళ (verbose_flag) {
	    my_getrusage(చక్రాలు+1);
	    print_rusage(చక్రాలు+1, చక్రాలు+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      కాలపట్టిక_simulate();

      ఒకవేళ (verbose_flag) {
	    my_getrusage(చక్రాలు+2);
	    print_rusage(చక్రాలు+2, చక్రాలు+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   లెక్క_సమయం_events, లెక్క_సమయం_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    లెక్క_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    లెక్క_కేటాయించు_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   లెక్క_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   లెక్క_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   లెక్క_కేటాయించు_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   లెక్క_కేటాయించు_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   లెక్క_కేటాయించు_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   లెక్క_gen_events, లెక్క_gen_pool());
      }

      final_cleanup();

      తిరిగి vvp_return_విలువ;
}
