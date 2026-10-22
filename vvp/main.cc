ধ্রুৱ ৱর্ণ COPYRIGHT[] =
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

#যদি defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#যদি defined(__MINGW32__)
# include  <windows.h>
#endif

#অন্তর্ভাৱ "libvvp.h"

using নামাকাশ std;

#যদি defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
বাহ্য "C" পূর্ণাঙ্ক getopt(পূর্ণাঙ্ক argc, ৱর্ণ*argv[], ধ্রুৱ ৱর্ণ*fmt);
বাহ্য "C" পূর্ণাঙ্ক optind;
বাহ্য "C" ধ্রুৱ ৱর্ণ*optarg;
#endif

তার্কিক version_flag = মিছা;

unsigned module_cnt = 0;
ধ্রুৱ ৱর্ণ*module_tab[64];

পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ*argv[])
{
      পূর্ণাঙ্ক opt;
      unsigned flag_errors = 0;
      ধ্রুৱ ৱর্ণ *logfile_name = 0x0;

      যেতিয়ালৈ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) ৱিকল্প (opt) {
         পক্ষ 'h':
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
	  পক্ষ 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    ভঙ্গ;
	  পক্ষ 'l':
	    logfile_name = optarg;
	    ভঙ্গ;
	  পক্ষ 'M':
	    যদি (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } অন্যথা {
		  vpip_add_module_path(optarg);
	    }
	    ভঙ্গ;
	  পক্ষ 'm':
	    module_tab[module_cnt++] = optarg;
	    ভঙ্গ;
	  পক্ষ 'n':
	    vvp_set_stop_is_finish(সঁচা, 0);
	    ভঙ্গ;
          পক্ষ 'N':
            vvp_set_stop_is_finish(সঁচা, 1);
            ভঙ্গ;
	  পক্ষ 'q':
	    vvp_set_quiet_flag(সঁচা);
	    ভঙ্গ;
	  পক্ষ 's':
	    schedule_stop(0);
	    ভঙ্গ;
	  পক্ষ 'v':
	    vvp_set_verbose_flag(সঁচা);
	    ভঙ্গ;
	  পক্ষ 'V':
	    version_flag = সঁচা;
	    ভঙ্গ;
	  স্ৱভাৱ:
	    flag_errors += 1;
      }

      যদি (flag_errors)
	    ঘূৰাই flag_errors;

      যদি (version_flag) {
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
	    ঘূৰাই 0;
      }

      যদি (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ঘূৰাই -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      বাবে (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      ঘূৰাই vvp_run(argv[optind]);
}
