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
# include  "version_tag.h"
# include  "config.h"
# include  "compile.h"
# include  <cstdio>
# include  <cstdlib>

#जइ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#जइ defined(__MINGW32__)
# include  <windows.h>
#endif

#अन्तर्भाव "libvvp.h"

using नामाकाश std;

#जइ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णाङ्क getopt(पूर्णाङ्क argc, वर्ण*argv[], ध्रुव वर्ण*fmt);
बाह्य "C" पूर्णाङ्क optind;
बाह्य "C" ध्रुव वर्ण*optarg;
#endif

तार्किक version_flag = मिच्छ;

unsigned module_cnt = 0;
ध्रुव वर्ण*module_tab[64];

पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण*argv[])
{
      पूर्णाङ्क opt;
      unsigned flag_errors = 0;
      ध्रुव वर्ण *logfile_name = 0x0;

      जाव ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) विकल्प (opt) {
         पक्ष 'h':
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
	  पक्ष 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    भङ्ग;
	  पक्ष 'l':
	    logfile_name = optarg;
	    भङ्ग;
	  पक्ष 'M':
	    जइ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } अण्णहा {
		  vpip_add_module_path(optarg);
	    }
	    भङ्ग;
	  पक्ष 'm':
	    module_tab[module_cnt++] = optarg;
	    भङ्ग;
	  पक्ष 'n':
	    vvp_set_stop_is_finish(सच्च, 0);
	    भङ्ग;
          पक्ष 'N':
            vvp_set_stop_is_finish(सच्च, 1);
            भङ्ग;
	  पक्ष 'q':
	    vvp_set_quiet_flag(सच्च);
	    भङ्ग;
	  पक्ष 's':
	    schedule_stop(0);
	    भङ्ग;
	  पक्ष 'v':
	    vvp_set_verbose_flag(सच्च);
	    भङ्ग;
	  पक्ष 'V':
	    version_flag = सच्च;
	    भङ्ग;
	  स्वभाव:
	    flag_errors += 1;
      }

      जइ (flag_errors)
	    प्रत्यागम flag_errors;

      जइ (version_flag) {
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
	    प्रत्यागम 0;
      }

      जइ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    प्रत्यागम -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      प्रति (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      प्रत्यागम vvp_run(argv[optind]);
}
