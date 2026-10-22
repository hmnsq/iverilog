ثابت حرف COPYRIGHT[] =
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

#اگر defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#اگر_ختم // defined(HAVE_SYS_RESOURCE_H)

#اگر defined(__MINGW32__)
# include  <windows.h>
#اگر_ختم

#شامل "libvvp.h"

استعمال نامخلا std;

ofstream debug_file;

#اگر defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
خارجی "C" صحیح getopt(صحیح argc, حرف*argv[], ثابت حرف*fmt);
خارجی "C" صحیح optind;
خارجی "C" ثابت حرف*optarg;
#اگر_ختم

منطقی verbose_flag = جھوٹ;
ساکن صحیح vvp_return_قدر = 0;
ساکن صحیح vvp_مستعمل = 0;

خالی vvp_سیٹ_stop_is_اختتام(منطقی flag, صحیح exit_رمز)
{
      خارجی منطقی stop_is_اختتام;
      خارجی صحیح  stop_is_اختتام_exit_رمز;

      stop_is_اختتام = flag;
      stop_is_اختتام_exit_رمز = exit_رمز;
}

خالی vvp_سیٹ_quiet_flag(منطقی flag)
{
      vpip_mcd0_disable = flag;
}

خالی vvp_سیٹ_verbose_flag(منطقی flag)
{
      verbose_flag = flag;
}

خالی vpip_سیٹ_return_قدر(صحیح value)
{
      vvp_return_قدر = value;
}

ساکن حرف log_بفر[4096];

#اگر defined(HAVE_SYS_RESOURCE_H)
ساکن خالی my_getrusage(ساخت rusage *a)
{
      getrusage(RUSAGE_SELF, a);

#     اگر defined(LINUX)
      {
	    FILE *statm;
	    غیرعلامتی siz, rss, shd;
	    طویل صفحہ_سائز = sysconf(_SC_PAGESIZE);
	    اگر (صفحہ_سائز==-1) صفحہ_سائز=0;
	    statm = fopen("/proc/self/statm", "r");
	    اگر (!statm) {
		  perror("/proc/self/statm");
		  واپس;
	    }
	      /* Given that these are in pages we'll limit the value to
	       * what will fit in a 32 bit integer to prevent undefined
	       * behavior in fscanf(). */
	    اگر (3 == fscanf(statm, "%9u %9u %9u", &siz, &rss, &shd)) {
		  a->ru_maxrss = صفحہ_سائز * siz;
		  a->ru_idrss  = صفحہ_سائز * rss;
		  a->ru_ixrss  = صفحہ_سائز * shd;
	    }
	    fclose(statm);
      }
#     endif
}

ساکن خالی print_rusage(ساخت rusage *a, ساخت rusage *b)
{
      دوہرا delta = a->ru_utime.tv_sec
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

#ورنہ // ! defined(HAVE_SYS_RESOURCE_H)

// Provide dummies
ساخت rusage { صحیح x; };
اندرونی ساکن خالی my_getrusage(ساخت rusage *) { }
اندرونی ساکن خالی print_rusage(ساخت rusage *, ساخت rusage *){};

#اگر_ختم // ! defined(HAVE_SYS_RESOURCE_H)

ساکن منطقی have_ivl_نسخہ = جھوٹ;
/*
 * Verify that the input file has a compatible version.
 */
خالی verify_نسخہ(حرف*ivl_ver, حرف*commit)
{
      have_ivl_نسخہ = سچ;

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ... VVP file version %s", ivl_ver);
	    اگر (commit) vpi_mcd_printf(1, " %s", commit);
	    vpi_mcd_printf(1, "\n");
      }
      حذف[] commit;

      صحیح file_major, file_minor, file_minor2;
      حرف file_extra[128];

	// Old style format: 0.<major>.<minor> <extra>
	// This also catches a potential new-new format that has
	// another sub-minor number.
      file_extra[0] = 0;
      صحیح rc = لڑی_سے_پڑھو(ivl_ver, "%d.%d.%d %127s", &file_major, &file_minor, &file_minor2, file_extra);

	// If it wasn't the old style format, try the new format:
	// <major>.<minor> <extra>
      اگر (rc == 2) {
	    file_extra[0] = 0;
	    rc = لڑی_سے_پڑھو(ivl_ver, "%d.%d %127s", &file_major, &file_minor, file_extra);
	    أكد((rc == 2) || (rc == 3));
	    file_minor2 = 0;
      }
      حذف[] ivl_ver;

	// If this was the old format, the file_major will be 0. In
	// this case it is not really what we meant, so convert to the
	// new format.
      اگر (file_major == 0) {
	    file_major = file_minor;
	    file_minor = file_minor2;
	    file_minor2 = 0;
      }

      اگر (نسخہ_MAJOR != file_major) {
	    vpi_mcd_printf(1, "Error: VVP input file %d.%d can not "
			   "be run with run time version %s\n",
			   file_major, file_minor, نسخہ);
	    باہر_نکلو(1);
      }

      اگر (نسخہ_MINOR < file_minor) {
	    vpi_mcd_printf(1, "Warning: VVP input file sub version %d.%d"
			   " is greater than the run time version %s.\n",
			   file_major, file_minor, نسخہ);
      }
}

