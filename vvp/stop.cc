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
ತಾರ್ಕಿಕ stop_is_finish = ಅಸತ್ಯ;  /* When set, $stop acts like $finish (set in main.cc). */
ಪೂರ್ಣಾಙ್ಕ  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
ಸ್ಥಿರ ವರ್ಣ* readline_stub(ಧ್ರುವ ವರ್ಣ*prompt)
{
      ವರ್ಣ buf[256];

      ಯದಿ (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      ಯದಿ (fgets(buf, ಪರಿಮಾಣ(buf), stdin)) {
	    ವರ್ಣ*nl = buf + strlen(buf);
	    ಯಾವತ್ (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    ಪ್ರತ್ಯಾಗಮ strdup(buf);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}
#define readline(x) readline_stub(x)
#endif


ಸ್ಥಿರ ತಾರ್ಕಿಕ interact_flag = ಸತ್ಯ;

ಸ್ಥಿರ ಶೂನ್ಯ cmd_call(unsigned argc, ಧ್ರುವ ವರ್ಣ*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      ಯದಿ (stop_current_scope == 0) {
	    vpip_make_ಬೇರು_iterator(table, ntable);

      } ಅನ್ಯಥಾ {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, ಪರಿಮಾಣ(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, ಪರಿಮಾಣ(vpiHandle)));

      unsigned errors = 0;

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    ತಾರ್ಕಿಕ add_to_free_list = ಅಸತ್ಯ;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    ಯದಿ (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    ಯದಿ (handle == 0 && argv[idx+1][0] == '"') {
		  ವರ್ಣ*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), ಅಸತ್ಯ);
		  add_to_free_list = ಸತ್ಯ;
		  ಮುಕ್ತ(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    ಯದಿ (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = ನೂತನ __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = ಸತ್ಯ;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    ಪ್ರತಿ (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ಧ್ರುವ __vpiScope*scope;
		  ಧ್ರುವ ವರ್ಣ*name;

		  ವಿಕಲ್ಪ (table[tmp]->get_type_code()) {

		      ಪಕ್ಷ vpiModule:
		      ಪಕ್ಷ vpiGenScope:
		      ಪಕ್ಷ vpiFunction:
		      ಪಕ್ಷ vpiTask:
		      ಪಕ್ಷ vpiNamedBegin:
		      ಪಕ್ಷ vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			ಯದಿ (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			ಭಙ್ಗ;

		      ಪಕ್ಷ vpiReg:
		      ಪಕ್ಷ vpiNet:
		      ಪಕ್ಷ vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			ಯದಿ (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			ಭಙ್ಗ;
		  }
	    }

	    ಯದಿ (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    ಯದಿ (add_to_free_list)
		  vpi_free[idx] = handle;
	    ಅನ್ಯಥಾ
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      ಯದಿ (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                ಸತ್ಯ, ಅಸತ್ಯ,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    ಯದಿ (call_handle == 0)
		  ಗಚ್ಛ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    ಯದಿ (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      ಮುಕ್ತ(vpi_argv);
      ಮುಕ್ತ(vpi_free);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_cont(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      interact_flag = ಅಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_step(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      interact_flag = ಅಸತ್ಯ;
      schedule_single_step(0);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_finish(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      interact_flag = ಅಸತ್ಯ;
      schedule_finish(0);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_help(unsigned, ಧ್ರುವ ವರ್ಣ*[]);

ಸ್ಥಿರ ಶೂನ್ಯ cmd_list(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      __vpiHandle**table;
      unsigned ntable;

      ಯದಿ (stop_current_scope == 0) {
	    vpip_make_ಬೇರು_iterator(table, ntable);

      } ಅನ್ಯಥಾ {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    ಧ್ರುವ __vpiScope*scope;
	    ಸಂರಚನಾ __vpiSignal*sig;

	    ವಿಕಲ್ಪ (table[idx]->get_type_code()) {
		ಪಕ್ಷ vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  ಯದಿ ((sig->msb.get_ಮೌಲ್ಯ() == 0) && (sig->lsb.get_ಮೌಲ್ಯ() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  ಅನ್ಯಥಾ
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ಮೌಲ್ಯ(), sig->lsb.get_ಮೌಲ್ಯ());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  ಯದಿ ((sig->msb.get_ಮೌಲ್ಯ() == 0) && (sig->lsb.get_ಮೌಲ್ಯ() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  ಅನ್ಯಥಾ
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ಮೌಲ್ಯ(), sig->lsb.get_ಮೌಲ್ಯ());
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  ಭಙ್ಗ;

		ಸ್ವಭಾವ:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  ಭಙ್ಗ;
	    }

      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_load(unsigned argc, ಧ್ರುವ ವರ್ಣ*argv[])
{
      unsigned idx;

      ಪ್ರತಿ (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ತೆಗೆ(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      ಯದಿ (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_ತಳ್ಳು(unsigned argc, ಧ್ರುವ ವರ್ಣ* argv[])
{
      ಪ್ರತಿ (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    ಯದಿ (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } ಅನ್ಯಥಾ {
		  vpip_make_ಬೇರು_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    ಪ್ರತಿ (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  ಯದಿ (scope == 0)
			ಅನುವರ್ತ;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  ಯದಿ (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			ಭಙ್ಗ;
		  }
	    }

	    ಯದಿ (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    stop_current_scope = child;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_time(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_trace(unsigned argc, ಧ್ರುವ ವರ್ಣ*argv[])
{
      ಪ್ರತಿಜ್ಞಾ(argc);
      ವಿಕಲ್ಪ (argc) {
	ಪಕ್ಷ 1:
	    show_file_ಸಾಲು = ಸತ್ಯ;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	ಪಕ್ಷ 2:
	    ಯದಿ ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_ಸಾಲು = ಸತ್ಯ;
	    } ಅನ್ಯಥಾ show_file_ಸಾಲು = ಅಸತ್ಯ;
	    ಭಙ್ಗ;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      ಯದಿ (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_ಸಾಲು = ಅಸತ್ಯ;
      } ಅನ್ಯಥಾ {
	    printf("Turning statement tracing %s.\n",
	           show_file_ಸಾಲು ? "on" : "off");
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_where(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      ಧ್ರುವ __vpiScope*cur = stop_current_scope;

      ಯಾವತ್ (cur) {
	    ವಿಕಲ್ಪ (cur->get_type_code()) {
		ಪಕ್ಷ vpiModule:
		  printf("module %s\n", cur->scope_name());
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  ಭಙ್ಗ;
	    }

	    cur = cur->scope;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ cmd_unknown(unsigned, ಧ್ರುವ ವರ್ಣ*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ಸ್ಥಿರ ಸಂರಚನಾ {
      ಧ್ರುವ ವರ್ಣ*name;
      ಶೂನ್ಯ (*proc)(unsigned argc, ಧ್ರುವ ವರ್ಣ*argv[]);
      ಧ್ರುವ ವರ್ಣ*summary;
} cmd_table[] = {
      { "cd",     &cmd_ತಳ್ಳು,
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
      { "pop",    &cmd_ತೆಗೆ,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ತಳ್ಳು,
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

ಸ್ಥಿರ ಶೂನ್ಯ cmd_help(unsigned, ಧ್ರುವ ವರ್ಣ*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      ಪ್ರತಿ (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ಸ್ಥಿರ ಶೂನ್ಯ invoke_command(ವರ್ಣ*txt)
{
      unsigned argc = 0;
      ಧ್ರುವ ವರ್ಣ**argv = const_cast<ಧ್ರುವ ವರ್ಣ**>(ನೂತನ ವರ್ಣ*[strlen(txt)/2]);

	/* Chop the line into words. */
      ಪ್ರತಿ (ವರ್ಣ*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    ಯದಿ (cp[0] == '"') {
		  ವರ್ಣ*tmp = strchr(cp+1, '"');
		  ಯದಿ (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			ಪ್ರತ್ಯಾಗಮ;
		  }

		  cp = tmp + 1;

	    } ಅನ್ಯಥಾ {
		  cp += strcspn(cp, " ");
	    }

	    ಯದಿ (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      ಯದಿ (argc > 0) {

	    ಯದಿ (argv[0][0] == '$') {
		  ಯದಿ (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } ಅನ್ಯಥಾ {
			cmd_call(argc, argv);
		  }
	    } ಅನ್ಯಥಾ {
		  unsigned idx;
		  ಪ್ರತಿ (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			ಯದಿ (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      ಭಙ್ಗ;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

ಸ್ಥಿರ ಶೂನ್ಯ invoke_command_const(ಧ್ರುವ ವರ್ಣ*txt)
{
      ವರ್ಣ *vtxt = strdup(txt);
      invoke_command(vtxt);
      ಮುಕ್ತ(vtxt);
}

ಶೂನ್ಯ stop_handler(ಪೂರ್ಣಾಙ್ಕ rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      ಯದಿ (stop_is_finish) {
	    vpip_set_return_ಮೌಲ್ಯ(stop_is_finish_exit_code);
	    schedule_finish(0);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = ಸತ್ಯ;
      ಯಾವತ್ (interact_flag) {
	    ವರ್ಣ*ಒಳಹರಿವು = readline("> ");
	    ಯದಿ (ಒಳಹರಿವು == 0)
		  ಭಙ್ಗ;


	      /* Advance to the first input character. */
	    ವರ್ಣ*first = ಒಳಹರಿವು;
	    ಯಾವತ್ (*first && isspace(*first))
		  first += 1;

	    ಯದಿ (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    ಮುಕ್ತ(ಒಳಹರಿವು);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
