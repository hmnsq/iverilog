/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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
 * Callbacks are objects that carry a function to be called when some
 * event in the simulation occurs. The VPI code create a __vpiCallback
 * object, and that object is put in some location that the simulation
 * can look when the event in question is tripped.
 */

# include  "vpi_user.h"
# include  "vpi_priv.h"
# include  "vvp_net.h"
# include  "schedule.h"
# include  "event.h"
# include  "vvp_net_sig.h"
# include  "config.h"
#अगर_घोषित CHECK_WITH_VALGRIND
#समावेश  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using namespace std;

अटल स्थिर अक्षर*cb_reason_name(PLI_INT32 reason)
{
      चयन (reason) {
	  हाल cb_मूल्य_Change:
	    वापस "cbValueChange";
	  हाल cbStmt:
	    वापस "cbStmt";
	  हाल cbForce:
	    वापस "cbForce";
	  हाल cbRelease:
	    वापस "cbRelease";
	  हाल cbAtStartOfSimTime:
	    वापस "cbAtStartOfSimTime";
	  हाल cbReadWriteSynch:
	    वापस "cbReadWriteSynch";
	  हाल cbReadOnlySynch:
	    वापस "cbReadOnlySynch";
	  हाल cb_अग्र_Sim_Time:
	    वापस "cbNextSimTime";
	  हाल cbAfterDelay:
	    वापस "cbAfterDelay";
	  हाल cbEndOfCompile:
	    वापस "cbEndOfCompile";
	  हाल cbStartOfSimulation:
	    वापस "cbStartOfSimulation";
	  हाल cbEndOfSimulation:
	    वापस "cbEndOfSimulation";
	  हाल cbError:
	    वापस "cbError";
	  हाल cbTchkViolation:
	    वापस "cbTchkViolation";
	  हाल cbStartOfSave:
	    वापस "cbStartOfSave";
	  हाल cbEndOfSave:
	    वापस "cbEndOfSave";
	  हाल cbStartOfRestart:
	    वापस "cbStartOfRestart";
	  हाल cbEndOfRestart:
	    वापस "cbEndOfRestart";
	  हाल cbStartOfReset:
	    वापस "cbStartOfReset";
	  हाल cbEndOfReset:
	    वापस "cbEndOfReset";
	  हाल cbEnterInteractive:
	    वापस "cbEnterInteractive";
	  हाल cbExitInteractive:
	    वापस "cbExitInteractive";
	  हाल cbInteractiveScopeChange:
	    वापस "cbInteractiveScopeChange";
	  हाल cbUnresolvedSystf:
	    वापस "cbUnresolvedSystf";
	  हाल cbAtEndOfSimTime:
	    वापस "cbAtEndOfSimTime";
	  शेष:
	    वापस "unrecognised";
      }
}

अटल bool check_callback_time(p_cb_तथ्य तथ्य, bool allow_suppress)
{
      निश्चित(तथ्य);
      अगर (!तथ्य->समय) {
	    अगर (!allow_suppress) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(तथ्य->reason));
		  वापस false;
	    }
	    वापस true;
      }
      चयन (तथ्य->समय->type) {
	  हाल vpiSimTime:
	    अवरोध;
	  हाल vpiScaledRealTime:
	    अवरोध;
	  हाल vpiSuppressTime:
	    अगर (!allow_suppress) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(तथ्य->reason));
		  वापस false;
	    }
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(तथ्य->reason));
	    वापस false;
      }
      वापस true;
}

अटल व्योम set_callback_time(p_cb_तथ्य तथ्य)
{
      निश्चित(तथ्य && तथ्य->समय);
      तथ्य->समय->निचला  = 0;
      तथ्य->समय->ऊपरी = 0;
      तथ्य->समय->real = 0.0;
      चयन (तथ्य->समय->type) {
	  हाल vpiSimTime:
	    vpip_time_to_timestruct(तथ्य->समय, schedule_simtime());
	    अवरोध;
	  हाल vpiScaledRealTime:
	    तथ्य->समय->real = vpip_scaled_time_from_handle(schedule_simtime(), तथ्य->obj);
	    अवरोध;
	  हाल vpiSuppressTime:
	    अवरोध;
	  शेष:
	    निश्चित(0);
	    अवरोध;
      }
}

