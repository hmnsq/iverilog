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
तार्किक stop_is_finish = असत्य;  /* When set, $stop acts like $finish (set in main.cc). */
पूर्णाङ्क  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
स्थिर वर्ण* readline_stub(ध्रुव वर्ण*prompt)
{
      वर्ण buf[256];

      यदि (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      यदि (fgets(buf, परिमाण(buf), stdin)) {
	    वर्ण*nl = buf + strlen(buf);
	    यावत् (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    प्रत्यागम strdup(buf);
      }

      प्रत्यागम 0;
}
#define readline(x) readline_stub(x)
#endif


स्थिर तार्किक interact_flag = सत्य;

स्थिर शून्य cmd_call(unsigned argc, ध्रुव वर्ण*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      यदि (stop_current_scope == 0) {
	    vpip_make_मूळ_iterator(table, ntable);

      } अन्यथा {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, परिमाण(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, परिमाण(vpiHandle)));

      unsigned errors = 0;

      प्रति (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    तार्किक add_to_free_list = असत्य;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    यदि (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    यदि (handle == 0 && argv[idx+1][0] == '"') {
		  वर्ण*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), असत्य);
		  add_to_free_list = सत्य;
		  मुक्त(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    यदि (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = नूतन __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = सत्य;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    प्रति (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ध्रुव __vpiScope*scope;
		  ध्रुव वर्ण*name;

		  विकल्प (table[tmp]->get_type_code()) {

		      पक्ष vpiModule:
		      पक्ष vpiGenScope:
		      पक्ष vpiFunction:
		      पक्ष vpiTask:
		      पक्ष vpiNamedBegin:
		      पक्ष vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			यदि (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			भङ्ग;

		      पक्ष vpiReg:
		      पक्ष vpiNet:
		      पक्ष vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			यदि (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			भङ्ग;
		  }
	    }

	    यदि (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    यदि (add_to_free_list)
		  vpi_free[idx] = handle;
	    अन्यथा
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      यदि (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                सत्य, असत्य,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    यदि (call_handle == 0)
		  गच्छ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      प्रति (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    यदि (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      मुक्त(vpi_argv);
      मुक्त(vpi_free);
}

स्थिर शून्य cmd_cont(unsigned, ध्रुव वर्ण*[])
{
      interact_flag = असत्य;
}

स्थिर शून्य cmd_step(unsigned, ध्रुव वर्ण*[])
{
      interact_flag = असत्य;
      schedule_single_step(0);
}

स्थिर शून्य cmd_finish(unsigned, ध्रुव वर्ण*[])
{
      interact_flag = असत्य;
      schedule_finish(0);
}

स्थिर शून्य cmd_help(unsigned, ध्रुव वर्ण*[]);

स्थिर शून्य cmd_list(unsigned, ध्रुव वर्ण*[])
{
      __vpiHandle**table;
      unsigned ntable;

      यदि (stop_current_scope == 0) {
	    vpip_make_मूळ_iterator(table, ntable);

      } अन्यथा {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      प्रति (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    ध्रुव __vpiScope*scope;
	    संरचना __vpiSignal*sig;

	    विकल्प (table[idx]->get_type_code()) {
		पक्ष vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  भङ्ग;

		पक्ष vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  यदि ((sig->msb.get_मूल्य() == 0) && (sig->lsb.get_मूल्य() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  अन्यथा
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_मूल्य(), sig->lsb.get_मूल्य());
		  भङ्ग;

		पक्ष vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  यदि ((sig->msb.get_मूल्य() == 0) && (sig->lsb.get_मूल्य() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  अन्यथा
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_मूल्य(), sig->lsb.get_मूल्य());
		  भङ्ग;

		पक्ष vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  भङ्ग;

		स्वभाव:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  भङ्ग;
	    }

      }
}

स्थिर शून्य cmd_load(unsigned argc, ध्रुव वर्ण*argv[])
{
      unsigned idx;

      प्रति (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

स्थिर शून्य cmd_काढा(unsigned, ध्रुव वर्ण*[])
{
      यदि (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

स्थिर शून्य cmd_ढकला(unsigned argc, ध्रुव वर्ण* argv[])
{
      प्रति (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    यदि (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } अन्यथा {
		  vpip_make_मूळ_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    प्रति (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  यदि (scope == 0)
			अनुवर्त;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  यदि (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			भङ्ग;
		  }
	    }

	    यदि (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  प्रत्यागम;
	    }

	    stop_current_scope = child;
      }
}

स्थिर शून्य cmd_time(unsigned, ध्रुव वर्ण*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

स्थिर शून्य cmd_trace(unsigned argc, ध्रुव वर्ण*argv[])
{
      प्रतिज्ञा(argc);
      विकल्प (argc) {
	पक्ष 1:
	    show_file_ओळ = सत्य;
	    भङ्ग;
	स्वभाव:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	पक्ष 2:
	    यदि ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_ओळ = सत्य;
	    } अन्यथा show_file_ओळ = असत्य;
	    भङ्ग;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      यदि (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_ओळ = असत्य;
      } अन्यथा {
	    printf("Turning statement tracing %s.\n",
	           show_file_ओळ ? "on" : "off");
      }
}

स्थिर शून्य cmd_where(unsigned, ध्रुव वर्ण*[])
{
      ध्रुव __vpiScope*cur = stop_current_scope;

      यावत् (cur) {
	    विकल्प (cur->get_type_code()) {
		पक्ष vpiModule:
		  printf("module %s\n", cur->scope_name());
		  भङ्ग;
		पक्ष vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  भङ्ग;
		स्वभाव:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  भङ्ग;
	    }

	    cur = cur->scope;
      }
}

स्थिर शून्य cmd_unknown(unsigned, ध्रुव वर्ण*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

स्थिर संरचना {
      ध्रुव वर्ण*name;
      शून्य (*proc)(unsigned argc, ध्रुव वर्ण*argv[]);
      ध्रुव वर्ण*summary;
} cmd_table[] = {
      { "cd",     &cmd_ढकला,
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
      { "pop",    &cmd_काढा,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ढकला,
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

स्थिर शून्य cmd_help(unsigned, ध्रुव वर्ण*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      प्रति (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


स्थिर शून्य invoke_command(वर्ण*txt)
{
      unsigned argc = 0;
      ध्रुव वर्ण**argv = const_cast<ध्रुव वर्ण**>(नूतन वर्ण*[strlen(txt)/2]);

	/* Chop the line into words. */
      प्रति (वर्ण*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    यदि (cp[0] == '"') {
		  वर्ण*tmp = strchr(cp+1, '"');
		  यदि (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			प्रत्यागम;
		  }

		  cp = tmp + 1;

	    } अन्यथा {
		  cp += strcspn(cp, " ");
	    }

	    यदि (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      यदि (argc > 0) {

	    यदि (argv[0][0] == '$') {
		  यदि (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } अन्यथा यदि (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } अन्यथा {
			cmd_call(argc, argv);
		  }
	    } अन्यथा {
		  unsigned idx;
		  प्रति (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			यदि (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      भङ्ग;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

स्थिर शून्य invoke_command_const(ध्रुव वर्ण*txt)
{
      वर्ण *vtxt = strdup(txt);
      invoke_command(vtxt);
      मुक्त(vtxt);
}

शून्य stop_handler(पूर्णाङ्क rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      यदि (stop_is_finish) {
	    vpip_set_return_मूल्य(stop_is_finish_exit_code);
	    schedule_finish(0);
	    प्रत्यागम;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = सत्य;
      यावत् (interact_flag) {
	    वर्ण*आदान = readline("> ");
	    यदि (आदान == 0)
		  भङ्ग;


	      /* Advance to the first input character. */
	    वर्ण*first = आदान;
	    यावत् (*first && isspace(*first))
		  first += 1;

	    यदि (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    मुक्त(आदान);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
