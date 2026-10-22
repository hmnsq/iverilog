const var_na COPYRIGHT[] =
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

#ᨀᨘ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#ᨀᨘ defined(__MINGW32__)
# include  <windows.h>
#endif

#include "libvvp.h"

using namespace std;

#ᨀᨘ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
extern "C" puur_naa_nka getopt(puur_naa_nka argc, var_na*argv[], const var_na*fmt);
extern "C" puur_naa_nka optind;
extern "C" const var_na*optarg;
#endif

bool version_flag = false;

unsigned module_cnt = 0;
const var_na*module_tab[64];

puur_naa_nka main(puur_naa_nka argc, var_na*argv[])
{
      puur_naa_nka opt;
      unsigned flag_errors = 0;
      const var_na *logfile_name = 0x0;

      ᨀᨛ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) switch (opt) {
         pak_sa 'h':
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
	  pak_sa 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    bha_nga;
	  pak_sa 'l':
	    logfile_name = optarg;
	    bha_nga;
	  pak_sa 'M':
	    ᨀᨘ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } ᨀᨘᨁ {
		  vpip_add_module_path(optarg);
	    }
	    bha_nga;
	  pak_sa 'm':
	    module_tab[module_cnt++] = optarg;
	    bha_nga;
	  pak_sa 'n':
	    vvp_set_stop_is_finish(true, 0);
	    bha_nga;
          pak_sa 'N':
            vvp_set_stop_is_finish(true, 1);
            bha_nga;
	  pak_sa 'q':
	    vvp_set_quiet_flag(true);
	    bha_nga;
	  pak_sa 's':
	    schedule_stop(0);
	    bha_nga;
	  pak_sa 'v':
	    vvp_set_verbose_flag(true);
	    bha_nga;
	  pak_sa 'V':
	    version_flag = true;
	    bha_nga;
	  default:
	    flag_errors += 1;
      }

      ᨀᨘ (flag_errors)
	    ᨀᨘᨒ flag_errors;

      ᨀᨘ (version_flag) {
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
	    ᨀᨘᨒ 0;
      }

      ᨀᨘ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ᨀᨘᨒ -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      for (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      ᨀᨘᨒ vvp_run(argv[optind]);
}
