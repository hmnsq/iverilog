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

ਸਥਿਰ ivl_dll_t*dll_list = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ dll_list_cnt = 0;

#ਜੇ defined(__MINGW32__) || defined (__CYGWIN__)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ PLI_UINT32 (*vpip_ਸੈੱਟ_callback_t)(vpip_routines_s*, PLI_UINT32);
#ਜੇ_ਅੰਤ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*vlog_startup_routines_t)(ਖਾਲੀ);

# define VPIP_MODULE_ਰਸਤਾ_MAX 64

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* vpip_module_ਰਸਤਾ[VPIP_MODULE_ਰਸਤਾ_MAX] = {0};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_module_ਰਸਤਾ_cnt = 0;

ਸਥਿਰ ਤਾਰਕਿਕ disable_default_paths = ਝੂਠ;

ਖਾਲੀ vpip_clear_module_paths()
{
      vpip_module_ਰਸਤਾ_cnt = 0;
      vpip_module_ਰਸਤਾ[0] = 0;
      disable_default_paths = ਸੱਚ;
}

ਖਾਲੀ vpip_ਜੋੜੋ_module_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ*ਰਸਤਾ)
{
      ਜੇ (vpip_module_ਰਸਤਾ_cnt >= VPIP_MODULE_ਰਸਤਾ_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      vpip_module_ਰਸਤਾ[vpip_module_ਰਸਤਾ_cnt++] = ਰਸਤਾ;
}

ਖਾਲੀ vpip_ਜੋੜੋ_env_and_default_module_paths()
{
      ਜੇ (disable_default_paths)
	    ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
      ਅਚਲ ਅੱਖਰ ਰਸਤਾ_sep = ';';
#ਨਹੀਂਤਾਂ
      ਅਚਲ ਅੱਖਰ ਰਸਤਾ_sep = ':';
#ਜੇ_ਅੰਤ

      ਜੇ (ਅੱਖਰ *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            ਅੱਖਰ *ptr = var;
            ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = var+ਲੜੀ_ਲੰਬਾਈ(var);
            ਪੂਰਨ len = 0;
            ਜਦੋਂ (ptr <= ਸਿਰਾ) {
                  ਜੇ (*ptr == 0 || *ptr == ਰਸਤਾ_sep) {
                        ਜੇ (len > 0) {
                              vpip_ਜੋੜੋ_module_ਰਸਤਾ(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } ਨਹੀਂਤਾਂ {
                        len++;
                  }
                  ptr++;
            }
      }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
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
      ਅੱਖਰ *s;
      ਅੱਖਰ basepath[4096], tmp[4096];
      GetModuleFileName(ਨਿਰਾ, tmp, ਆਕਾਰ tmp);
	/* Convert to a short name to remove any embedded spaces. */
      Get_Short_ਰਸਤਾ_Name(tmp, basepath, ਆਕਾਰ basepath);
      s = strrchr(basepath, '\\');
      ਜੇ (s) *s = 0;
      ਨਹੀਂਤਾਂ {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      s = strrchr(basepath, '\\');
      ਜੇ (s) *s = 0;
      ਨਹੀਂਤਾਂ {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_ਜੋੜੋ_module_ਰਸਤਾ(strdup(basepath));
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MODULE_DIR1
      vpip_ਜੋੜੋ_module_ਰਸਤਾ(MODULE_DIR1);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MODULE_DIR2
      vpip_ਜੋੜੋ_module_ਰਸਤਾ(MODULE_DIR2);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ ਲੱਦੋ_module_delete(ਖਾਲੀ)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      ਮੁਕਤ(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

ਖਾਲੀ vpip_ਲੱਦੋ_module(ਅਚਲ ਅੱਖਰ*name)
{
      ਬਣਤਰ stat sb;
      ਪੂਰਨ rc;
      ਤਾਰਕਿਕ export_flag = ਝੂਠ;
      ਅੱਖਰ buf[4096];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
      ਅਚਲ ਅੱਖਰ sep = '\\';
#ਨਹੀਂਤਾਂ
      ਅਚਲ ਅੱਖਰ sep = '/';
#ਜੇ_ਅੰਤ

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
      ਜੇ (strchr(name, '\\') || strchr(name, '/')) {
#ਨਹੀਂਤਾਂ
      ਜੇ (strchr(name, sep)) {
#ਜੇ_ਅੰਤ
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = ਝੂਠ;
	    rc = stat(name, &sb);

	    ਜੇ (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = ਝੂਠ;
		  snprintf(buf, ਆਕਾਰ(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  ਜੇ (rc != 0) {
			export_flag = ਸੱਚ;
			snprintf(buf, ਆਕਾਰ(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  ਜੇ (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			ਵਾਪਸ;
		  }
	    } ਨਹੀਂਤਾਂ {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } ਨਹੀਂਤਾਂ {
	    rc = -1;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0
		       ; (rc != 0) && (idx < vpip_module_ਰਸਤਾ_cnt)
		       ;  idx += 1) {
		  export_flag = ਝੂਠ;
		  snprintf(buf, ਆਕਾਰ(buf), "%s%c%s.vpi",
			   vpip_module_ਰਸਤਾ[idx], sep, name);
		  rc = stat(buf,&sb);

		  ਜੇ (rc != 0) {
			export_flag = ਸੱਚ;
			snprintf(buf, ਆਕਾਰ(buf), "%s%c%s.vpl",
				 vpip_module_ਰਸਤਾ[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    ਜੇ (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  ਵਾਪਸ;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      ਜੇ(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	ਵਾਪਸ;
      }

#ਜੇ defined(__MINGW32__) || defined (__CYGWIN__)
      ਖਾਲੀ*function = ivl_dlsym(dll, "vpip_set_callback");
      ਜੇ (function) {
            vpip_ਸੈੱਟ_callback_t ਸੈੱਟ_callback = reinterpret_cast<vpip_ਸੈੱਟ_callback_t>(function);
            ਜੇ (!ਸੈੱਟ_callback(&vpi_routines, vpip_routines_ਸੰਸਕਰਣ)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          ਵਾਪਸ;
            }
      }
#ਜੇ_ਅੰਤ

      ਖਾਲੀ*ਸਾਰਣੀ = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      ਜੇ (ਸਾਰਣੀ == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    ਵਾਪਸ;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*ਆਕਾਰ(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_ਰਜਿਸਟਰ;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)ਸਾਰਣੀ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
}
