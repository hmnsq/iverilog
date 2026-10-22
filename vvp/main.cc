സ്ഥിരം അക്ഷരം COPYRIGHT[] =
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
# include  "version_tag.h"
# include  "config.h"
# include  "compile.h"
# include  <cstdio>
# include  <cstdlib>

#എങ്കിൽ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#എങ്കിൽ defined(__MINGW32__)
# include  <windows.h>
#endif

#അന്തര്ഭാവ "libvvp.h"

using നാമാകാശ std;

#എങ്കിൽ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ബാഹ്യ "C" പൂർണ്ണസംഖ്യ getopt(പൂർണ്ണസംഖ്യ argc, അക്ഷരം*argv[], സ്ഥിരം അക്ഷരം*fmt);
ബാഹ്യ "C" പൂർണ്ണസംഖ്യ optind;
ബാഹ്യ "C" സ്ഥിരം അക്ഷരം*optarg;
#endif

തർക്കം version_flag = അസത്യം;

unsigned module_cnt = 0;
സ്ഥിരം അക്ഷരം*module_tab[64];

പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം*argv[])
{
      പൂർണ്ണസംഖ്യ opt;
      unsigned flag_errors = 0;
      സ്ഥിരം അക്ഷരം *logfile_name = 0x0;

      ആകുമ്പോൾ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) തിരഞ്ഞെടുപ്പ് (opt) {
         അവസ്ഥ 'h':
           fprintf(stderr,
                   "Usage: vvp [options] input-file [+plusargs...]\n"
                   "Options:\n"
                   " -h             Print this help message.\n"
                   " -i             Interactive mode (unbuffered stdio).\n"
                   " -l file        Logfile, '-' for <stderr>\n"
                   " -M path        VPI module directory\n"
		   " -M -           Clear VPI module path\n"
                   " -m module      Load vpi module.\n"
		   " -n             Non-interactive ($stop = $finish).\n"
                   " -N             Same as -n, but exit code is 1 instead of 0\n"
		   " -q             Quiet mode (suppress output on MCD bit 0).\n"
		   " -s             $stop right away.\n"
                   " -v             Verbose progress messages.\n"
                   " -V             Print the version information.\n" );
           exit(0);
	  അവസ്ഥ 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    നിർത്തുക;
	  അവസ്ഥ 'l':
	    logfile_name = optarg;
	    നിർത്തുക;
	  അവസ്ഥ 'M':
	    എങ്കിൽ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } അല്ലെങ്കിൽ {
		  vpip_add_module_path(optarg);
	    }
	    നിർത്തുക;
	  അവസ്ഥ 'm':
	    module_tab[module_cnt++] = optarg;
	    നിർത്തുക;
	  അവസ്ഥ 'n':
	    vvp_set_stop_is_finish(സത്യം, 0);
	    നിർത്തുക;
          അവസ്ഥ 'N':
            vvp_set_stop_is_finish(സത്യം, 1);
            നിർത്തുക;
	  അവസ്ഥ 'q':
	    vvp_set_quiet_flag(സത്യം);
	    നിർത്തുക;
	  അവസ്ഥ 's':
	    schedule_stop(0);
	    നിർത്തുക;
	  അവസ്ഥ 'v':
	    vvp_set_verbose_flag(സത്യം);
	    നിർത്തുക;
	  അവസ്ഥ 'V':
	    version_flag = സത്യം;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    flag_errors += 1;
      }

      എങ്കിൽ (flag_errors)
	    തിരികെ flag_errors;

      എങ്കിൽ (version_flag) {
	    fprintf(stderr, "Icarus Verilog runtime version " VERSION " ("
	                    VERSION_TAG ")\n\n");
	    fprintf(stderr, "%s\n\n", COPYRIGHT);
	    fprintf(stderr,
"  This program is free software; you can redistribute it and/or modify\n"
"  it under the terms of the GNU General Public License as published by\n"
"  the Free Software Foundation; either version 2 of the License, or\n"
"  (at your option) any later version.\n"
"\n"
"  This program is distributed in the hope that it will be useful,\n"
"  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"  GNU General Public License for more details.\n"
"\n"
"  You should have received a copy of the GNU General Public License along\n"
"  with this program; if not, write to the Free Software Foundation, Inc.,\n"
"  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.\n\n"
);
	    തിരികെ 0;
      }

      എങ്കിൽ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    തിരികെ -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      വേണ്ടി (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      തിരികെ vvp_run(argv[optind]);
}
