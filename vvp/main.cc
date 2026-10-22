ثابت حرف COPYRIGHT[] =
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

#إذا defined(HAVE_GETOPT_H)
# include  <getopt.h>
#جے_انت

#إذا defined(__MINGW32__)
# include  <windows.h>
#جے_انت

#ضمن "libvvp.h"

ورتوں نطاق std;

#إذا defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
خارجي "C" صحيح getopt(صحيح argc, حرف*argv[], ثابت حرف*fmt);
خارجي "C" صحيح optind;
خارجي "C" ثابت حرف*optarg;
#جے_انت

منطقي نسخہ_flag = خطأ;

بنانشانی module_cnt = 0;
ثابت حرف*module_tab[64];

صحيح مکھ(صحيح argc, حرف*argv[])
{
      صحيح opt;
      بنانشانی flag_errors = 0;
      ثابت حرف *logfile_name = 0x0;

      طالما ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) بدل (opt) {
         حالة 'h':
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
           باہر_نکلو(0);
	  حالة 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    اكسر;
	  حالة 'l':
	    logfile_name = optarg;
	    اكسر;
	  حالة 'M':
	    إذا (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } وإلا {
		  vpip_جوڑو_module_رستہ(optarg);
	    }
	    اكسر;
	  حالة 'm':
	    module_tab[module_cnt++] = optarg;
	    اكسر;
	  حالة 'n':
	    vvp_سیٹ_stop_is_انت(سچ, 0);
	    اكسر;
          حالة 'N':
            vvp_سیٹ_stop_is_انت(سچ, 1);
            اكسر;
	  حالة 'q':
	    vvp_سیٹ_quiet_flag(سچ);
	    اكسر;
	  حالة 's':
	    ویلا_سارنی_stop(0);
	    اكسر;
	  حالة 'v':
	    vvp_سیٹ_verbose_flag(سچ);
	    اكسر;
	  حالة 'V':
	    نسخہ_flag = سچ;
	    اكسر;
	  افتراضي:
	    flag_errors += 1;
      }

      إذا (flag_errors)
	    أرجع flag_errors;

      إذا (نسخہ_flag) {
	    fprintf(stderr, "Icarus Verilog runtime version " نسخہ " ("
	                    نسخہ_ٹیگ ")\n\n");
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
	    أرجع 0;
      }

      إذا (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    أرجع -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      لكل (بنانشانی idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_لدو_module(module_tab[idx]);

      أرجع vvp_لگاتار(argv[optind]);
}
