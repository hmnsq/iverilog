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
# include  "version_tag.h"
# include  "config.h"
# include  "compile.h"
# include  <cstdio>
# include  <cstdlib>

#ಆದರೆ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__MINGW32__)
# include  <windows.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "libvvp.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

#ಆದರೆ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಂಕ getopt(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ*argv[], ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*fmt);
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಂಕ optind;
ಬಾಹ್ಯ "C" ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*optarg;
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ ಆವೃತ್ತಿ_flag = ಸುಳ್ಳು;

ಚಿಹ್ನೆರಹಿತ module_cnt = 0;
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*module_tab[64];

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ*argv[])
{
      ಪೂರ್ಣಾಂಕ opt;
      ಚಿಹ್ನೆರಹಿತ flag_errors = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *logfile_name = 0x0;

      ಆಗ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) ಆಯ್ಕೆ (opt) {
         ಸ್ಥಿತಿ 'h':
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
           ನಿರ್ಗಮಿಸು(0);
	  ಸ್ಥಿತಿ 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'l':
	    logfile_name = optarg;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'M':
	    ಆದರೆ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(optarg);
	    }
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'm':
	    module_tab[module_cnt++] = optarg;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'n':
	    vvp_ಗಣ_stop_is_ಮುಕ್ತಾಯ(ನಿಜ, 0);
	    ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 'N':
            vvp_ಗಣ_stop_is_ಮುಕ್ತಾಯ(ನಿಜ, 1);
            ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'q':
	    vvp_ಗಣ_quiet_flag(ನಿಜ);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 's':
	    ವೇಳಾಪಟ್ಟಿ_stop(0);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'v':
	    vvp_ಗಣ_verbose_flag(ನಿಜ);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'V':
	    ಆವೃತ್ತಿ_flag = ನಿಜ;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    flag_errors += 1;
      }

      ಆದರೆ (flag_errors)
	    ಹಿಂತಿರುಗಿ flag_errors;

      ಆದರೆ (ಆವೃತ್ತಿ_flag) {
	    fprintf(stderr, "Icarus Verilog runtime version " ಆವೃತ್ತಿ " ("
	                    ಆವೃತ್ತಿ_ಟ್ಯಾಗ್ ")\n\n");
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
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಆದರೆ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ಹಿಂತಿರುಗಿ -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_ತುಂಬು_module(module_tab[idx]);

      ಹಿಂತಿರುಗಿ vvp_ಸತತ(argv[optind]);
}
