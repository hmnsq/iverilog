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
#அந்தர்பாவ  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using நாமாகாஶ std;

நிலையான மாறிலி எழுத்து*cb_reason_name(PLI_INT32 reason)
{
      தேர்வு (reason) {
	  நிலை cb_மதிப்பு_Change:
	    திரும்பு "cbValueChange";
	  நிலை cbStmt:
	    திரும்பு "cbStmt";
	  நிலை cbForce:
	    திரும்பு "cbForce";
	  நிலை cbRelease:
	    திரும்பு "cbRelease";
	  நிலை cbAtStartOfSimTime:
	    திரும்பு "cbAtStartOfSimTime";
	  நிலை cbReadWriteSynch:
	    திரும்பு "cbReadWriteSynch";
	  நிலை cbReadOnlySynch:
	    திரும்பு "cbReadOnlySynch";
	  நிலை cbNextSimTime:
	    திரும்பு "cbNextSimTime";
	  நிலை cbAfterDelay:
	    திரும்பு "cbAfterDelay";
	  நிலை cbEndOfCompile:
	    திரும்பு "cbEndOfCompile";
	  நிலை cbStartOfSimulation:
	    திரும்பு "cbStartOfSimulation";
	  நிலை cbEndOfSimulation:
	    திரும்பு "cbEndOfSimulation";
	  நிலை cbError:
	    திரும்பு "cbError";
	  நிலை cbTchkViolation:
	    திரும்பு "cbTchkViolation";
	  நிலை cbStartOfSave:
	    திரும்பு "cbStartOfSave";
	  நிலை cbEndOfSave:
	    திரும்பு "cbEndOfSave";
	  நிலை cbStartOfRestart:
	    திரும்பு "cbStartOfRestart";
	  நிலை cbEndOfRestart:
	    திரும்பு "cbEndOfRestart";
	  நிலை cbStartOfReset:
	    திரும்பு "cbStartOfReset";
	  நிலை cbEndOfReset:
	    திரும்பு "cbEndOfReset";
	  நிலை cbEnterInteractive:
	    திரும்பு "cbEnterInteractive";
	  நிலை cbExitInteractive:
	    திரும்பு "cbExitInteractive";
	  நிலை cbInteractiveScopeChange:
	    திரும்பு "cbInteractiveScopeChange";
	  நிலை cbUnresolvedSystf:
	    திரும்பு "cbUnresolvedSystf";
	  நிலை cbAtEndOfSimTime:
	    திரும்பு "cbAtEndOfSimTime";
	  இயல்பு:
	    திரும்பு "unrecognised";
      }
}

நிலையான தருக்க check_callback_time(p_cb_data data, தருக்க allow_suppress)
{
      ப்ரதிஜ்ஞா(data);
      எனில் (!data->time) {
	    எனில் (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  திரும்பு பொய்;
	    }
	    திரும்பு உண்மை;
      }
      தேர்வு (data->time->type) {
	  நிலை vpiSimTime:
	    நிறுத்து;
	  நிலை vpiScaledRealTime:
	    நிறுத்து;
	  நிலை vpiSuppressTime:
	    எனில் (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  திரும்பு பொய்;
	    }
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    திரும்பு பொய்;
      }
      திரும்பு உண்மை;
}

