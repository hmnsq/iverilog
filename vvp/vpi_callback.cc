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

स्थिर अचर वर्ण*cb_reason_name(PLI_INT32 reason)
{
      विकल्प (reason) {
	  पक्ष cb_मान_Change:
	    फर्काऊ "cbValueChange";
	  पक्ष cbStmt:
	    फर्काऊ "cbStmt";
	  पक्ष cbForce:
	    फर्काऊ "cbForce";
	  पक्ष cbRelease:
	    फर्काऊ "cbRelease";
	  पक्ष cbAtStartOfSimTime:
	    फर्काऊ "cbAtStartOfSimTime";
	  पक्ष cbReadWriteSynch:
	    फर्काऊ "cbReadWriteSynch";
	  पक्ष cbReadOnlySynch:
	    फर्काऊ "cbReadOnlySynch";
	  पक्ष cbNextSimTime:
	    फर्काऊ "cbNextSimTime";
	  पक्ष cbAfterDelay:
	    फर्काऊ "cbAfterDelay";
	  पक्ष cbEndOfCompile:
	    फर्काऊ "cbEndOfCompile";
	  पक्ष cbStartOfSimulation:
	    फर्काऊ "cbStartOfSimulation";
	  पक्ष cbEndOfSimulation:
	    फर्काऊ "cbEndOfSimulation";
	  पक्ष cbError:
	    फर्काऊ "cbError";
	  पक्ष cbTchkViolation:
	    फर्काऊ "cbTchkViolation";
	  पक्ष cbStartOfSave:
	    फर्काऊ "cbStartOfSave";
	  पक्ष cbEndOfSave:
	    फर्काऊ "cbEndOfSave";
	  पक्ष cbStartOfRestart:
	    फर्काऊ "cbStartOfRestart";
	  पक्ष cbEndOfRestart:
	    फर्काऊ "cbEndOfRestart";
	  पक्ष cbStartOfReset:
	    फर्काऊ "cbStartOfReset";
	  पक्ष cbEndOfReset:
	    फर्काऊ "cbEndOfReset";
	  पक्ष cbEnterInteractive:
	    फर्काऊ "cbEnterInteractive";
	  पक्ष cbExitInteractive:
	    फर्काऊ "cbExitInteractive";
	  पक्ष cbInteractiveScopeChange:
	    फर्काऊ "cbInteractiveScopeChange";
	  पक्ष cbUnresolvedSystf:
	    फर्काऊ "cbUnresolvedSystf";
	  पक्ष cbAtEndOfSimTime:
	    फर्काऊ "cbAtEndOfSimTime";
	  स्वभाव:
	    फर्काऊ "unrecognised";
      }
}

स्थिर तार्किक check_callback_time(p_cb_data data, तार्किक allow_suppress)
{
      प्रतिज्ञा(data);
      यदि (!data->समय) {
	    यदि (!allow_suppress) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  फर्काऊ असत्य;
	    }
	    फर्काऊ सत्य;
      }
      विकल्प (data->समय->type) {
	  पक्ष vpiSimTime:
	    रोक;
	  पक्ष vpiScaledRealTime:
	    रोक;
	  पक्ष vpiSuppressTime:
	    यदि (!allow_suppress) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  फर्काऊ असत्य;
	    }
	    रोक;
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    फर्काऊ असत्य;
      }
      फर्काऊ सत्य;
}

