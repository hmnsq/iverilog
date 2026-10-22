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
#جے_تعریف_شدہ USE_READLINE
# include  <readline/readline.h>
#جے_انت
#جے_تعریف_شدہ USE_HISTORY
# include  <readline/history.h>
#جے_انت
# include  <cstring>
# include  <cstdlib>
# include  "ivl_alloc.h"

__vpiScope*stop_current_scope = 0;
منطقي stop_is_انت = خطأ;  /* When set, $stop acts like $finish (set in main.cc). */
صحيح  stop_is_انت_exit_کوڈ = 0;

#جے_بنا_تعریف USE_READLINE
ساكن حرف* readline_stub(ثابت حرف*prompt)
{
      حرف buf[256];

      إذا (prompt && prompt[0]) {
	    fputs(prompt, stdout);
	    fflush(stdout);
      }

      إذا (fgets(buf, حجم(buf), stdin)) {
	    حرف*nl = buf + لڑی_لمبائی(buf);
	    طالما (nl > buf && خلا_اے(nl[-1])) {
		  nl -= 1;
		  nl[0] = 0;
	    }

	    أرجع strdup(buf);
      }

      أرجع 0;
}
#تعریف_کرو readline(x) readline_stub(x)
#جے_انت


ساكن منطقي interact_flag = سچ;

