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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_READLINE
# include  <readline/readline.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HISTORY
# include  <readline/history.h>
#ਜੇ_ਅੰਤ
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
ਤਾਰਕਿਕ stop_is_ਅੰਤ = ਝੂਠ;  /* When set, $stop acts like $finish (set in main.cc). */
ਪੂਰਨ  stop_is_ਅੰਤ_exit_ਕੋਡ = 0;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_READLINE
ਸਥਿਰ ਅੱਖਰ* readline_stub(ਅਚਲ ਅੱਖਰ*prompt)
{
      ਅੱਖਰ buf[256];

      ਜੇ (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      ਜੇ (fgets(buf, ਆਕਾਰ(buf), stdin)) {
	    ਅੱਖਰ*nl = buf + ਲੜੀ_ਲੰਬਾਈ(buf);
	    ਜਦੋਂ (nl > buf && ਖਾਲੀ_ਥਾਂ_ਹੈ(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    ਵਾਪਸ strdup(buf);
      }

      ਵਾਪਸ 0;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ readline(x) readline_stub(x)
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਤਾਰਕਿਕ interact_flag = ਸੱਚ;

ਸਥਿਰ ਖਾਲੀ cmd_call(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਅਚਲ ਅੱਖਰ*argv[])
{
      __vpiHandle**ਸਾਰਣੀ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ntable;

      ਜੇ (stop_current_scope == 0) {
	    vpip_make_ਜੜ੍ਹ_iterator(ਸਾਰਣੀ, ntable);

      } ਨਹੀਂਤਾਂ {
	    ਸਾਰਣੀ = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(vpi_argc, ਆਕਾਰ(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(vpi_argc, ਆਕਾਰ(vpiHandle)));

      ਬਿਨਾਨਿਸ਼ਾਨੀ errors = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    ਤਾਰਕਿਕ ਜੋੜੋ_ਮੰਜ਼ਿਲ_free_list = ਝੂਠ;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    ਜੇ (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    ਜੇ (handle == 0 && argv[idx+1][0] == '"') {
		  ਅੱਖਰ*tmp = strdup(argv[idx+1]);
		  tmp[ਲੜੀ_ਲੰਬਾਈ(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), ਝੂਠ);
		  ਜੋੜੋ_ਮੰਜ਼ਿਲ_free_list = ਸੱਚ;
		  ਮੁਕਤ(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    ਜੇ (handle == 0
		&& strspn(argv[idx+1],"0123456789") == ਲੜੀ_ਲੰਬਾਈ(argv[idx+1])) {
		  handle = ਨਵਾਂ __vpiDecConst(strtol(argv[idx+1],0,10));
		  ਜੋੜੋ_ਮੰਜ਼ਿਲ_free_list = ਸੱਚ;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ਅਚਲ __vpiScope*scope;
		  ਅਚਲ ਅੱਖਰ*name;

		  ਚੋਣ (ਸਾਰਣੀ[tmp]->get_type_ਕੋਡ()) {

		      ਹਾਲਤ vpiModule:
		      ਹਾਲਤ vpiGenScope:
		      ਹਾਲਤ vpiFunction:
		      ਹਾਲਤ vpiTask:
		      ਹਾਲਤ vpiNamedBegin:
		      ਹਾਲਤ vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[tmp]);
			ਜੇ (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = ਸਾਰਣੀ[tmp];
			ਰੋਕੋ;

		      ਹਾਲਤ vpiReg:
		      ਹਾਲਤ vpiNet:
		      ਹਾਲਤ vpiParameter:
			name = vpi_get_str(vpiName,ਸਾਰਣੀ[tmp]);
			ਜੇ (strcmp(argv[idx+1], name) == 0)
			      handle = ਸਾਰਣੀ[tmp];
			ਰੋਕੋ;
		  }
	    }

	    ਜੇ (handle == 0) {
		  ਲਿਖੋ("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    ਜੇ (ਜੋੜੋ_ਮੰਜ਼ਿਲ_free_list)
		  vpi_free[idx] = handle;
	    ਨਹੀਂਤਾਂ
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      ਜੇ (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                ਸੱਚ, ਝੂਠ,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    ਜੇ (call_handle == 0)
		  ਜਾਓ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    ਜੇ (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      ਮੁਕਤ(vpi_argv);
      ਮੁਕਤ(vpi_free);
}

ਸਥਿਰ ਖਾਲੀ cmd_cont(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      interact_flag = ਝੂਠ;
}

ਸਥਿਰ ਖਾਲੀ cmd_ਕਦਮ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      interact_flag = ਝੂਠ;
      ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ(0);
}

ਸਥਿਰ ਖਾਲੀ cmd_ਅੰਤ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      interact_flag = ਝੂਠ;
      ਸਮਾਂ_ਸਾਰਣੀ_ਅੰਤ(0);
}

ਸਥਿਰ ਖਾਲੀ cmd_help(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[]);

ਸਥਿਰ ਖਾਲੀ cmd_list(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      __vpiHandle**ਸਾਰਣੀ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ntable;

      ਜੇ (stop_current_scope == 0) {
	    vpip_make_ਜੜ੍ਹ_iterator(ਸਾਰਣੀ, ntable);

      } ਨਹੀਂਤਾਂ {
	    ਸਾਰਣੀ = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      ਲਿਖੋ("%u items in this scope:\n", ntable);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ntable ;  idx += 1) {

	    ਅਚਲ __vpiScope*scope;
	    ਬਣਤਰ __vpiSignal*sig;

	    ਚੋਣ (ਸਾਰਣੀ[idx]->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("package : %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiModule:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("module  : %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("generate: %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiTask:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("task    : %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("function: %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("block   : %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[idx]);
		  ਲਿਖੋ("fork    : %s\n", scope->scope_name());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiParameter:
		  ਲਿਖੋ("param   : %s\n", vpi_get_str(vpiName, ਸਾਰਣੀ[idx]));
		  ਰੋਕੋ;

		ਹਾਲਤ vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(ਸਾਰਣੀ[idx]);
		  ਜੇ ((sig->msb.get_ਮੁੱਲ() == 0) && (sig->lsb.get_ਮੁੱਲ() == 0))
			ਲਿਖੋ("reg     : %s%s\n",
			       vpi_get_str(vpiName, ਸਾਰਣੀ[idx]),
			       sig->signed_flag? "signed " : "");
		  ਨਹੀਂਤਾਂ
			ਲਿਖੋ("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, ਸਾਰਣੀ[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ਮੁੱਲ(), sig->lsb.get_ਮੁੱਲ());
		  ਰੋਕੋ;

		ਹਾਲਤ vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(ਸਾਰਣੀ[idx]);
		  ਜੇ ((sig->msb.get_ਮੁੱਲ() == 0) && (sig->lsb.get_ਮੁੱਲ() == 0))
			ਲਿਖੋ("net     : %s%s\n",
			       vpi_get_str(vpiName, ਸਾਰਣੀ[idx]),
			       sig->signed_flag? "signed " : "");
		  ਨਹੀਂਤਾਂ
			ਲਿਖੋ("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, ਸਾਰਣੀ[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ਮੁੱਲ(), sig->lsb.get_ਮੁੱਲ());
		  ਰੋਕੋ;

		ਹਾਲਤ vpi_ਪੋਰਟ:
		  ਲਿਖੋ("port    : %s -- %s\n",
			 vpi_get_str(vpiName, ਸਾਰਣੀ[idx]),
			 direction_as_string(vpi_get(vpiDirection, ਸਾਰਣੀ[idx])));
		  ਰੋਕੋ;

		ਮੂਲ:
		  ਲਿਖੋ("%8s: <vpi handle>\n",
			 vpi_type_as_string(ਸਾਰਣੀ[idx]->get_type_ਕੋਡ()));
		  ਰੋਕੋ;
	    }

      }
}

ਸਥਿਰ ਖਾਲੀ cmd_ਲੱਦੋ(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਅਚਲ ਅੱਖਰ*argv[])
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

      ਲਈ (idx = 1 ;  idx < argc ;  idx += 1) {
	    ਲਿਖੋ("Loading module %s...\n", argv[idx]);
	    vpip_ਲੱਦੋ_module(argv[idx]);
      }
}

ਸਥਿਰ ਖਾਲੀ cmd_ਕੱਢੋ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      ਜੇ (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ਸਥਿਰ ਖਾਲੀ cmd_ਧੱਕੋ(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਅਚਲ ਅੱਖਰ* argv[])
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**ਸਾਰਣੀ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ntable;

	    ਜੇ (stop_current_scope) {
		  ਸਾਰਣੀ = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } ਨਹੀਂਤਾਂ {
		  vpip_make_ਜੜ੍ਹ_iterator(ਸਾਰਣੀ, ntable);
	    }

	    __vpiScope*ਬੱਚਾ = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(ਸਾਰਣੀ[tmp]);
		  ਜੇ (scope == 0)
			ਜਾਰੀ;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  ਜੇ (strcmp(scope->scope_name(), argv[idx]) == 0) {
			ਬੱਚਾ = scope;
			ਰੋਕੋ;
		  }
	    }

	    ਜੇ (ਬੱਚਾ == 0) {
		  ਲਿਖੋ("Scope %s not found.\n", argv[idx]);
		  ਵਾਪਸ;
	    }

	    stop_current_scope = ਬੱਚਾ;
      }
}

ਸਥਿਰ ਖਾਲੀ cmd_ਸਮਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      vvp_time64_t ticks = ਸਮਾਂ_ਸਾਰਣੀ_simtime();
      ਲਿਖੋ("%" ਸਮਾਂ_FMT_U " ticks\n", ticks);
}

ਸਥਿਰ ਖਾਲੀ cmd_trace(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਅਚਲ ਅੱਖਰ*argv[])
{
      ਪ੍ਰਤਿਜ੍ਞਾ(argc);
      ਚੋਣ (argc) {
	ਹਾਲਤ 1:
	    show_file_ਸਤਰ = ਸੱਚ;
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਲਿਖੋ("Only using the first argument to trace.\n");
	    // fallthrough
	ਹਾਲਤ 2:
	    ਜੇ ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_ਸਤਰ = ਸੱਚ;
	    } ਨਹੀਂਤਾਂ show_file_ਸਤਰ = ਝੂਠ;
	    ਰੋਕੋ;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      ਜੇ (!ਕੋਡ_is_instrumented) {
	    ਲਿਖੋ("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    ਲਿਖੋ("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_ਸਤਰ = ਝੂਠ;
      } ਨਹੀਂਤਾਂ {
	    ਲਿਖੋ("Turning statement tracing %s.\n",
	           show_file_ਸਤਰ ? "on" : "off");
      }
}

ਸਥਿਰ ਖਾਲੀ cmd_where(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      ਅਚਲ __vpiScope*cur = stop_current_scope;

      ਜਦੋਂ (cur) {
	    ਚੋਣ (cur->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpiModule:
		  ਲਿਖੋ("module %s\n", cur->scope_name());
		  ਰੋਕੋ;
		ਹਾਲਤ vpiGenScope:
		  ਲਿਖੋ("generate %s\n", cur->scope_name());
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਲਿਖੋ("scope (%d) %s;\n", cur->get_type_ਕੋਡ(), cur->scope_name());
		  ਰੋਕੋ;
	    }

	    cur = cur->scope;
      }
}

ਸਥਿਰ ਖਾਲੀ cmd_unknown(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*argv[])
{
      ਲਿਖੋ("Unknown command: %s\n", argv[0]);
      ਲਿਖੋ("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ਸਥਿਰ ਬਣਤਰ {
      ਅਚਲ ਅੱਖਰ*name;
      ਖਾਲੀ (*proc)(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਅਚਲ ਅੱਖਰ*argv[]);
      ਅਚਲ ਅੱਖਰ*summary;
} cmd_ਸਾਰਣੀ[] = {
      { "cd",     &cmd_ਧੱਕੋ,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_ਅੰਤ,
        "Synonym for finish."},
      { "finish", &cmd_ਅੰਤ,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_ਲੱਦੋ,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_ਕੱਢੋ,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ਧੱਕੋ,
        "Descend into the named scope."},
      { "step",   &cmd_ਕਦਮ,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_ਸਮਾਂ,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

ਸਥਿਰ ਖਾਲੀ cmd_help(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ ਅੱਖਰ*[])
{
      ਲਿਖੋ("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  cmd_ਸਾਰਣੀ[idx].name != 0 ;  idx += 1) {
	    ਲਿਖੋ("%-8s - %s\n", cmd_ਸਾਰਣੀ[idx].name, cmd_ਸਾਰਣੀ[idx].summary);
      }

      ਲਿਖੋ("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ਸਥਿਰ ਖਾਲੀ invoke_command(ਅੱਖਰ*txt)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ argc = 0;
      ਅਚਲ ਅੱਖਰ**argv = const_cast<ਅਚਲ ਅੱਖਰ**>(ਨਵਾਂ ਅੱਖਰ*[ਲੜੀ_ਲੰਬਾਈ(txt)/2]);

	/* Chop the line into words. */
      ਲਈ (ਅੱਖਰ*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    ਜੇ (cp[0] == '"') {
		  ਅੱਖਰ*tmp = strchr(cp+1, '"');
		  ਜੇ (tmp == 0) {
			ਲਿਖੋ("Missing close-quote: %s\n", cp);
			ਮਿਟਾਓ[]argv;
			ਵਾਪਸ;
		  }

		  cp = tmp + 1;

	    } ਨਹੀਂਤਾਂ {
		  cp += strcspn(cp, " ");
	    }

	    ਜੇ (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      ਜੇ (argc > 0) {

	    ਜੇ (argv[0][0] == '$') {
		  ਜੇ (strcmp(argv[0], "$stop") == 0) {
			ਲਿਖੋ("The simulator is already stopped!\n");
		  } ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[0], "$finish") == 0){
			cmd_ਅੰਤ(argc, argv);
		  } ਨਹੀਂਤਾਂ {
			cmd_call(argc, argv);
		  }
	    } ਨਹੀਂਤਾਂ {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
		  ਲਈ (idx = 0 ;  cmd_ਸਾਰਣੀ[idx].name ;  idx += 1)
			ਜੇ (strcmp(cmd_ਸਾਰਣੀ[idx].name, argv[0]) == 0)
			      ਰੋਕੋ;

		  cmd_ਸਾਰਣੀ[idx].proc (argc, argv);
	    }

      }

      ਮਿਟਾਓ[]argv;
}

ਸਥਿਰ ਖਾਲੀ invoke_command_const(ਅਚਲ ਅੱਖਰ*txt)
{
      ਅੱਖਰ *vtxt = strdup(txt);
      invoke_command(vtxt);
      ਮੁਕਤ(vtxt);
}

ਖਾਲੀ stop_handler(ਪੂਰਨ rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      ਜੇ (stop_is_ਅੰਤ) {
	    vpip_ਸੈੱਟ_return_ਮੁੱਲ(stop_is_ਅੰਤ_exit_ਕੋਡ);
	    ਸਮਾਂ_ਸਾਰਣੀ_ਅੰਤ(0);
	    ਵਾਪਸ;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" ਸਮਾਂ_FMT_U " ticks.\n",
		     ਸਮਾਂ_ਸਾਰਣੀ_simtime());

      interact_flag = ਸੱਚ;
      ਜਦੋਂ (interact_flag) {
	    ਅੱਖਰ*ਨਿਵੇਸ਼ = readline("> ");
	    ਜੇ (ਨਿਵੇਸ਼ == 0)
		  ਰੋਕੋ;


	      /* Advance to the first input character. */
	    ਅੱਖਰ*ਪਹਿਲਾ = ਨਿਵੇਸ਼;
	    ਜਦੋਂ (*ਪਹਿਲਾ && ਖਾਲੀ_ਥਾਂ_ਹੈ(*ਪਹਿਲਾ))
		  ਪਹਿਲਾ += 1;

	    ਜੇ (ਪਹਿਲਾ[0] != 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HISTORY
		  ਜੋੜੋ_history(ਪਹਿਲਾ);
#ਜੇ_ਅੰਤ
		  invoke_command(ਪਹਿਲਾ);
	    }

	    ਮੁਕਤ(ਨਿਵੇਸ਼);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
