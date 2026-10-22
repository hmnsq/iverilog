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
#यदि_परिभाषितम् USE_READLINE
# include  <readline/readline.h>
#यदि_अन्तः
#यदि_परिभाषितम् USE_HISTORY
# include  <readline/history.h>
#यदि_अन्तः
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
तार्किक stop_is_finish = असत्य;  /* When set, $stop acts like $finish (set in main.cc). */
पूर्णाङ्क  stop_is_finish_exit_संहिता = 0;

#यदि_अपरिभाषितम् USE_READLINE
स्थिर वर्ण* readline_stub(ध्रुव वर्ण*prompt)
{
      वर्ण buf[256];

      यदि (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      यदि (fgets(buf, परिमाण(buf), stdin)) {
	    वर्ण*nl = buf + वाक्यदैर्घ्यम्(buf);
	    यावत् (nl > buf && रिक्तम्_वा(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    प्रत्यागम strdup(buf);
      }

      प्रत्यागम 0;
}
#परिभाषय readline(x) readline_stub(x)
#यदि_अन्तः


स्थिर तार्किक interact_flag = सत्य;

स्थिर शून्य cmd_call(अचिह्नित argc, ध्रुव वर्ण*argv[])
{
      __vpiHandle**सारणी;
      अचिह्नित ntable;

      यदि (stop_current_scope == 0) {
	    vpip_make_मूलम्_iterator(सारणी, ntable);

      } अन्यथा {
	    सारणी = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      अचिह्नित vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(शून्यस्मृतिं_देहि(vpi_argc, परिमाण(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(शून्यस्मृतिं_देहि(vpi_argc, परिमाण(vpiHandle)));

      अचिह्नित errors = 0;

      प्रति (अचिह्नित idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    तार्किक सङ्कलय_गन्तव्यम्_free_list = असत्य;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    यदि (stop_current_scope && (वाक्यं_तुलय(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    यदि (handle == 0 && argv[idx+1][0] == '"') {
		  वर्ण*tmp = strdup(argv[idx+1]);
		  tmp[वाक्यदैर्घ्यम्(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), असत्य);
		  सङ्कलय_गन्तव्यम्_free_list = सत्य;
		  मुक्त(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    यदि (handle == 0
		&& strspn(argv[idx+1],"0123456789") == वाक्यदैर्घ्यम्(argv[idx+1])) {
		  handle = नूतन __vpiDecConst(strtol(argv[idx+1],0,10));
		  सङ्कलय_गन्तव्यम्_free_list = सत्य;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    प्रति (अचिह्नित tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ध्रुव __vpiScope*scope;
		  ध्रुव वर्ण*name;

		  विकल्प (सारणी[tmp]->get_type_संहिता()) {

		      पक्ष vpiModule:
		      पक्ष vpiGenScope:
		      पक्ष vpiFunction:
		      पक्ष vpiTask:
		      पक्ष vpiNamedBegin:
		      पक्ष vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(सारणी[tmp]);
			यदि (वाक्यं_तुलय(scope->scope_name(), argv[idx+1]) == 0)
			      handle = सारणी[tmp];
			भङ्ग;

		      पक्ष vpiReg:
		      पक्ष vpiNet:
		      पक्ष vpiParameter:
			name = vpi_get_str(vpiName,सारणी[tmp]);
			यदि (वाक्यं_तुलय(argv[idx+1], name) == 0)
			      handle = सारणी[tmp];
			भङ्ग;
		  }
	    }

	    यदि (handle == 0) {
		  प्रारूप_लिख("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    यदि (सङ्कलय_गन्तव्यम्_free_list)
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
      प्रति (अचिह्नित idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    यदि (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      मुक्त(vpi_argv);
      मुक्त(vpi_free);
}

स्थिर शून्य cmd_cont(अचिह्नित, ध्रुव वर्ण*[])
{
      interact_flag = असत्य;
}

स्थिर शून्य cmd_चरणः(अचिह्नित, ध्रुव वर्ण*[])
{
      interact_flag = असत्य;
      कालसारणी_single_चरणः(0);
}

स्थिर शून्य cmd_finish(अचिह्नित, ध्रुव वर्ण*[])
{
      interact_flag = असत्य;
      कालसारणी_finish(0);
}

स्थिर शून्य cmd_help(अचिह्नित, ध्रुव वर्ण*[]);

स्थिर शून्य cmd_list(अचिह्नित, ध्रुव वर्ण*[])
{
      __vpiHandle**सारणी;
      अचिह्नित ntable;

      यदि (stop_current_scope == 0) {
	    vpip_make_मूलम्_iterator(सारणी, ntable);

      } अन्यथा {
	    सारणी = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      प्रारूप_लिख("%u items in this scope:\n", ntable);
      प्रति (अचिह्नित idx = 0 ;  idx < ntable ;  idx += 1) {

	    ध्रुव __vpiScope*scope;
	    संरचना __vpiSignal*sig;

	    विकल्प (सारणी[idx]->get_type_संहिता()) {
		पक्ष vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("package : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiModule:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("module  : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("generate: %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiTask:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("task    : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("function: %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("block   : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(सारणी[idx]);
		  प्रारूप_लिख("fork    : %s\n", scope->scope_name());
		  भङ्ग;

		पक्ष vpiParameter:
		  प्रारूप_लिख("param   : %s\n", vpi_get_str(vpiName, सारणी[idx]));
		  भङ्ग;

		पक्ष vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(सारणी[idx]);
		  यदि ((sig->msb.get_मानम्() == 0) && (sig->lsb.get_मानम्() == 0))
			प्रारूप_लिख("reg     : %s%s\n",
			       vpi_get_str(vpiName, सारणी[idx]),
			       sig->signed_flag? "signed " : "");
		  अन्यथा
			प्रारूप_लिख("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, सारणी[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_मानम्(), sig->lsb.get_मानम्());
		  भङ्ग;

		पक्ष vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(सारणी[idx]);
		  यदि ((sig->msb.get_मानम्() == 0) && (sig->lsb.get_मानम्() == 0))
			प्रारूप_लिख("net     : %s%s\n",
			       vpi_get_str(vpiName, सारणी[idx]),
			       sig->signed_flag? "signed " : "");
		  अन्यथा
			प्रारूप_लिख("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, सारणी[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_मानम्(), sig->lsb.get_मानम्());
		  भङ्ग;

		पक्ष vpi_पत्तनम्:
		  प्रारूप_लिख("port    : %s -- %s\n",
			 vpi_get_str(vpiName, सारणी[idx]),
			 direction_as_string(vpi_get(vpiDirection, सारणी[idx])));
		  भङ्ग;

		स्वभाव:
		  प्रारूप_लिख("%8s: <vpi handle>\n",
			 vpi_type_as_string(सारणी[idx]->get_type_संहिता()));
		  भङ्ग;
	    }

      }
}

स्थिर शून्य cmd_आरोपय(अचिह्नित argc, ध्रुव वर्ण*argv[])
{
      अचिह्नित idx;

      प्रति (idx = 1 ;  idx < argc ;  idx += 1) {
	    प्रारूप_लिख("Loading module %s...\n", argv[idx]);
	    vpip_आरोपय_module(argv[idx]);
      }
}

स्थिर शून्य cmd_निष्कासय(अचिह्नित, ध्रुव वर्ण*[])
{
      यदि (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

स्थिर शून्य cmd_प्रक्षिप(अचिह्नित argc, ध्रुव वर्ण* argv[])
{
      प्रति (अचिह्नित idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**सारणी;
	    अचिह्नित ntable;

	    यदि (stop_current_scope) {
		  सारणी = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } अन्यथा {
		  vpip_make_मूलम्_iterator(सारणी, ntable);
	    }

	    __vpiScope*child = 0;
	    प्रति (अचिह्नित tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(सारणी[tmp]);
		  यदि (scope == 0)
			अनुवर्त;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  यदि (वाक्यं_तुलय(scope->scope_name(), argv[idx]) == 0) {
			child = scope;
			भङ्ग;
		  }
	    }

	    यदि (child == 0) {
		  प्रारूप_लिख("Scope %s not found.\n", argv[idx]);
		  प्रत्यागम;
	    }

	    stop_current_scope = child;
      }
}

स्थिर शून्य cmd_time(अचिह्नित, ध्रुव वर्ण*[])
{
      vvp_time64_t ticks = कालसारणी_simtime();
      प्रारूप_लिख("%" TIME_FMT_U " ticks\n", ticks);
}

स्थिर शून्य cmd_trace(अचिह्नित argc, ध्रुव वर्ण*argv[])
{
      प्रतिज्ञा(argc);
      विकल्प (argc) {
	पक्ष 1:
	    show_file_पङ्क्तिः = सत्य;
	    भङ्ग;
	स्वभाव:
	    प्रारूप_लिख("Only using the first argument to trace.\n");
	    // fallthrough
	पक्ष 2:
	    यदि ((वाक्यं_तुलय(argv[1], "on") == 0) || (वाक्यं_तुलय(argv[1], "1") == 0)) {
		  show_file_पङ्क्तिः = सत्य;
	    } अन्यथा show_file_पङ्क्तिः = असत्य;
	    भङ्ग;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      यदि (!संहिता_is_instrumented) {
	    प्रारूप_लिख("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    प्रारूप_लिख("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_पङ्क्तिः = असत्य;
      } अन्यथा {
	    प्रारूप_लिख("Turning statement tracing %s.\n",
	           show_file_पङ्क्तिः ? "on" : "off");
      }
}

स्थिर शून्य cmd_where(अचिह्नित, ध्रुव वर्ण*[])
{
      ध्रुव __vpiScope*cur = stop_current_scope;

      यावत् (cur) {
	    विकल्प (cur->get_type_संहिता()) {
		पक्ष vpiModule:
		  प्रारूप_लिख("module %s\n", cur->scope_name());
		  भङ्ग;
		पक्ष vpiGenScope:
		  प्रारूप_लिख("generate %s\n", cur->scope_name());
		  भङ्ग;
		स्वभाव:
		  प्रारूप_लिख("scope (%d) %s;\n", cur->get_type_संहिता(), cur->scope_name());
		  भङ्ग;
	    }

	    cur = cur->scope;
      }
}

स्थिर शून्य cmd_unknown(अचिह्नित, ध्रुव वर्ण*argv[])
{
      प्रारूप_लिख("Unknown command: %s\n", argv[0]);
      प्रारूप_लिख("Try the help command to get a summary\n"
	     "of available commands.\n");
}

स्थिर संरचना {
      ध्रुव वर्ण*name;
      शून्य (*proc)(अचिह्नित argc, ध्रुव वर्ण*argv[]);
      ध्रुव वर्ण*summary;
} cmd_सारणी[] = {
      { "cd",     &cmd_प्रक्षिप,
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
      { "load",   &cmd_आरोपय,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_निष्कासय,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_प्रक्षिप,
        "Descend into the named scope."},
      { "step",   &cmd_चरणः,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_time,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

स्थिर शून्य cmd_help(अचिह्नित, ध्रुव वर्ण*[])
{
      प्रारूप_लिख("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      प्रति (अचिह्नित idx = 0 ;  cmd_सारणी[idx].name != 0 ;  idx += 1) {
	    प्रारूप_लिख("%-8s - %s\n", cmd_सारणी[idx].name, cmd_सारणी[idx].summary);
      }

      प्रारूप_लिख("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


स्थिर शून्य invoke_command(वर्ण*txt)
{
      अचिह्नित argc = 0;
      ध्रुव वर्ण**argv = const_cast<ध्रुव वर्ण**>(नूतन वर्ण*[वाक्यदैर्घ्यम्(txt)/2]);

	/* Chop the line into words. */
      प्रति (वर्ण*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    यदि (cp[0] == '"') {
		  वर्ण*tmp = strchr(cp+1, '"');
		  यदि (tmp == 0) {
			प्रारूप_लिख("Missing close-quote: %s\n", cp);
			विलोपय[]argv;
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
		  यदि (वाक्यं_तुलय(argv[0], "$stop") == 0) {
			प्रारूप_लिख("The simulator is already stopped!\n");
		  } अन्यथा यदि (वाक्यं_तुलय(argv[0], "$finish") == 0){
			cmd_finish(argc, argv);
		  } अन्यथा {
			cmd_call(argc, argv);
		  }
	    } अन्यथा {
		  अचिह्नित idx;
		  प्रति (idx = 0 ;  cmd_सारणी[idx].name ;  idx += 1)
			यदि (वाक्यं_तुलय(cmd_सारणी[idx].name, argv[0]) == 0)
			      भङ्ग;

		  cmd_सारणी[idx].proc (argc, argv);
	    }

      }

      विलोपय[]argv;
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
	    vpip_समुच्चयः_return_मानम्(stop_is_finish_exit_संहिता);
	    कालसारणी_finish(0);
	    प्रत्यागम;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" TIME_FMT_U " ticks.\n",
		     कालसारणी_simtime());

      interact_flag = सत्य;
      यावत् (interact_flag) {
	    वर्ण*निवेशः = readline("> ");
	    यदि (निवेशः == 0)
		  भङ्ग;


	      /* Advance to the first input character. */
	    वर्ण*first = निवेशः;
	    यावत् (*first && रिक्तम्_वा(*first))
		  first += 1;

	    यदि (first[0] != 0) {
#यदि_परिभाषितम् USE_HISTORY
		  सङ्कलय_history(first);
#यदि_अन्तः
		  invoke_command(first);
	    }

	    मुक्त(निवेशः);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