ساكن فراغ cmd_call(بنانشانی argc, ثابت حرف*argv[])
{
      __vpiHandle**جدول;
      بنانشانی ntable;

      إذا (stop_current_scope == 0) {
	    vpip_make_جڑ_iterator(جدول, ntable);

      } وإلا {
	    جدول = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

	/* This is an array of vpiHandles, for passing to the created
	   command. */
      بنانشانی vpi_argc = argc - 1;
      vpiHandle*vpi_argv = static_cast<vpiHandle*>(صفر_حافظہ_دیو(vpi_argc, حجم(vpiHandle)));
      vpiHandle*vpi_free = static_cast<vpiHandle*>(صفر_حافظہ_دیو(vpi_argc, حجم(vpiHandle)));

      بنانشانی errors = 0;

      لكل (بنانشانی idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    vpiHandle handle = 0;
	    منطقي جوڑو_منزل_free_list = خطأ;

	      /* Detect the special case that the argument is the
		 .(dot) string. This represents the handle for the
		 current scope. */
	    إذا (stop_current_scope && (strcmp(argv[idx+1], ".") == 0))
		  handle = stop_current_scope;

	      /* Is the argument a quoted string? */
	    إذا (handle == 0 && argv[idx+1][0] == '"') {
		  حرف*tmp = strdup(argv[idx+1]);
		  tmp[لڑی_لمبائی(tmp)-1] = 0;

		    /* Create a temporary vpiStringConst to pass as a
		       handle. Make it temporary so that memory is
		       reclaimed after the call is completed. */
		  handle = vpip_make_string_const(strdup(tmp+1), خطأ);
		  جوڑو_منزل_free_list = سچ;
		  حرر(tmp);
	    }

	      /* Is the argument a decimal constant? */
	    إذا (handle == 0
		&& strspn(argv[idx+1],"0123456789") == لڑی_لمبائی(argv[idx+1])) {
		  handle = جديد __vpiDecConst(strtol(argv[idx+1],0,10));
		  جوڑو_منزل_free_list = سچ;
	    }

	      /* Try to find the vpiHandle within this scope that has
		 the name in argv[idx+2]. Look in the current scope. */

	    لكل (بنانشانی tmp = 0 ;  (tmp < ntable)&& !handle ;  tmp += 1) {
		  ثابت __vpiScope*scope;
		  ثابت حرف*name;

		  بدل (جدول[tmp]->get_type_کوڈ()) {

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
		  لکھو("call error: I don't know how to"
			 " pass %s to %s\n", argv[idx+1], argv[0]);
		  errors += 1;
	    }

	    vpi_argv[idx] = handle;
	    إذا (جوڑو_منزل_free_list)
		  vpi_free[idx] = handle;
	    وإلا
		  vpi_free[idx] = 0;

      }

	/* If there are no errors so far, then make up a call to the
	   vpi task and execute that call. Free the call structure
	   when we finish. */
      إذا (errors == 0) {
	    vpiHandle call_handle = vpip_build_vpi_call(argv[0], 0, 0, 0,
	                                                سچ, خطأ,
	                                                vpi_argc, vpi_argv,
							0, 0, 0,
	                                                1, 0);
	    إذا (call_handle == 0)
		  اذهب out;

	    vpip_execute_vpi_call(0, call_handle);
	    vpi_free_object(call_handle);
      }

 out:
      لكل (بنانشانی idx = 0 ;  idx < vpi_argc ;  idx += 1) {
	    إذا (vpi_free[idx])
		  vpi_free_object(vpi_free[idx]);
      }

      حرر(vpi_argv);
      حرر(vpi_free);
}

ساكن فراغ cmd_cont(بنانشانی, ثابت حرف*[])
{
      interact_flag = خطأ;
}

ساكن فراغ cmd_قدم(بنانشانی, ثابت حرف*[])
{
      interact_flag = خطأ;
      ویلا_سارنی_single_قدم(0);
}

ساكن فراغ cmd_انت(بنانشانی, ثابت حرف*[])
{
      interact_flag = خطأ;
      ویلا_سارنی_انت(0);
}

ساكن فراغ cmd_help(بنانشانی, ثابت حرف*[]);

ساكن فراغ cmd_list(بنانشانی, ثابت حرف*[])
{
      __vpiHandle**جدول;
      بنانشانی ntable;

      إذا (stop_current_scope == 0) {
	    vpip_make_جڑ_iterator(جدول, ntable);

      } وإلا {
	    جدول = &stop_current_scope->intern[0];
	    ntable = stop_current_scope->intern.size();
      }

      لکھو("%u items in this scope:\n", ntable);
      لكل (بنانشانی idx = 0 ;  idx < ntable ;  idx += 1) {

	    ثابت __vpiScope*scope;
	    بنية __vpiSignal*sig;

	    بدل (جدول[idx]->get_type_کوڈ()) {
		حالة vpiPackage:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("package : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiModule:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("module  : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiGenScope:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("generate: %s\n", scope->scope_name());
		  اكسر;

		حالة vpiTask:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("task    : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiFunction:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("function: %s\n", scope->scope_name());
		  اكسر;

		حالة vpiNamedBegin:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("block   : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiNamedFork:
		  scope = dynamic_cast<__vpiScope*>(جدول[idx]);
		  لکھو("fork    : %s\n", scope->scope_name());
		  اكسر;

		حالة vpiParameter:
		  لکھو("param   : %s\n", vpi_get_str(vpiName, جدول[idx]));
		  اكسر;

		حالة vpiReg:
		  sig = dynamic_cast<__vpiSignal*>(جدول[idx]);
		  إذا ((sig->msb.get_قدر() == 0) && (sig->lsb.get_قدر() == 0))
			لکھو("reg     : %s%s\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "");
		  وإلا
			لکھو("reg     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_قدر(), sig->lsb.get_قدر());
		  اكسر;

		حالة vpiNet:
		  sig = dynamic_cast<__vpiSignal*>(جدول[idx]);
		  إذا ((sig->msb.get_قدر() == 0) && (sig->lsb.get_قدر() == 0))
			لکھو("net     : %s%s\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "");
		  وإلا
			لکھو("net     : %s%s[%d:%d]\n",
			       vpi_get_str(vpiName, جدول[idx]),
			       sig->signed_flag? "signed " : "",
			       sig->msb.get_قدر(), sig->lsb.get_قدر());
		  اكسر;

		حالة vpi_پورٹ:
		  لکھو("port    : %s -- %s\n",
			 vpi_get_str(vpiName, جدول[idx]),
			 direction_as_string(vpi_get(vpiDirection, جدول[idx])));
		  اكسر;

		افتراضي:
		  لکھو("%8s: <vpi handle>\n",
			 vpi_type_as_string(جدول[idx]->get_type_کوڈ()));
		  اكسر;
	    }

      }
}

ساكن فراغ cmd_لدو(بنانشانی argc, ثابت حرف*argv[])
{
      بنانشانی idx;

      لكل (idx = 1 ;  idx < argc ;  idx += 1) {
	    لکھو("Loading module %s...\n", argv[idx]);
	    vpip_لدو_module(argv[idx]);
      }
}

ساكن فراغ cmd_نکالو(بنانشانی, ثابت حرف*[])
{
      إذا (stop_current_scope != 0)
	    stop_current_scope = stop_current_scope->scope;
}

ساكن فراغ cmd_ڈالو(بنانشانی argc, ثابت حرف* argv[])
{
      لكل (بنانشانی idx = 1 ;  idx < argc ;  idx += 1) {
	    __vpiHandle**جدول;
	    بنانشانی ntable;

	    إذا (stop_current_scope) {
		  جدول = &stop_current_scope->intern[0];
		  ntable = stop_current_scope->intern.size();
	    } وإلا {
		  vpip_make_جڑ_iterator(جدول, ntable);
	    }

	    __vpiScope*بچہ = 0;
	    لكل (بنانشانی tmp = 0 ;  tmp < ntable ;  tmp += 1) {
		  // Try to convert this item to a scope. If this is not a
		  // scope, then continue.
		  __vpiScope*scope = dynamic_cast<__vpiScope*>(جدول[tmp]);
		  إذا (scope == 0)
			استمر;

		  // This is a scope, and the name matches, then
		  // report that I found the child.
		  إذا (strcmp(scope->scope_name(), argv[idx]) == 0) {
			بچہ = scope;
			اكسر;
		  }
	    }

	    إذا (بچہ == 0) {
		  لکھو("Scope %s not found.\n", argv[idx]);
		  أرجع;
	    }

	    stop_current_scope = بچہ;
      }
}

ساكن فراغ cmd_ویلا(بنانشانی, ثابت حرف*[])
{
      vvp_time64_t ticks = ویلا_سارنی_simtime();
      لکھو("%" ویلا_FMT_U " ticks\n", ticks);
}

ساكن فراغ cmd_trace(بنانشانی argc, ثابت حرف*argv[])
{
      أكد(argc);
      بدل (argc) {
	حالة 1:
	    show_file_سطر = سچ;
	    اكسر;
	افتراضي:
	    لکھو("Only using the first argument to trace.\n");
	    // fallthrough
	حالة 2:
	    إذا ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		  show_file_سطر = سچ;
	    } وإلا show_file_سطر = خطأ;
	    اكسر;
      }

	/* You can't trace a file if the compiler didn't insert the
	 * %file_line opcodes. */
      إذا (!کوڈ_is_instrumented) {
	    لکھو("The vvp input must be instrumented before tracing is "
	           "available.\n");
	    لکھو("Recompile with the -pfileline=1 flag to instrument "
	           "the input.\n");
	    show_file_سطر = خطأ;
      } وإلا {
	    لکھو("Turning statement tracing %s.\n",
	           show_file_سطر ? "on" : "off");
      }
}

ساكن فراغ cmd_where(بنانشانی, ثابت حرف*[])
{
      ثابت __vpiScope*cur = stop_current_scope;

      طالما (cur) {
	    بدل (cur->get_type_کوڈ()) {
		حالة vpiModule:
		  لکھو("module %s\n", cur->scope_name());
		  اكسر;
		حالة vpiGenScope:
		  لکھو("generate %s\n", cur->scope_name());
		  اكسر;
		افتراضي:
		  لکھو("scope (%d) %s;\n", cur->get_type_کوڈ(), cur->scope_name());
		  اكسر;
	    }

	    cur = cur->scope;
      }
}

ساكن فراغ cmd_unknown(بنانشانی, ثابت حرف*argv[])
{
      لکھو("Unknown command: %s\n", argv[0]);
      لکھو("Try the help command to get a summary\n"
	     "of available commands.\n");
}

ساكن بنية {
      ثابت حرف*name;
      فراغ (*proc)(بنانشانی argc, ثابت حرف*argv[]);
      ثابت حرف*summary;
} cmd_جدول[] = {
      { "cd",     &cmd_ڈالو,
        "Synonym for push."},
      { "cont",   &cmd_cont,
        "Resume (continue) the simulation"},
      { "exit", &cmd_انت,
        "Synonym for finish."},
      { "finish", &cmd_انت,
        "Finish the simulation."},
      { "help",   &cmd_help,
        "Get help."},
      { "list",   &cmd_list,
        "List items in the current scope."},
      { "load",   &cmd_لدو,
        "Load a VPI module, a la vvp -m."},
      { "ls",     &cmd_list,
        "Shorthand for \"list\"."},
      { "pop",    &cmd_نکالو,
        "Pop one scope from the scope stack."},
      { "push",   &cmd_ڈالو,
        "Descend into the named scope."},
      { "step",   &cmd_قدم,
        "Single-step the scheduler for 1 event."},
      { "time",   &cmd_ویلا,
        "Print the current simulation time."},
      { "trace",   &cmd_trace,
        "Control statement tracing (on/off) when the code is instrumented."},
      { "where",  &cmd_where,
        "Show current scope, and scope hierarchy stack."},
      { 0,        &cmd_unknown, 0}
};

ساكن فراغ cmd_help(بنانشانی, ثابت حرف*[])
{
      لکھو("Commands can be from the following table of base commands,\n"
	     "or can be invocations of system tasks/functions.\n\n");
      لكل (بنانشانی idx = 0 ;  cmd_جدول[idx].name != 0 ;  idx += 1) {
	    لکھو("%-8s - %s\n", cmd_جدول[idx].name, cmd_جدول[idx].summary);
      }

      لکھو("\nIf the command name starts with a '$' character, it\n"
	     "is taken to be the name of a system task, and a call is\n"
	     "built up and executed. For example, \"$display foo\" will\n"
	     "call the function as $display(foo).\n");
}


ساكن فراغ invoke_command(حرف*txt)
{
      بنانشانی argc = 0;
      ثابت حرف**argv = const_cast<ثابت حرف**>(جديد حرف*[لڑی_لمبائی(txt)/2]);

	/* Chop the line into words. */
      لكل (حرف*cp = txt+strspn(txt, " ")
		 ; *cp; cp += strspn(cp, " ")) {
	    argv[argc] = cp;

	    إذا (cp[0] == '"') {
		  حرف*tmp = strchr(cp+1, '"');
		  إذا (tmp == 0) {
			لکھو("Missing close-quote: %s\n", cp);
			مٹاؤ[]argv;
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
			لکھو("The simulator is already stopped!\n");
		  } وإلا إذا (strcmp(argv[0], "$finish") == 0){
			cmd_انت(argc, argv);
		  } وإلا {
			cmd_call(argc, argv);
		  }
	    } وإلا {
		  بنانشانی idx;
		  لكل (idx = 0 ;  cmd_جدول[idx].name ;  idx += 1)
			إذا (strcmp(cmd_جدول[idx].name, argv[0]) == 0)
			      اكسر;

		  cmd_جدول[idx].proc (argc, argv);
	    }

      }

      مٹاؤ[]argv;
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
      إذا (stop_is_انت) {
	    vpip_سیٹ_return_قدر(stop_is_انت_exit_کوڈ);
	    ویلا_سارنی_انت(0);
	    أرجع;
      }

      vpi_mcd_printf(1,"** VVP Stop(%d) **\n", rc);
      vpi_mcd_printf(1,"** Flushing output streams.\n");
      invoke_command_const("$fflush");
      invoke_command_const("$dumpflush");
      vpi_mcd_printf(1,"** Current simulation time is %" ویلا_FMT_U " ticks.\n",
		     ویلا_سارنی_simtime());

      interact_flag = سچ;
      طالما (interact_flag) {
	    حرف*داخلہ = readline("> ");
	    إذا (داخلہ == 0)
		  اكسر;


	      /* Advance to the first input character. */
	    حرف*پہلا = داخلہ;
	    طالما (*پہلا && خلا_اے(*پہلا))
		  پہلا += 1;

	    إذا (پہلا[0] != 0) {
#جے_تعریف_شدہ USE_HISTORY
		  جوڑو_history(پہلا);
#جے_انت
		  invoke_command(پہلا);
	    }

	    حرر(داخلہ);
      }

      vpi_mcd_printf(1,"** Continue **\n");
}
