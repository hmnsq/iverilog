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

ساكن ivl_dll_t*dll_list = 0;
ساكن بیعلامت dll_list_cnt = 0;

#إذا defined(__MINGW32__) || defined (__CYGWIN__)
تعريفنوع PLI_UINT32 (*vpip_مجموعه_callback_t)(vpip_routines_s*, PLI_UINT32);
#پایان_اگر
تعريفنوع فراغ (*vlog_startup_routines_t)(فراغ);

# define VPIP_MODULE_مسیر_MAX 64

ساكن ثابت حرف* vpip_module_مسیر[VPIP_MODULE_مسیر_MAX] = {0};

ساكن بیعلامت vpip_module_مسیر_cnt = 0;

ساكن منطقي disable_default_paths = خطأ;

فراغ vpip_clear_module_paths()
{
      vpip_module_مسیر_cnt = 0;
      vpip_module_مسیر[0] = 0;
      disable_default_paths = درست;
}

فراغ vpip_جمع_کن_module_مسیر(ثابت حرف*مسیر)
{
      إذا (vpip_module_مسیر_cnt >= VPIP_MODULE_مسیر_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    خروج(1);
      }
      vpip_module_مسیر[vpip_module_مسیر_cnt++] = مسیر;
}

فراغ vpip_جمع_کن_env_and_default_module_paths()
{
      إذا (disable_default_paths)
	    أرجع;

#اگر_تعریف_شده __MINGW32__
      ثابت حرف مسیر_sep = ';';
#وإلا
      ثابت حرف مسیر_sep = ':';
#پایان_اگر

      إذا (حرف *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            حرف *ptr = var;
            ثابت حرف *انتها = var+طول_رشته(var);
            صحيح len = 0;
            طالما (ptr <= انتها) {
                  إذا (*ptr == 0 || *ptr == مسیر_sep) {
                        إذا (len > 0) {
                              vpip_جمع_کن_module_مسیر(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } وإلا {
                        len++;
                  }
                  ptr++;
            }
      }

#اگر_تعریف_شده __MINGW32__
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
      حرف *s;
      حرف basepath[4096], tmp[4096];
      GetModuleFileName(عدم, tmp, حجم tmp);
	/* Convert to a short name to remove any embedded spaces. */
      Get_Short_مسیر_Name(tmp, basepath, حجم basepath);
      s = strrchr(basepath, '\\');
      إذا (s) *s = 0;
      وإلا {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    خروج(1);
      }
      s = strrchr(basepath, '\\');
      إذا (s) *s = 0;
      وإلا {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    خروج(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_جمع_کن_module_مسیر(strdup(basepath));
#وإلا
#اگر_تعریف_شده MODULE_DIR1
      vpip_جمع_کن_module_مسیر(MODULE_DIR1);
#پایان_اگر
#پایان_اگر
#اگر_تعریف_شده MODULE_DIR2
      vpip_جمع_کن_module_مسیر(MODULE_DIR2);
#پایان_اگر
}

فراغ بارگیری_module_delete(فراغ)
{
      لكل (بیعلامت idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      حرر(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

فراغ vpip_بارگیری_module(ثابت حرف*name)
{
      بنية stat sb;
      صحيح rc;
      منطقي export_flag = خطأ;
      حرف buf[4096];

#اگر_تعریف_شده __MINGW32__
      ثابت حرف sep = '\\';
#وإلا
      ثابت حرف sep = '/';
#پایان_اگر

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#اگر_تعریف_شده __MINGW32__
      إذا (strchr(name, '\\') || strchr(name, '/')) {
#وإلا
      إذا (strchr(name, sep)) {
#پایان_اگر
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = خطأ;
	    rc = stat(name, &sb);

	    إذا (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = خطأ;
		  snprintf(buf, حجم(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  إذا (rc != 0) {
			export_flag = درست;
			snprintf(buf, حجم(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  إذا (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			أرجع;
		  }
	    } وإلا {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } وإلا {
	    rc = -1;
	    لكل (بیعلامت idx = 0
		       ; (rc != 0) && (idx < vpip_module_مسیر_cnt)
		       ;  idx += 1) {
		  export_flag = خطأ;
		  snprintf(buf, حجم(buf), "%s%c%s.vpi",
			   vpip_module_مسیر[idx], sep, name);
		  rc = stat(buf,&sb);

		  إذا (rc != 0) {
			export_flag = درست;
			snprintf(buf, حجم(buf), "%s%c%s.vpl",
				 vpip_module_مسیر[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    إذا (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  أرجع;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      إذا(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	أرجع;
      }

#إذا defined(__MINGW32__) || defined (__CYGWIN__)
      فراغ*function = ivl_dlsym(dll, "vpip_set_callback");
      إذا (function) {
            vpip_مجموعه_callback_t مجموعه_callback = reinterpret_cast<vpip_مجموعه_callback_t>(function);
            إذا (!مجموعه_callback(&vpi_routines, vpip_routines_نسخه)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          أرجع;
            }
      }
#پایان_اگر

      فراغ*جدول = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      إذا (جدول == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    أرجع;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*حجم(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_نما_flag = VPI_نما_ثبات;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)جدول;
      لكل (بیعلامت tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_نما_flag = VPI_نما_NONE;
}
