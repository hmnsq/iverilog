ଅଚଳ ଅକ୍ଷର COPYRIGHT[] =
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

#ଯଦି defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#ଯଦି defined(__MINGW32__)
# include  <windows.h>
#endif

#ଅନ୍ତର୍ଭାଵ "libvvp.h"

using ନାମାକାଶ std;

#ଯଦି defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ବାହ୍ୟ "C" ପୂର୍ଣ୍ଣସଂଖ୍ୟା getopt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର*argv[], ଅଚଳ ଅକ୍ଷର*fmt);
ବାହ୍ୟ "C" ପୂର୍ଣ୍ଣସଂଖ୍ୟା optind;
ବାହ୍ୟ "C" ଅଚଳ ଅକ୍ଷର*optarg;
#endif

ତାର୍କିକ version_flag = ମିଥ୍ୟା;

unsigned module_cnt = 0;
ଅଚଳ ଅକ୍ଷର*module_tab[64];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର*argv[])
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt;
      unsigned flag_errors = 0;
      ଅଚଳ ଅକ୍ଷର *logfile_name = 0x0;

      ଯେତେବେଳେ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) ବାଛ (opt) {
         ସ୍ଥିତି 'h':
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
	  ସ୍ଥିତି 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'l':
	    logfile_name = optarg;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'M':
	    ଯଦି (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } ଅନ୍ୟଥା {
		  vpip_add_module_path(optarg);
	    }
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'm':
	    module_tab[module_cnt++] = optarg;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'n':
	    vvp_set_stop_is_finish(ସତ୍ୟ, 0);
	    ଭାଙ୍ଗ;
          ସ୍ଥିତି 'N':
            vvp_set_stop_is_finish(ସତ୍ୟ, 1);
            ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'q':
	    vvp_set_quiet_flag(ସତ୍ୟ);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 's':
	    schedule_stop(0);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'v':
	    vvp_set_verbose_flag(ସତ୍ୟ);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'V':
	    version_flag = ସତ୍ୟ;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    flag_errors += 1;
      }

      ଯଦି (flag_errors)
	    ଫେରନ୍ତୁ flag_errors;

      ଯଦି (version_flag) {
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
	    ଫେରନ୍ତୁ 0;
      }

      ଯଦି (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ଫେରନ୍ତୁ -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      ପାଇଁ (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      ଫେରନ୍ତୁ vvp_run(argv[optind]);
}
