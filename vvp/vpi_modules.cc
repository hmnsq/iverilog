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

സ്ഥിതം ivl_dll_t*dll_list = 0;
സ്ഥിതം unsigned dll_list_cnt = 0;

#എങ്കിൽ defined(__MINGW32__) || defined (__CYGWIN__)
തരനിർവചനം PLI_UINT32 (*vpip_set_callback_t)(vpip_routines_s*, PLI_UINT32);
#endif
തരനിർവചനം ശൂന്യം (*vlog_startup_routines_t)(ശൂന്യം);

# define VPIP_MODULE_PATH_MAX 64

സ്ഥിതം സ്ഥിരം അക്ഷരം* vpip_module_path[VPIP_MODULE_PATH_MAX] = {0};

സ്ഥിതം unsigned vpip_module_path_cnt = 0;

സ്ഥിതം തർക്കം disable_default_paths = അസത്യം;

ശൂന്യം vpip_clear_module_paths()
{
      vpip_module_path_cnt = 0;
      vpip_module_path[0] = 0;
      disable_default_paths = സത്യം;
}

ശൂന്യം vpip_add_module_path(സ്ഥിരം അക്ഷരം*path)
{
      എങ്കിൽ (vpip_module_path_cnt >= VPIP_MODULE_PATH_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    exit(1);
      }
      vpip_module_path[vpip_module_path_cnt++] = path;
}

ശൂന്യം vpip_add_env_and_default_module_paths()
{
      എങ്കിൽ (disable_default_paths)
	    തിരികെ;

#ifdef __MINGW32__
      സ്ഥിരം അക്ഷരം path_sep = ';';
#അല്ലെങ്കിൽ
      സ്ഥിരം അക്ഷരം path_sep = ':';
#endif

      എങ്കിൽ (അക്ഷരം *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            അക്ഷരം *ptr = var;
            സ്ഥിരം അക്ഷരം *end = var+strlen(var);
            പൂർണ്ണസംഖ്യ len = 0;
            ആകുമ്പോൾ (ptr <= end) {
                  എങ്കിൽ (*ptr == 0 || *ptr == path_sep) {
                        എങ്കിൽ (len > 0) {
                              vpip_add_module_path(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } അല്ലെങ്കിൽ {
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
      അക്ഷരം *s;
      അക്ഷരം basepath[4096], tmp[4096];
      GetModuleFileName(ശൂന്യ, tmp, വലുപ്പം tmp);
	/* Convert to a short name to remove any embedded spaces. */
      GetShortPathName(tmp, basepath, വലുപ്പം basepath);
      s = strrchr(basepath, '\\');
      എങ്കിൽ (s) *s = 0;
      അല്ലെങ്കിൽ {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    exit(1);
      }
      s = strrchr(basepath, '\\');
      എങ്കിൽ (s) *s = 0;
      അല്ലെങ്കിൽ {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    exit(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_add_module_path(strdup(basepath));
#അല്ലെങ്കിൽ
#ifdef MODULE_DIR1
      vpip_add_module_path(MODULE_DIR1);
#endif
#endif
#ifdef MODULE_DIR2
      vpip_add_module_path(MODULE_DIR2);
#endif
}

ശൂന്യം load_module_delete(ശൂന്യം)
{
      വേണ്ടി (unsigned idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      സ്വതന്ത്രം(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

ശൂന്യം vpip_load_module(സ്ഥിരം അക്ഷരം*name)
{
      ഘടന stat sb;
      പൂർണ്ണസംഖ്യ rc;
      തർക്കം export_flag = അസത്യം;
      അക്ഷരം buf[4096];

#ifdef __MINGW32__
      സ്ഥിരം അക്ഷരം sep = '\\';
#അല്ലെങ്കിൽ
      സ്ഥിരം അക്ഷരം sep = '/';
#endif

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#ifdef __MINGW32__
      എങ്കിൽ (strchr(name, '\\') || strchr(name, '/')) {
#അല്ലെങ്കിൽ
      എങ്കിൽ (strchr(name, sep)) {
#endif
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = അസത്യം;
	    rc = stat(name, &sb);

	    എങ്കിൽ (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = അസത്യം;
		  snprintf(buf, വലുപ്പം(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  എങ്കിൽ (rc != 0) {
			export_flag = സത്യം;
			snprintf(buf, വലുപ്പം(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  എങ്കിൽ (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			തിരികെ;
		  }
	    } അല്ലെങ്കിൽ {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } അല്ലെങ്കിൽ {
	    rc = -1;
	    വേണ്ടി (unsigned idx = 0
		       ; (rc != 0) && (idx < vpip_module_path_cnt)
		       ;  idx += 1) {
		  export_flag = അസത്യം;
		  snprintf(buf, വലുപ്പം(buf), "%s%c%s.vpi",
			   vpip_module_path[idx], sep, name);
		  rc = stat(buf,&sb);

		  എങ്കിൽ (rc != 0) {
			export_flag = സത്യം;
			snprintf(buf, വലുപ്പം(buf), "%s%c%s.vpl",
				 vpip_module_path[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    എങ്കിൽ (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  തിരികെ;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      എങ്കിൽ(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	തിരികെ;
      }

#എങ്കിൽ defined(__MINGW32__) || defined (__CYGWIN__)
      ശൂന്യം*function = ivl_dlsym(dll, "vpip_set_callback");
      എങ്കിൽ (function) {
            vpip_set_callback_t set_callback = reinterpret_cast<vpip_set_callback_t>(function);
            എങ്കിൽ (!set_callback(&vpi_routines, vpip_routines_version)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          തിരികെ;
            }
      }
#endif

      ശൂന്യം*table = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      എങ്കിൽ (table == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    തിരികെ;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*വലുപ്പം(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_mode_flag = VPI_MODE_REGISTER;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)table;
      വേണ്ടി (unsigned tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_mode_flag = VPI_MODE_NONE;
}