/*
 * Callback handles are created when the VPI function registers a
 * callback. The handle is stored by the run time, and it triggered
 * when the run-time thing that it is waiting for happens.
 *
 * This is the thing that the VPI code references by the vpiHandle. It
 * also points to callback data that the caller attached to the event,
 * as well as the time structure to receive data.
 *
 * The cb_sync is a private member that points to the schedulable
 * event that is triggered when the event happens. The sync_cb class
 * represents the action to execute when the scheduler gets to this
 * event. This member is only used for things like cbReadOnlySync.
 */

श्रेणी sync_callback;

काष्ठा sync_cb  : खुला vvp_gen_event_s {
      sync_callback*handle;
      bool sync_flag;

      ~sync_cb () override { }

      भव व्योम run_run() override;
};

अंतरभूत __vpiCallback::__vpiCallback()
{
      अग्र_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

पूर्णांक __vpiCallback::get_type_code(व्योम) स्थिर
{ वापस vpiCallback; }


मूल्य_callback::मूल्य_callback(p_cb_तथ्य तथ्य)
{
      cb_तथ्य = *तथ्य;
      अगर (तथ्य->समय) {
	    cb_time = *(तथ्य->समय);
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_तथ्य.समय = &cb_time;
      अगर (तथ्य->value) {
	    cb_मूल्य = *(तथ्य->value);
      } अन्यथा {
	    cb_मूल्य.format = vpiSuppressVal;
      }
      cb_तथ्य.value = &cb_मूल्य;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
bool मूल्य_callback::test_मूल्य_callback_ready(व्योम)
{
      वापस true;
}

अटल व्योम vpip_real_मूल्य_change(मूल्य_callback*cbh, vpiHandle ref)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      निश्चित(obj);

      obj->add_vpi_callback(cbh);
}

श्रेणी मूल्य_part_callback : खुला मूल्य_callback {
    खुला:
      explicit मूल्य_part_callback(p_cb_तथ्य तथ्य);
      ~मूल्य_part_callback() override;

      मूल्य_part_callback(स्थिर मूल्य_part_callback&) = मिटाओ;
      मूल्य_part_callback& चालक=(स्थिर मूल्य_part_callback&) = मिटाओ;

      bool test_मूल्य_callback_ready(व्योम) override;

    निजी:
      अक्षर*मूल्य_bits;
      माप_प्रकार मूल्य_off;
};

अंतरभूत मूल्य_part_callback::मूल्य_part_callback(p_cb_तथ्य तथ्य)
: मूल्य_callback(तथ्य)
{
      काष्ठा __vpiPV*pobj = dynamic_cast<__vpiPV*>(तथ्य->obj);
      निश्चित(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      निश्चित(sig_fil);

      sig_fil->add_vpi_callback(यह);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      sig_fil->get_मूल्य(&tmp_मूल्य);

      मूल्य_bits = नया अक्षर[pobj->width+1];
      मूल्य_off = pobj->parent->vpi_get(vpi_आकार) - pobj->width - pobj->tbase;

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width);
      मूल्य_bits[pobj->width] = 0;
}

मूल्य_part_callback::~मूल्य_part_callback()
{
      मिटाओ[]मूल्य_bits;
}

bool मूल्य_part_callback::test_मूल्य_callback_ready(व्योम)
{
      काष्ठा __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_तथ्य.obj);
      निश्चित(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      निश्चित(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      sig_fil->get_मूल्य(&tmp_मूल्य);

      अगर (स_भेद(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width) == 0)
	    वापस false;

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width);
      वापस true;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
अटल मूल्य_callback*make_मूल्य_change_part(p_cb_तथ्य तथ्य)
{
	/* Attach the __vpiCallback object to the signal. */
      मूल्य_callback*cbh = नया मूल्य_part_callback(तथ्य);
      वापस cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
अटल मूल्य_callback* make_मूल्य_change(p_cb_तथ्य तथ्य)
{
      अगर (!check_callback_time(तथ्य, true))
	    वापस 0;

      अगर (vpi_get(vpiAutomatic, तथ्य->obj)) {
            ख_लिखो(मानक_त्रुटि, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, तथ्य->obj));
            वापस 0;
      }

	// Special case: the target object is a vpiPartSelect
      अगर (तथ्य->obj->get_type_code() == vpiPartSelect) {
            अगर (तथ्य->obj->vpi_handle(vpiArray))
	        वापस vpip_array_word_change(तथ्य);
            अन्यथा
	        वापस make_मूल्य_change_part(तथ्य);
      }

      अगर (तथ्य->obj->get_type_code() == vpiMemoryWord)
	    वापस vpip_array_word_change(तथ्य);

      अगर (तथ्य->obj->get_type_code() == vpiMemory)
	    वापस vpip_array_change(तथ्य);

      मूल्य_callback*obj = नया मूल्य_callback(तथ्य);

      निश्चित(तथ्य->obj);
      चयन (तथ्य->obj->get_type_code()) {

	  हाल vpiReg:
	  हाल vpiNet:
	  हाल vpiIntegerVar:
	  हाल vpiBitVar:
	  हाल vpiByteVar:
	  हाल vpiShortIntVar:
	  हाल vpiIntVar:
	  हाल vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    काष्ठा __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(तथ्य->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->गाँठ->fil);
	    निश्चित(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    अवरोध;

	  हाल vpiRealVar:
	    vpip_real_मूल्य_change(obj, तथ्य->obj);
	    अवरोध;

	  हाल vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(तथ्य->obj);
	    nev->add_vpi_callback(obj);
	    अवरोध;

	  हाल vpiModule:
	  हाल vpiConstant:
	  हाल vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    तथ्य->obj->get_type_code());
	    मिटाओ obj;
	    वापस 0;
      }

      वापस obj;
}

श्रेणी sync_callback : खुला __vpiCallback {
    खुला:
      explicit sync_callback(p_cb_तथ्य तथ्य);
      ~sync_callback() override;

      sync_callback(स्थिर sync_callback&) = मिटाओ;
      sync_callback& चालक=(स्थिर sync_callback&) = मिटाओ;

    खुला:
	// scheduled event
      काष्ठा sync_cb* cb_sync;
	// user supplied callback data
      काष्ठा t_vpi_time cb_time;

    निजी:
};

अंतरभूत sync_callback::sync_callback(p_cb_तथ्य तथ्य)
{
      cb_sync = 0;

      cb_तथ्य = *तथ्य;
      निश्चित(तथ्य->समय);
      cb_time = *(तथ्य->समय);
      cb_तथ्य.समय = &cb_time;
}

sync_callback::~sync_callback()
{
      मिटाओ cb_sync;
}

व्योम sync_cb::run_run()
{
      अगर (handle == 0)
	    वापस;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      अगर (cur->cb_तथ्य.cb_rtn != 0) {
	    set_callback_time(&cur->cb_तथ्य);
	    निश्चित(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_तथ्य.cb_rtn)(&cur->cb_तथ्य);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      मिटाओ cur;
}

अटल vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      चयन (obj->cb_time.type) {
	  हाल vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    अवरोध;

	  हाल vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_तथ्य.obj));
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "get_sync_cb_time: Unsupported time type %d.\n",
	            (पूर्णांक)obj->cb_time.type);
	    निश्चित(0);
	    अवरोध;
      }
      वापस tv;
}

