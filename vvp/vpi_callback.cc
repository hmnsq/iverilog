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

स्थिर ध्रुव वर्ण*cb_reason_name(PLI_INT32 reason)
{
      विकल्प (reason) {
	  पक्ष cb_मूल्य_Change:
	    प्रत्यागम "cbValueChange";
	  पक्ष cbStmt:
	    प्रत्यागम "cbStmt";
	  पक्ष cbForce:
	    प्रत्यागम "cbForce";
	  पक्ष cbRelease:
	    प्रत्यागम "cbRelease";
	  पक्ष cbAtStartOfSimTime:
	    प्रत्यागम "cbAtStartOfSimTime";
	  पक्ष cbReadWriteSynch:
	    प्रत्यागम "cbReadWriteSynch";
	  पक्ष cbReadOnlySynch:
	    प्रत्यागम "cbReadOnlySynch";
	  पक्ष cbNextSimTime:
	    प्रत्यागम "cbNextSimTime";
	  पक्ष cbAfterDelay:
	    प्रत्यागम "cbAfterDelay";
	  पक्ष cbEndOfCompile:
	    प्रत्यागम "cbEndOfCompile";
	  पक्ष cbStartOfSimulation:
	    प्रत्यागम "cbStartOfSimulation";
	  पक्ष cbEndOfSimulation:
	    प्रत्यागम "cbEndOfSimulation";
	  पक्ष cbError:
	    प्रत्यागम "cbError";
	  पक्ष cbTchkViolation:
	    प्रत्यागम "cbTchkViolation";
	  पक्ष cbStartOfSave:
	    प्रत्यागम "cbStartOfSave";
	  पक्ष cbEndOfSave:
	    प्रत्यागम "cbEndOfSave";
	  पक्ष cbStartOfRestart:
	    प्रत्यागम "cbStartOfRestart";
	  पक्ष cbEndOfRestart:
	    प्रत्यागम "cbEndOfRestart";
	  पक्ष cbStartOfReset:
	    प्रत्यागम "cbStartOfReset";
	  पक्ष cbEndOfReset:
	    प्रत्यागम "cbEndOfReset";
	  पक्ष cbEnterInteractive:
	    प्रत्यागम "cbEnterInteractive";
	  पक्ष cbExitInteractive:
	    प्रत्यागम "cbExitInteractive";
	  पक्ष cbInteractiveScopeChange:
	    प्रत्यागम "cbInteractiveScopeChange";
	  पक्ष cbUnresolvedSystf:
	    प्रत्यागम "cbUnresolvedSystf";
	  पक्ष cbAtEndOfSimTime:
	    प्रत्यागम "cbAtEndOfSimTime";
	  स्वभाव:
	    प्रत्यागम "unrecognised";
      }
}

स्थिर तार्किक check_callback_time(p_cb_data data, तार्किक allow_suppress)
{
      प्रतिज्ञा(data);
      यदि (!data->time) {
	    यदि (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  प्रत्यागम असत्य;
	    }
	    प्रत्यागम सत्य;
      }
      विकल्प (data->time->type) {
	  पक्ष vpiSimTime:
	    भङ्ग;
	  पक्ष vpiScaledRealTime:
	    भङ्ग;
	  पक्ष vpiSuppressTime:
	    यदि (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  प्रत्यागम असत्य;
	    }
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    प्रत्यागम असत्य;
      }
      प्रत्यागम सत्य;
}

