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
#اگر_تعریف_شدہ USE_READLINE
# include  <readline/readline.h>
#اگر_ختم
#اگر_تعریف_شدہ USE_HISTORY
# include  <readline/history.h>
#اگر_ختم
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
منطقی stop_is_اختتام = جھوٹ;  /* When set, $stop acts like $finish (set in main.cc). */
صحیح  stop_is_اختتام_exit_رمز = 0;

#اگر_غیر_تعریف_شدہ USE_READLINE
ساکن حرف* readline_stub(ثابت حرف*prompt)
{
      حرف buf[256];

      اگر (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      اگر (fgets(buf, حجم(buf), stdin)) {
	    حرف*nl = buf + لڑی_لمبائی(buf);
	    جب (nl > buf && خلا_ہے(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    واپس strdup(buf);
      }

      واپس 0;
}
#تعریف_کرو readline(x) readline_stub(x)
#اگر_ختم


ساکن منطقی interact_flag = سچ;

ساکن خالی cmd_call(غیرعلامتی argc, ثابت حرف*argv[])
{
      __vpiHandle**جدول;
      غیرعلامتی ntable;

      اگر (stop_current_scope == 0) {
	    vpip_make_جڑ_iterator(جدول, ntable);

      } ورنہ {
	    جدول = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      غیرعلامتی vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(صفر_حافظہ_دو(vpi_argc, حجم(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(صفر_حافظہ_دو(vpi_argc, حجم(vpiHandle)));

      غیرعلامتی errors = 0;

      برائے (غیرعلامتی idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    منطقی جوڑو_مقصد_free_list = جھوٹ;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    اگر (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    اگر (handle == 0 && argv[idx+1][0] == '"') {
		  حرف*tmp = strdup(argv[idx+1]);
		  tmp[لڑی_لمبائی(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), جھوٹ);
		  جوڑو_مقصد_free_list = سچ;
		  آزاد(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    اگر (handle == 0
		&& strspn(argv[idx+1],"0123456789") == لڑی_لمبائی(argv[idx+1])) {
		  handle = نیا __vpiDecConst(strtol(argv[idx+1],0,10));
		  جوڑو_مقصد_free_list = سچ;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    برائے (غیرعلامتی tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ثابت __vpiScope*scope;
		  ثابت حرف*name;

		  انتخاب (جدول[tmp]->get_type_رمز()) {

		      حالت vpiModule:
		      حالت vpiGenScope:
		      حالت vpiFunction:
		      حالت vpiTask:
		      حالت vpiNamedBegin:
		      حالت vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(جدول[tmp]);
			اگر (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = جدول[tmp];
			توڑو;

		      حالت vpiReg:
		      حالت vpiNet:
		      حالت vpiParameter:
			name = vpi_get_str(vpiName,جدول[tmp]);
			اگر (strcmp(argv[idx+1], name) == 0)
			      handle = جدول[tmp];
			توڑو;
		  }
	    }

	    اگر (handle == 0) {
		  لکھو("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    اگر (جوڑو_مقصد_free_list)
		  vpi_free[idx] = handle;
	    ورنہ
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      اگر (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                سچ, جھوٹ,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    اگر (call_handle == 0)
		  جاؤ out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      برائے (غیرعلامتی idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    اگر (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      آزاد(vpi_argv);
      آزاد(vpi_free);
}

ساکن خالی cmd_cont(غیرعلامتی, ثابت حرف*[])
{
      interact_flag = جھوٹ;
}

ساکن خالی cmd_قدم(غیرعلامتی, ثابت حرف*[])
{
      interact_flag = جھوٹ;
      جدول_وقت_single_قدم(0);
}

ساکن خالی cmd_اختتام(غیرعلامتی, ثابت حرف*[])
{
      interact_flag = جھوٹ;
      جدول_وقت_اختتام(0);
}

ساکن خالی cmd_help(غیرعلامتی, ثابت حرف*[]);

ساکن خالی cmd_list(غیرعلامتی, ثابت حرف*[])
{
      __vpiHandle**جدول;
      غیرعلامتی ntable;

      اگر (stop_current_scope == 0) {
	    vpip_make_جڑ_iterator(جدول, ntable);

      } ورنہ {
	    جدول = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      لکھو("%u items in this scope:\n", ntable);
      برائے (غیرعلامتی idx = 0 ;  idx < ntable ;  idx += 1) {

	    ثابت __vpiScope*scope;
	    ساخت __vpiSignal*sig;

	    انتخاب (جدول[idx]->get_type_رمز()) {
		حالت vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("package : %s\n", scope->scope_name());
		  توڑو;

		حالت vpiModule:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("module  : %s\n", scope->scope_name());
		  توڑو;

		حالت vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("generate: %s\n", scope->scope_name());
		  توڑو;

		حالت vpiTask:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("task    : %s\n", scope->scope_name());
		  توڑو;

		حالت vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("function: %s\n", scope->scope_name());
		  توڑو;

		حالت vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("block   : %s\n", scope->scope_name());
		  توڑو;

		حالت vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("fork    : %s\n", scope->scope_name());
		  توڑو;

		حالت vpiParameter:
		  لکھو("param   : %s\n", vpi_get_str(vpiName, جدول[idx]));
		  توڑو;

		حالت vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(جدول[idx]);
		  اگر ((sig->msb.get_قدر() == 0) && (sig->lsb.get_قدر() == 0))
			لکھو("reg     : %s%s\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "");
		  ورنہ
			لکھو("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_قدر(), sig->lsb.get_قدر());
		  توڑو;

		حالت vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(جدول[idx]);
		  اگر ((sig->msb.get_قدر() == 0) && (sig->lsb.get_قدر() == 0))
			لکھو("net     : %s%s\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "");
		  ورنہ
			لکھو("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_قدر(), sig->lsb.get_قدر());
		  توڑو;

		حالت vpi_پورٹ:
		  لکھو("port    : %s -- %s\n",
			 vpi_get_str(vpiName, جدول[idx]),
			 direction_as_string(vpi_get(vpiDirection, جدول[idx])));
		  توڑو;

		طےشدہ:
		  لکھو("%8s: <vpi handle>\n",
			 vpi_type_as_string(جدول[idx]->get_type_رمز()));
		  توڑو;
	    }

      }
}

ساکن خالی cmd_بھرو(غیرعلامتی argc, ثابت حرف*argv[])
{
      غیرعلامتی idx;

      برائے (idx = 1 ;  idx < argc ;  idx += 1) {
	    لکھو("Loading module %s...\n", argv[idx]);
	    vpip_بھرو_module(argv[idx]);
      }
}

ساکن خالی cmd_نکالو(غیرعلامتی, ثابت حرف*[])
{
      اگر (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ساکن خالی cmd_ڈالو(غیرعلامتی argc, ثابت حرف* argv[])
{
      برائے (غیرعلامتی idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**جدول;
	    غیرعلامتی ntable;

	    اگر (stop_current_scope) {
		  جدول = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } ورنہ {
		  vpip_make_جڑ_iterator(جدول, ntable);
	    }

	    __vpiScope*بچہ = 0;
	    برائے (غیرعلامتی tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(جدول[tmp]);
		  اگر (scope == 0)
			جاری;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  اگر (strcmp(scope->scope_name(), argv[idx]) == 0) {
			بچہ = scope;
			توڑو;
		  }
	    }

	    اگر (بچہ == 0) {
		  لکھو("Scope %s not found.\n", argv[idx]);
		  واپس;
	    }

	    stop_current_scope = بچہ;
      }
}

ساکن خالی cmd_وقت(غیرعلامتی, ثابت حرف*[])
{
      vvp_time64_t ticks = جدول_وقت_simtime();
      لکھو("%" وقت_FMT_U " ticks\n", ticks);
}

ساکن خالی cmd_trace(غیرعلامتی argc, ثابت حرف*argv[])
{
      أكد(argc);
      انتخاب (argc) {
	حالت 1:
	    show_file_سطر = سچ;
	    توڑو;
	طےشدہ:
	    لکھو("Only using the first argument to trace.\n");
	    // fallthrough
	حالت 2:
	    اگر ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_سطر = سچ;
	    } ورنہ show_file_سطر = جھوٹ;
	    توڑو;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      اگر (!رمز_is_instrumented) {
	    لکھو("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    لکھو("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_سطر = جھوٹ;
      } ورنہ {
	    لکھو("Turning statement tracing %s.\n",
	           show_file_سطر ? "on" : "off");
      }
}

ساکن خالی cmd_where(غیرعلامتی, ثابت حرف*[])
{
      ثابت __vpiScope*cur = stop_current_scope;

      جب (cur) {
	    انتخاب (cur->get_type_رمز()) {
		حالت vpiModule:
		  لکھو("module %s\n", cur->scope_name());
		  توڑو;
		حالت vpiGenScope:
		  لکھو("generate %s\n", cur->scope_name());
		  توڑو;
		طےشدہ:
		  لکھو("scope (%d) %s;\n", cur->get_type_رمز(), cur->scope_name());
		  توڑو;
	    }

	    cur = cur->scope;
      }
}

ساکن خالی cmd_unknown(غیرعلامتی, ثابت حرف*argv[])
{
      لکھو("Unknown command: %s\n", argv[0]);
      لکھو("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ساکن ساخت {
      ثابت حرف*name;
      خالی (*proc)(غیرعلامتی argc, ثابت حرف*argv[]);
      ثابت حرف*summary;
} cmd_جدول[] = {
      { "cd",     &cmd_ڈالو,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_اختتام,
        "Synonym for finish."},
      { "finish", &cmd_اختتام,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_بھرو,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_نکالو,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ڈالو,
        "Descend into the named scope."},
      { "step",   &cmd_قدم,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_وقت,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

ساکن خالی cmd_help(غیرعلامتی, ثابت حرف*[])
{
      لکھو("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      برائے (غیرعلامتی idx = 0 ;  cmd_جدول[idx].name != 0 ;  idx += 1) {
	    لکھو("%-8s - %s\n", cmd_جدول[idx].name, cmd_جدول[idx].summary);
      }

      لکھو("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ساکن خالی invoke_command(حرف*txt)
{
      غیرعلامتی argc = 0;
      ثابت حرف**argv = const_cast<ثابت حرف**>(نیا حرف*[لڑی_لمبائی(txt)/2]);

	/* Chop the line into words. */
      برائے (حرف*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    اگر (cp[0] == '"') {
		  حرف*tmp = strchr(cp+1, '"');
		  اگر (tmp == 0) {
			لکھو("Missing close-quote: %s\n", cp);
			حذف[]argv;
			واپس;
		  }

		  cp = tmp + 1;

	    } ورنہ {
		  cp += strcspn(cp, " ");
	    }

	    اگر (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      اگر (argc > 0) {

	    اگر (argv[0][0] == '$') {
		  اگر (strcmp(argv[0], "$stop") == 0) {
			لکھو("The simulator is already stopped!\n");
		  } ورنہ اگر (strcmp(argv[0], "$finish") == 0){
			cmd_اختتام(argc, argv);
		  } ورنہ {
			cmd_call(argc, argv);
		  }
	    } ورنہ {
		  غیرعلامتی idx;
		  برائے (idx = 0 ;  cmd_جدول[idx].name ;  idx += 1)
			اگر (strcmp(cmd_جدول[idx].name, argv[0]) == 0)
			      توڑو;

		  cmd_جدول[idx].proc (argc, argv);
	    }

      }

      حذف[]argv;
}

ساکن خالی invoke_command_const(ثابت حرف*txt)
{
      حرف *vtxt = strdup(txt);
      invoke_command(vtxt);
      آزاد(vtxt);
}

خالی stop_handler(صحیح rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      اگر (stop_is_اختتام) {
	    vpip_سیٹ_return_قدر(stop_is_اختتام_exit_رمز);
	    جدول_وقت_اختتام(0);
	    واپس;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" وقت_FMT_U " ticks.\n",
		     جدول_وقت_simtime());

      interact_flag = سچ;
      جب (interact_flag) {
	    حرف*داخلہ = readline("> ");
	    اگر (داخلہ == 0)
		  توڑو;


	      /* Advance to the first input character. */
	    حرف*پہلا = داخلہ;
	    جب (*پہلا && خلا_ہے(*پہلا))
		  پہلا += 1;

	    اگر (پہلا[0] != 0) {
#اگر_تعریف_شدہ USE_HISTORY
		  جوڑو_history(پہلا);
#اگر_ختم
		  invoke_command(پہلا);
	    }

	    آزاد(داخلہ);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
