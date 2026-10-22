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
তার্কিক stop_is_finish = মিছা;  /* When set, $stop acts like $finish (set in main.cc). */
পূর্ণাঙ্ক  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
স্থির ৱর্ণ* readline_stub(ধ্রুৱ ৱর্ণ*prompt)
{
      ৱর্ণ buf[256];

      যদি (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      যদি (fgets(buf, পরিমাণ(buf), stdin)) {
	    ৱর্ণ*nl = buf + strlen(buf);
	    যেতিয়ালৈ (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    ঘূৰাই strdup(buf);
      }

      ঘূৰাই 0;
}
#define readline(x) readline_stub(x)
#endif


স্থির তার্কিক interact_flag = সঁচা;

স্থির শূন্য cmd_call(unsigned argc, ধ্রুৱ ৱর্ণ*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      যদি (stop_current_scope == 0) {
	    vpip_make_মূল_iterator(table, ntable);

      } অন্যথা {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, পরিমাণ(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, পরিমাণ(vpiHandle)));

      unsigned errors = 0;

      বাবে (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    তার্কিক add_to_free_list = মিছা;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    যদি (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    যদি (handle == 0 && argv[idx+1][0] == '"') {
		  ৱর্ণ*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), মিছা);
		  add_to_free_list = সঁচা;
		  মুক্ত(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    যদি (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = নূতন __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = সঁচা;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    বাবে (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ধ্রুৱ __vpiScope*scope;
		  ধ্রুৱ ৱর্ণ*name;

		  ৱিকল্প (table[tmp]->get_type_code()) {

		      পক্ষ vpiModule:
		      পক্ষ vpiGenScope:
		      পক্ষ vpiFunction:
		      পক্ষ vpiTask:
		      পক্ষ vpiNamedBegin:
		      পক্ষ vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			যদি (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			ভঙ্গ;

		      পক্ষ vpiReg:
		      পক্ষ vpiNet:
		      পক্ষ vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			যদি (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			ভঙ্গ;
		  }
	    }

	    যদি (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    যদি (add_to_free_list)
		  vpi_free[idx] = handle;
	    অন্যথা
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      যদি (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                সঁচা, মিছা,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    যদি (call_handle == 0)
		  গচ্ছ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      বাবে (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    যদি (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      মুক্ত(vpi_argv);
      মুক্ত(vpi_free);
}

স্থির শূন্য cmd_cont(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      interact_flag = মিছা;
}

স্থির শূন্য cmd_step(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      interact_flag = মিছা;
      schedule_single_step(0);
}

স্থির শূন্য cmd_finish(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      interact_flag = মিছা;
      schedule_finish(0);
}

স্থির শূন্য cmd_help(unsigned, ধ্রুৱ ৱর্ণ*[]);

স্থির শূন্য cmd_list(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      __vpiHandle**table;
      unsigned ntable;

      যদি (stop_current_scope == 0) {
	    vpip_make_মূল_iterator(table, ntable);

      } অন্যথা {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      বাবে (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    ধ্রুৱ __vpiScope*scope;
	    সংরচনা __vpiSignal*sig;

	    ৱিকল্প (table[idx]->get_type_code()) {
		পক্ষ vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  ভঙ্গ;

		পক্ষ vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  ভঙ্গ;

		পক্ষ vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  যদি ((sig->msb.get_মান() == 0) && (sig->lsb.get_মান() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  অন্যথা
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_মান(), sig->lsb.get_মান());
		  ভঙ্গ;

		পক্ষ vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  যদি ((sig->msb.get_মান() == 0) && (sig->lsb.get_মান() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  অন্যথা
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_মান(), sig->lsb.get_মান());
		  ভঙ্গ;

		পক্ষ vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  ভঙ্গ;

		স্ৱভাৱ:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  ভঙ্গ;
	    }

      }
}

স্থির শূন্য cmd_load(unsigned argc, ধ্রুৱ ৱর্ণ*argv[])
{
      unsigned idx;

      বাবে (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

স্থির শূন্য cmd_বেৰ_কৰো(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      যদি (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

স্থির শূন্য cmd_ঠেলো(unsigned argc, ধ্রুৱ ৱর্ণ* argv[])
{
      বাবে (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    যদি (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } অন্যথা {
		  vpip_make_মূল_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    বাবে (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  যদি (scope == 0)
			অনুৱর্ত;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  যদি (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			ভঙ্গ;
		  }
	    }

	    যদি (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  ঘূৰাই;
	    }

	    stop_current_scope = child;
      }
}

স্থির শূন্য cmd_time(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

স্থির শূন্য cmd_trace(unsigned argc, ধ্রুৱ ৱর্ণ*argv[])
{
      প্রতিজ্ঞা(argc);
      ৱিকল্প (argc) {
	পক্ষ 1:
	    show_file_সাৰি = সঁচা;
	    ভঙ্গ;
	স্ৱভাৱ:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	পক্ষ 2:
	    যদি ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_সাৰি = সঁচা;
	    } অন্যথা show_file_সাৰি = মিছা;
	    ভঙ্গ;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      যদি (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_সাৰি = মিছা;
      } অন্যথা {
	    printf("Turning statement tracing %s.\n",
	           show_file_সাৰি ? "on" : "off");
      }
}

স্থির শূন্য cmd_where(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      ধ্রুৱ __vpiScope*cur = stop_current_scope;

      যেতিয়ালৈ (cur) {
	    ৱিকল্প (cur->get_type_code()) {
		পক্ষ vpiModule:
		  printf("module %s\n", cur->scope_name());
		  ভঙ্গ;
		পক্ষ vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  ভঙ্গ;
		স্ৱভাৱ:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  ভঙ্গ;
	    }

	    cur = cur->scope;
      }
}

স্থির শূন্য cmd_unknown(unsigned, ধ্রুৱ ৱর্ণ*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

স্থির সংরচনা {
      ধ্রুৱ ৱর্ণ*name;
      শূন্য (*proc)(unsigned argc, ধ্রুৱ ৱর্ণ*argv[]);
      ধ্রুৱ ৱর্ণ*summary;
} cmd_table[] = {
      { "cd",     &cmd_ঠেলো,
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
      { "pop",    &cmd_বেৰ_কৰো,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ঠেলো,
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

স্থির শূন্য cmd_help(unsigned, ধ্রুৱ ৱর্ণ*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      বাবে (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


স্থির শূন্য invoke_command(ৱর্ণ*txt)
{
      unsigned argc = 0;
      ধ্রুৱ ৱর্ণ**argv = const_cast<ধ্রুৱ ৱর্ণ**>(নূতন ৱর্ণ*[strlen(txt)/2]);

	/* Chop the line into words. */
      বাবে (ৱর্ণ*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    যদি (cp[0] == '"') {
		  ৱর্ণ*tmp = strchr(cp+1, '"');
		  যদি (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			ঘূৰাই;
		  }

		  cp = tmp + 1;

	    } অন্যথা {
		  cp += strcspn(cp, " ");
	    }

	    যদি (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      যদি (argc > 0) {

	    যদি (argv[0][0] == '$') {
		  যদি (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } অন্যথা যদি (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } অন্যথা {
			cmd_call(argc, argv);
		  }
	    } অন্যথা {
		  unsigned idx;
		  বাবে (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			যদি (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      ভঙ্গ;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

স্থির শূন্য invoke_command_const(ধ্রুৱ ৱর্ণ*txt)
{
      ৱর্ণ *vtxt = strdup(txt);
      invoke_command(vtxt);
      মুক্ত(vtxt);
}

শূন্য stop_handler(পূর্ণাঙ্ক rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      যদি (stop_is_finish) {
	    vpip_set_return_মান(stop_is_finish_exit_code);
	    schedule_finish(0);
	    ঘূৰাই;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = সঁচা;
      যেতিয়ালৈ (interact_flag) {
	    ৱর্ণ*প্ৰবেশ = readline("> ");
	    যদি (প্ৰবেশ == 0)
		  ভঙ্গ;


	      /* Advance to the first input character. */
	    ৱর্ণ*first = প্ৰবেশ;
	    যেতিয়ালৈ (*first && isspace(*first))
		  first += 1;

	    যদি (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    মুক্ত(প্ৰবেশ);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