स्थिर शून्य set_callback_time(p_cb_data data)
{
      प्रतिज्ञा(data && data->time);
      data->time->निचला  = 0;
      data->time->ऊपरी = 0;
      data->time->real = 0.0;
      विकल्प (data->time->type) {
	  पक्ष vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    भङ्ग;
	  पक्ष vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    भङ्ग;
	  पक्ष vpiSuppressTime:
	    भङ्ग;
	  स्वभाव:
	    प्रतिज्ञा(0);
	    भङ्ग;
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

पूर्णाङ्क __vpiCallback::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiCallback; }


मूल्य_callback::मूल्य_callback(p_cb_data data)
{
      cb_data = *data;
      यदि (data->time) {
	    cb_time = *(data->time);
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      यदि (data->value) {
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
      प्रत्यागम सत्य;
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

      मूल्य_part_callback(ध्रुव मूल्य_part_callback&) = delete;
      मूल्य_part_callback& operator=(ध्रुव मूल्य_part_callback&) = delete;

      तार्किक test_मूल्य_callback_ready(शून्य) override;

    गुप्त:
      वर्ण*मूल्य_bits;
      size_t मूल्य_off;
};

अन्तःस्थ मूल्य_part_callback::मूल्य_part_callback(p_cb_data data)
: मूल्य_callback(data)
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
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      sig_fil->get_मूल्य(&tmp_मूल्य);

      मूल्य_bits = नूतन वर्ण[pobj->width+1];
      मूल्य_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width);
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

      यदि (memcmp(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width) == 0)
	    प्रत्यागम असत्य;

      memcpy(मूल्य_bits, tmp_मूल्य.value.str + मूल्य_off, pobj->width);
      प्रत्यागम सत्य;
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
      मूल्य_callback*cbh = नूतन मूल्य_part_callback(data);
      प्रत्यागम cbh;
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
      यदि (!check_callback_time(data, सत्य))
	    प्रत्यागम 0;

      यदि (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            प्रत्यागम 0;
      }

	// Special case: the target object is a vpiPartSelect
      यदि (data->obj->get_type_code() == vpiPartSelect) {
            यदि (data->obj->vpi_handle(vpiArray))
	        प्रत्यागम vpip_array_word_change(data);
            अन्यथा
	        प्रत्यागम make_मूल्य_change_part(data);
      }

      यदि (data->obj->get_type_code() == vpiMemoryWord)
	    प्रत्यागम vpip_array_word_change(data);

      यदि (data->obj->get_type_code() == vpiMemory)
	    प्रत्यागम vpip_array_change(data);

      मूल्य_callback*obj = नूतन मूल्य_callback(data);

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
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->गाँठ->fil);
	    प्रतिज्ञा(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    भङ्ग;

	  पक्ष vpiRealVar:
	    vpip_real_मूल्य_change(obj, data->obj);
	    भङ्ग;

	  पक्ष vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    भङ्ग;

	  पक्ष vpiModule:
	  पक्ष vpiConstant:
	  पक्ष vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    प्रत्यागम 0;
      }

      प्रत्यागम obj;
}

वर्ग sync_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(ध्रुव sync_callback&) = delete;
      sync_callback& operator=(ध्रुव sync_callback&) = delete;

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
      प्रतिज्ञा(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

शून्य sync_cb::run_run()
{
      यदि (handle == 0)
	    प्रत्यागम;

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
	    भङ्ग;

	  पक्ष vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (पूर्णाङ्क)obj->cb_time.type);
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
      प्रत्यागम tv;
}

स्थिर sync_callback* make_sync(p_cb_data data, तार्किक readonly_flag)
{
      यदि (!check_callback_time(data, असत्य))
	    प्रत्यागम 0;

      sync_callback*obj = नूतन sync_callback(data);

      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = readonly_flag? सत्य : असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, सत्य, readonly_flag);

      प्रत्यागम obj;
}

स्थिर संरचना __vpiCallback* make_afterdelay(p_cb_data data)
{
      यदि (!check_callback_time(data, असत्य))
	    प्रत्यागम 0;

      sync_callback*obj = नूतन sync_callback(data);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, असत्य);

      प्रत्यागम obj;
}

स्थिर संरचना __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      यदि (!check_callback_time(data, असत्य))
	    प्रत्यागम 0;

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

      प्रत्यागम obj;
}

स्थिर संरचना __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      यदि (!check_callback_time(data, असत्य))
	    प्रत्यागम 0;

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

      प्रत्यागम obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

वर्ग simulator_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit simulator_callback(ध्रुव संरचना t_cb_data*data);

    सार्वजनिक:
      संरचना t_vpi_time cb_time;
};

