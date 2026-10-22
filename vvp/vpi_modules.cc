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

ساکن ivl_dll_t*dll_list = 0;
ساکن غیرعلامتی dll_list_cnt = 0;

#اگر defined(__MINGW32__) || defined (__CYGWIN__)
قسمتعریف PLI_UINT32 (*vpip_سیٹ_callback_t)(vpip_routines_s*, PLI_UINT32);
#اگر_ختم
قسمتعریف خالی (*vlog_startup_routines_t)(خالی);

# define VPIP_MODULE_راستہ_MAX 64

ساکن ثابت حرف* vpip_module_راستہ[VPIP_MODULE_راستہ_MAX] = {0};

ساکن غیرعلامتی vpip_module_راستہ_cnt = 0;

ساکن منطقی disable_default_paths = جھوٹ;

خالی vpip_clear_module_paths()
{
      vpip_module_راستہ_cnt = 0;
      vpip_module_راستہ[0] = 0;
      disable_default_paths = سچ;
}

خالی vpip_جوڑو_module_راستہ(ثابت حرف*راستہ)
{
      اگر (vpip_module_راستہ_cnt >= VPIP_MODULE_راستہ_MAX) {
	    fprintf(stderr, "Too many module paths specified\n");
	    باہر_نکلو(1);
      }
      vpip_module_راستہ[vpip_module_راستہ_cnt++] = راستہ;
}

خالی vpip_جوڑو_env_and_default_module_paths()
{
      اگر (disable_default_paths)
	    واپس;

#اگر_تعریف_شدہ __MINGW32__
      ثابت حرف راستہ_sep = ';';
#ورنہ
      ثابت حرف راستہ_sep = ':';
#اگر_ختم

      اگر (حرف *var = ::getenv("IVERILOG_VPI_MODULE_PATH")) {
            حرف *ptr = var;
            ثابت حرف *انتہا = var+لڑی_لمبائی(var);
            صحیح len = 0;
            جب (ptr <= انتہا) {
                  اگر (*ptr == 0 || *ptr == راستہ_sep) {
                        اگر (len > 0) {
                              vpip_جوڑو_module_راستہ(strndup(var, len));
                        }
                        len = 0;
                        var = ptr+1;
                  } ورنہ {
                        len++;
                  }
                  ptr++;
            }
      }

#اگر_تعریف_شدہ __MINGW32__
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
      GetModuleFileName(تہی, tmp, حجم tmp);
	/* Convert to a short name to remove any embedded spaces. */
      Get_Short_راستہ_Name(tmp, basepath, حجم basepath);
      s = strrchr(basepath, '\\');
      اگر (s) *s = 0;
      ورنہ {
	    fprintf(stderr, "%s: Missing first \\ in exe path!\n", tmp);
	    باہر_نکلو(1);
      }
      s = strrchr(basepath, '\\');
      اگر (s) *s = 0;
      ورنہ {
	    fprintf(stderr, "%s: Missing second \\ in exe path!\n", tmp);
	    باہر_نکلو(1);
      }
      strcat(s, "\\lib\\ivl" IVL_SUFFIX);
      vpip_جوڑو_module_راستہ(strdup(basepath));
#ورنہ
#اگر_تعریف_شدہ MODULE_DIR1
      vpip_جوڑو_module_راستہ(MODULE_DIR1);
#اگر_ختم
#اگر_ختم
#اگر_تعریف_شدہ MODULE_DIR2
      vpip_جوڑو_module_راستہ(MODULE_DIR2);
#اگر_ختم
}

خالی بھرو_module_delete(خالی)
{
      برائے (غیرعلامتی idx = 0; idx < dll_list_cnt; idx += 1) {
	    ivl_dlclose(dll_list[idx]);
      }
      آزاد(dll_list);
      dll_list = 0;
      dll_list_cnt = 0;
}

