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
તાર્કિક stop_is_finish = ખોટું;  /* When set, $stop acts like $finish (set in main.cc). */
પૂર્ણાંક  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
સ્થિર અક્ષર* readline_stub(અચલ અક્ષર*prompt)
{
      અક્ષર buf[256];

      જો (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      જો (fgets(buf, કદ(buf), stdin)) {
	    અક્ષર*nl = buf + strlen(buf);
	    જ્યારે (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    પાછા strdup(buf);
      }

      પાછા 0;
}
#define readline(x) readline_stub(x)
#endif


સ્થિર તાર્કિક interact_flag = સાચું;

સ્થિર શૂન્ય cmd_call(unsigned argc, અચલ અક્ષર*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      જો (stop_current_scope == 0) {
	    vpip_make_મૂળ_iterator(table, ntable);

      } નહીંતર {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, કદ(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, કદ(vpiHandle)));

      unsigned errors = 0;

      માટે (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    તાર્કિક add_to_free_list = ખોટું;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    જો (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    જો (handle == 0 && argv[idx+1][0] == '"') {
		  અક્ષર*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), ખોટું);
		  add_to_free_list = સાચું;
		  મુક્ત(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    જો (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = નવું __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = સાચું;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    માટે (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  અચલ __vpiScope*scope;
		  અચલ અક્ષર*name;

		  પસંદગી (table[tmp]->get_type_code()) {

		      સ્થિતિ vpiModule:
		      સ્થિતિ vpiGenScope:
		      સ્થિતિ vpiFunction:
		      સ્થિતિ vpiTask:
		      સ્થિતિ vpiNamedBegin:
		      સ્થિતિ vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			જો (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			તોડો;

		      સ્થિતિ vpiReg:
		      સ્થિતિ vpiNet:
		      સ્થિતિ vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			જો (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			તોડો;
		  }
	    }

	    જો (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    જો (add_to_free_list)
		  vpi_free[idx] = handle;
	    નહીંતર
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      જો (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                સાચું, ખોટું,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    જો (call_handle == 0)
		  જાઓ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      માટે (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    જો (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      મુક્ત(vpi_argv);
      મુક્ત(vpi_free);
}

સ્થિર શૂન્ય cmd_cont(unsigned, અચલ અક્ષર*[])
{
      interact_flag = ખોટું;
}

સ્થિર શૂન્ય cmd_step(unsigned, અચલ અક્ષર*[])
{
      interact_flag = ખોટું;
      schedule_single_step(0);
}

સ્થિર શૂન્ય cmd_finish(unsigned, અચલ અક્ષર*[])
{
      interact_flag = ખોટું;
      schedule_finish(0);
}

સ્થિર શૂન્ય cmd_help(unsigned, અચલ અક્ષર*[]);

સ્થિર શૂન્ય cmd_list(unsigned, અચલ અક્ષર*[])
{
      __vpiHandle**table;
      unsigned ntable;

      જો (stop_current_scope == 0) {
	    vpip_make_મૂળ_iterator(table, ntable);

      } નહીંતર {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      માટે (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    અચલ __vpiScope*scope;
	    સંરચના __vpiSignal*sig;

	    પસંદગી (table[idx]->get_type_code()) {
		સ્થિતિ vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  તોડો;

		સ્થિતિ vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  તોડો;

		સ્થિતિ vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  જો ((sig->msb.get_કિંમત() == 0) && (sig->lsb.get_કિંમત() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  નહીંતર
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_કિંમત(), sig->lsb.get_કિંમત());
		  તોડો;

		સ્થિતિ vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  જો ((sig->msb.get_કિંમત() == 0) && (sig->lsb.get_કિંમત() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  નહીંતર
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_કિંમત(), sig->lsb.get_કિંમત());
		  તોડો;

		સ્થિતિ vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  તોડો;

		મૂળભૂત:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  તોડો;
	    }

      }
}

સ્થિર શૂન્ય cmd_load(unsigned argc, અચલ અક્ષર*argv[])
{
      unsigned idx;

      માટે (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

સ્થિર શૂન્ય cmd_કાઢો(unsigned, અચલ અક્ષર*[])
{
      જો (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

સ્થિર શૂન્ય cmd_ધકેલો(unsigned argc, અચલ અક્ષર* argv[])
{
      માટે (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    જો (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } નહીંતર {
		  vpip_make_મૂળ_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    માટે (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  જો (scope == 0)
			ચાલુ;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  જો (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			તોડો;
		  }
	    }

	    જો (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  પાછા;
	    }

	    stop_current_scope = child;
      }
}

સ્થિર શૂન્ય cmd_time(unsigned, અચલ અક્ષર*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

સ્થિર શૂન્ય cmd_trace(unsigned argc, અચલ અક્ષર*argv[])
{
      પ્રતિજ્ઞા(argc);
      પસંદગી (argc) {
	સ્થિતિ 1:
	    show_file_લીટી = સાચું;
	    તોડો;
	મૂળભૂત:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	સ્થિતિ 2:
	    જો ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_લીટી = સાચું;
	    } નહીંતર show_file_લીટી = ખોટું;
	    તોડો;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      જો (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_લીટી = ખોટું;
      } નહીંતર {
	    printf("Turning statement tracing %s.\n",
	           show_file_લીટી ? "on" : "off");
      }
}

સ્થિર શૂન્ય cmd_where(unsigned, અચલ અક્ષર*[])
{
      અચલ __vpiScope*cur = stop_current_scope;

      જ્યારે (cur) {
	    પસંદગી (cur->get_type_code()) {
		સ્થિતિ vpiModule:
		  printf("module %s\n", cur->scope_name());
		  તોડો;
		સ્થિતિ vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  તોડો;
		મૂળભૂત:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  તોડો;
	    }

	    cur = cur->scope;
      }
}

સ્થિર શૂન્ય cmd_unknown(unsigned, અચલ અક્ષર*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

સ્થિર સંરચના {
      અચલ અક્ષર*name;
      શૂન્ય (*proc)(unsigned argc, અચલ અક્ષર*argv[]);
      અચલ અક્ષર*summary;
} cmd_table[] = {
      { "cd",     &cmd_ધકેલો,
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
      { "pop",    &cmd_કાઢો,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ધકેલો,
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

સ્થિર શૂન્ય cmd_help(unsigned, અચલ અક્ષર*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      માટે (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


સ્થિર શૂન્ય invoke_command(અક્ષર*txt)
{
      unsigned argc = 0;
      અચલ અક્ષર**argv = const_cast<અચલ અક્ષર**>(નવું અક્ષર*[strlen(txt)/2]);

	/* Chop the line into words. */
      માટે (અક્ષર*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    જો (cp[0] == '"') {
		  અક્ષર*tmp = strchr(cp+1, '"');
		  જો (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			પાછા;
		  }

		  cp = tmp + 1;

	    } નહીંતર {
		  cp += strcspn(cp, " ");
	    }

	    જો (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      જો (argc > 0) {

	    જો (argv[0][0] == '$') {
		  જો (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } નહીંતર જો (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } નહીંતર {
			cmd_call(argc, argv);
		  }
	    } નહીંતર {
		  unsigned idx;
		  માટે (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			જો (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      તોડો;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

સ્થિર શૂન્ય invoke_command_const(અચલ અક્ષર*txt)
{
      અક્ષર *vtxt = strdup(txt);
      invoke_command(vtxt);
      મુક્ત(vtxt);
}

શૂન્ય stop_handler(પૂર્ણાંક rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      જો (stop_is_finish) {
	    vpip_set_return_કિંમત(stop_is_finish_exit_code);
	    schedule_finish(0);
	    પાછા;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = સાચું;
      જ્યારે (interact_flag) {
	    અક્ષર*આગત = readline("> ");
	    જો (આગત == 0)
		  તોડો;


	      /* Advance to the first input character. */
	    અક્ષર*first = આગત;
	    જ્યારે (*first && isspace(*first))
		  first += 1;

	    જો (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    મુક્ત(આગત);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
