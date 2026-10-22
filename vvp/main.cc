स्थिर अक्षर COPYRIGHT[] =
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

#अगर defined(HAVE_GETOPT_H)
# include  <getopt.h>
#पूर्ण_अगर

#अगर defined(__MINGW32__)
# include  <windows.h>
#पूर्ण_अगर

#समावेश "libvvp.h"

using namespace std;

#अगर defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
बाह्य "C" पूर्णांक getopt(पूर्णांक argc, अक्षर*argv[], स्थिर अक्षर*fmt);
बाह्य "C" पूर्णांक optind;
बाह्य "C" स्थिर अक्षर*optarg;
#पूर्ण_अगर

bool version_flag = false;

अचिन्हित module_cnt = 0;
स्थिर अक्षर*module_tab[64];

पूर्णांक मुख्य(पूर्णांक argc, अक्षर*argv[])
{
      पूर्णांक opt;
      अचिन्हित flag_errors = 0;
      स्थिर अक्षर *logfile_name = 0x0;

      जबतक ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != खातापूर्ण) चयन (opt) {
         हाल 'h':
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
	  हाल 'i':
	    रखो_भबफ(मानक_निकास, 0, _IONBF, 0);
	    अवरोध;
	  हाल 'l':
	    logfile_name = optarg;
	    अवरोध;
	  हाल 'M':
	    अगर (म_भेद(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } अन्यथा {
		  vpip_add_module_path(optarg);
	    }
	    अवरोध;
	  हाल 'm':
	    module_tab[module_cnt++] = optarg;
	    अवरोध;
	  हाल 'n':
	    vvp_set_stop_is_finish(true, 0);
	    अवरोध;
          हाल 'N':
            vvp_set_stop_is_finish(true, 1);
            अवरोध;
	  हाल 'q':
	    vvp_set_quiet_flag(true);
	    अवरोध;
	  हाल 's':
	    schedule_stop(0);
	    अवरोध;
	  हाल 'v':
	    vvp_set_verbose_flag(true);
	    अवरोध;
	  हाल 'V':
	    version_flag = true;
	    अवरोध;
	  शेष:
	    flag_errors += 1;
      }

      अगर (flag_errors)
	    वापस flag_errors;

      अगर (version_flag) {
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
	    वापस 0;
      }

      अगर (optind == argc) {
	    ख_लिखो(मानक_त्रुटि, "%s: no input file.\n", argv[0]);
	    वापस -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      क्रम (अचिन्हित idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_load_module(module_tab[idx]);

      वापस vvp_run(argv[optind]);
}