अन्तःस्थ simulator_callback::simulator_callback(ध्रुव संरचना t_cb_data*data)
{
      cb_data = *data;
      यदि ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } अन्यथा यदि (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
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
      यावत् (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      यावत् (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      यावत् (EndOfSimulation) {
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

      यावत् (EndOfCompile) {
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

      यावत् (StartOfSimulation) {
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

      यावत् (EndOfSimulation) {
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

      यावत् (next) {
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
	    प्रत्यागम 0;

      simulator_callback*obj = नूतन simulator_callback(data);

      /* Insert at head of list */
      विकल्प (data->reason) {
	  पक्ष cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    भङ्ग;
	  पक्ष cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    भङ्ग;
	  पक्ष cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    भङ्ग;
	  पक्ष cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    भङ्ग;
      }

      प्रत्यागम obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      संरचना __vpiCallback*obj = 0;

      प्रतिज्ञा(data);
      विकल्प (data->reason) {

	  पक्ष cb_मूल्य_Change:
	    obj = make_मूल्य_change(data);
	    भङ्ग;

	  पक्ष cbReadOnlySynch:
	    obj = make_sync(data, सत्य);
	    भङ्ग;

	  पक्ष cbReadWriteSynch:
	    obj = make_sync(data, असत्य);
	    भङ्ग;

	  पक्ष cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    भङ्ग;

	  पक्ष cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    भङ्ग;

	  पक्ष cbAfterDelay:
	    obj = make_afterdelay(data);
	    भङ्ग;

	  पक्ष cbEndOfCompile:
	  पक्ष cbStartOfSimulation:
	  पक्ष cbEndOfSimulation:
	  पक्ष cbNextSimTime:
	    obj = make_prepost(data);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (पूर्णाङ्क)data->reason);
	    भङ्ग;
      }

      प्रत्यागम obj;
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

      प्रत्यागम 1;
}

शून्य callback_execute(संरचना __vpiCallback*cur)
{
      ध्रुव vpi_mode_t save_mode = vpi_mode_flag;
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
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_शब्दगण = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      प्रतिज्ञा(vpi_callbacks_ == 0);
      प्रतिज्ञा(array_शब्दगण == 0);
}

शून्य vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      संरचना __vpi_array_word*tmp = नूतन __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_शब्दगण;
      array_शब्दगण = tmp;
}

शून्य vvp_vpi_callback::add_vpi_callback(मूल्य_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_vpi_callback::clear_all_callbacks()
{
      यावत् (vpi_callbacks_) {
	    मूल्य_callback *tmp = dynamic_cast<मूल्य_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      यावत् (array_शब्दगण) {
	    संरचना __vpi_array_word*tmp = array_शब्दगण->next;
	    delete array_शब्दगण;
	    array_शब्दगण = tmp;
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
      संरचना __vpi_array_word*array_word = array_शब्दगण;
      यावत् (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      मूल्य_callback *next = vpi_callbacks_;
      मूल्य_callback *prev = 0;

      यावत् (next) {
	    मूल्य_callback*cur = next;
	    next = dynamic_cast<मूल्य_callback*>(cur->next);

	    यदि (cur->cb_data.cb_rtn != 0) {
		  यदि (cur->test_मूल्य_callback_ready()) {
			यदि (cur->cb_data.value)
			      get_मूल्य(cur->cb_data.value);

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

शून्य vvp_signal_मूल्य::get_signal_मूल्य(संरचना t_vpi_मूल्य*vp)
{
      विकल्प (vp->format) {
	  पक्ष vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	  पक्ष vpiOctStrVal:
	  पक्ष vpiDecStrVal:
	  पक्ष vpiHexStrVal:
	  पक्ष vpiIntVal:
	  पक्ष vpiVectorVal:
	  पक्ष vpiStringVal:
	  पक्ष vpiRealVal: {
	    unsigned wid = मूल्य_size();
	    vvp_vector4_t vec4(wid);
	    प्रति (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_मूल्य(vec4, wid, असत्य, vp);
	    भङ्ग;
	  }

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (पूर्णाङ्क)vp->format);
      }
}

स्थिर शून्य real_signal_मूल्य(संरचना t_vpi_मूल्य*vp, द्विगुण rval)
{
      स्थिर ध्रुव size_t RBUF_SIZE = 64 + 1;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      विकल्प (vp->format) {
	  पक्ष vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  पक्ष vpiRealVal:
	    vp->value.real = rval;
	    भङ्ग;

	  पक्ष vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    यदि (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } अन्यथा {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    यदि (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    अन्यथा
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		यावत् (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		प्रति (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		यदि (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		भङ्ग;
	  }

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (पूर्णाङ्क)vp->format);
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
#यदि 0
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
#यदि 0
शून्य vvp_wire_string::get_मूल्य(संरचना t_vpi_मूल्य*val)
{
      प्रतिज्ञा(0);
}
#endif
