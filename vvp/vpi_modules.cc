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

ಸ್ಥಿರ ivl_dll_t*dll_list = 0;
ಸ್ಥಿರ unsigned dll_list_cnt = 0;

#ಯದಿ defined(__MINGW32__) || defined (__CYGWIN__)
ಪ್ರಕಾರನಿರ್ದೇಶ PLI_UINT32 (*vpip_set_callback_t)(vpip_routines_s*, PLI_UINT32);
#endif
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*vlog_startup_routines_t)(ಶೂನ್ಯ);

# define VPIP_MODULE_PATH_MAX 64

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* vpip_module_path[VPIP_MODULE_PATH_MAX] = {0};

ಸ್ಥಿರ unsigned vpip_module_path_cnt = 0;

ಸ್ಥಿರ ತಾರ್ಕಿಕ disable_default_paths = ಅಸತ್ಯ;

ಶೂನ್ಯ vpip_clear_module_paths()
{
      vpip_module_path_cnt = 0;
      vpip_module_path[0] = 0;
      disable_default_paths = ಸತ್ಯ;
}

ಶೂನ್ಯ vpip_add_module_path(ಧ್ರುವ ವರ್ಣ*path)
{
      ಯದಿ (vpip_module_path_cnt >= VPIP_MODULE_PATH_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    exit(1);
      }
      vpip_module_path[vpip_module_path_cnt++] = path;
}

ಶೂನ್ಯ vpip_add_env_and_default_module_paths()
{
      ಯದಿ (disable_default_paths)
	    ಪ್ರತ್ಯಾಗಮ;

#ifdef __MINGW32__
      ಧ್ರುವ ವರ್ಣ path_sep = ';';
#ಅನ್ಯಥಾ
      ಧ್ರುವ ವರ್ಣ path_sep = ':';
#endif

      ಯದಿ (ವರ್ಣ *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            ವರ್ಣ *ptr = var;
            ಧ್ರುವ ವರ್ಣ *end = var+strlen(var);
            ಪೂರ್ಣಾಙ್ಕ len = 0;
            ಯಾವತ್ (ptr <= end) {
                  ಯದಿ (*ptr == 0 || *ptr == path_sep) {
                        ಯದಿ (len > 0) {
                              vpip_add_module_path(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } ಅನ್ಯಥಾ {
                        len++;
                  }
                  ptr++;
            }
      }

#ifdef __MINGW32__
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
      ವರ್ಣ *s;
      ವರ್ಣ basepath[4096], tmp[4096];
      GetModuleFileName(ರಿಕ್ತ, tmp, ಪರಿಮಾಣ tmp);
	/* Convert to a short name to remove any embedded spaces. */
      GetShortPathName(tmp, basepath, ಪರಿಮಾಣ basepath);
      s = strrchr(basepath, '\\');
      ಯದಿ (s) *s = 0;
      ಅನ್ಯಥಾ {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    exit(1);
      }
      s = strrchr(basepath, '\\');
      ಯದಿ (s) *s = 0;
      ಅನ್ಯಥಾ {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    exit(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_add_module_path(strdup(basepath));
#ಅನ್ಯಥಾ
#ifdef MODULE_DIR1
      vpip_add_module_path(MODULE_DIR1);
#endif
#endif
#ifdef MODULE_DIR2
      vpip_add_module_path(MODULE_DIR2);
#endif
}

ಶೂನ್ಯ load_module_delete(ಶೂನ್ಯ)
{
      ಪ್ರತಿ (unsigned idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      ಮುಕ್ತ(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

ಶೂನ್ಯ vpip_load_module(ಧ್ರುವ ವರ್ಣ*name)
{
      ಸಂರಚನಾ stat sb;
      ಪೂರ್ಣಾಙ್ಕ rc;
      ತಾರ್ಕಿಕ export_flag = ಅಸತ್ಯ;
      ವರ್ಣ buf[4096];

#ifdef __MINGW32__
      ಧ್ರುವ ವರ್ಣ sep = '\\';
#ಅನ್ಯಥಾ
      ಧ್ರುವ ವರ್ಣ sep = '/';
#endif

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#ifdef __MINGW32__
      ಯದಿ (strchr(name, '\\') || strchr(name, '/')) {
#ಅನ್ಯಥಾ
      ಯದಿ (strchr(name, sep)) {
#endif
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = ಅಸತ್ಯ;
	    rc = stat(name, &sb);

	    ಯದಿ (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = ಅಸತ್ಯ;
		  snprintf(buf, ಪರಿಮಾಣ(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  ಯದಿ (rc != 0) {
			export_flag = ಸತ್ಯ;
			snprintf(buf, ಪರಿಮಾಣ(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  ಯದಿ (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			ಪ್ರತ್ಯಾಗಮ;
		  }
	    } ಅನ್ಯಥಾ {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } ಅನ್ಯಥಾ {
	    rc = -1;
	    ಪ್ರತಿ (unsigned idx = 0
		       ; (rc != 0) && (idx < vpip_module_path_cnt)
		       ;  idx += 1) {
		  export_flag = ಅಸತ್ಯ;
		  snprintf(buf, ಪರಿಮಾಣ(buf), "%s%c%s.vpi",
			   vpip_module_path[idx], sep, name);
		  rc = stat(buf,&sb);

		  ಯದಿ (rc != 0) {
			export_flag = ಸತ್ಯ;
			snprintf(buf, ಪರಿಮಾಣ(buf), "%s%c%s.vpl",
				 vpip_module_path[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    ಯದಿ (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  ಪ್ರತ್ಯಾಗಮ;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      ಯದಿ(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	ಪ್ರತ್ಯಾಗಮ;
      }

#ಯದಿ defined(__MINGW32__) || defined (__CYGWIN__)
      ಶೂನ್ಯ*function = ivl_dlsym(dll, "vpip_set_callback");
      ಯದಿ (function) {
            vpip_set_callback_t set_callback = reinterpret_cast<vpip_set_callback_t>(function);
            ಯದಿ (!set_callback(&vpi_routines, vpip_routines_version)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          ಪ್ರತ್ಯಾಗಮ;
            }
      }
#endif

      ಶೂನ್ಯ*table = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      ಯದಿ (table == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*ಪರಿಮಾಣ(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_mode_flag = VPI_MODE_REGISTER;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)table;
      ಪ್ರತಿ (unsigned tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_mode_flag = VPI_MODE_NONE;
}
