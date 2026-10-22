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
bool stop_is_finish = false;  /* When set, $stop acts like $finish (set in main.cc). */
ຈຳນວນເຕັມ  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
static var_na* readline_stub(const var_na*prompt)
{
      var_na buf[256];

      ຖ້າ (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      ຖ້າ (fgets(buf, parimaa_na(buf), stdin)) {
	    var_na*nl = buf + strlen(buf);
	    ໃນຂະນະທີ່ (nl > buf && isspace(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    ສົ່ງຄືນ strdup(buf);
      }

      ສົ່ງຄືນ 0;
}
#define readline(x) readline_stub(x)
#endif


static bool interact_flag = true;

static void cmd_call(unsigned argc, const var_na*argv[])
{
      __vpiHandle**table;
      unsigned ntable;

      ຖ້າ (stop_current_scope == 0) {
	    vpip_make_root_iterator(table, ntable);

      } ບໍ່ດັ່ງນັ້ນ {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      unsigned vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(calloc(vpi_argc, parimaa_na(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(calloc(vpi_argc, parimaa_na(vpiHandle)));

      unsigned errors = 0;

      ສຳລັບ (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    bool add_to_free_list = false;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    ຖ້າ (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    ຖ້າ (handle == 0 && argv[idx+1][0] == '"') {
		  var_na*tmp = strdup(argv[idx+1]);
		  tmp[strlen(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), false);
		  add_to_free_list = true;
		  free(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    ຖ້າ (handle == 0
		&& strspn(argv[idx+1],"0123456789") == strlen(argv[idx+1])) {
		  handle = new __vpiDecConst(strtol(argv[idx+1],0,10));
		  add_to_free_list = true;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    ສຳລັບ (unsigned tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  const __vpiScope*scope;
		  const var_na*name;

		  switch (table[tmp]->get_type_code()) {

		      pak_sa vpiModule:
		      pak_sa vpiGenScope:
		      pak_sa vpiFunction:
		      pak_sa vpiTask:
		      pak_sa vpiNamedBegin:
		      pak_sa vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			ຖ້າ (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			bha_nga;

		      pak_sa vpiReg:
		      pak_sa vpiNet:
		      pak_sa vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			ຖ້າ (strcmp(argv[idx+1], name) == 0)
			      handle = table[tmp];
			bha_nga;
		  }
	    }

	    ຖ້າ (handle == 0) {
		  printf("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    ຖ້າ (add_to_free_list)
		  vpi_free[idx] = handle;
	    ບໍ່ດັ່ງນັ້ນ
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      ຖ້າ (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                true, false,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    ຖ້າ (call_handle == 0)
		  goto out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      ສຳລັບ (unsigned idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    ຖ້າ (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      free(vpi_argv);
      free(vpi_free);
}

static void cmd_cont(unsigned, const var_na*[])
{
      interact_flag = false;
}

static void cmd_step(unsigned, const var_na*[])
{
      interact_flag = false;
      schedule_single_step(0);
}

static void cmd_finish(unsigned, const var_na*[])
{
      interact_flag = false;
      schedule_finish(0);
}

static void cmd_help(unsigned, const var_na*[]);

static void cmd_list(unsigned, const var_na*[])
{
      __vpiHandle**table;
      unsigned ntable;

      ຖ້າ (stop_current_scope == 0) {
	    vpip_make_root_iterator(table, ntable);

      } ບໍ່ດັ່ງນັ້ນ {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      printf("%u items in this scope:\n", ntable);
      ສຳລັບ (unsigned idx = 0 ;  idx < ntable ;  idx += 1) {

	    const __vpiScope*scope;
	    sa_nracanaa __vpiSignal*sig;

	    switch (table[idx]->get_type_code()) {
		pak_sa vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("package : %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("module  : %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("generate: %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("task    : %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("function: %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("block   : %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  printf("fork    : %s\n", scope->scope_name());
		  bha_nga;

		pak_sa vpiParameter:
		  printf("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  bha_nga;

		pak_sa vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  ຖ້າ ((sig->msb.get_value() == 0) && (sig->lsb.get_value() == 0))
			printf("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  ບໍ່ດັ່ງນັ້ນ
			printf("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_value(), sig->lsb.get_value());
		  bha_nga;

		pak_sa vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  ຖ້າ ((sig->msb.get_value() == 0) && (sig->lsb.get_value() == 0))
			printf("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  ບໍ່ດັ່ງນັ້ນ
			printf("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_value(), sig->lsb.get_value());
		  bha_nga;

		pak_sa vpiPort:
		  printf("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  bha_nga;

		default:
		  printf("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  bha_nga;
	    }

      }
}

static void cmd_load(unsigned argc, const var_na*argv[])
{
      unsigned idx;

      ສຳລັບ (idx = 1 ;  idx < argc ;  idx += 1) {
	    printf("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

static void cmd_pop(unsigned, const var_na*[])
{
      ຖ້າ (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

static void cmd_push(unsigned argc, const var_na* argv[])
{
      ສຳລັບ (unsigned idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    unsigned ntable;

	    ຖ້າ (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } ບໍ່ດັ່ງນັ້ນ {
		  vpip_make_root_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    ສຳລັບ (unsigned tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  ຖ້າ (scope == 0)
			continue;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  ຖ້າ (strcmp(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			bha_nga;
		  }
	    }

	    ຖ້າ (child == 0) {
		  printf("Scope %s not found.\n", argv[idx]);
		  ສົ່ງຄືນ;
	    }

	    stop_current_scope = child;
      }
}

static void cmd_time(unsigned, const var_na*[])
{
      vvp_time64_t ticks = schedule_simtime();
      printf("%" TIME_FMT_U " ticks\n", ticks);
}

static void cmd_trace(unsigned argc, const var_na*argv[])
{
      assert(argc);
      switch (argc) {
	pak_sa 1:
	    show_file_line = true;
	    bha_nga;
	default:
	    printf("Only using the first argument to trace.\n");
	    // fallthrough
	pak_sa 2:
	    ຖ້າ ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_line = true;
	    } ບໍ່ດັ່ງນັ້ນ show_file_line = false;
	    bha_nga;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      ຖ້າ (!code_is_instrumented) {
	    printf("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    printf("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_line = false;
      } ບໍ່ດັ່ງນັ້ນ {
	    printf("Turning statement tracing %s.\n",
	           show_file_line ? "on" : "off");
      }
}

static void cmd_where(unsigned, const var_na*[])
{
      const __vpiScope*cur = stop_current_scope;

      ໃນຂະນະທີ່ (cur) {
	    switch (cur->get_type_code()) {
		pak_sa vpiModule:
		  printf("module %s\n", cur->scope_name());
		  bha_nga;
		pak_sa vpiGenScope:
		  printf("generate %s\n", cur->scope_name());
		  bha_nga;
		default:
		  printf("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  bha_nga;
	    }

	    cur = cur->scope;
      }
}

static void cmd_unknown(unsigned, const var_na*argv[])
{
      printf("Unknown command: %s\n", argv[0]);
      printf("Try the help command to get a summary\n"
	     "of available commands.\n");
}

static sa_nracanaa {
      const var_na*name;
      void (*proc)(unsigned argc, const var_na*argv[]);
      const var_na*summary;
} cmd_table[] = {
      { "cd",     &cmd_push,
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
      { "pop",    &cmd_pop,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_push,
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

static void cmd_help(unsigned, const var_na*[])
{
      printf("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      ສຳລັບ (unsigned idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    printf("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      printf("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


static void invoke_command(var_na*txt)
{
      unsigned argc = 0;
      const var_na**argv = const_cast<const var_na**>(new var_na*[strlen(txt)/2]);

	/* Chop the line into words. */
      ສຳລັບ (var_na*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    ຖ້າ (cp[0] == '"') {
		  var_na*tmp = strchr(cp+1, '"');
		  ຖ້າ (tmp == 0) {
			printf("Missing close-quote: %s\n", cp);
			delete[]argv;
			ສົ່ງຄືນ;
		  }

		  cp = tmp + 1;

	    } ບໍ່ດັ່ງນັ້ນ {
		  cp += strcspn(cp, " ");
	    }

	    ຖ້າ (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      ຖ້າ (argc > 0) {

	    ຖ້າ (argv[0][0] == '$') {
		  ຖ້າ (strcmp(argv[0], "$stop") == 0) {
			printf("The simulator is already stopped!\n");
		  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (strcmp(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } ບໍ່ດັ່ງນັ້ນ {
			cmd_call(argc, argv);
		  }
	    } ບໍ່ດັ່ງນັ້ນ {
		  unsigned idx;
		  ສຳລັບ (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			ຖ້າ (strcmp(cmd_table[idx].name, argv[0]) == 0)
			      bha_nga;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

static void invoke_command_const(const var_na*txt)
{
      var_na *vtxt = strdup(txt);
      invoke_command(vtxt);
      free(vtxt);
}

void stop_handler(ຈຳນວນເຕັມ rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      ຖ້າ (stop_is_finish) {
	    vpip_set_return_value(stop_is_finish_exit_code);
	    schedule_finish(0);
	    ສົ່ງຄືນ;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = true;
      ໃນຂະນະທີ່ (interact_flag) {
	    var_na*input = readline("> ");
	    ຖ້າ (input == 0)
		  bha_nga;


	      /* Advance to the first input character. */
	    var_na*first = input;
	    ໃນຂະນະທີ່ (*first && isspace(*first))
		  first += 1;

	    ຖ້າ (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    free(input);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
