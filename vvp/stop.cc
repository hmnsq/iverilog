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
ତାର୍କିକ stop_is_finish = ମିଥ୍ୟା;  /* When set, $stop acts like $finish (set in main.cc). */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
ସ୍ଥିର ଅକ୍ଷର* readline_stub(ଅଚଳ ଅକ୍ଷର*prompt)
{
      ଅକ୍ଷର buf[256];

      ଯଦି (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      ଯଦି (fgets(buf, ଆକାର(buf), stdin)) {
	    ଅକ୍ଷର*nl = buf + strlen(buf);
	    ଯେତେବେଳେ (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    ଫେରନ୍ତୁ strdup(buf);
      }

      ଫେରନ୍ତୁ 0;
}
#define readline(x) readline_stub(x)
#endif


ସ୍ଥିର ତାର୍କିକ interact_flag = ସତ୍ୟ;

ସ୍ଥିର ଶୂନ୍ୟ cmd_call(unsigned argc, ଅଚଳ ଅକ୍ଷର*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      ଯଦି (stop_current_scope == 0) {
	    vpip_make_ମୂଳ_iterator(table, ntable);

      } ଅନ୍ୟଥା {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, ଆକାର(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, ଆକାର(vpiHandle)));

      unsigned errors = 0;

      ପାଇଁ (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    ତାର୍କିକ add_to_free_list = ମିଥ୍ୟା;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    ଯଦି (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    ଯଦି (handle == 0 && argv[idx+1][0] == '"') {
		  ଅକ୍ଷର*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), ମିଥ୍ୟା);
		  add_to_free_list = ସତ୍ୟ;
		  ମୁକ୍ତ(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    ଯଦି (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = ନୂତନ __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = ସତ୍ୟ;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    ପାଇଁ (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ଅଚଳ __vpiScope*scope;
		  ଅଚଳ ଅକ୍ଷର*name;

		  ବାଛ (table[tmp]->get_type_code()) {

		      ସ୍ଥିତି vpiModule:
		      ସ୍ଥିତି vpiGenScope:
		      ସ୍ଥିତି vpiFunction:
		      ସ୍ଥିତି vpiTask:
		      ସ୍ଥିତି vpiNamedBegin:
		      ସ୍ଥିତି vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			ଯଦି (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			ଭାଙ୍ଗ;

		      ସ୍ଥିତି vpiReg:
		      ସ୍ଥିତି vpiNet:
		      ସ୍ଥିତି vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			ଯଦି (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			ଭାଙ୍ଗ;
		  }
	    }

	    ଯଦି (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    ଯଦି (add_to_free_list)
		  vpi_free[idx] = handle;
	    ଅନ୍ୟଥା
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      ଯଦି (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                ସତ୍ୟ, ମିଥ୍ୟା,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    ଯଦି (call_handle == 0)
		  ଯାଅ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      ପାଇଁ (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    ଯଦି (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      ମୁକ୍ତ(vpi_argv);
      ମୁକ୍ତ(vpi_free);
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_cont(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      interact_flag = ମିଥ୍ୟା;
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_step(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      interact_flag = ମିଥ୍ୟା;
      schedule_single_step(0);
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_finish(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      interact_flag = ମିଥ୍ୟା;
      schedule_finish(0);
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_help(unsigned, ଅଚଳ ଅକ୍ଷର*[]);

ସ୍ଥିର ଶୂନ୍ୟ cmd_list(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      __vpiHandle**table;
      unsigned ntable;

      ଯଦି (stop_current_scope == 0) {
	    vpip_make_ମୂଳ_iterator(table, ntable);

      } ଅନ୍ୟଥା {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      ପାଇଁ (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    ଅଚଳ __vpiScope*scope;
	    ସଂରଚନା __vpiSignal*sig;

	    ବାଛ (table[idx]->get_type_code()) {
		ସ୍ଥିତି vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  ଯଦି ((sig->msb.get_ମୂଲ୍ୟ() == 0) && (sig->lsb.get_ମୂଲ୍ୟ() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  ଅନ୍ୟଥା
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ମୂଲ୍ୟ(), sig->lsb.get_ମୂଲ୍ୟ());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  ଯଦି ((sig->msb.get_ମୂଲ୍ୟ() == 0) && (sig->lsb.get_ମୂଲ୍ୟ() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  ଅନ୍ୟଥା
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_ମୂଲ୍ୟ(), sig->lsb.get_ମୂଲ୍ୟ());
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  ଭାଙ୍ଗ;
	    }

      }
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_load(unsigned argc, ଅଚଳ ଅକ୍ଷର*argv[])
{
      unsigned idx;

      ପାଇଁ (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_କାଢ଼(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      ଯଦି (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_ଠେଲ(unsigned argc, ଅଚଳ ଅକ୍ଷର* argv[])
{
      ପାଇଁ (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    ଯଦି (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } ଅନ୍ୟଥା {
		  vpip_make_ମୂଳ_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    ପାଇଁ (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  ଯଦି (scope == 0)
			ଜାରି;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  ଯଦି (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			ଭାଙ୍ଗ;
		  }
	    }

	    ଯଦି (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  ଫେରନ୍ତୁ;
	    }

	    stop_current_scope = child;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_time(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_trace(unsigned argc, ଅଚଳ ଅକ୍ଷର*argv[])
{
      ପ୍ରତିଜ୍ଞା(argc);
      ବାଛ (argc) {
	ସ୍ଥିତି 1:
	    show_file_ଧାଡ଼ି = ସତ୍ୟ;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	ସ୍ଥିତି 2:
	    ଯଦି ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_ଧାଡ଼ି = ସତ୍ୟ;
	    } ଅନ୍ୟଥା show_file_ଧାଡ଼ି = ମିଥ୍ୟା;
	    ଭାଙ୍ଗ;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      ଯଦି (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_ଧାଡ଼ି = ମିଥ୍ୟା;
      } ଅନ୍ୟଥା {
	    printf("Turning statement tracing %s.\n",
	           show_file_ଧାଡ଼ି ? "on" : "off");
      }
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_where(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      ଅଚଳ __vpiScope*cur = stop_current_scope;

      ଯେତେବେଳେ (cur) {
	    ବାଛ (cur->get_type_code()) {
		ସ୍ଥିତି vpiModule:
		  printf("module %s\n", cur->scope_name());
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  ଭାଙ୍ଗ;
	    }

	    cur = cur->scope;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ cmd_unknown(unsigned, ଅଚଳ ଅକ୍ଷର*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ସ୍ଥିର ସଂରଚନା {
      ଅଚଳ ଅକ୍ଷର*name;
      ଶୂନ୍ୟ (*proc)(unsigned argc, ଅଚଳ ଅକ୍ଷର*argv[]);
      ଅଚଳ ଅକ୍ଷର*summary;
} cmd_table[] = {
      { "cd",     &cmd_ଠେଲ,
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
      { "pop",    &cmd_କାଢ଼,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ଠେଲ,
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

ସ୍ଥିର ଶୂନ୍ୟ cmd_help(unsigned, ଅଚଳ ଅକ୍ଷର*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      ପାଇଁ (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ସ୍ଥିର ଶୂନ୍ୟ invoke_command(ଅକ୍ଷର*txt)
{
      unsigned argc = 0;
      ଅଚଳ ଅକ୍ଷର**argv = const_cast<ଅଚଳ ଅକ୍ଷର**>(ନୂତନ ଅକ୍ଷର*[strlen(txt)/2]);

	/* Chop the line into words. */
      ପାଇଁ (ଅକ୍ଷର*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    ଯଦି (cp[0] == '"') {
		  ଅକ୍ଷର*tmp = strchr(cp+1, '"');
		  ଯଦି (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			ଫେରନ୍ତୁ;
		  }

		  cp = tmp + 1;

	    } ଅନ୍ୟଥା {
		  cp += strcspn(cp, " ");
	    }

	    ଯଦି (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      ଯଦି (argc > 0) {

	    ଯଦି (argv[0][0] == '$') {
		  ଯଦି (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } ଅନ୍ୟଥା ଯଦି (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } ଅନ୍ୟଥା {
			cmd_call(argc, argv);
		  }
	    } ଅନ୍ୟଥା {
		  unsigned idx;
		  ପାଇଁ (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			ଯଦି (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      ଭାଙ୍ଗ;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

ସ୍ଥିର ଶୂନ୍ୟ invoke_command_const(ଅଚଳ ଅକ୍ଷର*txt)
{
      ଅକ୍ଷର *vtxt = strdup(txt);
      invoke_command(vtxt);
      ମୁକ୍ତ(vtxt);
}

ଶୂନ୍ୟ stop_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      ଯଦି (stop_is_finish) {
	    vpip_set_return_ମୂଲ୍ୟ(stop_is_finish_exit_code);
	    schedule_finish(0);
	    ଫେରନ୍ତୁ;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = ସତ୍ୟ;
      ଯେତେବେଳେ (interact_flag) {
	    ଅକ୍ଷର*ନିବେଶ = readline("> ");
	    ଯଦି (ନିବେଶ == 0)
		  ଭାଙ୍ଗ;


	      /* Advance to the first input character. */
	    ଅକ୍ଷର*first = ନିବେଶ;
	    ଯେତେବେଳେ (*first && isspace(*first))
		  first += 1;

	    ଯଦି (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    ମୁକ୍ତ(ନିବେଶ);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
