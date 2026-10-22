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

अटल ivl_dll_t*dll_list = 0;
अटल अचिन्हित dll_list_cnt = 0;

#अगर defined(__MINGW32__) || defined (__CYGWIN__)
प्रकार PLI_UINT32 (*vpip_set_callback_t)(vpip_routines_s*, PLI_UINT32);
#पूर्ण_अगर
प्रकार व्योम (*vlog_startup_routines_t)(व्योम);

# define VPIP_MODULE_PATH_MAX 64

अटल स्थिर अक्षर* vpip_module_path[VPIP_MODULE_PATH_MAX] = {0};

अटल अचिन्हित vpip_module_path_cnt = 0;

अटल bool disable_default_paths = false;

व्योम vpip_clear_module_paths()
{
      vpip_module_path_cnt = 0;
      vpip_module_path[0] = 0;
      disable_default_paths = true;
}

व्योम vpip_add_module_path(स्थिर अक्षर*path)
{
      अगर (vpip_module_path_cnt >= VPIP_MODULE_PATH_MAX) {
	    ख_लिखो(मानक_त्रुटि, "Too many module paths specified\n");
	    निकास(1);
      }
      vpip_module_path[vpip_module_path_cnt++] = path;
}

व्योम vpip_add_env_and_default_module_paths()
{
      अगर (disable_default_paths)
	    वापस;

#अगर_घोषित __MINGW32__
      स्थिर अक्षर path_sep = ';';
#अन्यथा
      स्थिर अक्षर path_sep = ':';
#पूर्ण_अगर

      अगर (अक्षर *var = ::दो_पर्या("IVERILOG_VPI_MODULE_PATH")) {
            अक्षर *ptr = var;
            स्थिर अक्षर *end = var+म_माप(var);
            पूर्णांक len = 0;
            जबतक (ptr <= end) {
                  अगर (*ptr == 0 || *ptr == path_sep) {
                        अगर (len > 0) {
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

#अगर_घोषित __MINGW32__
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
      अक्षर *s;
      अक्षर basepath[4096], tmp[4096];
      GetModuleFileName(शून्य, tmp, माप tmp);
	/* Convert to a short name to remove any embedded spaces. */
      GetShortPathName(tmp, basepath, माप basepath);
      s = म_खोजप(basepath, '\\');
      अगर (s) *s = 0;
      अन्यथा {
	    ख_लिखो(मानक_त्रुटि, "%s: Missing first \\ in exe path!\n", tmp);
	    निकास(1);
      }
      s = म_खोजप(basepath, '\\');
      अगर (s) *s = 0;
      अन्यथा {
	    ख_लिखो(मानक_त्रुटि, "%s: Missing second \\ in exe path!\n", tmp);
	    निकास(1);
      }
      म_जोड़ो(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_add_module_path(strdup(basepath));
#अन्यथा
#अगर_घोषित MODULE_DIR1
      vpip_add_module_path(MODULE_DIR1);
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_घोषित MODULE_DIR2
      vpip_add_module_path(MODULE_DIR2);
#पूर्ण_अगर
}

व्योम load_module_delete(व्योम)
{
      क्रम (अचिन्हित idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      मुक्त(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

व्योम vpip_load_module(स्थिर अक्षर*name)
{
      काष्ठा stat sb;
      पूर्णांक rc;
      bool export_flag = false;
      अक्षर buf[4096];

#अगर_घोषित __MINGW32__
      स्थिर अक्षर sep = '\\';
#अन्यथा
      स्थिर अक्षर sep = '/';
#पूर्ण_अगर

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#अगर_घोषित __MINGW32__
      अगर (म_अक्षर(name, '\\') || म_अक्षर(name, '/')) {
#अन्यथा
      अगर (म_अक्षर(name, sep)) {
#पूर्ण_अगर
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = false;
	    rc = stat(name, &sb);

	    अगर (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = false;
		  snprintf(buf, माप(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  अगर (rc != 0) {
			export_flag = true;
			snprintf(buf, माप(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  अगर (rc != 0) {
			ख_लिखो(मानक_त्रुटि, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			वापस;
		  }
	    } अन्यथा {
	      म_नकल(buf,name);   /* yes copy the name into the buffer */
	    }

      } अन्यथा {
	    rc = -1;
	    क्रम (अचिन्हित idx = 0
		       ; (rc != 0) && (idx < vpip_module_path_cnt)
		       ;  idx += 1) {
		  export_flag = false;
		  snprintf(buf, माप(buf), "%s%c%s.vpi",
			   vpip_module_path[idx], sep, name);
		  rc = stat(buf,&sb);

		  अगर (rc != 0) {
			export_flag = true;
			snprintf(buf, माप(buf), "%s%c%s.vpl",
				 vpip_module_path[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    अगर (rc != 0) {
		  ख_लिखो(मानक_त्रुटि, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  वापस;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      अगर(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	ख_लिखो(मानक_त्रुटि,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	वापस;
      }

#अगर defined(__MINGW32__) || defined (__CYGWIN__)
      व्योम*function = ivl_dlsym(dll, "vpip_set_callback");
      अगर (function) {
            vpip_set_callback_t set_callback = reinterpret_cast<vpip_set_callback_t>(function);
            अगर (!set_callback(&vpi_routines, vpip_routines_version)) {
	          ख_लिखो(मानक_त्रुटि, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          वापस;
            }
      }
#पूर्ण_अगर

      व्योम*table = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      अगर (table == 0) {
	    ख_लिखो(मानक_त्रुटि, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    वापस;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (पुनः_स्मृति(dll_list, dll_list_cnt*माप(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_mode_flag = VPI_MODE_REGISTER;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)table;
      क्रम (अचिन्हित tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_mode_flag = VPI_MODE_NONE;
}
