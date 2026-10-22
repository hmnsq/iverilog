स्थिरांक अक्षर COPYRIGHT[] =
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

#जर defined(HAVE_GETOPT_H)
# include  <getopt.h>
#endif

#जर defined(__MINGW32__)
# include  <windows.h>
#endif

#अन्तर्भाव "libvvp.h"

using नामाकाश std;

#जर defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णांक getopt(पूर्णांक argc, अक्षर*argv[], स्थिरांक अक्षर*fmt);
बाह्य "C" पूर्णांक optind;
बाह्य "C" स्थिरांक अक्षर*optarg;
#endif

तार्किक version_flag = असत्य;

अचिन्हित module_cnt = 0;
स्थिरांक अक्षर*module_tab[64];

पूर्णांक main(पूर्णांक argc, अक्षर*argv[])
{
      पूर्णांक opt;
      अचिन्हित flag_errors = 0;
      स्थिरांक अक्षर *logfile_name = 0x0;

      पर्यंत ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != खातापूर्ण) निवड (opt) {
         स्थिती 'h':
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
	  स्थिती 'i':
	    रखो_भबफ(मानक_निकास, 0, _IONBF, 0);
	    थांबा;
	  स्थिती 'l':
	    logfile_name = optarg;
	    थांबा;
	  स्थिती 'M':
	    जर (म_भेद(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } अन्यथा {
		  vpip_add_module_path(optarg);
	    }
	    थांबा;
	  स्थिती 'm':
	    module_tab[module_cnt++] = optarg;
	    थांबा;
	  स्थिती 'n':
	    vvp_set_stop_is_finish(सत्य, 0);
	    थांबा;
          स्थिती 'N':
            vvp_set_stop_is_finish(सत्य, 1);
            थांबा;
	  स्थिती 'q':
	    vvp_set_quiet_flag(सत्य);
	    थांबा;
	  स्थिती 's':
	    schedule_stop(0);
	    थांबा;
	  स्थिती 'v':
	    vvp_set_verbose_flag(सत्य);
	    थांबा;
	  स्थिती 'V':
	    version_flag = सत्य;
	    थांबा;
	  पूर्वनिर्धारित:
	    flag_errors += 1;
      }

      जर (flag_errors)
	    परत flag_errors;

      जर (version_flag) {
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
	    परत 0;
      }

      जर (optind == argc) {
	    ख_लिखो(मानक_त्रुटि, "%s: no input file.\n", argv[0]);
	    परत -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      साठी (अचिन्हित idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      परत vvp_run(argv[optind]);
}
