ਅਚਲ ਅੱਖਰ COPYRIGHT[] =
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

#ਜੇ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(__MINGW32__)
# include  <windows.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "libvvp.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

#ਜੇ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
ਬਾਹਰੀ "C" ਪੂਰਨ getopt(ਪੂਰਨ argc, ਅੱਖਰ*argv[], ਅਚਲ ਅੱਖਰ*fmt);
ਬਾਹਰੀ "C" ਪੂਰਨ optind;
ਬਾਹਰੀ "C" ਅਚਲ ਅੱਖਰ*optarg;
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ ਸੰਸਕਰਣ_flag = ਝੂਠ;

ਬਿਨਾਨਿਸ਼ਾਨੀ module_cnt = 0;
ਅਚਲ ਅੱਖਰ*module_tab[64];

ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ*argv[])
{
      ਪੂਰਨ opt;
      ਬਿਨਾਨਿਸ਼ਾਨੀ flag_errors = 0;
      ਅਚਲ ਅੱਖਰ *logfile_name = 0x0;

      ਜਦੋਂ ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) ਚੋਣ (opt) {
         ਹਾਲਤ 'h':
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
           ਬਾਹਰ_ਨਿਕਲੋ(0);
	  ਹਾਲਤ 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    ਰੋਕੋ;
	  ਹਾਲਤ 'l':
	    logfile_name = optarg;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'M':
	    ਜੇ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } ਨਹੀਂਤਾਂ {
		  vpip_ਜੋੜੋ_module_ਰਸਤਾ(optarg);
	    }
	    ਰੋਕੋ;
	  ਹਾਲਤ 'm':
	    module_tab[module_cnt++] = optarg;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'n':
	    vvp_ਸੈੱਟ_stop_is_ਅੰਤ(ਸੱਚ, 0);
	    ਰੋਕੋ;
          ਹਾਲਤ 'N':
            vvp_ਸੈੱਟ_stop_is_ਅੰਤ(ਸੱਚ, 1);
            ਰੋਕੋ;
	  ਹਾਲਤ 'q':
	    vvp_ਸੈੱਟ_quiet_flag(ਸੱਚ);
	    ਰੋਕੋ;
	  ਹਾਲਤ 's':
	    ਸਮਾਂ_ਸਾਰਣੀ_stop(0);
	    ਰੋਕੋ;
	  ਹਾਲਤ 'v':
	    vvp_ਸੈੱਟ_verbose_flag(ਸੱਚ);
	    ਰੋਕੋ;
	  ਹਾਲਤ 'V':
	    ਸੰਸਕਰਣ_flag = ਸੱਚ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    flag_errors += 1;
      }

      ਜੇ (flag_errors)
	    ਵਾਪਸ flag_errors;

      ਜੇ (ਸੰਸਕਰਣ_flag) {
	    fprintf(stderr, "Icarus Verilog runtime version " ਸੰਸਕਰਣ " ("
	                    ਸੰਸਕਰਣ_ਟੈਗ ")\n\n");
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
	    ਵਾਪਸ 0;
      }

      ਜੇ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    ਵਾਪਸ -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_ਲੱਦੋ_module(module_tab[idx]);

      ਵਾਪਸ vvp_ਲੜੀਵਾਰ(argv[optind]);
}