अटल sync_callback* make_sync(p_cb_तथ्य तथ्य, bool readonly_flag)
{
      अगर (!check_callback_time(तथ्य, false))
	    वापस 0;

      sync_callback*obj = नया sync_callback(तथ्य);

      काष्ठा sync_cb*cb = नया sync_cb;
      cb->sync_flag = readonly_flag? true : false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, true, readonly_flag);

      वापस obj;
}

अटल काष्ठा __vpiCallback* make_afterdelay(p_cb_तथ्य तथ्य)
{
      अगर (!check_callback_time(तथ्य, false))
	    वापस 0;

      sync_callback*obj = नया sync_callback(तथ्य);
      काष्ठा sync_cb*cb = नया sync_cb;
      cb->sync_flag = false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, false);

      वापस obj;
}

अटल काष्ठा __vpiCallback* make_at_start_of_sim_time(p_cb_तथ्य तथ्य)
{
      अगर (!check_callback_time(तथ्य, false))
	    वापस 0;

      sync_callback*obj = नया sync_callback(तथ्य);
      काष्ठा sync_cb*cb = नया sync_cb;
      cb->sync_flag = false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      अगर (cur > tv) {
	    tv = 0;
	    निश्चित(0);
      } अन्यथा अगर (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      वापस obj;
}

अटल काष्ठा __vpiCallback* make_at_end_of_sim_time(p_cb_तथ्य तथ्य)
{
      अगर (!check_callback_time(तथ्य, false))
	    वापस 0;

      sync_callback*obj = नया sync_callback(तथ्य);
      काष्ठा sync_cb*cb = नया sync_cb;
      cb->sync_flag = false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      अगर (cur > tv) {
	    tv = 0;
	    निश्चित(0);
      } अन्यथा अगर (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      वापस obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

श्रेणी simulator_callback : खुला __vpiCallback {
    खुला:
      explicit simulator_callback(स्थिर काष्ठा t_cb_तथ्य*तथ्य);

    खुला:
      काष्ठा t_vpi_time cb_time;
};

अंतरभूत simulator_callback::simulator_callback(स्थिर काष्ठा t_cb_तथ्य*तथ्य)
{
      cb_तथ्य = *तथ्य;
      अगर ((तथ्य->reason == cb_अग्र_Sim_Time) && तथ्य->समय) {
	    cb_time = *(तथ्य->समय);
      } अन्यथा अगर (तथ्य->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_तथ्य.समय = &cb_time;
}

अटल simulator_callback*अग्र_Sim_Time = 0;
अटल simulator_callback*EndOfCompile = 0;
अटल simulator_callback*StartOfSimulation = 0;
अटल simulator_callback*EndOfSimulation = 0;

#अगर_घोषित CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
व्योम simulator_cb_delete(व्योम)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      जबतक (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    मिटाओ cur;
      }

	/* Delete all the start of simulation callbacks. */
      जबतक (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    मिटाओ cur;
      }

	/* Delete all the end of simulation callbacks. */
      जबतक (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    मिटाओ cur;
      }
}
#पूर्ण_अगर

व्योम vpiEndOfCompile(व्योम) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      निश्चित(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      जबतक (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    अगर (cur->cb_तथ्य.cb_rtn != 0) {
		  (cur->cb_तथ्य.cb_rtn)(&cur->cb_तथ्य);
	    }
	    मिटाओ cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

व्योम vpiStartOfSim(व्योम) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      निश्चित(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      जबतक (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    अगर (cur->cb_तथ्य.cb_rtn != 0) {
		  (cur->cb_तथ्य.cb_rtn)(&cur->cb_तथ्य);
	    }
	    मिटाओ cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

व्योम vpiPostsim(व्योम) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      निश्चित(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      जबतक (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    अगर (cur->cb_तथ्य.cb_rtn != 0) {
		  set_callback_time(&cur->cb_तथ्य);
		  (cur->cb_तथ्य.cb_rtn)(&cur->cb_तथ्य);
	    }
	    मिटाओ cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
व्योम vpi_अग्र_Sim_Time(व्योम)
{
      simulator_callback* cur;
      simulator_callback* अग्र_2 = अग्र_Sim_Time;
      अग्र_Sim_Time = 0;

      निश्चित(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      जबतक (अग्र_2) {
            cur = अग्र_2;
            अग्र_2 = dynamic_cast<simulator_callback*>(cur->अग्र_2);
            अगर (cur->cb_तथ्य.cb_rtn != 0) {
                  set_callback_time(&cur->cb_तथ्य);
                  (cur->cb_तथ्य.cb_rtn)(&cur->cb_तथ्य);
            }
            मिटाओ cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

अटल simulator_callback* make_prepost(p_cb_तथ्य तथ्य)
{
      अगर ((तथ्य->reason == cb_अग्र_Sim_Time) && !check_callback_time(तथ्य, true))
	    वापस 0;

      simulator_callback*obj = नया simulator_callback(तथ्य);

      /* Insert at head of list */
      चयन (तथ्य->reason) {
	  हाल cbEndOfCompile:
	    obj->अग्र_2 = EndOfCompile;
	    EndOfCompile = obj;
	    अवरोध;
	  हाल cbStartOfSimulation:
	    obj->अग्र_2 = StartOfSimulation;
	    StartOfSimulation = obj;
	    अवरोध;
	  हाल cbEndOfSimulation:
	    obj->अग्र_2 = EndOfSimulation;
	    EndOfSimulation = obj;
	    अवरोध;
	  हाल cb_अग्र_Sim_Time:
	    obj->अग्र_2 = अग्र_Sim_Time;
	    अग्र_Sim_Time = obj;
	    अवरोध;
      }

      वापस obj;
}

vpiHandle vpi_register_cb(p_cb_तथ्य तथ्य)
{
      काष्ठा __vpiCallback*obj = 0;

      निश्चित(तथ्य);
      चयन (तथ्य->reason) {

	  हाल cb_मूल्य_Change:
	    obj = make_मूल्य_change(तथ्य);
	    अवरोध;

	  हाल cbReadOnlySynch:
	    obj = make_sync(तथ्य, true);
	    अवरोध;

	  हाल cbReadWriteSynch:
	    obj = make_sync(तथ्य, false);
	    अवरोध;

	  हाल cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(तथ्य);
	    अवरोध;

	  हाल cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(तथ्य);
	    अवरोध;

	  हाल cbAfterDelay:
	    obj = make_afterdelay(तथ्य);
	    अवरोध;

	  हाल cbEndOfCompile:
	  हाल cbStartOfSimulation:
	  हाल cbEndOfSimulation:
	  हाल cb_अग्र_Sim_Time:
	    obj = make_prepost(तथ्य);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (पूर्णांक)तथ्य->reason);
	    अवरोध;
      }

      वापस obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      काष्ठा __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      निश्चित(obj);
      obj->cb_तथ्य.cb_rtn = 0;

      वापस 1;
}

व्योम callback_execute(काष्ठा __vpiCallback*cur)
{
      स्थिर vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      निश्चित(cur->cb_तथ्य.cb_rtn);
      set_callback_time(&cur->cb_तथ्य);
      (cur->cb_तथ्य.cb_rtn)(&cur->cb_तथ्य);

      vpi_mode_flag = save_mode;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
काष्ठा __vpi_array_word {
      काष्ठा __vpi_array_word* अग्र_2;
      काष्ठा __vpiArray* array;
      अचिन्हित दीर्घ word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_शब्दगण = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      निश्चित(vpi_callbacks_ == 0);
      निश्चित(array_शब्दगण == 0);
}

व्योम vvp_vpi_callback::attach_as_word(vvp_array_t arr, अचिन्हित दीर्घ addr)
{
      काष्ठा __vpi_array_word*tmp = नया __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->अग्र_2 = array_शब्दगण;
      array_शब्दगण = tmp;
}

व्योम vvp_vpi_callback::add_vpi_callback(मूल्य_callback*cb)
{
      cb->अग्र_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_vpi_callback::clear_all_callbacks()
{
      जबतक (vpi_callbacks_) {
	    मूल्य_callback *tmp = dynamic_cast<मूल्य_callback*>
	                            (vpi_callbacks_->अग्र_2);
	    मिटाओ vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      जबतक (array_शब्दगण) {
	    काष्ठा __vpi_array_word*tmp = array_शब्दगण->अग्र_2;
	    मिटाओ array_शब्दगण;
	    array_शब्दगण = tmp;
      }
}
#पूर्ण_अगर

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
व्योम vvp_vpi_callback::run_vpi_callbacks()
{
      काष्ठा __vpi_array_word*array_word = array_शब्दगण;
      जबतक (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->अग्र_2;
      }

      मूल्य_callback *अग्र_2 = vpi_callbacks_;
      मूल्य_callback *prev = 0;

      जबतक (अग्र_2) {
	    मूल्य_callback*cur = अग्र_2;
	    अग्र_2 = dynamic_cast<मूल्य_callback*>(cur->अग्र_2);

	    अगर (cur->cb_तथ्य.cb_rtn != 0) {
		  अगर (cur->test_मूल्य_callback_ready()) {
			अगर (cur->cb_तथ्य.value)
			      get_मूल्य(cur->cb_तथ्य.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } अन्यथा अगर (prev == 0) {

		  vpi_callbacks_ = अग्र_2;
		  cur->अग्र_2 = 0;
		  मिटाओ cur;

	    } अन्यथा {
		  निश्चित(prev->अग्र_2 == cur);
		  prev->अग्र_2 = अग्र_2;
		  cur->अग्र_2 = 0;
		  मिटाओ cur;
	    }
      }
}

व्योम vvp_signal_मूल्य::get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp)
{
      चयन (vp->format) {
	  हाल vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    अवरोध;

	  हाल vpiBinStrVal:
	  हाल vpiOctStrVal:
	  हाल vpiDecStrVal:
	  हाल vpiHexStrVal:
	  हाल vpiIntVal:
	  हाल vpiVectorVal:
	  हाल vpiStringVal:
	  हाल vpiRealVal: {
	    अचिन्हित wid = मूल्य_आकार();
	    vvp_vector4_t vec4(wid);
	    क्रम (अचिन्हित idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_मूल्य(vec4, wid, false, vp);
	    अवरोध;
	  }

	  हाल vpiSuppressVal:
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (पूर्णांक)vp->format);
      }
}

अटल व्योम real_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp, द्विगुन rval)
{
      अटल स्थिर माप_प्रकार RBUF_आकार = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_परिणाम_buf(RBUF_आकार, RBUF_VAL));

      चयन (vp->format) {
	  हाल vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  हाल vpiRealVal:
	    vp->value.real = rval;
	    अवरोध;

	  हाल vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    अगर (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } अन्यथा {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    अवरोध;

	  हाल vpiDecStrVal:
	    अगर (std::isnan(rval))
		  snprintf(rbuf, RBUF_आकार, "%s", "nan");
	    अन्यथा
		  snprintf(rbuf, RBUF_आकार, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiHexStrVal:
	    snprintf(rbuf, RBUF_आकार, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		अचिन्हित len = 0;

		जबतक (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		क्रम (अचिन्हित idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		अगर (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		अवरोध;
	  }

	  हाल vpiSuppressVal:
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (पूर्णांक)vp->format);
      }
}

व्योम vvp_fun_signal_real_aa::get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp)
{
      real_signal_मूल्य(vp, real_मूल्य());
}

व्योम vvp_wire_real::get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp)
{
      real_signal_मूल्य(vp, real_मूल्य());
}

व्योम vvp_fun_signal_string_aa::get_signal_मूल्य(काष्ठा t_vpi_मूल्य*)
{
      निश्चित(0);
}
#अगर 0
व्योम vvp_wire_string::get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp)
{
      निश्चित(0);
}
#पूर्ण_अगर
व्योम vvp_wire_vec4::get_मूल्य(काष्ठा t_vpi_मूल्य*val)
{
      get_signal_मूल्य(val);
}

व्योम vvp_wire_vec8::get_मूल्य(काष्ठा t_vpi_मूल्य*val)
{
      get_signal_मूल्य(val);
}

व्योम vvp_wire_real::get_मूल्य(काष्ठा t_vpi_मूल्य*val)
{
      get_signal_मूल्य(val);
}
#अगर 0
व्योम vvp_wire_string::get_मूल्य(काष्ठा t_vpi_मूल्य*val)
{
      निश्चित(0);
}
#पूर्ण_अगर
