ধ্রুবক অক্ষর COPYRIGHT[] =
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

#অন্তর্ভাব "libvvp.h"

using নামাকাশ std;

#যদি defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
বাহ্যিক "C" পূর্ণসংখ্যা getopt(পূর্ণসংখ্যা argc, অক্ষর*argv[], ধ্রুবক অক্ষর*fmt);
বাহ্যিক "C" পূর্ণসংখ্যা optind;
বাহ্যিক "C" ধ্রুবক অক্ষর*optarg;
#endif

যৌক্তিক version_flag = মিথ্যা;

unsigned module_cnt = 0;
ধ্রুবক অক্ষর*module_tab[64];

পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর*argv[])
{
      পূর্ণসংখ্যা opt;
      unsigned flag_errors = 0;
      ধ্রুবক অক্ষর *logfile_name = 0x0;

      যখন ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) নির্বাচন (opt) {
         ক্ষেত্র 'h':
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
	  ক্ষেত্র 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    বিরতি;
	  ক্ষেত্র 'l':
	    logfile_name = optarg;
	    বিরতি;
	  ক্ষেত্র 'M':
	    যদি (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } অন্যথা {
		  vpip_add_module_path(optarg);
	    }
	    বিরতি;
	  ক্ষেত্র 'm':
	    module_tab[module_cnt++] = optarg;
	    বিরতি;
	  ক্ষেত্র 'n':
	    vvp_set_stop_is_finish(সত্য, 0);
	    বিরতি;
          ক্ষেত্র 'N':
            vvp_set_stop_is_finish(সত্য, 1);
            বিরতি;
	  ক্ষেত্র 'q':
	    vvp_set_quiet_flag(সত্য);
	    বিরতি;
	  ক্ষেত্র 's':
	    schedule_stop(0);
	    বিরতি;
	  ক্ষেত্র 'v':
	    vvp_set_verbose_flag(সত্য);
	    বিরতি;
	  ক্ষেত্র 'V':
	    version_flag = সত্য;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    flag_errors += 1;
      }

      যদি (flag_errors)
	    ফেরত flag_errors;

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
	    ফেরত 0;
      }

      যদি (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ফেরত -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      জন্য (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      ফেরত vvp_run(argv[optind]);
}
