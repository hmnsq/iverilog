/*
 * Copyright (c) 2003-2025 Stephen Williams (steve@icarus.com)
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

/*
 * This file provides a simple command line debugger for the vvp
 * runtime. It is a means to interact with the user running the
 * simulation.
 */

# include  "config.h"

# include  "vpi_priv.h"
# include  "vthread.h"
# include  "schedule.h"
# include  <cstdio>
# include  <cctype>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_READLINE
# include  <readline/readline.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HISTORY
# include  <readline/history.h>
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
ತಾರ್ಕಿಕ stop_is_ಮುಕ್ತಾಯ = ಸುಳ್ಳು;  /* When set, $stop acts like $finish (set in main.cc). */
ಪೂರ್ಣಾಂಕ  stop_is_ಮುಕ್ತಾಯ_exit_ಸಂಕೇತ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_READLINE
ಸ್ಥಿರ ಅಕ್ಷರ* readline_stub(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*prompt)
{
      ಅಕ್ಷರ buf[256];

      ಆದರೆ (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      ಆದರೆ (fgets(buf, ಗಾತ್ರ(buf), stdin)) {
	    ಅಕ್ಷರ*nl = buf + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
	    ಆಗ (nl > buf && ಖಾಲಿಯೇ(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    ಹಿಂತಿರುಗಿ strdup(buf);
      }

      ಹಿಂತಿರುಗಿ 0;
}
#ವ್ಯಾಖ್ಯಾನಿಸು readline(x) readline_stub(x)
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ತಾರ್ಕಿಕ interact_flag = ನಿಜ;

ಸ್ಥಿರ ಶೂನ್ಯ cmd_call(ಚಿಹ್ನೆರಹಿತ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*argv[])
{
      __vpiHandle**ಕೋಷ್ಟಕ;
      ಚಿಹ್ನೆರಹಿತ ntable;

      ಆದರೆ (stop_current_scope == 0) {
	    vpip_make_ಬೇರು_iterator(ಕೋಷ್ಟಕ, ntable);

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಕೋಷ್ಟಕ = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      ಚಿಹ್ನೆರಹಿತ vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(vpi_argc, ಗಾತ್ರ(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(vpi_argc, ಗಾತ್ರ(vpiHandle)));

      ಚಿಹ್ನೆರಹಿತ errors = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    ತಾರ್ಕಿಕ ಕೂಡಿಸು_ಗಮ್ಯ_free_list = ಸುಳ್ಳು;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    ಆದರೆ (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    ಆದರೆ (handle == 0 && argv[idx+1][0] == '"') {
		  ಅಕ್ಷರ*tmp = strdup(argv[idx+1]);
		  tmp[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), ಸುಳ್ಳು);
		  ಕೂಡಿಸು_ಗಮ್ಯ_free_list = ನಿಜ;
		  ಬಿಡುಗಡೆ(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    ಆದರೆ (handle == 0
		&& strspn(argv[idx+1],"0123456789") == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(argv[idx+1])) {
		  handle = ಹೊಸ __vpiDecConst(strtol(argv[idx+1],0,10));
		  ಕೂಡಿಸು_ಗಮ್ಯ_free_list = ನಿಜ;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ಸ್ಥಿರಾಂಕ __vpiScope*scope;
		  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name;

		  ಆಯ್ಕೆ (ಕೋಷ್ಟಕ[tmp]->get_type_ಸಂಕೇತ()) {

		      ಸ್ಥಿತಿ vpiModule:
		      ಸ್ಥಿತಿ vpiGenScope:
		      ಸ್ಥಿತಿ vpiFunction:
		      ಸ್ಥಿತಿ vpiTask:
		      ಸ್ಥಿತಿ vpiNamedBegin:
		      ಸ್ಥಿತಿ vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[tmp]);
			ಆದರೆ (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = ಕೋಷ್ಟಕ[tmp];
			ನಿಲ್ಲಿಸು;

		      ಸ್ಥಿತಿ vpiReg:
		      ಸ್ಥಿತಿ vpiNet:
		      ಸ್ಥಿತಿ vpiParameter:
			name = vpi_get_str(vpiName,ಕೋಷ್ಟಕ[tmp]);
			ಆದರೆ (strcmp(argv[idx+1], name) == 0)
			      handle = ಕೋಷ್ಟಕ[tmp];
			ನಿಲ್ಲಿಸು;
		  }
	    }

	    ಆದರೆ (handle == 0) {
		  ಬರೆ("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    ಆದರೆ (ಕೂಡಿಸು_ಗಮ್ಯ_free_list)
		  vpi_free[idx] = handle;
	    ಇಲ್ಲದಿದ್ದರೆ
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      ಆದರೆ (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                ನಿಜ, ಸುಳ್ಳು,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    ಆದರೆ (call_handle == 0)
		  ಹೋಗು out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    ಆದರೆ (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      ಬಿಡುಗಡೆ(vpi_argv);
      ಬಿಡುಗಡೆ(vpi_free);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_cont(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      interact_flag = ಸುಳ್ಳು;
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ಹೆಜ್ಜೆ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      interact_flag = ಸುಳ್ಳು;
      ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ(0);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ಮುಕ್ತಾಯ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      interact_flag = ಸುಳ್ಳು;
      ವೇಳಾಪಟ್ಟಿ_ಮುಕ್ತಾಯ(0);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_help(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[]);

ಸ್ಥಿರ ಶೂನ್ಯ cmd_list(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      __vpiHandle**ಕೋಷ್ಟಕ;
      ಚಿಹ್ನೆರಹಿತ ntable;

      ಆದರೆ (stop_current_scope == 0) {
	    vpip_make_ಬೇರು_iterator(ಕೋಷ್ಟಕ, ntable);

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಕೋಷ್ಟಕ = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      ಬರೆ("%u items in this scope:\n", ntable);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ntable ;  idx += 1) {

	    ಸ್ಥಿರಾಂಕ __vpiScope*scope;
	    ರಚನೆ __vpiSignal*sig;

	    ಆಯ್ಕೆ (ಕೋಷ್ಟಕ[idx]->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("package : %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiModule:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("module  : %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("generate: %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiTask:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("task    : %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("function: %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("block   : %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[idx]);
		  ಬರೆ("fork    : %s\n", scope->scope_name());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiParameter:
		  ಬರೆ("param   : %s\n", vpi_get_str(vpiName, ಕೋಷ್ಟಕ[idx]));
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(ಕೋಷ್ಟಕ[idx]);
		  ಆದರೆ ((sig->msb.get_ಮೌಲ್ಯ() == 0) && (sig->lsb.get_ಮೌಲ್ಯ() == 0))
			ಬರೆ("reg     : %s%s\n",
			       vpi_get_str(vpiName, ಕೋಷ್ಟಕ[idx]),
			       sig->signed_flag? "signed " : "");
		  ಇಲ್ಲದಿದ್ದರೆ
			ಬರೆ("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, ಕೋಷ್ಟಕ[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ಮೌಲ್ಯ(), sig->lsb.get_ಮೌಲ್ಯ());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(ಕೋಷ್ಟಕ[idx]);
		  ಆದರೆ ((sig->msb.get_ಮೌಲ್ಯ() == 0) && (sig->lsb.get_ಮೌಲ್ಯ() == 0))
			ಬರೆ("net     : %s%s\n",
			       vpi_get_str(vpiName, ಕೋಷ್ಟಕ[idx]),
			       sig->signed_flag? "signed " : "");
		  ಇಲ್ಲದಿದ್ದರೆ
			ಬರೆ("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, ಕೋಷ್ಟಕ[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ಮೌಲ್ಯ(), sig->lsb.get_ಮೌಲ್ಯ());
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpi_ಪೋರ್ಟ್:
		  ಬರೆ("port    : %s -- %s\n",
			 vpi_get_str(vpiName, ಕೋಷ್ಟಕ[idx]),
			 direction_as_string(vpi_get(vpiDirection, ಕೋಷ್ಟಕ[idx])));
		  ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಬರೆ("%8s: <vpi handle>\n",
			 vpi_type_as_string(ಕೋಷ್ಟಕ[idx]->get_type_ಸಂಕೇತ()));
		  ನಿಲ್ಲಿಸು;
	    }

      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ತುಂಬು(ಚಿಹ್ನೆರಹಿತ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*argv[])
{
      ಚಿಹ್ನೆರಹಿತ idx;

      ಗಾಗಿ (idx = 1 ;  idx < argc ;  idx += 1) {
	    ಬರೆ("Loading module %s...\n", argv[idx]);
	    vpip_ತುಂಬು_module(argv[idx]);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ತೆಗೆ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      ಆದರೆ (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ತಳ್ಳು(ಚಿಹ್ನೆರಹಿತ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* argv[])
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**ಕೋಷ್ಟಕ;
	    ಚಿಹ್ನೆರಹಿತ ntable;

	    ಆದರೆ (stop_current_scope) {
		  ಕೋಷ್ಟಕ = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vpip_make_ಬೇರು_iterator(ಕೋಷ್ಟಕ, ntable);
	    }

	    __vpiScope*ಮಗು = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(ಕೋಷ್ಟಕ[tmp]);
		  ಆದರೆ (scope == 0)
			ಮುಂದುವರಿ;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  ಆದರೆ (strcmp(scope->scope_name(), argv[idx]) == 0) {
			ಮಗು = scope;
			ನಿಲ್ಲಿಸು;
		  }
	    }

	    ಆದರೆ (ಮಗು == 0) {
		  ಬರೆ("Scope %s not found.\n", argv[idx]);
		  ಹಿಂತಿರುಗಿ;
	    }

	    stop_current_scope = ಮಗು;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ಸಮಯ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      vvp_time64_t ticks = ವೇಳಾಪಟ್ಟಿ_simtime();
      ಬರೆ("%" ಸಮಯ_FMT_U " ticks\n", ticks);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_trace(ಚಿಹ್ನೆರಹಿತ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*argv[])
{
      ಪ್ರತಿಜ್ಞಾ(argc);
      ಆಯ್ಕೆ (argc) {
	ಸ್ಥಿತಿ 1:
	    show_file_ಸಾಲು = ನಿಜ;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಬರೆ("Only using the first argument to trace.\n");
	    // fallthrough
	ಸ್ಥಿತಿ 2:
	    ಆದರೆ ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_ಸಾಲು = ನಿಜ;
	    } ಇಲ್ಲದಿದ್ದರೆ show_file_ಸಾಲು = ಸುಳ್ಳು;
	    ನಿಲ್ಲಿಸು;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      ಆದರೆ (!ಸಂಕೇತ_is_instrumented) {
	    ಬರೆ("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    ಬರೆ("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_ಸಾಲು = ಸುಳ್ಳು;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಬರೆ("Turning statement tracing %s.\n",
	           show_file_ಸಾಲು ? "on" : "off");
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_where(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      ಸ್ಥಿರಾಂಕ __vpiScope*cur = stop_current_scope;

      ಆಗ (cur) {
	    ಆಯ್ಕೆ (cur->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpiModule:
		  ಬರೆ("module %s\n", cur->scope_name());
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiGenScope:
		  ಬರೆ("generate %s\n", cur->scope_name());
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಬರೆ("scope (%d) %s;\n", cur->get_type_ಸಂಕೇತ(), cur->scope_name());
		  ನಿಲ್ಲಿಸು;
	    }

	    cur = cur->scope;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_unknown(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*argv[])
{
      ಬರೆ("Unknown command: %s\n", argv[0]);
      ಬರೆ("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ಸ್ಥಿರ ರಚನೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name;
      ಶೂನ್ಯ (*proc)(ಚಿಹ್ನೆರಹಿತ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*argv[]);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*summary;
} cmd_ಕೋಷ್ಟಕ[] = {
      { "cd",     &cmd_ತಳ್ಳು,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_ಮುಕ್ತಾಯ,
        "Synonym for finish."},
      { "finish", &cmd_ಮುಕ್ತಾಯ,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_ತುಂಬು,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_ತೆಗೆ,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ತಳ್ಳು,
        "Descend into the named scope."},
      { "step",   &cmd_ಹೆಜ್ಜೆ,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_ಸಮಯ,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

ಸ್ಥಿರ ಶೂನ್ಯ cmd_help(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[])
{
      ಬರೆ("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  cmd_ಕೋಷ್ಟಕ[idx].name != 0 ;  idx += 1) {
	    ಬರೆ("%-8s - %s\n", cmd_ಕೋಷ್ಟಕ[idx].name, cmd_ಕೋಷ್ಟಕ[idx].summary);
      }

      ಬರೆ("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ಸ್ಥಿರ ಶೂನ್ಯ invoke_command(ಅಕ್ಷರ*txt)
{
      ಚಿಹ್ನೆರಹಿತ argc = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**argv = const_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**>(ಹೊಸ ಅಕ್ಷರ*[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(txt)/2]);

	/* Chop the line into words. */
      ಗಾಗಿ (ಅಕ್ಷರ*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    ಆದರೆ (cp[0] == '"') {
		  ಅಕ್ಷರ*tmp = strchr(cp+1, '"');
		  ಆದರೆ (tmp == 0) {
			ಬರೆ("Missing close-quote: %s\n", cp);
			ಅಳಿಸು[]argv;
			ಹಿಂತಿರುಗಿ;
		  }

		  cp = tmp + 1;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  cp += strcspn(cp, " ");
	    }

	    ಆದರೆ (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      ಆದರೆ (argc > 0) {

	    ಆದರೆ (argv[0][0] == '$') {
		  ಆದರೆ (strcmp(argv[0], "$stop") == 0) {
			ಬರೆ("The simulator is already stopped!\n");
		  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[0], "$finish") == 0){
			cmd_ಮುಕ್ತಾಯ(argc, argv);
		  } ಇಲ್ಲದಿದ್ದರೆ {
			cmd_call(argc, argv);
		  }
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಚಿಹ್ನೆರಹಿತ idx;
		  ಗಾಗಿ (idx = 0 ;  cmd_ಕೋಷ್ಟಕ[idx].name ;  idx += 1)
			ಆದರೆ (strcmp(cmd_ಕೋಷ್ಟಕ[idx].name, argv[0]) == 0)
			      ನಿಲ್ಲಿಸು;

		  cmd_ಕೋಷ್ಟಕ[idx].proc (argc, argv);
	    }

      }

      ಅಳಿಸು[]argv;
}

ಸ್ಥಿರ ಶೂನ್ಯ invoke_command_const(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*txt)
{
      ಅಕ್ಷರ *vtxt = strdup(txt);
      invoke_command(vtxt);
      ಬಿಡುಗಡೆ(vtxt);
}

ಶೂನ್ಯ stop_handler(ಪೂರ್ಣಾಂಕ rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      ಆದರೆ (stop_is_ಮುಕ್ತಾಯ) {
	    vpip_ಗಣ_return_ಮೌಲ್ಯ(stop_is_ಮುಕ್ತಾಯ_exit_ಸಂಕೇತ);
	    ವೇಳಾಪಟ್ಟಿ_ಮುಕ್ತಾಯ(0);
	    ಹಿಂತಿರುಗಿ;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" ಸಮಯ_FMT_U " ticks.\n",
		     ವೇಳಾಪಟ್ಟಿ_simtime());

      interact_flag = ನಿಜ;
      ಆಗ (interact_flag) {
	    ಅಕ್ಷರ*ಒಳಹರಿವು = readline("> ");
	    ಆದರೆ (ಒಳಹರಿವು == 0)
		  ನಿಲ್ಲಿಸು;


	      /* Advance to the first input character. */
	    ಅಕ್ಷರ*ಮೊದಲ = ಒಳಹರಿವು;
	    ಆಗ (*ಮೊದಲ && ಖಾಲಿಯೇ(*ಮೊದಲ))
		  ಮೊದಲ += 1;

	    ಆದರೆ (ಮೊದಲ[0] != 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HISTORY
		  ಕೂಡಿಸು_history(ಮೊದಲ);
#ಆದರೆ_ಅಂತ್ಯ
		  invoke_command(ಮೊದಲ);
	    }

	    ಬಿಡುಗಡೆ(ಒಳಹರಿವು);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
