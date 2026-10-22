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
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ dll_list_cnt = 0;

#ಆದರೆ defined(__MINGW32__) || defined (__CYGWIN__)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ PLI_UINT32 (*vpip_ಗಣ_callback_t)(vpip_routines_s*, PLI_UINT32);
#ಆದರೆ_ಅಂತ್ಯ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*vlog_startup_routines_t)(ಶೂನ್ಯ);

# define VPIP_MODULE_ಮಾರ್ಗ_MAX 64

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpip_module_ಮಾರ್ಗ[VPIP_MODULE_ಮಾರ್ಗ_MAX] = {0};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ vpip_module_ಮಾರ್ಗ_cnt = 0;

ಸ್ಥಿರ ತಾರ್ಕಿಕ disable_default_paths = ಸುಳ್ಳು;

ಶೂನ್ಯ vpip_clear_module_paths()
{
      vpip_module_ಮಾರ್ಗ_cnt = 0;
      vpip_module_ಮಾರ್ಗ[0] = 0;
      disable_default_paths = ನಿಜ;
}

ಶೂನ್ಯ vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಮಾರ್ಗ)
{
      ಆದರೆ (vpip_module_ಮಾರ್ಗ_cnt >= VPIP_MODULE_ಮಾರ್ಗ_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    ನಿರ್ಗಮಿಸು(1);
      }
      vpip_module_ಮಾರ್ಗ[vpip_module_ಮಾರ್ಗ_cnt++] = ಮಾರ್ಗ;
}

ಶೂನ್ಯ vpip_ಕೂಡಿಸು_env_and_default_module_paths()
{
      ಆದರೆ (disable_default_paths)
	    ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಮಾರ್ಗ_sep = ';';
#ಇಲ್ಲದಿದ್ದರೆ
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಮಾರ್ಗ_sep = ':';
#ಆದರೆ_ಅಂತ್ಯ

      ಆದರೆ (ಅಕ್ಷರ *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            ಅಕ್ಷರ *ptr = var;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = var+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(var);
            ಪೂರ್ಣಾಂಕ len = 0;
            ಆಗ (ptr <= ಕೊನೆ) {
                  ಆದರೆ (*ptr == 0 || *ptr == ಮಾರ್ಗ_sep) {
                        ಆದರೆ (len > 0) {
                              vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } ಇಲ್ಲದಿದ್ದರೆ {
                        len++;
                  }
                  ptr++;
            }
      }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
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
      ಅಕ್ಷರ *s;
      ಅಕ್ಷರ basepath[4096], tmp[4096];
      GetModuleFileName(ಖಾಲಿ, tmp, ಗಾತ್ರ tmp);
	/* Convert to a short name to remove any embedded spaces. */
      Get_Short_ಮಾರ್ಗ_Name(tmp, basepath, ಗಾತ್ರ basepath);
      s = strrchr(basepath, '\\');
      ಆದರೆ (s) *s = 0;
      ಇಲ್ಲದಿದ್ದರೆ {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    ನಿರ್ಗಮಿಸು(1);
      }
      s = strrchr(basepath, '\\');
      ಆದರೆ (s) *s = 0;
      ಇಲ್ಲದಿದ್ದರೆ {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    ನಿರ್ಗಮಿಸು(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(strdup(basepath));
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MODULE_DIR1
      vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(MODULE_DIR1);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MODULE_DIR2
      vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(MODULE_DIR2);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ ತುಂಬು_module_delete(ಶೂನ್ಯ)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      ಬಿಡುಗಡೆ(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

ಶೂನ್ಯ vpip_ತುಂಬು_module(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name)
{
      ರಚನೆ stat sb;
      ಪೂರ್ಣಾಂಕ rc;
      ತಾರ್ಕಿಕ export_flag = ಸುಳ್ಳು;
      ಅಕ್ಷರ buf[4096];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ sep = '\\';
#ಇಲ್ಲದಿದ್ದರೆ
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ sep = '/';
#ಆದರೆ_ಅಂತ್ಯ

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
      ಆದರೆ (strchr(name, '\\') || strchr(name, '/')) {
#ಇಲ್ಲದಿದ್ದರೆ
      ಆದರೆ (strchr(name, sep)) {
#ಆದರೆ_ಅಂತ್ಯ
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = ಸುಳ್ಳು;
	    rc = stat(name, &sb);

	    ಆದರೆ (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = ಸುಳ್ಳು;
		  snprintf(buf, ಗಾತ್ರ(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  ಆದರೆ (rc != 0) {
			export_flag = ನಿಜ;
			snprintf(buf, ಗಾತ್ರ(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  ಆದರೆ (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			ಹಿಂತಿರುಗಿ;
		  }
	    } ಇಲ್ಲದಿದ್ದರೆ {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {
	    rc = -1;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0
		       ; (rc != 0) && (idx < vpip_module_ಮಾರ್ಗ_cnt)
		       ;  idx += 1) {
		  export_flag = ಸುಳ್ಳು;
		  snprintf(buf, ಗಾತ್ರ(buf), "%s%c%s.vpi",
			   vpip_module_ಮಾರ್ಗ[idx], sep, name);
		  rc = stat(buf,&sb);

		  ಆದರೆ (rc != 0) {
			export_flag = ನಿಜ;
			snprintf(buf, ಗಾತ್ರ(buf), "%s%c%s.vpl",
				 vpip_module_ಮಾರ್ಗ[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    ಆದರೆ (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  ಹಿಂತಿರುಗಿ;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      ಆದರೆ(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	ಹಿಂತಿರುಗಿ;
      }

#ಆದರೆ defined(__MINGW32__) || defined (__CYGWIN__)
      ಶೂನ್ಯ*function = ivl_dlsym(dll, "vpip_set_callback");
      ಆದರೆ (function) {
            vpip_ಗಣ_callback_t ಗಣ_callback = reinterpret_cast<vpip_ಗಣ_callback_t>(function);
            ಆದರೆ (!ಗಣ_callback(&vpi_routines, vpip_routines_ಆವೃತ್ತಿ)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          ಹಿಂತಿರುಗಿ;
            }
      }
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ*ಕೋಷ್ಟಕ = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      ಆದರೆ (ಕೋಷ್ಟಕ == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    ಹಿಂತಿರುಗಿ;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*ಗಾತ್ರ(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)ಕೋಷ್ಟಕ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
}
