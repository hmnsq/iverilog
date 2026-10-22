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

நிலையான ivl_dll_t*dll_list = 0;
நிலையான unsigned dll_list_cnt = 0;

#எனில் defined(__MINGW32__) || defined (__CYGWIN__)
வகைவரை PLI_UINT32 (*vpip_set_callback_t)(vpip_routines_s*, PLI_UINT32);
#endif
வகைவரை வெறுமை (*vlog_startup_routines_t)(வெறுமை);

# define VPIP_MODULE_PATH_MAX 64

நிலையான மாறிலி எழுத்து* vpip_module_path[VPIP_MODULE_PATH_MAX] = {0};

நிலையான unsigned vpip_module_path_cnt = 0;

நிலையான தருக்க disable_default_paths = பொய்;

வெறுமை vpip_clear_module_paths()
{
      vpip_module_path_cnt = 0;
      vpip_module_path[0] = 0;
      disable_default_paths = உண்மை;
}

வெறுமை vpip_add_module_path(மாறிலி எழுத்து*path)
{
      எனில் (vpip_module_path_cnt >= VPIP_MODULE_PATH_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    exit(1);
      }
      vpip_module_path[vpip_module_path_cnt++] = path;
}

வெறுமை vpip_add_env_and_default_module_paths()
{
      எனில் (disable_default_paths)
	    திரும்பு;

#ifdef __MINGW32__
      மாறிலி எழுத்து path_sep = ';';
#இல்லையெனில்
      மாறிலி எழுத்து path_sep = ':';
#endif

      எனில் (எழுத்து *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            எழுத்து *ptr = var;
            மாறிலி எழுத்து *end = var+strlen(var);
            முழு len = 0;
            போது (ptr <= end) {
                  எனில் (*ptr == 0 || *ptr == path_sep) {
                        எனில் (len > 0) {
                              vpip_add_module_path(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } இல்லையெனில் {
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
      எழுத்து *s;
      எழுத்து basepath[4096], tmp[4096];
      GetModuleFileName(வெற்று, tmp, அளவு tmp);
	/* Convert to a short name to remove any embedded spaces. */
      GetShortPathName(tmp, basepath, அளவு basepath);
      s = strrchr(basepath, '\\');
      எனில் (s) *s = 0;
      இல்லையெனில் {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    exit(1);
      }
      s = strrchr(basepath, '\\');
      எனில் (s) *s = 0;
      இல்லையெனில் {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    exit(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_add_module_path(strdup(basepath));
#இல்லையெனில்
#ifdef MODULE_DIR1
      vpip_add_module_path(MODULE_DIR1);
#endif
#endif
#ifdef MODULE_DIR2
      vpip_add_module_path(MODULE_DIR2);
#endif
}

வெறுமை load_module_delete(வெறுமை)
{
      க்கு (unsigned idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      விடுவி(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

வெறுமை vpip_load_module(மாறிலி எழுத்து*name)
{
      கட்டமைப்பு stat sb;
      முழு rc;
      தருக்க export_flag = பொய்;
      எழுத்து buf[4096];

#ifdef __MINGW32__
      மாறிலி எழுத்து sep = '\\';
#இல்லையெனில்
      மாறிலி எழுத்து sep = '/';
#endif

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#ifdef __MINGW32__
      எனில் (strchr(name, '\\') || strchr(name, '/')) {
#இல்லையெனில்
      எனில் (strchr(name, sep)) {
#endif
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = பொய்;
	    rc = stat(name, &sb);

	    எனில் (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = பொய்;
		  snprintf(buf, அளவு(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  எனில் (rc != 0) {
			export_flag = உண்மை;
			snprintf(buf, அளவு(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  எனில் (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			திரும்பு;
		  }
	    } இல்லையெனில் {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } இல்லையெனில் {
	    rc = -1;
	    க்கு (unsigned idx = 0
		       ; (rc != 0) && (idx < vpip_module_path_cnt)
		       ;  idx += 1) {
		  export_flag = பொய்;
		  snprintf(buf, அளவு(buf), "%s%c%s.vpi",
			   vpip_module_path[idx], sep, name);
		  rc = stat(buf,&sb);

		  எனில் (rc != 0) {
			export_flag = உண்மை;
			snprintf(buf, அளவு(buf), "%s%c%s.vpl",
				 vpip_module_path[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    எனில் (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  திரும்பு;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      எனில்(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	திரும்பு;
      }

#எனில் defined(__MINGW32__) || defined (__CYGWIN__)
      வெறுமை*function = ivl_dlsym(dll, "vpip_set_callback");
      எனில் (function) {
            vpip_set_callback_t set_callback = reinterpret_cast<vpip_set_callback_t>(function);
            எனில் (!set_callback(&vpi_routines, vpip_routines_version)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          திரும்பு;
            }
      }
#endif

      வெறுமை*table = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      எனில் (table == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    திரும்பு;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*அளவு(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_mode_flag = VPI_MODE_REGISTER;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)table;
      க்கு (unsigned tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_mode_flag = VPI_MODE_NONE;
}
