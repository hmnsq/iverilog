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
#اگر_تعریف_شده USE_READLINE
# include  <readline/readline.h>
#پایان_اگر
#اگر_تعریف_شده USE_HISTORY
# include  <readline/history.h>
#پایان_اگر
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
منطقي stop_is_پایان = خطأ;  /* When set, $stop acts like $finish (set in main.cc). */
صحيح  stop_is_پایان_exit_کد = 0;

#اگر_تعریف_نشده USE_READLINE
ساكن حرف* readline_stub(ثابت حرف*prompt)
{
      حرف buf[256];

      إذا (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      إذا (fgets(buf, حجم(buf), stdin)) {
	    حرف*nl = buf + طول_رشته(buf);
	    طالما (nl > buf && آیا_فاصله(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    أرجع strdup(buf);
      }

      أرجع 0;
}
#تعریف_کن readline(x) readline_stub(x)
#پایان_اگر


ساكن منطقي interact_flag = درست;

ساكن فراغ cmd_call(بیعلامت argc, ثابت حرف*argv[])
{
      __vpiHandle**جدول;
      بیعلامت ntable;

      إذا (stop_current_scope == 0) {
	    vpip_make_ریشه_iterator(جدول, ntable);

      } وإلا {
	    جدول = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      بیعلامت vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(حافظه_صفر_بده(vpi_argc, حجم(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(حافظه_صفر_بده(vpi_argc, حجم(vpiHandle)));

      بیعلامت errors = 0;

      لكل (بیعلامت idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    منطقي جمع_کن_مقصد_free_list = خطأ;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    إذا (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    إذا (handle == 0 && argv[idx+1][0] == '"') {
		  حرف*tmp = strdup(argv[idx+1]);
		  tmp[طول_رشته(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), خطأ);
		  جمع_کن_مقصد_free_list = درست;
		  حرر(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    إذا (handle == 0
		&& strspn(argv[idx+1],"0123456789") == طول_رشته(argv[idx+1])) {
		  handle = جديد __vpiDecConst(strtol(argv[idx+1],0,10));
		  جمع_کن_مقصد_free_list = درست;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    لكل (بیعلامت tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ثابت __vpiScope*scope;
		  ثابت حرف*name;

		  بدل (جدول[tmp]->get_type_کد()) {

		      حالة vpiModule:
		      حالة vpiGenScope:
		      حالة vpiFunction:
		      حالة vpiTask:
		      حالة vpiNamedBegin:
		      حالة vpiNamedFork:
			scope = dynamic_cast<__vpiScope*>(جدول[tmp]);
			إذا (strcmp(scope->scope_name(), argv[idx+1]) == 0)
			      handle = جدول[tmp];
			اكسر;

		      حالة vpiReg:
		      حالة vpiNet:
		      حالة vpiParameter:
			name = vpi_get_str(vpiName,جدول[tmp]);
			إذا (strcmp(argv[idx+1], name) == 0)
			      handle = جدول[tmp];
			اكسر;
		  }
	    }

	    إذا (handle == 0) {
		  چاپ_کن("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    إذا (جمع_کن_مقصد_free_list)
		  vpi_free[idx] = handle;
	    وإلا
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      إذا (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                درست, خطأ,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    إذا (call_handle == 0)
		  اذهب out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      لكل (بیعلامت idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    إذا (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      حرر(vpi_argv);
      حرر(vpi_free);
}

ساكن فراغ cmd_cont(بیعلامت, ثابت حرف*[])
{
      interact_flag = خطأ;
}

ساكن فراغ cmd_گام(بیعلامت, ثابت حرف*[])
{
      interact_flag = خطأ;
      زمانبندی_single_گام(0);
}

ساكن فراغ cmd_پایان(بیعلامت, ثابت حرف*[])
{
      interact_flag = خطأ;
      زمانبندی_پایان(0);
}

ساكن فراغ cmd_help(بیعلامت, ثابت حرف*[]);

ساكن فراغ cmd_list(بیعلامت, ثابت حرف*[])
{
      __vpiHandle**جدول;
      بیعلامت ntable;

      إذا (stop_current_scope == 0) {
	    vpip_make_ریشه_iterator(جدول, ntable);

      } وإلا {
	    جدول = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      چاپ_کن("%u items in this scope:\n", ntable);
      لكل (بیعلامت idx = 0 ;  idx < ntable ;  idx += 1) {

	    ثابت __vpiScope*scope;
	    بنية __vpiSignal*sig;

	    بدل (جدول[idx]->get_type_کد()) {
		حالة vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("package : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiModule:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("module  : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("generate: %s\n", scope->scope_name());
		  اكسر;

		حالة vpiTask:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("task    : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("function: %s\n", scope->scope_name());
		  اكسر;

		حالة vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("block   : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  چاپ_کن("fork    : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiParameter:
		  چاپ_کن("param   : %s\n", vpi_get_str(vpiName, جدول[idx]));
		  اكسر;

		حالة vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(جدول[idx]);
		  إذا ((sig->msb.get_مقدار() == 0) && (sig->lsb.get_مقدار() == 0))
			چاپ_کن("reg     : %s%s\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "");
		  وإلا
			چاپ_کن("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_مقدار(), sig->lsb.get_مقدار());
		  اكسر;

		حالة vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(جدول[idx]);
		  إذا ((sig->msb.get_مقدار() == 0) && (sig->lsb.get_مقدار() == 0))
			چاپ_کن("net     : %s%s\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "");
		  وإلا
			چاپ_کن("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_مقدار(), sig->lsb.get_مقدار());
		  اكسر;

		حالة vpi_پورت:
		  چاپ_کن("port    : %s -- %s\n",
			 vpi_get_str(vpiName, جدول[idx]),
			 direction_as_string(vpi_get(vpiDirection, جدول[idx])));
		  اكسر;

		افتراضي:
		  چاپ_کن("%8s: <vpi handle>\n",
			 vpi_type_as_string(جدول[idx]->get_type_کد()));
		  اكسر;
	    }

      }
}

ساكن فراغ cmd_بارگیری(بیعلامت argc, ثابت حرف*argv[])
{
      بیعلامت idx;

      لكل (idx = 1 ;  idx < argc ;  idx += 1) {
	    چاپ_کن("Loading module %s...\n", argv[idx]);
	    vpip_بارگیری_module(argv[idx]);
      }
}

ساكن فراغ cmd_برداشت(بیعلامت, ثابت حرف*[])
{
      إذا (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ساكن فراغ cmd_افزودن(بیعلامت argc, ثابت حرف* argv[])
{
      لكل (بیعلامت idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**جدول;
	    بیعلامت ntable;

	    إذا (stop_current_scope) {
		  جدول = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } وإلا {
		  vpip_make_ریشه_iterator(جدول, ntable);
	    }

	    __vpiScope*فرزند = 0;
	    لكل (بیعلامت tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(جدول[tmp]);
		  إذا (scope == 0)
			استمر;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  إذا (strcmp(scope->scope_name(), argv[idx]) == 0) {
			فرزند = scope;
			اكسر;
		  }
	    }

	    إذا (فرزند == 0) {
		  چاپ_کن("Scope %s not found.\n", argv[idx]);
		  أرجع;
	    }

	    stop_current_scope = فرزند;
      }
}

ساكن فراغ cmd_زمان(بیعلامت, ثابت حرف*[])
{
      vvp_time64_t ticks = زمانبندی_simtime();
      چاپ_کن("%" زمان_FMT_U " ticks\n", ticks);
}

ساكن فراغ cmd_trace(بیعلامت argc, ثابت حرف*argv[])
{
      أكد(argc);
      بدل (argc) {
	حالة 1:
	    show_file_سطر = درست;
	    اكسر;
	افتراضي:
	    چاپ_کن("Only using the first argument to trace.\n");
	    // fallthrough
	حالة 2:
	    إذا ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_سطر = درست;
	    } وإلا show_file_سطر = خطأ;
	    اكسر;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      إذا (!کد_is_instrumented) {
	    چاپ_کن("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    چاپ_کن("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_سطر = خطأ;
      } وإلا {
	    چاپ_کن("Turning statement tracing %s.\n",
	           show_file_سطر ? "on" : "off");
      }
}

ساكن فراغ cmd_where(بیعلامت, ثابت حرف*[])
{
      ثابت __vpiScope*cur = stop_current_scope;

      طالما (cur) {
	    بدل (cur->get_type_کد()) {
		حالة vpiModule:
		  چاپ_کن("module %s\n", cur->scope_name());
		  اكسر;
		حالة vpiGenScope:
		  چاپ_کن("generate %s\n", cur->scope_name());
		  اكسر;
		افتراضي:
		  چاپ_کن("scope (%d) %s;\n", cur->get_type_کد(), cur->scope_name());
		  اكسر;
	    }

	    cur = cur->scope;
      }
}

ساكن فراغ cmd_unknown(بیعلامت, ثابت حرف*argv[])
{
      چاپ_کن("Unknown command: %s\n", argv[0]);
      چاپ_کن("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ساكن بنية {
      ثابت حرف*name;
      فراغ (*proc)(بیعلامت argc, ثابت حرف*argv[]);
      ثابت حرف*summary;
} cmd_جدول[] = {
      { "cd",     &cmd_افزودن,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_پایان,
        "Synonym for finish."},
      { "finish", &cmd_پایان,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_بارگیری,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_برداشت,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_افزودن,
        "Descend into the named scope."},
      { "step",   &cmd_گام,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_زمان,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

ساكن فراغ cmd_help(بیعلامت, ثابت حرف*[])
{
      چاپ_کن("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      لكل (بیعلامت idx = 0 ;  cmd_جدول[idx].name != 0 ;  idx += 1) {
	    چاپ_کن("%-8s - %s\n", cmd_جدول[idx].name, cmd_جدول[idx].summary);
      }

      چاپ_کن("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ساكن فراغ invoke_command(حرف*txt)
{
      بیعلامت argc = 0;
      ثابت حرف**argv = const_cast<ثابت حرف**>(جديد حرف*[طول_رشته(txt)/2]);

	/* Chop the line into words. */
      لكل (حرف*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    إذا (cp[0] == '"') {
		  حرف*tmp = strchr(cp+1, '"');
		  إذا (tmp == 0) {
			چاپ_کن("Missing close-quote: %s\n", cp);
			حذف[]argv;
			أرجع;
		  }

		  cp = tmp + 1;

	    } وإلا {
		  cp += strcspn(cp, " ");
	    }

	    إذا (*cp) *cp++ = 0;

	    argc += 1;
      }


	/* Look up the command, using argv[0] as the key. */
      إذا (argc > 0) {

	    إذا (argv[0][0] == '$') {
		  إذا (strcmp(argv[0], "$stop") == 0) {
			چاپ_کن("The simulator is already stopped!\n");
		  } وإلا إذا (strcmp(argv[0], "$finish") == 0){
			cmd_پایان(argc, argv);
		  } وإلا {
			cmd_call(argc, argv);
		  }
	    } وإلا {
		  بیعلامت idx;
		  لكل (idx = 0 ;  cmd_جدول[idx].name ;  idx += 1)
			إذا (strcmp(cmd_جدول[idx].name, argv[0]) == 0)
			      اكسر;

		  cmd_جدول[idx].proc (argc, argv);
	    }

      }

      حذف[]argv;
}

ساكن فراغ invoke_command_const(ثابت حرف*txt)
{
      حرف *vtxt = strdup(txt);
      invoke_command(vtxt);
      حرر(vtxt);
}

فراغ stop_handler(صحيح rc)
{
        /* The user may be running in a non-interactive environment, so
         * they want $stop and <Control-C> to be the same as $finish. */
      إذا (stop_is_پایان) {
	    vpip_مجموعه_return_مقدار(stop_is_پایان_exit_کد);
	    زمانبندی_پایان(0);
	    أرجع;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" زمان_FMT_U " ticks.\n",
		     زمانبندی_simtime());

      interact_flag = درست;
      طالما (interact_flag) {
	    حرف*درون_داد = readline("> ");
	    إذا (درون_داد == 0)
		  اكسر;


	      /* Advance to the first input character. */
	    حرف*اولی = درون_داد;
	    طالما (*اولی && آیا_فاصله(*اولی))
		  اولی += 1;

	    إذا (اولی[0] != 0) {
#اگر_تعریف_شده USE_HISTORY
		  جمع_کن_history(اولی);
#پایان_اگر
		  invoke_command(اولی);
	    }

	    حرر(درون_داد);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
