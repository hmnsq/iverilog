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
தருக்க stop_is_finish = பொய்;  /* When set, $stop acts like $finish (set in main.cc). */
முழு  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
நிலையான எழுத்து* readline_stub(மாறிலி எழுத்து*prompt)
{
      எழுத்து buf[256];

      எனில் (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      எனில் (fgets(buf, அளவு(buf), stdin)) {
	    எழுத்து*nl = buf + strlen(buf);
	    போது (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    திரும்பு strdup(buf);
      }

      திரும்பு 0;
}
#define readline(x) readline_stub(x)
#endif


நிலையான தருக்க interact_flag = உண்மை;

நிலையான வெறுமை cmd_call(unsigned argc, மாறிலி எழுத்து*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      எனில் (stop_current_scope == 0) {
	    vpip_make_வேர்_iterator(table, ntable);

      } இல்லையெனில் {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, அளவு(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, அளவு(vpiHandle)));

      unsigned errors = 0;

      க்கு (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    தருக்க add_to_free_list = பொய்;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    எனில் (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    எனில் (handle == 0 && argv[idx+1][0] == '"') {
		  எழுத்து*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), பொய்);
		  add_to_free_list = உண்மை;
		  விடுவி(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    எனில் (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = புதிய __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = உண்மை;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    க்கு (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  மாறிலி __vpiScope*scope;
		  மாறிலி எழுத்து*name;

		  தேர்வு (table[tmp]->get_type_code()) {

		      நிலை vpiModule:
		      நிலை vpiGenScope:
		      நிலை vpiFunction:
		      நிலை vpiTask:
		      நிலை vpiNamedBegin:
		      நிலை vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			எனில் (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			நிறுத்து;

		      நிலை vpiReg:
		      நிலை vpiNet:
		      நிலை vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			எனில் (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			நிறுத்து;
		  }
	    }

	    எனில் (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    எனில் (add_to_free_list)
		  vpi_free[idx] = handle;
	    இல்லையெனில்
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      எனில் (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                உண்மை, பொய்,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    எனில் (call_handle == 0)
		  செல் out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      க்கு (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    எனில் (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      விடுவி(vpi_argv);
      விடுவி(vpi_free);
}

நிலையான வெறுமை cmd_cont(unsigned, மாறிலி எழுத்து*[])
{
      interact_flag = பொய்;
}

நிலையான வெறுமை cmd_step(unsigned, மாறிலி எழுத்து*[])
{
      interact_flag = பொய்;
      schedule_single_step(0);
}

நிலையான வெறுமை cmd_finish(unsigned, மாறிலி எழுத்து*[])
{
      interact_flag = பொய்;
      schedule_finish(0);
}

நிலையான வெறுமை cmd_help(unsigned, மாறிலி எழுத்து*[]);

நிலையான வெறுமை cmd_list(unsigned, மாறிலி எழுத்து*[])
{
      __vpiHandle**table;
      unsigned ntable;

      எனில் (stop_current_scope == 0) {
	    vpip_make_வேர்_iterator(table, ntable);

      } இல்லையெனில் {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      க்கு (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    மாறிலி __vpiScope*scope;
	    கட்டமைப்பு __vpiSignal*sig;

	    தேர்வு (table[idx]->get_type_code()) {
		நிலை vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  நிறுத்து;

		நிலை vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  நிறுத்து;

		நிலை vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  எனில் ((sig->msb.get_மதிப்பு() == 0) && (sig->lsb.get_மதிப்பு() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  இல்லையெனில்
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_மதிப்பு(), sig->lsb.get_மதிப்பு());
		  நிறுத்து;

		நிலை vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  எனில் ((sig->msb.get_மதிப்பு() == 0) && (sig->lsb.get_மதிப்பு() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  இல்லையெனில்
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_மதிப்பு(), sig->lsb.get_மதிப்பு());
		  நிறுத்து;

		நிலை vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  நிறுத்து;

		இயல்பு:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  நிறுத்து;
	    }

      }
}

நிலையான வெறுமை cmd_load(unsigned argc, மாறிலி எழுத்து*argv[])
{
      unsigned idx;

      க்கு (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

நிலையான வெறுமை cmd_எடு(unsigned, மாறிலி எழுத்து*[])
{
      எனில் (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

நிலையான வெறுமை cmd_தள்ளு(unsigned argc, மாறிலி எழுத்து* argv[])
{
      க்கு (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    எனில் (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } இல்லையெனில் {
		  vpip_make_வேர்_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    க்கு (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  எனில் (scope == 0)
			தொடர்;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  எனில் (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			நிறுத்து;
		  }
	    }

	    எனில் (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  திரும்பு;
	    }

	    stop_current_scope = child;
      }
}

நிலையான வெறுமை cmd_time(unsigned, மாறிலி எழுத்து*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

நிலையான வெறுமை cmd_trace(unsigned argc, மாறிலி எழுத்து*argv[])
{
      ப்ரதிஜ்ஞா(argc);
      தேர்வு (argc) {
	நிலை 1:
	    show_file_வரி = உண்மை;
	    நிறுத்து;
	இயல்பு:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	நிலை 2:
	    எனில் ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_வரி = உண்மை;
	    } இல்லையெனில் show_file_வரி = பொய்;
	    நிறுத்து;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      எனில் (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_வரி = பொய்;
      } இல்லையெனில் {
	    printf("Turning statement tracing %s.\n",
	           show_file_வரி ? "on" : "off");
      }
}

நிலையான வெறுமை cmd_where(unsigned, மாறிலி எழுத்து*[])
{
      மாறிலி __vpiScope*cur = stop_current_scope;

      போது (cur) {
	    தேர்வு (cur->get_type_code()) {
		நிலை vpiModule:
		  printf("module %s\n", cur->scope_name());
		  நிறுத்து;
		நிலை vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  நிறுத்து;
		இயல்பு:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  நிறுத்து;
	    }

	    cur = cur->scope;
      }
}

நிலையான வெறுமை cmd_unknown(unsigned, மாறிலி எழுத்து*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

நிலையான கட்டமைப்பு {
      மாறிலி எழுத்து*name;
      வெறுமை (*proc)(unsigned argc, மாறிலி எழுத்து*argv[]);
      மாறிலி எழுத்து*summary;
} cmd_table[] = {
      { "cd",     &cmd_தள்ளு,
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
      { "pop",    &cmd_எடு,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_தள்ளு,
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

நிலையான வெறுமை cmd_help(unsigned, மாறிலி எழுத்து*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      க்கு (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


நிலையான வெறுமை invoke_command(எழுத்து*txt)
{
      unsigned argc = 0;
      மாறிலி எழுத்து**argv = const_cast<மாறிலி எழுத்து**>(புதிய எழுத்து*[strlen(txt)/2]);

	/* Chop the line into words. */
      க்கு (எழுத்து*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    எனில் (cp[0] == '"') {
		  எழுத்து*tmp = strchr(cp+1, '"');
		  எனில் (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			திரும்பு;
		  }

		  cp = tmp + 1;

	    } இல்லையெனில் {
		  cp += strcspn(cp, " ");
	    }

	    எனில் (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      எனில் (argc > 0) {

	    எனில் (argv[0][0] == '$') {
		  எனில் (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } இல்லையெனில் எனில் (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } இல்லையெனில் {
			cmd_call(argc, argv);
		  }
	    } இல்லையெனில் {
		  unsigned idx;
		  க்கு (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			எனில் (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      நிறுத்து;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

நிலையான வெறுமை invoke_command_const(மாறிலி எழுத்து*txt)
{
      எழுத்து *vtxt = strdup(txt);
      invoke_command(vtxt);
      விடுவி(vtxt);
}

வெறுமை stop_handler(முழு rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      எனில் (stop_is_finish) {
	    vpip_set_return_மதிப்பு(stop_is_finish_exit_code);
	    schedule_finish(0);
	    திரும்பு;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = உண்மை;
      போது (interact_flag) {
	    எழுத்து*உள்ளீடு = readline("> ");
	    எனில் (உள்ளீடு == 0)
		  நிறுத்து;


	      /* Advance to the first input character. */
	    எழுத்து*first = உள்ளீடு;
	    போது (*first && isspace(*first))
		  first += 1;

	    எனில் (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    விடுவி(உள்ளீடு);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
