స్థిరరాశి అక్షరం COPYRIGHT[] =
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

#ఒకవేళ defined(HAVE_GETOPT_H)
# include  <getopt.h>
#అయితే_ముగింపు

#ఒకవేళ defined(__MINGW32__)
# include  <windows.h>
#అయితే_ముగింపు

#అన్తర్భావ "libvvp.h"

ఉపయోగించి నామాకాశ std;

#ఒకవేళ defined(__MINGW32__) && !defined(HAVE_GETOPT_H)
బాహ్య "C" పూర్ణాంకం getopt(పూర్ణాంకం argc, అక్షరం*argv[], స్థిరరాశి అక్షరం*fmt);
బాహ్య "C" పూర్ణాంకం optind;
బాహ్య "C" స్థిరరాశి అక్షరం*optarg;
#అయితే_ముగింపు

తార్కిక సంస్కరణ_flag = అబద్ధం;

గుర్తులేని module_cnt = 0;
స్థిరరాశి అక్షరం*module_tab[64];

పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం*argv[])
{
      పూర్ణాంకం opt;
      గుర్తులేని flag_errors = 0;
      స్థిరరాశి అక్షరం *logfile_name = 0x0;

      అయితే ((opt = getopt(argc, argv, "+hil:M:m:nNqsvV")) != EOF) ఎంపిక (opt) {
         స్థితి 'h':
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
           నిష్క్రమించు(0);
	  స్థితి 'i':
	    setvbuf(stdout, 0, _IONBF, 0);
	    ఆపు;
	  స్థితి 'l':
	    logfile_name = optarg;
	    ఆపు;
	  స్థితి 'M':
	    ఒకవేళ (strcmp(optarg,"-") == 0) {
		  vpip_clear_module_paths();
	    } లేకపోతే {
		  vpip_కూడు_module_మార్గం(optarg);
	    }
	    ఆపు;
	  స్థితి 'm':
	    module_tab[module_cnt++] = optarg;
	    ఆపు;
	  స్థితి 'n':
	    vvp_సమితి_stop_is_ముగింపు(నిజం, 0);
	    ఆపు;
          స్థితి 'N':
            vvp_సమితి_stop_is_ముగింపు(నిజం, 1);
            ఆపు;
	  స్థితి 'q':
	    vvp_సమితి_quiet_flag(నిజం);
	    ఆపు;
	  స్థితి 's':
	    కాలపట్టిక_stop(0);
	    ఆపు;
	  స్థితి 'v':
	    vvp_సమితి_verbose_flag(నిజం);
	    ఆపు;
	  స్థితి 'V':
	    సంస్కరణ_flag = నిజం;
	    ఆపు;
	  అప్రమేయ:
	    flag_errors += 1;
      }

      ఒకవేళ (flag_errors)
	    తిరిగి flag_errors;

      ఒకవేళ (సంస్కరణ_flag) {
	    fprintf(stderr, "Icarus Verilog runtime version " సంస్కరణ " ("
	                    సంస్కరణ_ట్యాగ్ ")\n\n");
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
	    తిరిగి 0;
      }

      ఒకవేళ (optind == argc) {
	    fprintf(stderr, "%s: no input file.\n", argv[0]);
	    తిరిగి -1;
      }

      vvp_init(logfile_name, argc - optind, argv + optind);

      కొరకు (గుర్తులేని idx = 0 ;  idx < module_cnt ;  idx += 1)
	    vpip_ఎక్కించు_module(module_tab[idx]);

      తిరిగి vvp_నిరంతరం(argv[optind]);
}
