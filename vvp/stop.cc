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
पूर्णांक  stop_is_finish_exit_code = 0;

#ifndef USE_READLINE
स्थिर अक्षर* readline_stub(स्थिरांक अक्षर*prompt)
{
      अक्षर buf[256];

      जर (prompt && prompt[0]) {
	    ख_माला_दो(prompt, मानक_निकास);
	    ख_साफ(मानक_निकास);
      }

      जर (ख_माला_लो(buf, आकार(buf), मानक_निवेश)) {
	    अक्षर*nl = buf + म_माप(buf);
	    पर्यंत (nl > buf && है_खाली(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    परत strdup(buf);
      }

      परत 0;
}
#define readline(x) readline_stub(x)
#endif


स्थिर तार्किक interact_flag = सत्य;

स्थिर शून्य cmd_call(अचिन्हित argc, स्थिरांक अक्षर*argv[])
{
      __vpiHandle**table;
      अचिन्हित ntable;

      जर (stop_current_scope == 0) {
	    vpip_make_मूळ_iterator(table, ntable);

      } अन्यथा {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      अचिन्हित vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(सुस्मृति(vpi_argc, आकार(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(सुस्मृति(vpi_argc, आकार(vpiHandle)));

      अचिन्हित errors = 0;

      साठी (अचिन्हित idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    तार्किक add_to_free_list = असत्य;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    जर (stop_current_scope && (म_भेद(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    जर (handle == 0 && argv[idx+1][0] == '"') {
		  अक्षर*tmp = strdup(argv[idx+1]);
		  tmp[म_माप(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), असत्य);
		  add_to_free_list = सत्य;
		  मुक्त(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    जर (handle == 0
		&& म_अखोज(argv[idx+1],"0123456789") == म_माप(argv[idx+1])) {
		  handle = नवीन __vpiDecConst(म_से_दीर्घ(argv[idx+1],0,10));
		  add_to_free_list = सत्य;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    साठी (अचिन्हित tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  स्थिरांक __vpiScope*scope;
		  स्थिरांक अक्षर*name;

		  निवड (table[tmp]->get_type_code()) {

		      स्थिती vpiModule:
		      स्थिती vpiGenScope:
		      स्थिती vpiFunction:
		      स्थिती vpiTask:
		      स्थिती vpiNamedBegin:
		      स्थिती vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(table[tmp]);
			जर (म_भेद(scope->scope_name(), argv[idx+1]) == 0)
			      handle = table[tmp];
			थांबा;

		      स्थिती vpiReg:
		      स्थिती vpiNet:
		      स्थिती vpiParameter:
			name = vpi_get_str(vpiName,table[tmp]);
			जर (म_भेद(argv[idx+1], name) == 0)
			      handle = table[tmp];
			थांबा;
		  }
	    }

	    जर (handle == 0) {
		  म_लिखो("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    जर (add_to_free_list)
		  vpi_free[idx] = handle;
	    अन्यथा
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      जर (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                सत्य, असत्य,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    जर (call_handle == 0)
		  जा out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      साठी (अचिन्हित idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    जर (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      मुक्त(vpi_argv);
      मुक्त(vpi_free);
}

स्थिर शून्य cmd_cont(अचिन्हित, स्थिरांक अक्षर*[])
{
      interact_flag = असत्य;
}

स्थिर शून्य cmd_step(अचिन्हित, स्थिरांक अक्षर*[])
{
      interact_flag = असत्य;
      schedule_single_step(0);
}

स्थिर शून्य cmd_finish(अचिन्हित, स्थिरांक अक्षर*[])
{
      interact_flag = असत्य;
      schedule_finish(0);
}

स्थिर शून्य cmd_help(अचिन्हित, स्थिरांक अक्षर*[]);

स्थिर शून्य cmd_list(अचिन्हित, स्थिरांक अक्षर*[])
{
      __vpiHandle**table;
      अचिन्हित ntable;

      जर (stop_current_scope == 0) {
	    vpip_make_मूळ_iterator(table, ntable);

      } अन्यथा {
	    table = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      म_लिखो("%u items in this scope:\n", ntable);
      साठी (अचिन्हित idx = 0 ;  idx < ntable ;  idx += 1) {

	    स्थिरांक __vpiScope*scope;
	    संरचना __vpiSignal*sig;

	    निवड (table[idx]->get_type_code()) {
		स्थिती vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("package : %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiModule:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("module  : %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("generate: %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiTask:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("task    : %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("function: %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("block   : %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(table[idx]);
		  म_लिखो("fork    : %s\n", scope->scope_name());
		  थांबा;

		स्थिती vpiParameter:
		  म_लिखो("param   : %s\n", vpi_get_str(vpiName, table[idx]));
		  थांबा;

		स्थिती vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  जर ((sig->msb.get_मूल्य() == 0) && (sig->lsb.get_मूल्य() == 0))
			म_लिखो("reg     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  अन्यथा
			म_लिखो("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_मूल्य(), sig->lsb.get_मूल्य());
		  थांबा;

		स्थिती vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(table[idx]);
		  जर ((sig->msb.get_मूल्य() == 0) && (sig->lsb.get_मूल्य() == 0))
			म_लिखो("net     : %s%s\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "");
		  अन्यथा
			म_लिखो("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, table[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_मूल्य(), sig->lsb.get_मूल्य());
		  थांबा;

		स्थिती vpiPort:
		  म_लिखो("port    : %s -- %s\n",
			 vpi_get_str(vpiName, table[idx]),
			 direction_as_string(vpi_get(vpiDirection, table[idx])));
		  थांबा;

		पूर्वनिर्धारित:
		  म_लिखो("%8s: <vpi handle>\n",
			 vpi_type_as_string(table[idx]->get_type_code()));
		  थांबा;
	    }

      }
}

स्थिर शून्य cmd_load(अचिन्हित argc, स्थिरांक अक्षर*argv[])
{
      अचिन्हित idx;

      साठी (idx = 1 ;  idx < argc ;  idx += 1) {
	    म_लिखो("Loading module %s...\n", argv[idx]);
	    vpip_load_module(argv[idx]);
      }
}

स्थिर शून्य cmd_काढा(अचिन्हित, स्थिरांक अक्षर*[])
{
      जर (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

स्थिर शून्य cmd_ढकला(अचिन्हित argc, स्थिरांक अक्षर* argv[])
{
      साठी (अचिन्हित idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**table;
	    अचिन्हित ntable;

	    जर (stop_current_scope) {
		  table = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } अन्यथा {
		  vpip_make_मूळ_iterator(table, ntable);
	    }

	    __vpiScope*child = 0;
	    साठी (अचिन्हित tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(table[tmp]);
		  जर (scope == 0)
			सुरूठेवा;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  जर (म_भेद(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			थांबा;
		  }
	    }

	    जर (child == 0) {
		  म_लिखो("Scope %s not found.\n", argv[idx]);
		  परत;
	    }

	    stop_current_scope = child;
      }
}

स्थिर शून्य cmd_time(अचिन्हित, स्थिरांक अक्षर*[])
{
      vvp_time64_t ticks = schedule_simtime();
      म_लिखो("%" TIME_FMT_U " ticks\n", ticks);
}

स्थिर शून्य cmd_trace(अचिन्हित argc, स्थिरांक अक्षर*argv[])
{
      प्रतिज्ञा(argc);
      निवड (argc) {
	स्थिती 1:
	    show_file_ओळ = सत्य;
	    थांबा;
	पूर्वनिर्धारित:
	    म_लिखो("Only using the first argument to trace.\n");
	    // fallthrough
	स्थिती 2:
	    जर ((म_भेद(argv[1], "on") == 0) || (म_भेद(argv[1], "1") == 0)) {
		  show_file_ओळ = सत्य;
	    } अन्यथा show_file_ओळ = असत्य;
	    थांबा;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      जर (!code_is_instrumented) {
	    म_लिखो("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    म_लिखो("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_ओळ = असत्य;
      } अन्यथा {
	    म_लिखो("Turning statement tracing %s.\n",
	           show_file_ओळ ? "on" : "off");
      }
}

स्थिर शून्य cmd_where(अचिन्हित, स्थिरांक अक्षर*[])
{
      स्थिरांक __vpiScope*cur = stop_current_scope;

      पर्यंत (cur) {
	    निवड (cur->get_type_code()) {
		स्थिती vpiModule:
		  म_लिखो("module %s\n", cur->scope_name());
		  थांबा;
		स्थिती vpiGenScope:
		  म_लिखो("generate %s\n", cur->scope_name());
		  थांबा;
		पूर्वनिर्धारित:
		  म_लिखो("scope (%d) %s;\n", cur->get_type_code(), cur->scope_name());
		  थांबा;
	    }

	    cur = cur->scope;
      }
}

स्थिर शून्य cmd_unknown(अचिन्हित, स्थिरांक अक्षर*argv[])
{
      म_लिखो("Unknown command: %s\n", argv[0]);
      म_लिखो("Try the help command to get a summary\n"
	     "of available commands.\n");
}

स्थिर संरचना {
      स्थिरांक अक्षर*name;
      शून्य (*proc)(अचिन्हित argc, स्थिरांक अक्षर*argv[]);
      स्थिरांक अक्षर*summary;
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

स्थिर शून्य cmd_help(अचिन्हित, स्थिरांक अक्षर*[])
{
      म_लिखो("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      साठी (अचिन्हित idx = 0 ;  cmd_table[idx].name != 0 ;  idx += 1) {
	    म_लिखो("%-8s - %s\n", cmd_table[idx].name, cmd_table[idx].summary);
      }

      म_लिखो("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


स्थिर शून्य invoke_command(अक्षर*txt)
{
      अचिन्हित argc = 0;
      स्थिरांक अक्षर**argv = const_cast<स्थिरांक अक्षर**>(नवीन अक्षर*[म_माप(txt)/2]);

	/* Chop the line into words. */
      साठी (अक्षर*cp = txt+म_अखोज(txt, " ")
		 ; *cp; cp += म_अखोज(cp, " ")) {
	    argv[argc] = cp;

	    जर (cp[0] == '"') {
		  अक्षर*tmp = म_अक्षर(cp+1, '"');
		  जर (tmp == 0) {
			म_लिखो("Missing close-quote: %s\n", cp);
			delete[]argv;
			परत;
		  }

		  cp = tmp + 1;

	    } अन्यथा {
		  cp += म_खोज(cp, " ");
	    }

	    जर (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      जर (argc > 0) {

	    जर (argv[0][0] == '$') {
		  जर (म_भेद(argv[0], "$stop") == 0) {
			म_लिखो("The simulator is already stopped!\n");
		  } अन्यथा जर (म_भेद(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } अन्यथा {
			cmd_call(argc, argv);
		  }
	    } अन्यथा {
		  अचिन्हित idx;
		  साठी (idx = 0 ;  cmd_table[idx].name ;  idx += 1)
			जर (म_भेद(cmd_table[idx].name, argv[0]) == 0)
			      थांबा;

		  cmd_table[idx].proc (argc, argv);
	    }

      }

      delete[]argv;
}

स्थिर शून्य invoke_command_const(स्थिरांक अक्षर*txt)
{
      अक्षर *vtxt = strdup(txt);
      invoke_command(vtxt);
      मुक्त(vtxt);
}

शून्य stop_handler(पूर्णांक rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      जर (stop_is_finish) {
	    vpip_set_return_मूल्य(stop_is_finish_exit_code);
	    schedule_finish(0);
	    परत;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     schedule_simtime());

      interact_flag = सत्य;
      पर्यंत (interact_flag) {
	    अक्षर*आदान = readline("> ");
	    जर (आदान == 0)
		  थांबा;


	      /* Advance to the first input character. */
	    अक्षर*first = आदान;
	    पर्यंत (*first && है_खाली(*first))
		  first += 1;

	    जर (first[0] != 0) {
#ifdef USE_HISTORY
		  add_history(first);
#endif
		  invoke_command(first);
	    }

	    मुक्त(आदान);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
