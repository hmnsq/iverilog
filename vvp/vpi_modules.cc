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

स्थिर ivl_dll_t*dll_list = 0;
स्थिर अचिन्हित dll_list_cnt = 0;

#यदि defined(__MINGW32__) || defined (__CYGWIN__)
प्रकारनिर्देश PLI_UINT32 (*vpip_set_callback_t)(vpip_routines_s*, PLI_UINT32);
#endif
प्रकारनिर्देश शून्य (*vlog_startup_routines_t)(शून्य);

# define VPIP_MODULE_PATH_MAX 64

स्थिर ध्रुव वर्ण* vpip_module_path[VPIP_MODULE_PATH_MAX] = {0};

स्थिर अचिन्हित vpip_module_path_cnt = 0;

स्थिर तार्किक disable_default_paths = असत्य;

शून्य vpip_clear_module_paths()
{
      vpip_module_path_cnt = 0;
      vpip_module_path[0] = 0;
      disable_default_paths = सत्य;
}

शून्य vpip_add_module_path(ध्रुव वर्ण*path)
{
      यदि (vpip_module_path_cnt >= VPIP_MODULE_PATH_MAX) {
	    ख_लिखो(मानक_त्रुटि, "Too many module paths specified\n");
	    निकास(1);
      }
      vpip_module_path[vpip_module_path_cnt++] = path;
}

शून्य vpip_add_env_and_default_module_paths()
{
      यदि (disable_default_paths)
	    प्रत्यागम;

#ifdef __MINGW32__
      ध्रुव वर्ण path_sep = ';';
#अन्यथा
      ध्रुव वर्ण path_sep = ':';
#endif

      यदि (वर्ण *var = ::दो_पर्या("IVERILOG_VPI_MODULE_PATH")) {
            वर्ण *ptr = var;
            ध्रुव वर्ण *end = var+म_माप(var);
            पूर्णाङ्क len = 0;
            यावत् (ptr <= end) {
                  यदि (*ptr == 0 || *ptr == path_sep) {
                        यदि (len > 0) {
                              vpip_add_module_path(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } अन्यथा {
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
      वर्ण *s;
      वर्ण basepath[4096], tmp[4096];
      GetModuleFileName(रिक्त, tmp, परिमाण tmp);
	/* Convert to a short name to remove any embedded spaces. */
      GetShortPathName(tmp, basepath, परिमाण basepath);
      s = म_खोजप(basepath, '\\');
      यदि (s) *s = 0;
      अन्यथा {
	    ख_लिखो(मानक_त्रुटि, "%s: Missing first \\ in exe path!\n", tmp);
	    निकास(1);
      }
      s = म_खोजप(basepath, '\\');
      यदि (s) *s = 0;
      अन्यथा {
	    ख_लिखो(मानक_त्रुटि, "%s: Missing second \\ in exe path!\n", tmp);
	    निकास(1);
      }
      म_जोड़ो(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_add_module_path(strdup(basepath));
#अन्यथा
#ifdef MODULE_DIR1
      vpip_add_module_path(MODULE_DIR1);
#endif
#endif
#ifdef MODULE_DIR2
      vpip_add_module_path(MODULE_DIR2);
#endif
}

शून्य load_module_delete(शून्य)
{
      प्रति (अचिन्हित idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      मुक्त(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

शून्य vpip_load_module(ध्रुव वर्ण*name)
{
      संरचना stat sb;
      पूर्णाङ्क rc;
      तार्किक export_flag = असत्य;
      वर्ण buf[4096];

#ifdef __MINGW32__
      ध्रुव वर्ण sep = '\\';
#अन्यथा
      ध्रुव वर्ण sep = '/';
#endif

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#ifdef __MINGW32__
      यदि (म_अक्षर(name, '\\') || म_अक्षर(name, '/')) {
#अन्यथा
      यदि (म_अक्षर(name, sep)) {
#endif
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = असत्य;
	    rc = stat(name, &sb);

	    यदि (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = असत्य;
		  snprintf(buf, परिमाण(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  यदि (rc != 0) {
			export_flag = सत्य;
			snprintf(buf, परिमाण(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  यदि (rc != 0) {
			ख_लिखो(मानक_त्रुटि, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			प्रत्यागम;
		  }
	    } अन्यथा {
	      म_नकल(buf,name);   /* yes copy the name into the buffer */
	    }

      } अन्यथा {
	    rc = -1;
	    प्रति (अचिन्हित idx = 0
		       ; (rc != 0) && (idx < vpip_module_path_cnt)
		       ;  idx += 1) {
		  export_flag = असत्य;
		  snprintf(buf, परिमाण(buf), "%s%c%s.vpi",
			   vpip_module_path[idx], sep, name);
		  rc = stat(buf,&sb);

		  यदि (rc != 0) {
			export_flag = सत्य;
			snprintf(buf, परिमाण(buf), "%s%c%s.vpl",
				 vpip_module_path[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    यदि (rc != 0) {
		  ख_लिखो(मानक_त्रुटि, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  प्रत्यागम;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      यदि(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	ख_लिखो(मानक_त्रुटि,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	प्रत्यागम;
      }

#यदि defined(__MINGW32__) || defined (__CYGWIN__)
      शून्य*function = ivl_dlsym(dll, "vpip_set_callback");
      यदि (function) {
            vpip_set_callback_t set_callback = reinterpret_cast<vpip_set_callback_t>(function);
            यदि (!set_callback(&vpi_routines, vpip_routines_version)) {
	          ख_लिखो(मानक_त्रुटि, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          प्रत्यागम;
            }
      }
#endif

      शून्य*table = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      यदि (table == 0) {
	    ख_लिखो(मानक_त्रुटि, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    प्रत्यागम;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (पुनः_स्मृति(dll_list, dll_list_cnt*परिमाण(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_mode_flag = VPI_MODE_REGISTER;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)table;
      प्रति (अचिन्हित tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_mode_flag = VPI_MODE_NONE;
}
