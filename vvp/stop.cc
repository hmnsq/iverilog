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
#ifdef USE_READLINE
# include  <readline/readline.h>
#endif
#ifdef USE_HISTORY
# include  <readline/history.h>
#endif
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
തർക്കം stop_is_finish = അസത്യം;  /* When set, $stop acts like $finish (set in main.cc). */
പൂർണ്ണസംഖ്യ  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
സ്ഥിതം അക്ഷരം* readline_stub(സ്ഥിരം അക്ഷരം*prompt)
{
      അക്ഷരം buf[256];

      എങ്കിൽ (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      എങ്കിൽ (fgets(buf, വലുപ്പം(buf), stdin)) {
	    അക്ഷരം*nl = buf + strlen(buf);
	    ആകുമ്പോൾ (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    തിരികെ strdup(buf);
      }

      തിരികെ 0;
}
#define readline(x) readline_stub(x)
#endif


സ്ഥിതം തർക്കം interact_flag = സത്യം;

സ്ഥിതം ശൂന്യം cmd_call(unsigned argc, സ്ഥിരം അക്ഷരം*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      എങ്കിൽ (stop_current_scope == 0) {
	    vpip_make_വേര്_iterator(table, ntable);

      } അല്ലെങ്കിൽ {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, വലുപ്പം(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, വലുപ്പം(vpiHandle)));

      unsigned errors = 0;

      വേണ്ടി (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    തർക്കം add_to_free_list = അസത്യം;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    എങ്കിൽ (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    എങ്കിൽ (handle == 0 && argv[idx+1][0] == '"') {
		  അക്ഷരം*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), അസത്യം);
		  add_to_free_list = സത്യം;
		  സ്വതന്ത്രം(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    എങ്കിൽ (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = പുതിയ __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = സത്യം;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    വേണ്ടി (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  സ്ഥിരം __vpiScope*scope;
		  സ്ഥിരം അക്ഷരം*name;

		  തിരഞ്ഞെടുപ്പ് (table[tmp]->get_type_code()) {

		      അവസ്ഥ vpiModule:
		      അവസ്ഥ vpiGenScope:
		      അവസ്ഥ vpiFunction:
		      അവസ്ഥ vpiTask:
		      അവസ്ഥ vpiNamedBegin:
		      അവസ്ഥ vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			എങ്കിൽ (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			നിർത്തുക;

		      അവസ്ഥ vpiReg:
		      അവസ്ഥ vpiNet:
		      അവസ്ഥ vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			എങ്കിൽ (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			നിർത്തുക;
		  }
	    }

	    എങ്കിൽ (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    എങ്കിൽ (add_to_free_list)
		  vpi_free[idx] = handle;
	    അല്ലെങ്കിൽ
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      എങ്കിൽ (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                സത്യം, അസത്യം,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    എങ്കിൽ (call_handle == 0)
		  പോകുക out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      വേണ്ടി (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    എങ്കിൽ (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      സ്വതന്ത്രം(vpi_argv);
      സ്വതന്ത്രം(vpi_free);
}

സ്ഥിതം ശൂന്യം cmd_cont(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      interact_flag = അസത്യം;
}

സ്ഥിതം ശൂന്യം cmd_step(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      interact_flag = അസത്യം;
      schedule_single_step(0);
}

സ്ഥിതം ശൂന്യം cmd_finish(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      interact_flag = അസത്യം;
      schedule_finish(0);
}

സ്ഥിതം ശൂന്യം cmd_help(unsigned, സ്ഥിരം അക്ഷരം*[]);

സ്ഥിതം ശൂന്യം cmd_list(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      __vpiHandle**table;
      unsigned ntable;

      എങ്കിൽ (stop_current_scope == 0) {
	    vpip_make_വേര്_iterator(table, ntable);

      } അല്ലെങ്കിൽ {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      വേണ്ടി (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    സ്ഥിരം __vpiScope*scope;
	    ഘടന __vpiSignal*sig;

	    തിരഞ്ഞെടുപ്പ് (table[idx]->get_type_code()) {
		അവസ്ഥ vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  നിർത്തുക;

		അവസ്ഥ vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  നിർത്തുക;

		അവസ്ഥ vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  എങ്കിൽ ((sig->msb.get_മൂല്യം() == 0) && (sig->lsb.get_മൂല്യം() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  അല്ലെങ്കിൽ
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_മൂല്യം(), sig->lsb.get_മൂല്യം());
		  നിർത്തുക;

		അവസ്ഥ vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  എങ്കിൽ ((sig->msb.get_മൂല്യം() == 0) && (sig->lsb.get_മൂല്യം() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  അല്ലെങ്കിൽ
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_മൂല്യം(), sig->lsb.get_മൂല്യം());
		  നിർത്തുക;

		അവസ്ഥ vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  നിർത്തുക;
	    }

      }
}

സ്ഥിതം ശൂന്യം cmd_load(unsigned argc, സ്ഥിരം അക്ഷരം*argv[])
{
      unsigned idx;

      വേണ്ടി (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

സ്ഥിതം ശൂന്യം cmd_എടുക്കുക(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      എങ്കിൽ (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

സ്ഥിതം ശൂന്യം cmd_തള്ളുക(unsigned argc, സ്ഥിരം അക്ഷരം* argv[])
{
      വേണ്ടി (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    എങ്കിൽ (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } അല്ലെങ്കിൽ {
		  vpip_make_വേര്_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    വേണ്ടി (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  എങ്കിൽ (scope == 0)
			തുടരുക;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  എങ്കിൽ (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			നിർത്തുക;
		  }
	    }

	    എങ്കിൽ (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  തിരികെ;
	    }

	    stop_current_scope = child;
      }
}

സ്ഥിതം ശൂന്യം cmd_time(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

സ്ഥിതം ശൂന്യം cmd_trace(unsigned argc, സ്ഥിരം അക്ഷരം*argv[])
{
      പ്രതിജ്ഞാ(argc);
      തിരഞ്ഞെടുപ്പ് (argc) {
	അവസ്ഥ 1:
	    show_file_വരി = സത്യം;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	അവസ്ഥ 2:
	    എങ്കിൽ ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_വരി = സത്യം;
	    } അല്ലെങ്കിൽ show_file_വരി = അസത്യം;
	    നിർത്തുക;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      എങ്കിൽ (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_വരി = അസത്യം;
      } അല്ലെങ്കിൽ {
	    printf("Turning statement tracing %s.\n",
	           show_file_വരി ? "on" : "off");
      }
}

സ്ഥിതം ശൂന്യം cmd_where(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      സ്ഥിരം __vpiScope*cur = stop_current_scope;

      ആകുമ്പോൾ (cur) {
	    തിരഞ്ഞെടുപ്പ് (cur->get_type_code()) {
		അവസ്ഥ vpiModule:
		  printf("module %s\n", cur->scope_name());
		  നിർത്തുക;
		അവസ്ഥ vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  നിർത്തുക;
	    }

	    cur = cur->scope;
      }
}

സ്ഥിതം ശൂന്യം cmd_unknown(unsigned, സ്ഥിരം അക്ഷരം*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

സ്ഥിതം ഘടന {
      സ്ഥിരം അക്ഷരം*name;
      ശൂന്യം (*proc)(unsigned argc, സ്ഥിരം അക്ഷരം*argv[]);
      സ്ഥിരം അക്ഷരം*summary;
} cmd_table[] = {
      { "cd",     &cmd_തള്ളുക,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_finish,
        "Synonym for finish."},
      { "finish", &cmd_finish,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_load,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_എടുക്കുക,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_തള്ളുക,
        "Descend into the named scope."},
      { "step",   &cmd_step,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_time,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

സ്ഥിതം ശൂന്യം cmd_help(unsigned, സ്ഥിരം അക്ഷരം*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      വേണ്ടി (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


സ്ഥിതം ശൂന്യം invoke_command(അക്ഷരം*txt)
{
      unsigned argc = 0;
      സ്ഥിരം അക്ഷരം**argv = const_cast<സ്ഥിരം അക്ഷരം**>(പുതിയ അക്ഷരം*[strlen(txt)/2]);

	/* Chop the line into words. */
      വേണ്ടി (അക്ഷരം*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    എങ്കിൽ (cp[0] == '"') {
		  അക്ഷരം*tmp = strchr(cp+1, '"');
		  എങ്കിൽ (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			തിരികെ;
		  }

		  cp = tmp + 1;

	    } അല്ലെങ്കിൽ {
		  cp += strcspn(cp, " ");
	    }

	    എങ്കിൽ (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      എങ്കിൽ (argc > 0) {

	    എങ്കിൽ (argv[0][0] == '$') {
		  എങ്കിൽ (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } അല്ലെങ്കിൽ {
			cmd_call(argc, argv);
		  }
	    } അല്ലെങ്കിൽ {
		  unsigned idx;
		  വേണ്ടി (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			എങ്കിൽ (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      നിർത്തുക;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

സ്ഥിതം ശൂന്യം invoke_command_const(സ്ഥിരം അക്ഷരം*txt)
{
      അക്ഷരം *vtxt = strdup(txt);
      invoke_command(vtxt);
      സ്വതന്ത്രം(vtxt);
}

ശൂന്യം stop_handler(പൂർണ്ണസംഖ്യ rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      എങ്കിൽ (stop_is_finish) {
	    vpip_set_return_മൂല്യം(stop_is_finish_exit_code);
	    schedule_finish(0);
	    തിരികെ;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = സത്യം;
      ആകുമ്പോൾ (interact_flag) {
	    അക്ഷരം*നിവേശം = readline("> ");
	    എങ്കിൽ (നിവേശം == 0)
		  നിർത്തുക;


	      /* Advance to the first input character. */
	    അക്ഷരം*first = നിവേശം;
	    ആകുമ്പോൾ (*first && isspace(*first))
		  first += 1;

	    എങ്കിൽ (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    സ്വതന്ത്രം(നിവേശം);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
