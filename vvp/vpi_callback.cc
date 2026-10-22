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
#شامل  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using فضاینام std;

ایستا ثابت نویسه*cb_reason_name(PLI_INT32 reason)
{
      انتخاب (reason) {
	  حالت cb_مقدار_Change:
	    بازگشت "cbValueChange";
	  حالت cbStmt:
	    بازگشت "cbStmt";
	  حالت cbForce:
	    بازگشت "cbForce";
	  حالت cbRelease:
	    بازگشت "cbRelease";
	  حالت cbAtStartOfSimTime:
	    بازگشت "cbAtStartOfSimTime";
	  حالت cbReadWriteSynch:
	    بازگشت "cbReadWriteSynch";
	  حالت cbReadOnlySynch:
	    بازگشت "cbReadOnlySynch";
	  حالت cbNextSimTime:
	    بازگشت "cbNextSimTime";
	  حالت cbAfterDelay:
	    بازگشت "cbAfterDelay";
	  حالت cbEndOfCompile:
	    بازگشت "cbEndOfCompile";
	  حالت cbStartOfSimulation:
	    بازگشت "cbStartOfSimulation";
	  حالت cbEndOfSimulation:
	    بازگشت "cbEndOfSimulation";
	  حالت cbError:
	    بازگشت "cbError";
	  حالت cbTchkViolation:
	    بازگشت "cbTchkViolation";
	  حالت cbStartOfSave:
	    بازگشت "cbStartOfSave";
	  حالت cbEndOfSave:
	    بازگشت "cbEndOfSave";
	  حالت cbStartOfRestart:
	    بازگشت "cbStartOfRestart";
	  حالت cbEndOfRestart:
	    بازگشت "cbEndOfRestart";
	  حالت cbStartOfReset:
	    بازگشت "cbStartOfReset";
	  حالت cbEndOfReset:
	    بازگشت "cbEndOfReset";
	  حالت cbEnterInteractive:
	    بازگشت "cbEnterInteractive";
	  حالت cbExitInteractive:
	    بازگشت "cbExitInteractive";
	  حالت cbInteractiveScopeChange:
	    بازگشت "cbInteractiveScopeChange";
	  حالت cbUnresolvedSystf:
	    بازگشت "cbUnresolvedSystf";
	  حالت cbAtEndOfSimTime:
	    بازگشت "cbAtEndOfSimTime";
	  پیشفرض:
	    بازگشت "unrecognised";
      }
}

ایستا منطقی check_callback_time(p_cb_data data, منطقی allow_suppress)
{
      أكد(data);
      اگر (!data->time) {
	    اگر (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  بازگشت نادرست;
	    }
	    بازگشت درست;
      }
      انتخاب (data->time->type) {
	  حالت vpiSimTime:
	    بشکن;
	  حالت vpiScaledRealTime:
	    بشکن;
	  حالت vpiSuppressTime:
	    اگر (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  بازگشت نادرست;
	    }
	    بشکن;
	  پیشفرض:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    بازگشت نادرست;
      }
      بازگشت درست;
}

