ಧ್ರುವ ವರ್ಣ COPYRIGHT[] =
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

#ಯದಿ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#ಯದಿ defined(__MINGW32__)
# include  <windows.h>
#endif

#ಅನ್ತರ್ಭಾವ "libvvp.h"

using ನಾಮಾಕಾಶ std;

#ಯದಿ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಙ್ಕ getopt(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ*argv[], ಧ್ರುವ ವರ್ಣ*fmt);
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಙ್ಕ optind;
ಬಾಹ್ಯ "C" ಧ್ರುವ ವರ್ಣ*optarg;
#endif

ತಾರ್ಕಿಕ version_flag = ಅಸತ್ಯ;

unsigned module_cnt = 0;
ಧ್ರುವ ವರ್ಣ*module_tab[64];

ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ*argv[])
{
      ಪೂರ್ಣಾಙ್ಕ opt;
      unsigned flag_errors = 0;
      ಧ್ರುವ ವರ್ಣ *logfile_name = 0x0;

      ಯಾವತ್ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) ವಿಕಲ್ಪ (opt) {
         ಪಕ್ಷ 'h':
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
	  ಪಕ್ಷ 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'l':
	    logfile_name = optarg;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'M':
	    ಯದಿ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } ಅನ್ಯಥಾ {
		  vpip_add_module_path(optarg);
	    }
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'm':
	    module_tab[module_cnt++] = optarg;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'n':
	    vvp_set_stop_is_finish(ಸತ್ಯ, 0);
	    ಭಙ್ಗ;
          ಪಕ್ಷ 'N':
            vvp_set_stop_is_finish(ಸತ್ಯ, 1);
            ಭಙ್ಗ;
	  ಪಕ್ಷ 'q':
	    vvp_set_quiet_flag(ಸತ್ಯ);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 's':
	    schedule_stop(0);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'v':
	    vvp_set_verbose_flag(ಸತ್ಯ);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'V':
	    version_flag = ಸತ್ಯ;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    flag_errors += 1;
      }

      ಯದಿ (flag_errors)
	    ಪ್ರತ್ಯಾಗಮ flag_errors;

      ಯದಿ (version_flag) {
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
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ಯದಿ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ಪ್ರತ್ಯಾಗಮ -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      ಪ್ರತ್ಯಾಗಮ vvp_run(argv[optind]);
}
