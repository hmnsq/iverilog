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
#నిర్వచితమైతే USE_READLINE
# include  <readline/readline.h>
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HISTORY
# include  <readline/history.h>
#అయితే_ముగింపు
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
తార్కిక stop_is_ముగింపు = అబద్ధం;  /* When set, $stop acts like $finish (set in main.cc). */
పూర్ణాంకం  stop_is_ముగింపు_exit_సంకేతం = 0;

#నిర్వచితం_కాకపోతే USE_READLINE
స్థిర అక్షరం* readline_stub(స్థిరరాశి అక్షరం*prompt)
{
      అక్షరం buf[256];

      ఒకవేళ (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      ఒకవేళ (fgets(buf, పరిమాణం(buf), stdin)) {
	    అక్షరం*nl = buf + అక్షరశ్రేణి_పొడవు(buf);
	    అయితే (nl > buf && ఖాళీనా(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    తిరిగి strdup(buf);
      }

      తిరిగి 0;
}
#నిర్వచించు readline(x) readline_stub(x)
#అయితే_ముగింపు


స్థిర తార్కిక interact_flag = నిజం;

స్థిర శూన్య cmd_call(గుర్తులేని argc, స్థిరరాశి అక్షరం*argv[])
{
      __vpiHandle**పట్టిక;
      గుర్తులేని ntable;

      ఒకవేళ (stop_current_scope == 0) {
	    vpip_make_మూలం_iterator(పట్టిక, ntable);

      } లేకపోతే {
	    పట్టిక = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      గుర్తులేని vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(vpi_argc, పరిమాణం(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(vpi_argc, పరిమాణం(vpiHandle)));

      గుర్తులేని errors = 0;

      కొరకు (గుర్తులేని idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    తార్కిక కూడు_గమ్యం_free_list = అబద్ధం;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    ఒకవేళ (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    ఒకవేళ (handle == 0 && argv[idx+1][0] == '"') {
		  అక్షరం*tmp = strdup(argv[idx+1]);
		  tmp[అక్షరశ్రేణి_పొడవు(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), అబద్ధం);
		  కూడు_గమ్యం_free_list = నిజం;
		  విడుదల(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    ఒకవేళ (handle == 0
		&& strspn(argv[idx+1],"0123456789") == అక్షరశ్రేణి_పొడవు(argv[idx+1])) {
		  handle = కొత్త __vpiDecConst(strtol(argv[idx+1],0,10));
		  కూడు_గమ్యం_free_list = నిజం;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    కొరకు (గుర్తులేని tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  స్థిరరాశి __vpiScope*scope;
		  స్థిరరాశి అక్షరం*name;

		  ఎంపిక (పట్టిక[tmp]->get_type_సంకేతం()) {

		      స్థితి vpiModule:
		      స్థితి vpiGenScope:
		      స్థితి vpiFunction:
		      స్థితి vpiTask:
		      స్థితి vpiNamedBegin:
		      స్థితి vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(పట్టిక[tmp]);
			ఒకవేళ (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = పట్టిక[tmp];
			ఆపు;

		      స్థితి vpiReg:
		      స్థితి vpiNet:
		      స్థితి vpiParameter:
			name = vpi_get_str(vpiName,పట్టిక[tmp]);
			ఒకవేళ (strcmp(argv[idx+1], name) == 0)
			      handle = పట్టిక[tmp];
			ఆపు;
		  }
	    }

	    ఒకవేళ (handle == 0) {
		  రాయి("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    ఒకవేళ (కూడు_గమ్యం_free_list)
		  vpi_free[idx] = handle;
	    లేకపోతే
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      ఒకవేళ (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                నిజం, అబద్ధం,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    ఒకవేళ (call_handle == 0)
		  వెళ్ళు out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      కొరకు (గుర్తులేని idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    ఒకవేళ (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      విడుదల(vpi_argv);
      విడుదల(vpi_free);
}

స్థిర శూన్య cmd_cont(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      interact_flag = అబద్ధం;
}

స్థిర శూన్య cmd_అడుగు(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      interact_flag = అబద్ధం;
      కాలపట్టిక_single_అడుగు(0);
}

స్థిర శూన్య cmd_ముగింపు(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      interact_flag = అబద్ధం;
      కాలపట్టిక_ముగింపు(0);
}

స్థిర శూన్య cmd_help(గుర్తులేని, స్థిరరాశి అక్షరం*[]);

స్థిర శూన్య cmd_list(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      __vpiHandle**పట్టిక;
      గుర్తులేని ntable;

      ఒకవేళ (stop_current_scope == 0) {
	    vpip_make_మూలం_iterator(పట్టిక, ntable);

      } లేకపోతే {
	    పట్టిక = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      రాయి("%u items in this scope:\n", ntable);
      కొరకు (గుర్తులేని idx = 0 ;  idx < ntable ;  idx += 1) {

	    స్థిరరాశి __vpiScope*scope;
	    నిర్మాణం __vpiSignal*sig;

	    ఎంపిక (పట్టిక[idx]->get_type_సంకేతం()) {
		స్థితి vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("package : %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiModule:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("module  : %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("generate: %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiTask:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("task    : %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("function: %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("block   : %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(పట్టిక[idx]);
		  రాయి("fork    : %s\n", scope->scope_name());
		  ఆపు;

		స్థితి vpiParameter:
		  రాయి("param   : %s\n", vpi_get_str(vpiName, పట్టిక[idx]));
		  ఆపు;

		స్థితి vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(పట్టిక[idx]);
		  ఒకవేళ ((sig->msb.get_విలువ() == 0) && (sig->lsb.get_విలువ() == 0))
			రాయి("reg     : %s%s\n",
			       vpi_get_str(vpiName, పట్టిక[idx]),
			       sig->signed_flag? "signed " : "");
		  లేకపోతే
			రాయి("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, పట్టిక[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_విలువ(), sig->lsb.get_విలువ());
		  ఆపు;

		స్థితి vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(పట్టిక[idx]);
		  ఒకవేళ ((sig->msb.get_విలువ() == 0) && (sig->lsb.get_విలువ() == 0))
			రాయి("net     : %s%s\n",
			       vpi_get_str(vpiName, పట్టిక[idx]),
			       sig->signed_flag? "signed " : "");
		  లేకపోతే
			రాయి("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, పట్టిక[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_విలువ(), sig->lsb.get_విలువ());
		  ఆపు;

		స్థితి vpi_పోర్టు:
		  రాయి("port    : %s -- %s\n",
			 vpi_get_str(vpiName, పట్టిక[idx]),
			 direction_as_string(vpi_get(vpiDirection, పట్టిక[idx])));
		  ఆపు;

		అప్రమేయ:
		  రాయి("%8s: <vpi handle>\n",
			 vpi_type_as_string(పట్టిక[idx]->get_type_సంకేతం()));
		  ఆపు;
	    }

      }
}

స్థిర శూన్య cmd_ఎక్కించు(గుర్తులేని argc, స్థిరరాశి అక్షరం*argv[])
{
      గుర్తులేని idx;

      కొరకు (idx = 1 ;  idx < argc ;  idx += 1) {
	    రాయి("Loading module %s...\n", argv[idx]);
	    vpip_ఎక్కించు_module(argv[idx]);
      }
}

స్థిర శూన్య cmd_తీయి(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      ఒకవేళ (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

స్థిర శూన్య cmd_నెట్టు(గుర్తులేని argc, స్థిరరాశి అక్షరం* argv[])
{
      కొరకు (గుర్తులేని idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**పట్టిక;
	    గుర్తులేని ntable;

	    ఒకవేళ (stop_current_scope) {
		  పట్టిక = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } లేకపోతే {
		  vpip_make_మూలం_iterator(పట్టిక, ntable);
	    }

	    __vpiScope*బిడ్డ = 0;
	    కొరకు (గుర్తులేని tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(పట్టిక[tmp]);
		  ఒకవేళ (scope == 0)
			కొనసాగు;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  ఒకవేళ (strcmp(scope->scope_name(), argv[idx]) == 0) {
			బిడ్డ = scope;
			ఆపు;
		  }
	    }

	    ఒకవేళ (బిడ్డ == 0) {
		  రాయి("Scope %s not found.\n", argv[idx]);
		  తిరిగి;
	    }

	    stop_current_scope = బిడ్డ;
      }
}

స్థిర శూన్య cmd_సమయం(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      vvp_time64_t ticks = కాలపట్టిక_simtime();
      రాయి("%" సమయం_FMT_U " ticks\n", ticks);
}

స్థిర శూన్య cmd_trace(గుర్తులేని argc, స్థిరరాశి అక్షరం*argv[])
{
      ప్రతిజ్ఞా(argc);
      ఎంపిక (argc) {
	స్థితి 1:
	    show_file_పంక్తి = నిజం;
	    ఆపు;
	అప్రమేయ:
	    రాయి("Only using the first argument to trace.\n");
	    // fallthrough
	స్థితి 2:
	    ఒకవేళ ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_పంక్తి = నిజం;
	    } లేకపోతే show_file_పంక్తి = అబద్ధం;
	    ఆపు;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      ఒకవేళ (!సంకేతం_is_instrumented) {
	    రాయి("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    రాయి("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_పంక్తి = అబద్ధం;
      } లేకపోతే {
	    రాయి("Turning statement tracing %s.\n",
	           show_file_పంక్తి ? "on" : "off");
      }
}

స్థిర శూన్య cmd_where(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      స్థిరరాశి __vpiScope*cur = stop_current_scope;

      అయితే (cur) {
	    ఎంపిక (cur->get_type_సంకేతం()) {
		స్థితి vpiModule:
		  రాయి("module %s\n", cur->scope_name());
		  ఆపు;
		స్థితి vpiGenScope:
		  రాయి("generate %s\n", cur->scope_name());
		  ఆపు;
		అప్రమేయ:
		  రాయి("scope (%d) %s;\n", cur->get_type_సంకేతం(), cur->scope_name());
		  ఆపు;
	    }

	    cur = cur->scope;
      }
}

స్థిర శూన్య cmd_unknown(గుర్తులేని, స్థిరరాశి అక్షరం*argv[])
{
      రాయి("Unknown command: %s\n", argv[0]);
      రాయి("Try the help command to get a summary\n"
	     "of available commands.\n");
}

స్థిర నిర్మాణం {
      స్థిరరాశి అక్షరం*name;
      శూన్య (*proc)(గుర్తులేని argc, స్థిరరాశి అక్షరం*argv[]);
      స్థిరరాశి అక్షరం*summary;
} cmd_పట్టిక[] = {
      { "cd",     &cmd_నెట్టు,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_ముగింపు,
        "Synonym for finish."},
      { "finish", &cmd_ముగింపు,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_ఎక్కించు,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_తీయి,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_నెట్టు,
        "Descend into the named scope."},
      { "step",   &cmd_అడుగు,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_సమయం,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

స్థిర శూన్య cmd_help(గుర్తులేని, స్థిరరాశి అక్షరం*[])
{
      రాయి("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      కొరకు (గుర్తులేని idx = 0 ;  cmd_పట్టిక[idx].name != 0 ;  idx += 1) {
	    రాయి("%-8s - %s\n", cmd_పట్టిక[idx].name, cmd_పట్టిక[idx].summary);
      }

      రాయి("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


స్థిర శూన్య invoke_command(అక్షరం*txt)
{
      గుర్తులేని argc = 0;
      స్థిరరాశి అక్షరం**argv = const_cast<స్థిరరాశి అక్షరం**>(కొత్త అక్షరం*[అక్షరశ్రేణి_పొడవు(txt)/2]);

	/* Chop the line into words. */
      కొరకు (అక్షరం*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    ఒకవేళ (cp[0] == '"') {
		  అక్షరం*tmp = strchr(cp+1, '"');
		  ఒకవేళ (tmp == 0) {
			రాయి("Missing close-quote: %s\n", cp);
			తొలగించు[]argv;
			తిరిగి;
		  }

		  cp = tmp + 1;

	    } లేకపోతే {
		  cp += strcspn(cp, " ");
	    }

	    ఒకవేళ (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      ఒకవేళ (argc > 0) {

	    ఒకవేళ (argv[0][0] == '$') {
		  ఒకవేళ (strcmp(argv[0], "$stop") == 0) {
			రాయి("The simulator is already stopped!\n");
		  } లేకపోతే ఒకవేళ (strcmp(argv[0], "$finish") == 0){
			cmd_ముగింపు(argc, argv);
		  } లేకపోతే {
			cmd_call(argc, argv);
		  }
	    } లేకపోతే {
		  గుర్తులేని idx;
		  కొరకు (idx = 0 ;  cmd_పట్టిక[idx].name ;  idx += 1)
			ఒకవేళ (strcmp(cmd_పట్టిక[idx].name, argv[0]) == 0)
			      ఆపు;

		  cmd_పట్టిక[idx].proc (argc, argv);
	    }

      }

      తొలగించు[]argv;
}

స్థిర శూన్య invoke_command_const(స్థిరరాశి అక్షరం*txt)
{
      అక్షరం *vtxt = strdup(txt);
      invoke_command(vtxt);
      విడుదల(vtxt);
}

శూన్య stop_handler(పూర్ణాంకం rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      ఒకవేళ (stop_is_ముగింపు) {
	    vpip_సమితి_return_విలువ(stop_is_ముగింపు_exit_సంకేతం);
	    కాలపట్టిక_ముగింపు(0);
	    తిరిగి;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" సమయం_FMT_U " ticks.\n",
		     కాలపట్టిక_simtime());

      interact_flag = నిజం;
      అయితే (interact_flag) {
	    అక్షరం*ఆదానం = readline("> ");
	    ఒకవేళ (ఆదానం == 0)
		  ఆపు;


	      /* Advance to the first input character. */
	    అక్షరం*మొదటి = ఆదానం;
	    అయితే (*మొదటి && ఖాళీనా(*మొదటి))
		  మొదటి += 1;

	    ఒకవేళ (మొదటి[0] != 0) {
#నిర్వచితమైతే USE_HISTORY
		  కూడు_history(మొదటి);
#అయితే_ముగింపు
		  invoke_command(మొదటి);
	    }

	    విడుదల(ఆదానం);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
