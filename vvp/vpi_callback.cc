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
#ifdef CHECK_WITH_VALGRIND
#अन्तर्भाव  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using नामाकाश std;

स्थिर स्थिरांक अक्षर*cb_reason_name(PLI_INT32 reason)
{
      निवड (reason) {
	  स्थिती cb_मूल्य_Change:
	    परत "cbValueChange";
	  स्थिती cbStmt:
	    परत "cbStmt";
	  स्थिती cbForce:
	    परत "cbForce";
	  स्थिती cbRelease:
	    परत "cbRelease";
	  स्थिती cbAtStartOfSimTime:
	    परत "cbAtStartOfSimTime";
	  स्थिती cbReadWriteSynch:
	    परत "cbReadWriteSynch";
	  स्थिती cbReadOnlySynch:
	    परत "cbReadOnlySynch";
	  स्थिती cbNextSimTime:
	    परत "cbNextSimTime";
	  स्थिती cbAfterDelay:
	    परत "cbAfterDelay";
	  स्थिती cbEndOfCompile:
	    परत "cbEndOfCompile";
	  स्थिती cbStartOfSimulation:
	    परत "cbStartOfSimulation";
	  स्थिती cbEndOfSimulation:
	    परत "cbEndOfSimulation";
	  स्थिती cbError:
	    परत "cbError";
	  स्थिती cbTchkViolation:
	    परत "cbTchkViolation";
	  स्थिती cbStartOfSave:
	    परत "cbStartOfSave";
	  स्थिती cbEndOfSave:
	    परत "cbEndOfSave";
	  स्थिती cbStartOfRestart:
	    परत "cbStartOfRestart";
	  स्थिती cbEndOfRestart:
	    परत "cbEndOfRestart";
	  स्थिती cbStartOfReset:
	    परत "cbStartOfReset";
	  स्थिती cbEndOfReset:
	    परत "cbEndOfReset";
	  स्थिती cbEnterInteractive:
	    परत "cbEnterInteractive";
	  स्थिती cbExitInteractive:
	    परत "cbExitInteractive";
	  स्थिती cbInteractiveScopeChange:
	    परत "cbInteractiveScopeChange";
	  स्थिती cbUnresolvedSystf:
	    परत "cbUnresolvedSystf";
	  स्थिती cbAtEndOfSimTime:
	    परत "cbAtEndOfSimTime";
	  पूर्वनिर्धारित:
	    परत "unrecognised";
      }
}

स्थिर तार्किक check_callback_time(p_cb_data data, तार्किक allow_suppress)
{
      प्रतिज्ञा(data);
      जर (!data->समय) {
	    जर (!allow_suppress) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  परत असत्य;
	    }
	    परत सत्य;
      }
      निवड (data->समय->type) {
	  स्थिती vpiSimTime:
	    थांबा;
	  स्थिती vpiScaledRealTime:
	    थांबा;
	  स्थिती vpiSuppressTime:
	    जर (!allow_suppress) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  परत असत्य;
	    }
	    थांबा;
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    परत असत्य;
      }
      परत सत्य;
}