خالی vpip_بھرو_module(ثابت حرف*name)
{
      ساخت stat sb;
      صحیح rc;
      منطقی export_flag = جھوٹ;
      حرف buf[4096];

#اگر_تعریف_شدہ __MINGW32__
      ثابت حرف sep = '\\';
#ورنہ
      ثابت حرف sep = '/';
#اگر_ختم

      ivl_dll_t dll = 0;
      buf[0] = 0;                     /* terminate the string */
#اگر_تعریف_شدہ __MINGW32__
      اگر (strchr(name, '\\') || strchr(name, '/')) {
#ورنہ
      اگر (strchr(name, sep)) {
#اگر_ختم
	      /* If the name has at least one directory character in
		 it, then assume it is a complete name, maybe including any
		 possible .vpi suffix. */
	    export_flag = جھوٹ;
	    rc = stat(name, &sb);

	    اگر (rc != 0) {            /* did we find a file? */
	          /* no, try with a .vpi suffix too */
		  export_flag = جھوٹ;
		  snprintf(buf, حجم(buf), "%s.vpi", name);
		  rc = stat(buf, &sb);

		    /* Try also with the .vpl suffix. */
		  اگر (rc != 0) {
			export_flag = سچ;
			snprintf(buf, حجم(buf), "%s.vpl", name);
			rc = stat(buf, &sb);
		  }

		  اگر (rc != 0) {
			fprintf(stderr, "%s: Unable to find module file `%s' "
				"or `%s.vpi'.\n", name, name, name);
			واپس;
		  }
	    } ورنہ {
	      strcpy(buf,name);   /* yes copy the name into the buffer */
	    }

      } ورنہ {
	    rc = -1;
	    برائے (غیرعلامتی idx = 0
		       ; (rc != 0) && (idx < vpip_module_راستہ_cnt)
		       ;  idx += 1) {
		  export_flag = جھوٹ;
		  snprintf(buf, حجم(buf), "%s%c%s.vpi",
			   vpip_module_راستہ[idx], sep, name);
		  rc = stat(buf,&sb);

		  اگر (rc != 0) {
			export_flag = سچ;
			snprintf(buf, حجم(buf), "%s%c%s.vpl",
				 vpip_module_راستہ[idx], sep, name);
			rc = stat(buf,&sb);
		  }
	    }

	    اگر (rc != 0) {
		  fprintf(stderr, "%s: Unable to find a "
			  "`%s.vpi' module on the search path.\n",
			  name, name);
		  واپس;
	    }

      }

      /* must have found some file that could possibly be a vpi module
       * try to open it as a shared object.
       */
      dll = ivl_dlopen(buf, export_flag);
      اگر(dll==0) {
	/* hmm, this failed, let the user know what has really gone wrong */
	fprintf(stderr,"%s:`%s' failed to open using dlopen() because:\n"
		"    %s.\n",name,buf,dlerror());

	واپس;
      }

#اگر defined(__MINGW32__) || defined (__CYGWIN__)
      خالی*function = ivl_dlsym(dll, "vpip_set_callback");
      اگر (function) {
            vpip_سیٹ_callback_t سیٹ_callback = reinterpret_cast<vpip_سیٹ_callback_t>(function);
            اگر (!سیٹ_callback(&vpi_routines, vpip_routines_نسخہ)) {
	          fprintf(stderr, "Failed to link VPI module %s. Try rebuilding it with iverilog-vpi.\n", name);
	          ivl_dlclose(dll);
	          واپس;
            }
      }
#اگر_ختم

      خالی*جدول = ivl_dlsym(dll, LU "vlog_startup_routines" TU);
      اگر (جدول == 0) {
	    fprintf(stderr, "%s: no vlog_startup_routines\n", name);
	    ivl_dlclose(dll);
	    واپس;
      }

	/* Add the dll to the list so it can be closed when we are done. */
      dll_list_cnt += 1;
      dll_list = static_cast<ivl_dll_t*>
                 (realloc(dll_list, dll_list_cnt*حجم(ivl_dll_t)));
      dll_list[dll_list_cnt-1] = dll;

      vpi_منوال_flag = VPI_منوال_رجسٹر;
      vlog_startup_routines_t*routines = (vlog_startup_routines_t*)جدول;
      برائے (غیرعلامتی tmp = 0 ;  routines[tmp] ;  tmp += 1)
	    (routines[tmp])();
      vpi_منوال_flag = VPI_منوال_NONE;
}