நிலையான வெறுமை set_callback_time(p_cb_data data)
{
      ப்ரதிஜ்ஞா(data && data->time);
      data->time->கீழ்  = 0;
      data->time->உயர்வு = 0;
      data->time->real = 0.0;
      தேர்வு (data->time->type) {
	  நிலை vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    நிறுத்து;
	  நிலை vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    நிறுத்து;
	  நிலை vpiSuppressTime:
	    நிறுத்து;
	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
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

வகுப்பு sync_callback;

கட்டமைப்பு sync_cb  : பொது vvp_gen_event_s {
      sync_callback*handle;
      தருக்க sync_flag;

      ~sync_cb () override { }

      மெய்நிகர் வெறுமை run_run() override;
};

உள்வரி __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

முழு __vpiCallback::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiCallback; }


மதிப்பு_callback::மதிப்பு_callback(p_cb_data data)
{
      cb_data = *data;
      எனில் (data->time) {
	    cb_time = *(data->time);
      } இல்லையெனில் {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      எனில் (data->value) {
	    cb_மதிப்பு = *(data->value);
      } இல்லையெனில் {
	    cb_மதிப்பு.format = vpiSuppressVal;
      }
      cb_data.value = &cb_மதிப்பு;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
தருக்க மதிப்பு_callback::test_மதிப்பு_callback_ready(வெறுமை)
{
      திரும்பு உண்மை;
}

நிலையான வெறுமை vpip_real_மதிப்பு_change(மதிப்பு_callback*cbh, vpiHandle ref)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      ப்ரதிஜ்ஞா(obj);

      obj->add_vpi_callback(cbh);
}

வகுப்பு மதிப்பு_part_callback : பொது மதிப்பு_callback {
    பொது:
      explicit மதிப்பு_part_callback(p_cb_data data);
      ~மதிப்பு_part_callback() override;

      மதிப்பு_part_callback(மாறிலி மதிப்பு_part_callback&) = delete;
      மதிப்பு_part_callback& operator=(மாறிலி மதிப்பு_part_callback&) = delete;

      தருக்க test_மதிப்பு_callback_ready(வெறுமை) override;

    தனிப்பட்ட:
      எழுத்து*மதிப்பு_bits;
      size_t மதிப்பு_off;
};

உள்வரி மதிப்பு_part_callback::மதிப்பு_part_callback(p_cb_data data)
: மதிப்பு_callback(data)
{
      கட்டமைப்பு __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      ப்ரதிஜ்ஞா(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ப்ரதிஜ்ஞா(sig_fil);

      sig_fil->add_vpi_callback(இது);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_மதிப்பு tmp_மதிப்பு;
      tmp_மதிப்பு.format = vpiBinStrVal;
      sig_fil->get_மதிப்பு(&tmp_மதிப்பு);

      மதிப்பு_bits = புதிய எழுத்து[pobj->width+1];
      மதிப்பு_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(மதிப்பு_bits, tmp_மதிப்பு.value.str + மதிப்பு_off, pobj->width);
      மதிப்பு_bits[pobj->width] = 0;
}

மதிப்பு_part_callback::~மதிப்பு_part_callback()
{
      delete[]மதிப்பு_bits;
}

தருக்க மதிப்பு_part_callback::test_மதிப்பு_callback_ready(வெறுமை)
{
      கட்டமைப்பு __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      ப்ரதிஜ்ஞா(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ப்ரதிஜ்ஞா(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_மதிப்பு tmp_மதிப்பு;
      tmp_மதிப்பு.format = vpiBinStrVal;
      sig_fil->get_மதிப்பு(&tmp_மதிப்பு);

      எனில் (memcmp(மதிப்பு_bits, tmp_மதிப்பு.value.str + மதிப்பு_off, pobj->width) == 0)
	    திரும்பு பொய்;

      memcpy(மதிப்பு_bits, tmp_மதிப்பு.value.str + மதிப்பு_off, pobj->width);
      திரும்பு உண்மை;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
நிலையான மதிப்பு_callback*make_மதிப்பு_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      மதிப்பு_callback*cbh = புதிய மதிப்பு_part_callback(data);
      திரும்பு cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
நிலையான மதிப்பு_callback* make_மதிப்பு_change(p_cb_data data)
{
      எனில் (!check_callback_time(data, உண்மை))
	    திரும்பு 0;

      எனில் (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            திரும்பு 0;
      }

	// Special case: the target object is a vpiPartSelect
      எனில் (data->obj->get_type_code() == vpiPartSelect) {
            எனில் (data->obj->vpi_handle(vpiArray))
	        திரும்பு vpip_array_word_change(data);
            இல்லையெனில்
	        திரும்பு make_மதிப்பு_change_part(data);
      }

      எனில் (data->obj->get_type_code() == vpiMemoryWord)
	    திரும்பு vpip_array_word_change(data);

      எனில் (data->obj->get_type_code() == vpiMemory)
	    திரும்பு vpip_array_change(data);

      மதிப்பு_callback*obj = புதிய மதிப்பு_callback(data);

      ப்ரதிஜ்ஞா(data->obj);
      தேர்வு (data->obj->get_type_code()) {

	  நிலை vpiReg:
	  நிலை vpiNet:
	  நிலை vpiIntegerVar:
	  நிலை vpiBitVar:
	  நிலை vpiByteVar:
	  நிலை vpiShortIntVar:
	  நிலை vpiIntVar:
	  நிலை vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    கட்டமைப்பு __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->முனை->fil);
	    ப்ரதிஜ்ஞா(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    நிறுத்து;

	  நிலை vpiRealVar:
	    vpip_real_மதிப்பு_change(obj, data->obj);
	    நிறுத்து;

	  நிலை vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    நிறுத்து;

	  நிலை vpiModule:
	  நிலை vpiConstant:
	  நிலை vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    திரும்பு 0;
      }

      திரும்பு obj;
}

வகுப்பு sync_callback : பொது __vpiCallback {
    பொது:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(மாறிலி sync_callback&) = delete;
      sync_callback& operator=(மாறிலி sync_callback&) = delete;

    பொது:
	// scheduled event
      கட்டமைப்பு sync_cb* cb_sync;
	// user supplied callback data
      கட்டமைப்பு t_vpi_time cb_time;

    தனிப்பட்ட:
};

உள்வரி sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      ப்ரதிஜ்ஞா(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

வெறுமை sync_cb::run_run()
{
      எனில் (handle == 0)
	    திரும்பு;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      எனில் (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    ப்ரதிஜ்ஞா(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

நிலையான vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      தேர்வு (obj->cb_time.type) {
	  நிலை vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    நிறுத்து;

	  நிலை vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (முழு)obj->cb_time.type);
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }
      திரும்பு tv;
}

நிலையான sync_callback* make_sync(p_cb_data data, தருக்க readonly_flag)
{
      எனில் (!check_callback_time(data, பொய்))
	    திரும்பு 0;

      sync_callback*obj = புதிய sync_callback(data);

      கட்டமைப்பு sync_cb*cb = புதிய sync_cb;
      cb->sync_flag = readonly_flag? உண்மை : பொய்;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, உண்மை, readonly_flag);

      திரும்பு obj;
}

நிலையான கட்டமைப்பு __vpiCallback* make_afterdelay(p_cb_data data)
{
      எனில் (!check_callback_time(data, பொய்))
	    திரும்பு 0;

      sync_callback*obj = புதிய sync_callback(data);
      கட்டமைப்பு sync_cb*cb = புதிய sync_cb;
      cb->sync_flag = பொய்;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, பொய்);

      திரும்பு obj;
}

நிலையான கட்டமைப்பு __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      எனில் (!check_callback_time(data, பொய்))
	    திரும்பு 0;

      sync_callback*obj = புதிய sync_callback(data);
      கட்டமைப்பு sync_cb*cb = புதிய sync_cb;
      cb->sync_flag = பொய்;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      எனில் (cur > tv) {
	    tv = 0;
	    ப்ரதிஜ்ஞா(0);
      } இல்லையெனில் எனில் (cur == tv) {
	    tv = 0;
      } இல்லையெனில் {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      திரும்பு obj;
}

நிலையான கட்டமைப்பு __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      எனில் (!check_callback_time(data, பொய்))
	    திரும்பு 0;

      sync_callback*obj = புதிய sync_callback(data);
      கட்டமைப்பு sync_cb*cb = புதிய sync_cb;
      cb->sync_flag = பொய்;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      எனில் (cur > tv) {
	    tv = 0;
	    ப்ரதிஜ்ஞா(0);
      } இல்லையெனில் எனில் (cur == tv) {
	    tv = 0;
      } இல்லையெனில் {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      திரும்பு obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

வகுப்பு simulator_callback : பொது __vpiCallback {
    பொது:
      explicit simulator_callback(மாறிலி கட்டமைப்பு t_cb_data*data);

    பொது:
      கட்டமைப்பு t_vpi_time cb_time;
};

உள்வரி simulator_callback::simulator_callback(மாறிலி கட்டமைப்பு t_cb_data*data)
{
      cb_data = *data;
      எனில் ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } இல்லையெனில் எனில் (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } இல்லையெனில் {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

நிலையான simulator_callback*NextSimTime = 0;
நிலையான simulator_callback*EndOfCompile = 0;
நிலையான simulator_callback*StartOfSimulation = 0;
நிலையான simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
வெறுமை simulator_cb_delete(வெறுமை)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      போது (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      போது (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      போது (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

வெறுமை vpiEndOfCompile(வெறுமை) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ப்ரதிஜ்ஞா(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      போது (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    எனில் (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

வெறுமை vpiStartOfSim(வெறுமை) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ப்ரதிஜ்ஞா(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      போது (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    எனில் (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

வெறுமை vpiPostsim(வெறுமை) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      ப்ரதிஜ்ஞா(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      போது (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    எனில் (cur->cb_data.cb_rtn != 0) {
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
வெறுமை vpiNextSimTime(வெறுமை)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      ப்ரதிஜ்ஞா(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      போது (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            எனில் (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

நிலையான simulator_callback* make_prepost(p_cb_data data)
{
      எனில் ((data->reason == cbNextSimTime) && !check_callback_time(data, உண்மை))
	    திரும்பு 0;

      simulator_callback*obj = புதிய simulator_callback(data);

      /* Insert at head of list */
      தேர்வு (data->reason) {
	  நிலை cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    நிறுத்து;
	  நிலை cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    நிறுத்து;
	  நிலை cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    நிறுத்து;
	  நிலை cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    நிறுத்து;
      }

      திரும்பு obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      கட்டமைப்பு __vpiCallback*obj = 0;

      ப்ரதிஜ்ஞா(data);
      தேர்வு (data->reason) {

	  நிலை cb_மதிப்பு_Change:
	    obj = make_மதிப்பு_change(data);
	    நிறுத்து;

	  நிலை cbReadOnlySynch:
	    obj = make_sync(data, உண்மை);
	    நிறுத்து;

	  நிலை cbReadWriteSynch:
	    obj = make_sync(data, பொய்);
	    நிறுத்து;

	  நிலை cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    நிறுத்து;

	  நிலை cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    நிறுத்து;

	  நிலை cbAfterDelay:
	    obj = make_afterdelay(data);
	    நிறுத்து;

	  நிலை cbEndOfCompile:
	  நிலை cbStartOfSimulation:
	  நிலை cbEndOfSimulation:
	  நிலை cbNextSimTime:
	    obj = make_prepost(data);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (முழு)data->reason);
	    நிறுத்து;
      }

      திரும்பு obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      கட்டமைப்பு __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      ப்ரதிஜ்ஞா(obj);
      obj->cb_data.cb_rtn = 0;

      திரும்பு 1;
}

வெறுமை callback_execute(கட்டமைப்பு __vpiCallback*cur)
{
      மாறிலி vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ப்ரதிஜ்ஞா(cur->cb_data.cb_rtn);
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
கட்டமைப்பு __vpi_array_word {
      கட்டமைப்பு __vpi_array_word* next;
      கட்டமைப்பு __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_சொற்கள் = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      ப்ரதிஜ்ஞா(vpi_callbacks_ == 0);
      ப்ரதிஜ்ஞா(array_சொற்கள் == 0);
}

வெறுமை vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      கட்டமைப்பு __vpi_array_word*tmp = புதிய __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_சொற்கள்;
      array_சொற்கள் = tmp;
}

வெறுமை vvp_vpi_callback::add_vpi_callback(மதிப்பு_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_vpi_callback::clear_all_callbacks()
{
      போது (vpi_callbacks_) {
	    மதிப்பு_callback *tmp = dynamic_cast<மதிப்பு_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      போது (array_சொற்கள்) {
	    கட்டமைப்பு __vpi_array_word*tmp = array_சொற்கள்->next;
	    delete array_சொற்கள்;
	    array_சொற்கள் = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
வெறுமை vvp_vpi_callback::run_vpi_callbacks()
{
      கட்டமைப்பு __vpi_array_word*array_word = array_சொற்கள்;
      போது (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      மதிப்பு_callback *next = vpi_callbacks_;
      மதிப்பு_callback *prev = 0;

      போது (next) {
	    மதிப்பு_callback*cur = next;
	    next = dynamic_cast<மதிப்பு_callback*>(cur->next);

	    எனில் (cur->cb_data.cb_rtn != 0) {
		  எனில் (cur->test_மதிப்பு_callback_ready()) {
			எனில் (cur->cb_data.value)
			      get_மதிப்பு(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } இல்லையெனில் எனில் (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } இல்லையெனில் {
		  ப்ரதிஜ்ஞா(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

வெறுமை vvp_signal_மதிப்பு::get_signal_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*vp)
{
      தேர்வு (vp->format) {
	  நிலை vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    நிறுத்து;

	  நிலை vpiBinStrVal:
	  நிலை vpiOctStrVal:
	  நிலை vpiDecStrVal:
	  நிலை vpiHexStrVal:
	  நிலை vpiIntVal:
	  நிலை vpiVectorVal:
	  நிலை vpiStringVal:
	  நிலை vpiRealVal: {
	    unsigned wid = மதிப்பு_size();
	    vvp_vector4_t vec4(wid);
	    க்கு (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_மதிப்பு(vec4, wid, பொய், vp);
	    நிறுத்து;
	  }

	  நிலை vpiSuppressVal:
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (முழு)vp->format);
      }
}

நிலையான வெறுமை real_signal_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*vp, இரட்டை rval)
{
      நிலையான மாறிலி size_t RBUF_SIZE = 64 + 1;
      எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      தேர்வு (vp->format) {
	  நிலை vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  நிலை vpiRealVal:
	    vp->value.real = rval;
	    நிறுத்து;

	  நிலை vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    எனில் (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } இல்லையெனில் {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    நிறுத்து;

	  நிலை vpiDecStrVal:
	    எனில் (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    இல்லையெனில்
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		போது (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		க்கு (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		எனில் (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		நிறுத்து;
	  }

	  நிலை vpiSuppressVal:
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (முழு)vp->format);
      }
}

வெறுமை vvp_fun_signal_real_aa::get_signal_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*vp)
{
      real_signal_மதிப்பு(vp, real_மதிப்பு());
}

வெறுமை vvp_wire_real::get_signal_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*vp)
{
      real_signal_மதிப்பு(vp, real_மதிப்பு());
}

வெறுமை vvp_fun_signal_string_aa::get_signal_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*)
{
      ப்ரதிஜ்ஞா(0);
}
#எனில் 0
வெறுமை vvp_wire_string::get_signal_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*vp)
{
      ப்ரதிஜ்ஞா(0);
}
#endif
வெறுமை vvp_wire_vec4::get_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*val)
{
      get_signal_மதிப்பு(val);
}

வெறுமை vvp_wire_vec8::get_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*val)
{
      get_signal_மதிப்பு(val);
}

வெறுமை vvp_wire_real::get_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*val)
{
      get_signal_மதிப்பு(val);
}
#எனில் 0
வெறுமை vvp_wire_string::get_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*val)
{
      ப்ரதிஜ்ஞா(0);
}
#endif