स्थिर शून्य set_callback_time(p_cb_data data)
{
      प्रतिज्ञा(data && data->समय);
      data->समय->खालचा  = 0;
      data->समय->वरचा = 0;
      data->समय->real = 0.0;
      निवड (data->समय->type) {
	  स्थिती vpiSimTime:
	    vpip_time_to_timestruct(data->समय, schedule_simtime());
	    थांबा;
	  स्थिती vpiScaledRealTime:
	    data->समय->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    थांबा;
	  स्थिती vpiSuppressTime:
	    थांबा;
	  पूर्वनिर्धारित:
	    प्रतिज्ञा(0);
	    थांबा;
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

वर्ग sync_callback;

संरचना sync_cb  : सार्वजनिक vvp_gen_event_s {
      sync_callback*handle;
      तार्किक sync_flag;

      ~sync_cb () override { }

      आभासी शून्य run_run() override;
};

ओळीत __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

पूर्णांक __vpiCallback::get_type_code(शून्य) स्थिरांक
{ परत vpiCallback; }


मूल्य_callback::मूल्य_callback(p_cb_data data)
{
      cb_data = *data;
      जर (data->समय) {
	    cb_time = *(data->समय);
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.समय = &cb_time;
      जर (data->value) {
	    cb_मूल्य = *(data->value);
      } अन्यथा {
	    cb_मूल्य.format = vpiSuppressVal;
      }
      cb_data.value = &cb_मूल्य;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
तार्किक मूल्य_callback::test_मूल्य_callback_ready(शून्य)
{
      परत सत्य;
}

स्थिर शून्य vpip_real_मूल्य_change(मूल्य_callback*cbh, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      प्रतिज्ञा(obj);

      obj->add_vpi_callback(cbh);
}

वर्ग मूल्य_part_callback : सार्वजनिक मूल्य_callback {
    सार्वजनिक:
      explicit मूल्य_part_callback(p_cb_data data);
      ~मूल्य_part_callback() override;

      मूल्य_part_callback(स्थिरांक मूल्य_part_callback&) = delete;
      मूल्य_part_callback& चालक=(स्थिरांक मूल्य_part_callback&) = delete;

      तार्किक test_मूल्य_callback_ready(शून्य) override;

    खाजगी:
      अक्षर*मूल्य_bits;
      माप_प्रकार मूल्य_off;
};

ओळीत मूल्य_part_callback::मूल्य_part_callback(p_cb_data data)
: मूल्य_callback(data)
{
      संरचना __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      प्रतिज्ञा(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      प्रतिज्ञा(sig_fil);

      sig_fil->add_vpi_callback(हे);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      sig_fil->get_मूल्य(&tmp_मूल्य);

      मूल्य_bits = नवीन अक्षर[pobj->width+1];
      मूल्य_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width);
      मूल्य_bits[pobj->width] = 0;
}

मूल्य_part_callback::~मूल्य_part_callback()
{
      delete[]मूल्य_bits;
}

तार्किक मूल्य_part_callback::test_मूल्य_callback_ready(शून्य)
{
      संरचना __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      प्रतिज्ञा(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      प्रतिज्ञा(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      sig_fil->get_मूल्य(&tmp_मूल्य);

      जर (स_भेद(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width) == 0)
	    परत असत्य;

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width);
      परत सत्य;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
स्थिर मूल्य_callback*make_मूल्य_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      मूल्य_callback*cbh = नवीन मूल्य_part_callback(data);
      परत cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
स्थिर मूल्य_callback* make_मूल्य_change(p_cb_data data)
{
      जर (!check_callback_time(data, सत्य))
	    परत 0;

      जर (vpi_get(vpiAutomatic, data->obj)) {
            ख_लिखो(मानक_त्रुटि, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            परत 0;
      }

	// Special case: the target object is a vpiPartSelect
      जर (data->obj->get_type_code() == vpiPartSelect) {
            जर (data->obj->vpi_handle(vpiArray))
	        परत vpip_array_word_change(data);
            अन्यथा
	        परत make_मूल्य_change_part(data);
      }

      जर (data->obj->get_type_code() == vpiMemoryWord)
	    परत vpip_array_word_change(data);

      जर (data->obj->get_type_code() == vpiMemory)
	    परत vpip_array_change(data);

      मूल्य_callback*obj = नवीन मूल्य_callback(data);

      प्रतिज्ञा(data->obj);
      निवड (data->obj->get_type_code()) {

	  स्थिती vpiReg:
	  स्थिती vpiNet:
	  स्थिती vpiIntegerVar:
	  स्थिती vpiBitVar:
	  स्थिती vpiByteVar:
	  स्थिती vpiShortIntVar:
	  स्थिती vpiIntVar:
	  स्थिती vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    संरचना __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->गाठ->fil);
	    प्रतिज्ञा(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    थांबा;

	  स्थिती vpiRealVar:
	    vpip_real_मूल्य_change(obj, data->obj);
	    थांबा;

	  स्थिती vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    थांबा;

	  स्थिती vpiModule:
	  स्थिती vpiConstant:
	  स्थिती vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    परत 0;
      }

      परत obj;
}

वर्ग sync_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(स्थिरांक sync_callback&) = delete;
      sync_callback& चालक=(स्थिरांक sync_callback&) = delete;

    सार्वजनिक:
	// scheduled event
      संरचना sync_cb* cb_sync;
	// user supplied callback data
      संरचना t_vpi_time cb_time;

    खाजगी:
};

ओळीत sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      प्रतिज्ञा(data->समय);
      cb_time = *(data->समय);
      cb_data.समय = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

शून्य sync_cb::run_run()
{
      जर (handle == 0)
	    परत;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      जर (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

स्थिर vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      निवड (obj->cb_time.type) {
	  स्थिती vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    थांबा;

	  स्थिती vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "get_sync_cb_time: Unsupported time type %d.\n",
	            (पूर्णांक)obj->cb_time.type);
	    प्रतिज्ञा(0);
	    थांबा;
      }
      परत tv;
}

स्थिर sync_callback* make_sync(p_cb_data data, तार्किक readonly_flag)
{
      जर (!check_callback_time(data, असत्य))
	    परत 0;

      sync_callback*obj = नवीन sync_callback(data);

      संरचना sync_cb*cb = नवीन sync_cb;
      cb->sync_flag = readonly_flag? सत्य : असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, सत्य, readonly_flag);

      परत obj;
}

स्थिर संरचना __vpiCallback* make_afterdelay(p_cb_data data)
{
      जर (!check_callback_time(data, असत्य))
	    परत 0;

      sync_callback*obj = नवीन sync_callback(data);
      संरचना sync_cb*cb = नवीन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, असत्य);

      परत obj;
}

स्थिर संरचना __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      जर (!check_callback_time(data, असत्य))
	    परत 0;

      sync_callback*obj = नवीन sync_callback(data);
      संरचना sync_cb*cb = नवीन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      जर (cur > tv) {
	    tv = 0;
	    प्रतिज्ञा(0);
      } अन्यथा जर (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      परत obj;
}

स्थिर संरचना __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      जर (!check_callback_time(data, असत्य))
	    परत 0;

      sync_callback*obj = नवीन sync_callback(data);
      संरचना sync_cb*cb = नवीन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      जर (cur > tv) {
	    tv = 0;
	    प्रतिज्ञा(0);
      } अन्यथा जर (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      परत obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

वर्ग simulator_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit simulator_callback(स्थिरांक संरचना t_cb_data*data);

    सार्वजनिक:
      संरचना t_vpi_time cb_time;
};

ओळीत simulator_callback::simulator_callback(स्थिरांक संरचना t_cb_data*data)
{
      cb_data = *data;
      जर ((data->reason == cbNextSimTime) && data->समय) {
	    cb_time = *(data->समय);
      } अन्यथा जर (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.समय = &cb_time;
}

स्थिर simulator_callback*NextSimTime = 0;
स्थिर simulator_callback*EndOfCompile = 0;
स्थिर simulator_callback*StartOfSimulation = 0;
स्थिर simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
शून्य simulator_cb_delete(शून्य)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      पर्यंत (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      पर्यंत (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      पर्यंत (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

शून्य vpiEndOfCompile(शून्य) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      पर्यंत (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    जर (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

शून्य vpiStartOfSim(शून्य) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      पर्यंत (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    जर (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

शून्य vpiPostsim(शून्य) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      पर्यंत (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    जर (cur->cb_data.cb_rtn != 0) {
		  set_callback_time(&cur->cb_data);
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
शून्य vpiNextSimTime(शून्य)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      पर्यंत (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            जर (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

स्थिर simulator_callback* make_prepost(p_cb_data data)
{
      जर ((data->reason == cbNextSimTime) && !check_callback_time(data, सत्य))
	    परत 0;

      simulator_callback*obj = नवीन simulator_callback(data);

      /* Insert at head of list */
      निवड (data->reason) {
	  स्थिती cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    थांबा;
	  स्थिती cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    थांबा;
	  स्थिती cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    थांबा;
	  स्थिती cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    थांबा;
      }

      परत obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      संरचना __vpiCallback*obj = 0;

      प्रतिज्ञा(data);
      निवड (data->reason) {

	  स्थिती cb_मूल्य_Change:
	    obj = make_मूल्य_change(data);
	    थांबा;

	  स्थिती cbReadOnlySynch:
	    obj = make_sync(data, सत्य);
	    थांबा;

	  स्थिती cbReadWriteSynch:
	    obj = make_sync(data, असत्य);
	    थांबा;

	  स्थिती cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    थांबा;

	  स्थिती cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    थांबा;

	  स्थिती cbAfterDelay:
	    obj = make_afterdelay(data);
	    थांबा;

	  स्थिती cbEndOfCompile:
	  स्थिती cbStartOfSimulation:
	  स्थिती cbEndOfSimulation:
	  स्थिती cbNextSimTime:
	    obj = make_prepost(data);
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (पूर्णांक)data->reason);
	    थांबा;
      }

      परत obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      संरचना __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      प्रतिज्ञा(obj);
      obj->cb_data.cb_rtn = 0;

      परत 1;
}

शून्य callback_execute(संरचना __vpiCallback*cur)
{
      स्थिरांक vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      प्रतिज्ञा(cur->cb_data.cb_rtn);
      set_callback_time(&cur->cb_data);
      (cur->cb_data.cb_rtn)(&cur->cb_data);

      vpi_mode_flag = save_mode;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
संरचना __vpi_array_word {
      संरचना __vpi_array_word* next;
      संरचना __vpiArray* array;
      अचिन्हित दीर्घ word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_शब्द = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      प्रतिज्ञा(vpi_callbacks_ == 0);
      प्रतिज्ञा(array_शब्द == 0);
}

शून्य vvp_vpi_callback::attach_as_word(vvp_array_t arr, अचिन्हित दीर्घ addr)
{
      संरचना __vpi_array_word*tmp = नवीन __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_शब्द;
      array_शब्द = tmp;
}

शून्य vvp_vpi_callback::add_vpi_callback(मूल्य_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_vpi_callback::clear_all_callbacks()
{
      पर्यंत (vpi_callbacks_) {
	    मूल्य_callback *tmp = dynamic_cast<मूल्य_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      पर्यंत (array_शब्द) {
	    संरचना __vpi_array_word*tmp = array_शब्द->next;
	    delete array_शब्द;
	    array_शब्द = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
शून्य vvp_vpi_callback::run_vpi_callbacks()
{
      संरचना __vpi_array_word*array_word = array_शब्द;
      पर्यंत (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      मूल्य_callback *next = vpi_callbacks_;
      मूल्य_callback *prev = 0;

      पर्यंत (next) {
	    मूल्य_callback*cur = next;
	    next = dynamic_cast<मूल्य_callback*>(cur->next);

	    जर (cur->cb_data.cb_rtn != 0) {
		  जर (cur->test_मूल्य_callback_ready()) {
			जर (cur->cb_data.value)
			      get_मूल्य(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } अन्यथा जर (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } अन्यथा {
		  प्रतिज्ञा(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

शून्य vvp_signal_मूल्य::get_signal_मूल्य(संरचना t_vpi_मूल्य*vp)
{
      निवड (vp->format) {
	  स्थिती vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    थांबा;

	  स्थिती vpiBinStrVal:
	  स्थिती vpiOctStrVal:
	  स्थिती vpiDecStrVal:
	  स्थिती vpiHexStrVal:
	  स्थिती vpiIntVal:
	  स्थिती vpiVectorVal:
	  स्थिती vpiStringVal:
	  स्थिती vpiRealVal: {
	    अचिन्हित wid = मूल्य_size();
	    vvp_vector4_t vec4(wid);
	    साठी (अचिन्हित idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_मूल्य(vec4, wid, असत्य, vp);
	    थांबा;
	  }

	  स्थिती vpiSuppressVal:
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (पूर्णांक)vp->format);
      }
}

स्थिर शून्य real_signal_मूल्य(संरचना t_vpi_मूल्य*vp, द्विगुण rval)
{
      स्थिर स्थिरांक माप_प्रकार RBUF_SIZE = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      निवड (vp->format) {
	  स्थिती vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  स्थिती vpiRealVal:
	    vp->value.real = rval;
	    थांबा;

	  स्थिती vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    जर (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } अन्यथा {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    थांबा;

	  स्थिती vpiDecStrVal:
	    जर (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    अन्यथा
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    थांबा;

	  स्थिती vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    थांबा;

	  स्थिती vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		अचिन्हित len = 0;

		पर्यंत (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		साठी (अचिन्हित idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		जर (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		थांबा;
	  }

	  स्थिती vpiSuppressVal:
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (पूर्णांक)vp->format);
      }
}

शून्य vvp_fun_signal_real_aa::get_signal_मूल्य(संरचना t_vpi_मूल्य*vp)
{
      real_signal_मूल्य(vp, real_मूल्य());
}

शून्य vvp_wire_real::get_signal_मूल्य(संरचना t_vpi_मूल्य*vp)
{
      real_signal_मूल्य(vp, real_मूल्य());
}

शून्य vvp_fun_signal_string_aa::get_signal_मूल्य(संरचना t_vpi_मूल्य*)
{
      प्रतिज्ञा(0);
}
#जर 0
शून्य vvp_wire_string::get_signal_मूल्य(संरचना t_vpi_मूल्य*vp)
{
      प्रतिज्ञा(0);
}
#endif
शून्य vvp_wire_vec4::get_मूल्य(संरचना t_vpi_मूल्य*val)
{
      get_signal_मूल्य(val);
}

शून्य vvp_wire_vec8::get_मूल्य(संरचना t_vpi_मूल्य*val)
{
      get_signal_मूल्य(val);
}

शून्य vvp_wire_real::get_मूल्य(संरचना t_vpi_मूल्य*val)
{
      get_signal_मूल्य(val);
}
#जर 0
शून्य vvp_wire_string::get_मूल्य(संरचना t_vpi_मूल्य*val)
{
      प्रतिज्ञा(0);
}
#endif
