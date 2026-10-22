અચલ અક્ષર COPYRIGHT[] =
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

#જો defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#જો defined(__MINGW32__)
# include  <windows.h>
#endif

#અન્તર્ભાવ "libvvp.h"

using નામાકાશ std;

#જો defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
બાહ્ય "C" પૂર્ણાંક getopt(પૂર્ણાંક argc, અક્ષર*argv[], અચલ અક્ષર*fmt);
બાહ્ય "C" પૂર્ણાંક optind;
બાહ્ય "C" અચલ અક્ષર*optarg;
#endif

તાર્કિક version_flag = ખોટું;

unsigned module_cnt = 0;
અચલ અક્ષર*module_tab[64];

પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર*argv[])
{
      પૂર્ણાંક opt;
      unsigned flag_errors = 0;
      અચલ અક્ષર *logfile_name = 0x0;

      જ્યારે ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) પસંદગી (opt) {
         સ્થિતિ 'h':
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
	  સ્થિતિ 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    તોડો;
	  સ્થિતિ 'l':
	    logfile_name = optarg;
	    તોડો;
	  સ્થિતિ 'M':
	    જો (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } નહીંતર {
		  vpip_add_module_path(optarg);
	    }
	    તોડો;
	  સ્થિતિ 'm':
	    module_tab[module_cnt++] = optarg;
	    તોડો;
	  સ્થિતિ 'n':
	    vvp_set_stop_is_finish(સાચું, 0);
	    તોડો;
          સ્થિતિ 'N':
            vvp_set_stop_is_finish(સાચું, 1);
            તોડો;
	  સ્થિતિ 'q':
	    vvp_set_quiet_flag(સાચું);
	    તોડો;
	  સ્થિતિ 's':
	    schedule_stop(0);
	    તોડો;
	  સ્થિતિ 'v':
	    vvp_set_verbose_flag(સાચું);
	    તોડો;
	  સ્થિતિ 'V':
	    version_flag = સાચું;
	    તોડો;
	  મૂળભૂત:
	    flag_errors += 1;
      }

      જો (flag_errors)
	    પાછા flag_errors;

      જો (version_flag) {
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
	    પાછા 0;
      }

      જો (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    પાછા -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      માટે (unsigned idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      પાછા vvp_run(argv[optind]);
}
