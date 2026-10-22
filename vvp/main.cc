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

#यदि defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#यदि defined(__MINGW32__)
# include  <windows.h>
#endif

#अन्तर्भाव "libvvp.h"

using नामाकाश std;

#यदि defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णाङ्क getopt(पूर्णाङ्क argc, वर्ण*argv[], ध्रुव वर्ण*fmt);
बाह्य "C" पूर्णाङ्क optind;
बाह्य "C" ध्रुव वर्ण*optarg;
#endif

तार्किक version_flag = असत्य;

अचिन्हित module_cnt = 0;
ध्रुव वर्ण*module_tab[64];

पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण*argv[])
{
      पूर्णाङ्क opt;
      अचिन्हित flag_errors = 0;
      ध्रुव वर्ण *logfile_name = 0x0;

      यावत् ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != खातापूर्ण) विकल्प (opt) {
         पक्ष 'h':
           ख_लिखो(मानक_त्रुटि,
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
           निकास(0);
	  पक्ष 'i':
	    रखो_भबफ(मानक_निकास, 0, _IONBF, 0);
	    भङ्ग;
	  पक्ष 'l':
	    logfile_name = optarg;
	    भङ्ग;
	  पक्ष 'M':
	    यदि (म_भेद(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } अन्यथा {
		  vpip_add_module_path(optarg);
	    }
	    भङ्ग;
	  पक्ष 'm':
	    module_tab[module_cnt++] = optarg;
	    भङ्ग;
	  पक्ष 'n':
	    vvp_set_stop_is_finish(सत्य, 0);
	    भङ्ग;
          पक्ष 'N':
            vvp_set_stop_is_finish(सत्य, 1);
            भङ्ग;
	  पक्ष 'q':
	    vvp_set_quiet_flag(सत्य);
	    भङ्ग;
	  पक्ष 's':
	    schedule_stop(0);
	    भङ्ग;
	  पक्ष 'v':
	    vvp_set_verbose_flag(सत्य);
	    भङ्ग;
	  पक्ष 'V':
	    version_flag = सत्य;
	    भङ्ग;
	  स्वभाव:
	    flag_errors += 1;
      }

      यदि (flag_errors)
	    प्रत्यागम flag_errors;

      यदि (version_flag) {
	    ख_लिखो(मानक_त्रुटि, "Icarus Verilog runtime version " VERSION " ("
	                    VERSION_TAG ")\n\n");
	    ख_लिखो(मानक_त्रुटि, "%s\n\n", COPYRIGHT);
	    ख_लिखो(मानक_त्रुटि,
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

      यदि (optind == argc) {
	    ख_लिखो(मानक_त्रुटि, "%s: no input file.\n", argv[0]);
	    प्रत्यागम -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      प्रति (अचिन्हित idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      प्रत्यागम vvp_run(argv[optind]);
}
