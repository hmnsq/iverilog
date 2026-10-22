மாறிலி எழுத்து COPYRIGHT[] =
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

#எனில் defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#எனில் defined(__MINGW32__)
# include  <windows.h>
#endif

#அந்தர்பாவ "libvvp.h"

using நாமாகாஶ std;

#எனில் defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
வெளி "C" முழு getopt(முழு argc, எழுத்து*argv[], மாறிலி எழுத்து*fmt);
வெளி "C" முழு optind;
வெளி "C" மாறிலி எழுத்து*optarg;
#endif

தருக்க version_flag = பொய்;

unsigned module_cnt = 0;
மாறிலி எழுத்து*module_tab[64];

முழு முதன்மை(முழு argc, எழுத்து*argv[])
{
      முழு opt;
      unsigned flag_errors = 0;
      மாறிலி எழுத்து *logfile_name = 0x0;

      போது ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) தேர்வு (opt) {
         நிலை 'h':
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
	  நிலை 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    நிறுத்து;
	  நிலை 'l':
	    logfile_name = optarg;
	    நிறுத்து;
	  நிலை 'M':
	    எனில் (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } இல்லையெனில் {
		  vpip_add_module_path(optarg);
	    }
	    நிறுத்து;
	  நிலை 'm':
	    module_tab[module_cnt++] = optarg;
	    நிறுத்து;
	  நிலை 'n':
	    vvp_set_stop_is_finish(உண்மை, 0);
	    நிறுத்து;
          நிலை 'N':
            vvp_set_stop_is_finish(உண்மை, 1);
            நிறுத்து;
	  நிலை 'q':
	    vvp_set_quiet_flag(உண்மை);
	    நிறுத்து;
	  நிலை 's':
	    schedule_stop(0);
	    நிறுத்து;
	  நிலை 'v':
	    vvp_set_verbose_flag(உண்மை);
	    நிறுத்து;
	  நிலை 'V':
	    version_flag = உண்மை;
	    நிறுத்து;
	  இயல்பு:
	    flag_errors += 1;
      }

      எனில் (flag_errors)
	    திரும்பு flag_errors;

      எனில் (version_flag) {
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
	    திரும்பு 0;
      }

      எனில் (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    திரும்பு -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      க்கு (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      திரும்பு vvp_run(argv[optind]);
}
