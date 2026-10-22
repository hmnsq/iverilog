/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
 *
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

# include  "config.h"
# include  "vpi_priv.h"
# include  "ivl_dlfcn.h"
# include  "vvp_cleanup.h"
# include  <cstdio>
# include  <cstring>
# include  <sys/types.h>
# include  <sys/stat.h>
# include  "ivl_alloc.h"

స్థిర ivl_dll_t*dll_list = 0;
స్థిర గుర్తులేని dll_list_cnt = 0;

#ఒకవేళ defined(__MINGW32__) || defined (__CYGWIN__)
రకనిర్వచనం PLI_UINT32 (*vpip_సమితి_callback_t)(vpip_routines_s*, PLI_UINT32);
#అయితే_ముగింపు
రకనిర్వచనం శూన్య (*vlog_startup_routines_t)(శూన్య);

# define VPIP_MODULE_మార్గం_MAX 64

స్థిర స్థిరరాశి అక్షరం* vpip_module_మార్గం[VPIP_MODULE_మార్గం_MAX] = {0};

స్థిర గుర్తులేని vpip_module_మార్గం_cnt = 0;

స్థిర తార్కిక disable_default_paths = అబద్ధం;

శూన్య vpip_clear_module_paths()
{
      vpip_module_మార్గం_cnt = 0;
      vpip_module_మార్గం[0] = 0;
      disable_default_paths = నిజం;
}

శూన్య vpip_కూడు_module_మార్గం(స్థిరరాశి అక్షరం*మార్గం)
{
      ఒకవేళ (vpip_module_మార్గం_cnt >= VPIP_MODULE_మార్గం_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    నిష్క్రమించు(1);
      }
      vpip_module_మార్గం[vpip_module_మార్గం_cnt++] = మార్గం;
}

శూన్య vpip_కూడు_env_and_default_module_paths()
{
      ఒకవేళ (disable_default_paths)
	    తిరిగి;

#నిర్వచితమైతే __MINGW32__
      స్థిరరాశి అక్షరం మార్గం_sep = ';';
#లేకపోతే
      స్థిరరాశి అక్షరం మార్గం_sep = ':';
#అయితే_ముగింపు

      ఒకవేళ (అక్షరం *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            అక్షరం *ptr = var;
            స్థిరరాశి అక్షరం *చివర = var+అక్షరశ్రేణి_పొడవు(var);
            పూర్ణాంకం len = 0;
            అయితే (ptr <= చివర) {
                  ఒకవేళ (*ptr == 0 || *ptr == మార్గం_sep) {
                        ఒకవేళ (len > 0) {
                              vpip_కూడు_module_మార్గం(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } లేకపోతే {
                        len++;
                  }
                  ptr++;
            }
      }

#నిర్వచితమైతే __MINGW32__
	/* Calculate the module path from the path to the command.
	   This is necessary because of the installation process on
	   Windows. Mostly, it is those darn drive letters, but oh
	   well. We know the command path is formed like this:

		D:\iverilog\bin\iverilog.exe

	   The IVL_ROOT in a Windows installation is the path:

		D:\iverilog\lib\ivl$(suffix)

	   so we chop the file name and the last directory by
	   turning the last two \ characters to null. Then we append
	   the lib\ivl$(suffix) to finish. */
      అక్షరం *s;
      అక్షరం basepath[4096], tmp[4096];
      GetModuleFileName(ఖాళీ, tmp, పరిమాణం tmp);
	/* Convert to a short name to remove any embedded spaces. */
      Get_Short_మార్గం_Name(tmp, basepath, పరిమాణం basepath);
      s = strrchr(basepath, '\\');
      ఒకవేళ (s) *s = 0;
      లేకపోతే {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    నిష్క్రమించు(1);
      }
      s = strrchr(basepath, '\\');
      ఒకవేళ (s) *s = 0;
      లేకపోతే {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    నిష్క్రమించు(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_కూడు_module_మార్గం(strdup(basepath));
#లేకపోతే
#నిర్వచితమైతే MODULE_DIR1
      vpip_కూడు_module_మార్గం(MODULE_DIR1);
#అయితే_ముగింపు
#అయితే_ముగింపు
#నిర్వచితమైతే MODULE_DIR2
      vpip_కూడు_module_మార్గం(MODULE_DIR2);
#అయితే_ముగింపు
}

శూన్య ఎక్కించు_module_delete(శూన్య)
{
      కొరకు (గుర్తులేని idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      విడుదల(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

శూన్య vpip_ఎక్కించు_module(స్థిరరాశి అక్షరం*name)
{
      నిర్మాణం stat sb;
      పూర్ణాంకం rc;
      తార్కిక export_flag = అబద్ధం;
      అక్షరం buf[4096];

#నిర్వచితమైతే __MINGW32__
      స్థిరరాశి అక్షరం sep = '\\';
#లేకపోతే
      స్థిరరాశి అక్షరం sep = '/';
#అయితే_ముగింపు

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#నిర్వచితమైతే __MINGW32__
      ఒకవేళ (strchr(name, '\\') || strchr(name, '/')) {
#లేకపోతే
      ఒకవేళ (strchr(name, sep)) {
#అయితే_ముగింపు
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = అబద్ధం;
	    rc = stat(name, &sb);

	    ఒకవేళ (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = అబద్ధం;
		  snprintf(buf, పరిమాణం(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  ఒకవేళ (rc != 0) {
			export_flag = నిజం;
			snprintf(buf, పరిమాణం(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  ఒకవేళ (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			తిరిగి;
		  }
	    } లేకపోతే {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } లేకపోతే {
	    rc = -1;
	    కొరకు (గుర్తులేని idx = 0
		       ; (rc != 0) && (idx < vpip_module_మార్గం_cnt)
		       ;  idx += 1) {
		  export_flag = అబద్ధం;
		  snprintf(buf, పరిమాణం(buf), "%s%c%s.vpi",
			   vpip_module_మార్గం[idx], sep, name);
		  rc = stat(buf,&sb);

		  ఒకవేళ (rc != 0) {
			export_flag = నిజం;
			snprintf(buf, పరిమాణం(buf), "%s%c%s.vpl",
				 vpip_module_మార్గం[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    ఒకవేళ (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  తిరిగి;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      ఒకవేళ(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	తిరిగి;
      }

#ఒకవేళ defined(__MINGW32__) || defined (__CYGWIN__)
      శూన్య*function = ivl_dlsym(dll, "vpip_set_callback");
      ఒకవేళ (function) {
            vpip_సమితి_callback_t సమితి_callback = reinterpret_cast<vpip_సమితి_callback_t>(function);
            ఒకవేళ (!సమితి_callback(&vpi_routines, vpip_routines_సంస్కరణ)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          తిరిగి;
            }
      }
#అయితే_ముగింపు

      శూన్య*పట్టిక = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      ఒకవేళ (పట్టిక == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    తిరిగి;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*పరిమాణం(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_బహుళకం_flag = VPI_బహుళకం_రిజిస్టరు;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)పట్టిక;
      కొరకు (గుర్తులేని tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
}