ایستا تهی set_callback_time(p_cb_data data)
{
      أكد(data && data->time);
      data->time->کم  = 0;
      data->time->بالا = 0;
      data->time->real = 0.0;
      انتخاب (data->time->type) {
	  حالت vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    بشکن;
	  حالت vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    بشکن;
	  حالت vpiSuppressTime:
	    بشکن;
	  پیشفرض:
	    أكد(0);
	    بشکن;
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

رده sync_callback;

ساختار sync_cb  : عمومی vvp_gen_event_s {
      sync_callback*handle;
      منطقی sync_flag;

      ~sync_cb () override { }

      مجازی تهی run_run() override;
};

درونخطی __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحیح __vpiCallback::get_type_code(تهی) ثابت
{ بازگشت vpiCallback; }


مقدار_callback::مقدار_callback(p_cb_data data)
{
      cb_data = *data;
      اگر (data->time) {
	    cb_time = *(data->time);
      } وگرنه {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      اگر (data->value) {
	    cb_مقدار = *(data->value);
      } وگرنه {
	    cb_مقدار.format = vpiSuppressVal;
      }
      cb_data.value = &cb_مقدار;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
منطقی مقدار_callback::test_مقدار_callback_ready(تهی)
{
      بازگشت درست;
}

ایستا تهی vpip_real_مقدار_change(مقدار_callback*cbh, vpiHandle ref)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      أكد(obj);

      obj->add_vpi_callback(cbh);
}

رده مقدار_part_callback : عمومی مقدار_callback {
    عمومی:
      explicit مقدار_part_callback(p_cb_data data);
      ~مقدار_part_callback() override;

      مقدار_part_callback(ثابت مقدار_part_callback&) = delete;
      مقدار_part_callback& operator=(ثابت مقدار_part_callback&) = delete;

      منطقی test_مقدار_callback_ready(تهی) override;

    خصوصی:
      نویسه*مقدار_bits;
      size_t مقدار_off;
};

درونخطی مقدار_part_callback::مقدار_part_callback(p_cb_data data)
: مقدار_callback(data)
{
      ساختار __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->add_vpi_callback(این);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      sig_fil->get_مقدار(&tmp_مقدار);

      مقدار_bits = جدید نویسه[pobj->width+1];
      مقدار_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(مقدار_bits, tmp_مقدار.value.str + مقدار_off, pobj->width);
      مقدار_bits[pobj->width] = 0;
}

مقدار_part_callback::~مقدار_part_callback()
{
      delete[]مقدار_bits;
}

منطقی مقدار_part_callback::test_مقدار_callback_ready(تهی)
{
      ساختار __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      sig_fil->get_مقدار(&tmp_مقدار);

      اگر (memcmp(مقدار_bits, tmp_مقدار.value.str + مقدار_off, pobj->width) == 0)
	    بازگشت نادرست;

      memcpy(مقدار_bits, tmp_مقدار.value.str + مقدار_off, pobj->width);
      بازگشت درست;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ایستا مقدار_callback*make_مقدار_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      مقدار_callback*cbh = جدید مقدار_part_callback(data);
      بازگشت cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ایستا مقدار_callback* make_مقدار_change(p_cb_data data)
{
      اگر (!check_callback_time(data, درست))
	    بازگشت 0;

      اگر (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            بازگشت 0;
      }

	// Special case: the target object is a vpiPartSelect
      اگر (data->obj->get_type_code() == vpiPartSelect) {
            اگر (data->obj->vpi_handle(vpiArray))
	        بازگشت vpip_array_word_change(data);
            وگرنه
	        بازگشت make_مقدار_change_part(data);
      }

      اگر (data->obj->get_type_code() == vpiMemoryWord)
	    بازگشت vpip_array_word_change(data);

      اگر (data->obj->get_type_code() == vpiMemory)
	    بازگشت vpip_array_change(data);

      مقدار_callback*obj = جدید مقدار_callback(data);

      أكد(data->obj);
      انتخاب (data->obj->get_type_code()) {

	  حالت vpiReg:
	  حالت vpiNet:
	  حالت vpiIntegerVar:
	  حالت vpiBitVar:
	  حالت vpiByteVar:
	  حالت vpiShortIntVar:
	  حالت vpiIntVar:
	  حالت vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    ساختار __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->گره->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    بشکن;

	  حالت vpiRealVar:
	    vpip_real_مقدار_change(obj, data->obj);
	    بشکن;

	  حالت vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    بشکن;

	  حالت vpiModule:
	  حالت vpiConstant:
	  حالت vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    بازگشت 0;
      }

      بازگشت obj;
}

رده sync_callback : عمومی __vpiCallback {
    عمومی:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = delete;
      sync_callback& operator=(ثابت sync_callback&) = delete;

    عمومی:
	// scheduled event
      ساختار sync_cb* cb_sync;
	// user supplied callback data
      ساختار t_vpi_time cb_time;

    خصوصی:
};

درونخطی sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      أكد(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

تهی sync_cb::run_run()
{
      اگر (handle == 0)
	    بازگشت;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      اگر (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    أكد(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

ایستا vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      انتخاب (obj->cb_time.type) {
	  حالت vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    بشکن;

	  حالت vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحیح)obj->cb_time.type);
	    أكد(0);
	    بشکن;
      }
      بازگشت tv;
}

ایستا sync_callback* make_sync(p_cb_data data, منطقی readonly_flag)
{
      اگر (!check_callback_time(data, نادرست))
	    بازگشت 0;

      sync_callback*obj = جدید sync_callback(data);

      ساختار sync_cb*cb = جدید sync_cb;
      cb->sync_flag = readonly_flag? درست : نادرست;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, درست, readonly_flag);

      بازگشت obj;
}

ایستا ساختار __vpiCallback* make_afterdelay(p_cb_data data)
{
      اگر (!check_callback_time(data, نادرست))
	    بازگشت 0;

      sync_callback*obj = جدید sync_callback(data);
      ساختار sync_cb*cb = جدید sync_cb;
      cb->sync_flag = نادرست;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, نادرست);

      بازگشت obj;
}

ایستا ساختار __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      اگر (!check_callback_time(data, نادرست))
	    بازگشت 0;

      sync_callback*obj = جدید sync_callback(data);
      ساختار sync_cb*cb = جدید sync_cb;
      cb->sync_flag = نادرست;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      اگر (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وگرنه اگر (cur == tv) {
	    tv = 0;
      } وگرنه {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      بازگشت obj;
}

ایستا ساختار __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      اگر (!check_callback_time(data, نادرست))
	    بازگشت 0;

      sync_callback*obj = جدید sync_callback(data);
      ساختار sync_cb*cb = جدید sync_cb;
      cb->sync_flag = نادرست;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      اگر (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وگرنه اگر (cur == tv) {
	    tv = 0;
      } وگرنه {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      بازگشت obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

رده simulator_callback : عمومی __vpiCallback {
    عمومی:
      explicit simulator_callback(ثابت ساختار t_cb_data*data);

    عمومی:
      ساختار t_vpi_time cb_time;
};

درونخطی simulator_callback::simulator_callback(ثابت ساختار t_cb_data*data)
{
      cb_data = *data;
      اگر ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } وگرنه اگر (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } وگرنه {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

ایستا simulator_callback*NextSimTime = 0;
ایستا simulator_callback*EndOfCompile = 0;
ایستا simulator_callback*StartOfSimulation = 0;
ایستا simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
تهی simulator_cb_delete(تهی)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      وقتی (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      وقتی (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      وقتی (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

تهی vpiEndOfCompile(تهی) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      وقتی (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    اگر (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

تهی vpiStartOfSim(تهی) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      وقتی (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    اگر (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

تهی vpiPostsim(تهی) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      وقتی (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    اگر (cur->cb_data.cb_rtn != 0) {
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
تهی vpiNextSimTime(تهی)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      وقتی (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            اگر (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ایستا simulator_callback* make_prepost(p_cb_data data)
{
      اگر ((data->reason == cbNextSimTime) && !check_callback_time(data, درست))
	    بازگشت 0;

      simulator_callback*obj = جدید simulator_callback(data);

      /* Insert at head of list */
      انتخاب (data->reason) {
	  حالت cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    بشکن;
	  حالت cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    بشکن;
	  حالت cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    بشکن;
	  حالت cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    بشکن;
      }

      بازگشت obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      ساختار __vpiCallback*obj = 0;

      أكد(data);
      انتخاب (data->reason) {

	  حالت cb_مقدار_Change:
	    obj = make_مقدار_change(data);
	    بشکن;

	  حالت cbReadOnlySynch:
	    obj = make_sync(data, درست);
	    بشکن;

	  حالت cbReadWriteSynch:
	    obj = make_sync(data, نادرست);
	    بشکن;

	  حالت cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    بشکن;

	  حالت cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    بشکن;

	  حالت cbAfterDelay:
	    obj = make_afterdelay(data);
	    بشکن;

	  حالت cbEndOfCompile:
	  حالت cbStartOfSimulation:
	  حالت cbEndOfSimulation:
	  حالت cbNextSimTime:
	    obj = make_prepost(data);
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (صحیح)data->reason);
	    بشکن;
      }

      بازگشت obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      ساختار __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      أكد(obj);
      obj->cb_data.cb_rtn = 0;

      بازگشت 1;
}

تهی callback_execute(ساختار __vpiCallback*cur)
{
      ثابت vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      أكد(cur->cb_data.cb_rtn);
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
ساختار __vpi_array_word {
      ساختار __vpi_array_word* next;
      ساختار __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_واژگان = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      أكد(vpi_callbacks_ == 0);
      أكد(array_واژگان == 0);
}

تهی vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      ساختار __vpi_array_word*tmp = جدید __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_واژگان;
      array_واژگان = tmp;
}

تهی vvp_vpi_callback::add_vpi_callback(مقدار_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
تهی vvp_vpi_callback::clear_all_callbacks()
{
      وقتی (vpi_callbacks_) {
	    مقدار_callback *tmp = dynamic_cast<مقدار_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      وقتی (array_واژگان) {
	    ساختار __vpi_array_word*tmp = array_واژگان->next;
	    delete array_واژگان;
	    array_واژگان = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
تهی vvp_vpi_callback::run_vpi_callbacks()
{
      ساختار __vpi_array_word*array_word = array_واژگان;
      وقتی (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      مقدار_callback *next = vpi_callbacks_;
      مقدار_callback *prev = 0;

      وقتی (next) {
	    مقدار_callback*cur = next;
	    next = dynamic_cast<مقدار_callback*>(cur->next);

	    اگر (cur->cb_data.cb_rtn != 0) {
		  اگر (cur->test_مقدار_callback_ready()) {
			اگر (cur->cb_data.value)
			      get_مقدار(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } وگرنه اگر (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } وگرنه {
		  أكد(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

تهی vvp_signal_مقدار::get_signal_مقدار(ساختار t_vpi_مقدار*vp)
{
      انتخاب (vp->format) {
	  حالت vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    بشکن;

	  حالت vpiBinStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiHexStrVal:
	  حالت vpiIntVal:
	  حالت vpiVectorVal:
	  حالت vpiStringVal:
	  حالت vpiRealVal: {
	    unsigned wid = مقدار_size();
	    vvp_vector4_t vec4(wid);
	    برای (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_مقدار(vec4, wid, نادرست, vp);
	    بشکن;
	  }

	  حالت vpiSuppressVal:
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (صحیح)vp->format);
      }
}

ایستا تهی real_signal_مقدار(ساختار t_vpi_مقدار*vp, دوگانه rval)
{
      ایستا ثابت size_t RBUF_SIZE = 64 + 1;
      نویسه*rbuf = static_cast<نویسه *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      انتخاب (vp->format) {
	  حالت vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالت vpiRealVal:
	    vp->value.real = rval;
	    بشکن;

	  حالت vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    اگر (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } وگرنه {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    بشکن;

	  حالت vpiDecStrVal:
	    اگر (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    وگرنه
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		وقتی (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		برای (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		اگر (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		بشکن;
	  }

	  حالت vpiSuppressVal:
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (صحیح)vp->format);
      }
}

تهی vvp_fun_signal_real_aa::get_signal_مقدار(ساختار t_vpi_مقدار*vp)
{
      real_signal_مقدار(vp, real_مقدار());
}

تهی vvp_wire_real::get_signal_مقدار(ساختار t_vpi_مقدار*vp)
{
      real_signal_مقدار(vp, real_مقدار());
}

تهی vvp_fun_signal_string_aa::get_signal_مقدار(ساختار t_vpi_مقدار*)
{
      أكد(0);
}
#اگر 0
تهی vvp_wire_string::get_signal_مقدار(ساختار t_vpi_مقدار*vp)
{
      أكد(0);
}
#endif
تهی vvp_wire_vec4::get_مقدار(ساختار t_vpi_مقدار*val)
{
      get_signal_مقدار(val);
}

تهی vvp_wire_vec8::get_مقدار(ساختار t_vpi_مقدار*val)
{
      get_signal_مقدار(val);
}

تهی vvp_wire_real::get_مقدار(ساختار t_vpi_مقدار*val)
{
      get_signal_مقدار(val);
}
#اگر 0
تهی vvp_wire_string::get_مقدار(ساختار t_vpi_مقدار*val)
{
      أكد(0);
}
#endif