صحیح vpip_delay_selection = _vpiDelaySelTypical;
خالی سیٹ_delay_selection(ثابت حرف* sel)
{
      اگر (strcmp("TYPICAL", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelTypical;
      } ورنہ اگر (strcmp("MINIMUM", sel) == 0) {
	    vpip_delay_selection = _vpiDelaySelMinimum;
      } ورنہ اگر (strcmp("MAXIMUM", sel) == 0) {
	    vpip_delay_selection = vpi_Delay_Sel_زیادہ_سے_زیادہ;
      } ورنہ {
	    vpi_mcd_printf(1, "Error: Unknown delay selection \"%s\"!", sel);
	    باہر_نکلو(1);
      }
      حذف[] sel;
}

ساکن خالی final_cleanup()
{
      vvp_object::cleanup();

	/*
	 * We only need to cleanup the memory if we are checking with valgrind.
	 */
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	/* Clean up the file name table. */
      برائے (vector<ثابت حرف*>::iterator cur = file_نام.begin();
           cur != file_نام.انتہا() ; ++ cur ) {
	    حذف[] *cur;
      }
	/* Clear the static result buffer. */
      (خالی)need_نتیجہ_buf(0, RBUF_DEL);
      codespace_delete();
      جڑ_جدول_delete();
      def_جدول_delete();
      vpi_mcd_delete();
      dec_str_delete();
      modpath_delete();
      vpi_handle_delete();
      vpi_ڈھیر_delete();
      udp_defns_delete();
      island_delete();
      signal_pool_delete();
      vvp_net_pool_delete();
      ufunc_pool_delete();
#اگر_ختم
	/*
	 * Unload the VPI modules. This is essential for MinGW, to ensure
	 * dump files are flushed before the main process terminates, as
	 * the DLL termination code is called after all remaining open
	 * files are automatically closed.
	 */
      بھرو_module_delete();

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      simulator_cb_delete();
#اگر_ختم
}

خارجی خالی vpip_mcd_init(FILE *لوگارتھم);
خارجی خالی vvp_vpi_init(خالی);

ساکن خالی report_مستعمل(خالی)
{
  fprintf(stderr,
	  "This VVP simulation has already run and can not be reused\n");
}

خالی vvp_init(ثابت حرف *logfile_name, صحیح argc, حرف*argv[])
{
      ساخت rusage چکر;
      FILE *logfile = 0x0;
      خارجی خالی vpi_سیٹ_vlog_info(صحیح, حرف**);

      اگر (vvp_مستعمل++) {
          report_مستعمل();
          واپس;
      }

      اگر( ::getenv("VVP_WAIT_FOR_DEBUGGER") != 0 ) {
          fprintf( stderr, "Waiting for debugger...\n");
          منطقی debugger_release = جھوٹ;
          جب( !debugger_release )  {
#اگر defined(__MINGW32__)
              Sleep(1000);
#ورنہ
              نوم(1);
#اگر_ختم
        }
      }

      vpip_جوڑو_env_and_default_module_paths();

	/* If the VVP_DEBUG variable is set, then it contains the path
	   to the vvp debug file. Open it for output. */

      اگر (ثابت حرف*راستہ = getenv("VVP_DEBUG")) {
	    debug_file.open(راستہ, ios::out);
      }

	/* This is needed to get the MCD I/O routines ready for
	   anything. It is done early because it is plausible that the
	   compile might affect it, and it is cheap to do. */

      اگر (logfile_name) {
	    اگر (!strcmp(logfile_name, "-"))
		  logfile = stderr;
	    ورنہ {
		  logfile = fopen(logfile_name, "w");
		  اگر (!logfile) {
		        perror(logfile_name);
		        باہر_نکلو(1);
		  }
		  setvbuf(logfile, log_بفر, _IOLBF, حجم(log_بفر));
	    }
      }

      vpip_mcd_init(logfile);

      اگر (verbose_flag) {
	    my_getrusage(&چکر);
	    vpi_mcd_printf(1, "Compiling VVP ...\n");
      }

      vvp_vpi_init();

	/* Make the extended arguments available to the simulation. */
      vpi_سیٹ_vlog_info(argc, argv);

      compile_init();
}

صحیح vvp_تسلسل(ثابت حرف *design_راستہ)
{
      ساخت rusage ادوار[3];
      صحیح ret_cd;

      (خالی) COPYRIGHT;
      اگر (vvp_مستعمل++ != 1) {
          اگر (vvp_مستعمل == 1)
              fprintf(stderr, "vvp_init() has not been called\n");
          ورنہ
              report_مستعمل();
          واپس 1;
      }
      ++vvp_مستعمل;

      ret_cd = compile_design(design_راستہ);
      destroy_lexor();
      print_vpi_call_errors();
      اگر (ret_cd) واپس ret_cd;

      اگر (!have_ivl_نسخہ) {
	    اگر (verbose_flag) vpi_mcd_printf(1, "... ");
	    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
	                      "version!\n");
      }

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, "Compile cleanup...\n");
      }

      compile_cleanup();

      اگر (compile_errors > 0) {
	    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
		    design_راستہ, compile_errors);
	    final_cleanup();
	    واپس compile_errors;
      }

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
			   گنتی_functors, vvp_net_fun_t::ڈھیر_کل());
	    vpi_mcd_printf(1, "           %8lu logic\n",  گنتی_functors_logic);
	    vpi_mcd_printf(1, "           %8lu bufif\n",  گنتی_functors_bufif);
	    vpi_mcd_printf(1, "           %8lu resolv\n",گنتی_functors_resolv);
	    vpi_mcd_printf(1, "           %8lu signals\n", گنتی_functors_sig);
	    vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
			   گنتی_filters, vvp_net_fil_t::ڈھیر_کل());
	    vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
	                   گنتی_opcodes, سائز_opcodes);
	    vpi_mcd_printf(1, " ... %8lu nets\n",     گنتی_vpi_nets);
	    vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
			   گنتی_vvp_nets, سائز_vvp_nets);
	    vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
			   گنتی_net_arrays, گنتی_net_array_الفاظ);
	    vpi_mcd_printf(1, " ... %8lu memories\n",
			   گنتی_var_arrays+گنتی_real_arrays);
	    vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
			   گنتی_var_arrays, گنتی_var_array_الفاظ);
	    vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
			   گنتی_real_arrays, گنتی_real_array_الفاظ);
	    vpi_mcd_printf(1, " ... %8lu scopes\n",   گنتی_vpi_scopes);
      }

      اگر (verbose_flag) {
	    my_getrusage(ادوار+1);
	    print_rusage(ادوار+1, ادوار+0);
	    vpi_mcd_printf(1, "Running ...\n");
      }


      جدول_وقت_simulate();

      اگر (verbose_flag) {
	    my_getrusage(ادوار+2);
	    print_rusage(ادوار+2, ادوار+1);

	    vpi_mcd_printf(1, "Event counts:\n");
	    vpi_mcd_printf(1, "    %8lu time steps (pool=%lu)\n",
			   گنتی_وقت_events, گنتی_وقت_pool());
	    vpi_mcd_printf(1, "    %8lu thread schedule events\n",
		    گنتی_thread_events);
	    vpi_mcd_printf(1, "    %8lu assign events\n",
		    گنتی_تفویض_events);
	    vpi_mcd_printf(1, "             ...assign(vec4) pool=%lu\n",
			   گنتی_assign4_pool());
	    vpi_mcd_printf(1, "             ...assign(vec8) pool=%lu\n",
			   گنتی_assign8_pool());
	    vpi_mcd_printf(1, "             ...assign(real) pool=%lu\n",
			   گنتی_تفویض_real_pool());
	    vpi_mcd_printf(1, "             ...assign(word) pool=%lu\n",
			   گنتی_تفویض_aword_pool());
	    vpi_mcd_printf(1, "             ...assign(word/r) pool=%lu\n",
			   گنتی_تفویض_arword_pool());
	    vpi_mcd_printf(1, "    %8lu other events (pool=%lu)\n",
			   گنتی_gen_events, گنتی_gen_pool());
      }

      final_cleanup();

      واپس vvp_return_قدر;
}