स्थिर शून्य set_callback_time(p_cb_data data)
{
      प्रतिज्ञा(data && data->समय);
      data->समय->तल्लो  = 0;
      data->समय->माथिल्लो = 0;
      data->समय->real = 0.0;
      विकल्प (data->समय->type) {
	  पक्ष vpiSimTime:
	    vpip_time_to_timestruct(data->समय, schedule_simtime());
	    रोक;
	  पक्ष vpiScaledRealTime:
	    data->समय->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    रोक;
	  पक्ष vpiSuppressTime:
	    रोक;
	  स्वभाव:
	    प्रतिज्ञा(0);
	    रोक;
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

      आभास शून्य run_run() override;
};

अन्तःस्थ __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

पूर्णाङ्क __vpiCallback::get_type_code(शून्य) अचर
{ फर्काऊ vpiCallback; }


मान_callback::मान_callback(p_cb_data data)
{
      cb_data = *data;
      यदि (data->समय) {
	    cb_time = *(data->समय);
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.समय = &cb_time;
      यदि (data->value) {
	    cb_मान = *(data->value);
      } अन्यथा {
	    cb_मान.format = vpiSuppressVal;
      }
      cb_data.value = &cb_मान;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
तार्किक मान_callback::test_मान_callback_ready(शून्य)
{
      फर्काऊ सत्य;
}

स्थिर शून्य vpip_real_मान_change(मान_callback*cbh, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      प्रतिज्ञा(obj);

      obj->add_vpi_callback(cbh);
}

वर्ग मान_part_callback : सार्वजनिक मान_callback {
    सार्वजनिक:
      explicit मान_part_callback(p_cb_data data);
      ~मान_part_callback() override;

      मान_part_callback(अचर मान_part_callback&) = delete;
      मान_part_callback& चालक=(अचर मान_part_callback&) = delete;

      तार्किक test_मान_callback_ready(शून्य) override;

    गुप्त:
      वर्ण*मान_bits;
      माप_प्रकार मान_off;
};

अन्तःस्थ मान_part_callback::मान_part_callback(p_cb_data data)
: मान_callback(data)
{
      संरचना __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      प्रतिज्ञा(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      प्रतिज्ञा(sig_fil);

      sig_fil->add_vpi_callback(एतत्);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_मान tmp_मान;
      tmp_मान.format = vpiBinStrVal;
      sig_fil->get_मान(&tmp_मान);

      मान_bits = नूतन वर्ण[pobj->width+1];
      मान_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      स_नकल(मान_bits, tmp_मान.value.str + मान_off, pobj->width);
      मान_bits[pobj->width] = 0;
}

मान_part_callback::~मान_part_callback()
{
      delete[]मान_bits;
}

तार्किक मान_part_callback::test_मान_callback_ready(शून्य)
{
      संरचना __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      प्रतिज्ञा(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      प्रतिज्ञा(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मान tmp_मान;
      tmp_मान.format = vpiBinStrVal;
      sig_fil->get_मान(&tmp_मान);

      यदि (स_भेद(मान_bits, tmp_मान.value.str + मान_off, pobj->width) == 0)
	    फर्काऊ असत्य;

      स_नकल(मान_bits, tmp_मान.value.str + मान_off, pobj->width);
      फर्काऊ सत्य;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
स्थिर मान_callback*make_मान_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      मान_callback*cbh = नूतन मान_part_callback(data);
      फर्काऊ cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
स्थिर मान_callback* make_मान_change(p_cb_data data)
{
      यदि (!check_callback_time(data, सत्य))
	    फर्काऊ 0;

      यदि (vpi_get(vpiAutomatic, data->obj)) {
            ख_लिखो(मानक_त्रुटि, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            फर्काऊ 0;
      }

	// Special case: the target object is a vpiPartSelect
      यदि (data->obj->get_type_code() == vpiPartSelect) {
            यदि (data->obj->vpi_handle(vpiArray))
	        फर्काऊ vpip_array_word_change(data);
            अन्यथा
	        फर्काऊ make_मान_change_part(data);
      }

      यदि (data->obj->get_type_code() == vpiMemoryWord)
	    फर्काऊ vpip_array_word_change(data);

      यदि (data->obj->get_type_code() == vpiMemory)
	    फर्काऊ vpip_array_change(data);

      मान_callback*obj = नूतन मान_callback(data);

      प्रतिज्ञा(data->obj);
      विकल्प (data->obj->get_type_code()) {

	  पक्ष vpiReg:
	  पक्ष vpiNet:
	  पक्ष vpiIntegerVar:
	  पक्ष vpiBitVar:
	  पक्ष vpiByteVar:
	  पक्ष vpiShortIntVar:
	  पक्ष vpiIntVar:
	  पक्ष vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    संरचना __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->गाँठो->fil);
	    प्रतिज्ञा(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    रोक;

	  पक्ष vpiRealVar:
	    vpip_real_मान_change(obj, data->obj);
	    रोक;

	  पक्ष vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    रोक;

	  पक्ष vpiModule:
	  पक्ष vpiConstant:
	  पक्ष vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    फर्काऊ 0;
      }

      फर्काऊ obj;
}

वर्ग sync_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(अचर sync_callback&) = delete;
      sync_callback& चालक=(अचर sync_callback&) = delete;

    सार्वजनिक:
	// scheduled event
      संरचना sync_cb* cb_sync;
	// user supplied callback data
      संरचना t_vpi_time cb_time;

    गुप्त:
};

अन्तःस्थ sync_callback::sync_callback(p_cb_data data)
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
      यदि (handle == 0)
	    फर्काऊ;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      यदि (cur->cb_data.cb_rtn != 0) {
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
      विकल्प (obj->cb_time.type) {
	  पक्ष vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    रोक;

	  पक्ष vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "get_sync_cb_time: Unsupported time type %d.\n",
	            (पूर्णाङ्क)obj->cb_time.type);
	    प्रतिज्ञा(0);
	    रोक;
      }
      फर्काऊ tv;
}

स्थिर sync_callback* make_sync(p_cb_data data, तार्किक readonly_flag)
{
      यदि (!check_callback_time(data, असत्य))
	    फर्काऊ 0;

      sync_callback*obj = नूतन sync_callback(data);

      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = readonly_flag? सत्य : असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, सत्य, readonly_flag);

      फर्काऊ obj;
}

स्थिर संरचना __vpiCallback* make_afterdelay(p_cb_data data)
{
      यदि (!check_callback_time(data, असत्य))
	    फर्काऊ 0;

      sync_callback*obj = नूतन sync_callback(data);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, असत्य);

      फर्काऊ obj;
}

स्थिर संरचना __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      यदि (!check_callback_time(data, असत्य))
	    फर्काऊ 0;

      sync_callback*obj = नूतन sync_callback(data);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      यदि (cur > tv) {
	    tv = 0;
	    प्रतिज्ञा(0);
      } अन्यथा यदि (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      फर्काऊ obj;
}

स्थिर संरचना __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      यदि (!check_callback_time(data, असत्य))
	    फर्काऊ 0;

      sync_callback*obj = नूतन sync_callback(data);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      यदि (cur > tv) {
	    tv = 0;
	    प्रतिज्ञा(0);
      } अन्यथा यदि (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      फर्काऊ obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

वर्ग simulator_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit simulator_callback(अचर संरचना t_cb_data*data);

    सार्वजनिक:
      संरचना t_vpi_time cb_time;
};

अन्तःस्थ simulator_callback::simulator_callback(अचर संरचना t_cb_data*data)
{
      cb_data = *data;
      यदि ((data->reason == cbNextSimTime) && data->समय) {
	    cb_time = *(data->समय);
      } अन्यथा यदि (data->reason == cbEndOfSimulation) {
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
      जबसम्म (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      जबसम्म (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      जबसम्म (EndOfSimulation) {
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

      जबसम्म (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    यदि (cur->cb_data.cb_rtn != 0) {
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

      जबसम्म (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    यदि (cur->cb_data.cb_rtn != 0) {
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

      जबसम्म (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    यदि (cur->cb_data.cb_rtn != 0) {
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

      जबसम्म (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            यदि (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

स्थिर simulator_callback* make_prepost(p_cb_data data)
{
      यदि ((data->reason == cbNextSimTime) && !check_callback_time(data, सत्य))
	    फर्काऊ 0;

      simulator_callback*obj = नूतन simulator_callback(data);

      /* Insert at head of list */
      विकल्प (data->reason) {
	  पक्ष cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    रोक;
	  पक्ष cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    रोक;
	  पक्ष cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    रोक;
	  पक्ष cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    रोक;
      }

      फर्काऊ obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      संरचना __vpiCallback*obj = 0;

      प्रतिज्ञा(data);
      विकल्प (data->reason) {

	  पक्ष cb_मान_Change:
	    obj = make_मान_change(data);
	    रोक;

	  पक्ष cbReadOnlySynch:
	    obj = make_sync(data, सत्य);
	    रोक;

	  पक्ष cbReadWriteSynch:
	    obj = make_sync(data, असत्य);
	    रोक;

	  पक्ष cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    रोक;

	  पक्ष cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    रोक;

	  पक्ष cbAfterDelay:
	    obj = make_afterdelay(data);
	    रोक;

	  पक्ष cbEndOfCompile:
	  पक्ष cbStartOfSimulation:
	  पक्ष cbEndOfSimulation:
	  पक्ष cbNextSimTime:
	    obj = make_prepost(data);
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (पूर्णाङ्क)data->reason);
	    रोक;
      }

      फर्काऊ obj;
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

      फर्काऊ 1;
}

शून्य callback_execute(संरचना __vpiCallback*cur)
{
      अचर vpi_mode_t save_mode = vpi_mode_flag;
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
      array_शब्दहरू = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      प्रतिज्ञा(vpi_callbacks_ == 0);
      प्रतिज्ञा(array_शब्दहरू == 0);
}

शून्य vvp_vpi_callback::attach_as_word(vvp_array_t arr, अचिन्हित दीर्घ addr)
{
      संरचना __vpi_array_word*tmp = नूतन __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_शब्दहरू;
      array_शब्दहरू = tmp;
}

शून्य vvp_vpi_callback::add_vpi_callback(मान_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_vpi_callback::clear_all_callbacks()
{
      जबसम्म (vpi_callbacks_) {
	    मान_callback *tmp = dynamic_cast<मान_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      जबसम्म (array_शब्दहरू) {
	    संरचना __vpi_array_word*tmp = array_शब्दहरू->next;
	    delete array_शब्दहरू;
	    array_शब्दहरू = tmp;
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
      संरचना __vpi_array_word*array_word = array_शब्दहरू;
      जबसम्म (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      मान_callback *next = vpi_callbacks_;
      मान_callback *prev = 0;

      जबसम्म (next) {
	    मान_callback*cur = next;
	    next = dynamic_cast<मान_callback*>(cur->next);

	    यदि (cur->cb_data.cb_rtn != 0) {
		  यदि (cur->test_मान_callback_ready()) {
			यदि (cur->cb_data.value)
			      get_मान(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } अन्यथा यदि (prev == 0) {

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

शून्य vvp_signal_मान::get_signal_मान(संरचना t_vpi_मान*vp)
{
      विकल्प (vp->format) {
	  पक्ष vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    रोक;

	  पक्ष vpiBinStrVal:
	  पक्ष vpiOctStrVal:
	  पक्ष vpiDecStrVal:
	  पक्ष vpiHexStrVal:
	  पक्ष vpiIntVal:
	  पक्ष vpiVectorVal:
	  पक्ष vpiStringVal:
	  पक्ष vpiRealVal: {
	    अचिन्हित wid = मान_size();
	    vvp_vector4_t vec4(wid);
	    लागि (अचिन्हित idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_मान(vec4, wid, असत्य, vp);
	    रोक;
	  }

	  पक्ष vpiSuppressVal:
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (पूर्णाङ्क)vp->format);
      }
}

स्थिर शून्य real_signal_मान(संरचना t_vpi_मान*vp, द्विगुण rval)
{
      स्थिर अचर माप_प्रकार RBUF_SIZE = 64 + 1;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      विकल्प (vp->format) {
	  पक्ष vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  पक्ष vpiRealVal:
	    vp->value.real = rval;
	    रोक;

	  पक्ष vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    यदि (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } अन्यथा {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    रोक;

	  पक्ष vpiDecStrVal:
	    यदि (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    अन्यथा
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    रोक;

	  पक्ष vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    रोक;

	  पक्ष vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		अचिन्हित len = 0;

		जबसम्म (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		लागि (अचिन्हित idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		यदि (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		रोक;
	  }

	  पक्ष vpiSuppressVal:
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (पूर्णाङ्क)vp->format);
      }
}

शून्य vvp_fun_signal_real_aa::get_signal_मान(संरचना t_vpi_मान*vp)
{
      real_signal_मान(vp, real_मान());
}

शून्य vvp_wire_real::get_signal_मान(संरचना t_vpi_मान*vp)
{
      real_signal_मान(vp, real_मान());
}

शून्य vvp_fun_signal_string_aa::get_signal_मान(संरचना t_vpi_मान*)
{
      प्रतिज्ञा(0);
}
#यदि 0
शून्य vvp_wire_string::get_signal_मान(संरचना t_vpi_मान*vp)
{
      प्रतिज्ञा(0);
}
#endif
शून्य vvp_wire_vec4::get_मान(संरचना t_vpi_मान*val)
{
      get_signal_मान(val);
}

शून्य vvp_wire_vec8::get_मान(संरचना t_vpi_मान*val)
{
      get_signal_मान(val);
}

शून्य vvp_wire_real::get_मान(संरचना t_vpi_मान*val)
{
      get_signal_मान(val);
}
#यदि 0
शून्य vvp_wire_string::get_मान(संरचना t_vpi_मान*val)
{
      प्रतिज्ञा(0);
}
#endif
