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
#ضمن  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using نطاق std;

ساكن ثابت حرف*cb_reason_name(PLI_INT32 reason)
{
      بدل (reason) {
	  حالة cb_بەها_Change:
	    أرجع "cbValueChange";
	  حالة cbStmt:
	    أرجع "cbStmt";
	  حالة cbForce:
	    أرجع "cbForce";
	  حالة cbRelease:
	    أرجع "cbRelease";
	  حالة cbAtStartOfSimTime:
	    أرجع "cbAtStartOfSimTime";
	  حالة cbReadWriteSynch:
	    أرجع "cbReadWriteSynch";
	  حالة cbReadOnlySynch:
	    أرجع "cbReadOnlySynch";
	  حالة cbNextSimTime:
	    أرجع "cbNextSimTime";
	  حالة cbAfterDelay:
	    أرجع "cbAfterDelay";
	  حالة cbEndOfCompile:
	    أرجع "cbEndOfCompile";
	  حالة cbStartOfSimulation:
	    أرجع "cbStartOfSimulation";
	  حالة cbEndOfSimulation:
	    أرجع "cbEndOfSimulation";
	  حالة cbError:
	    أرجع "cbError";
	  حالة cbTchkViolation:
	    أرجع "cbTchkViolation";
	  حالة cbStartOfSave:
	    أرجع "cbStartOfSave";
	  حالة cbEndOfSave:
	    أرجع "cbEndOfSave";
	  حالة cbStartOfRestart:
	    أرجع "cbStartOfRestart";
	  حالة cbEndOfRestart:
	    أرجع "cbEndOfRestart";
	  حالة cbStartOfReset:
	    أرجع "cbStartOfReset";
	  حالة cbEndOfReset:
	    أرجع "cbEndOfReset";
	  حالة cbEnterInteractive:
	    أرجع "cbEnterInteractive";
	  حالة cbExitInteractive:
	    أرجع "cbExitInteractive";
	  حالة cbInteractiveScopeChange:
	    أرجع "cbInteractiveScopeChange";
	  حالة cbUnresolvedSystf:
	    أرجع "cbUnresolvedSystf";
	  حالة cbAtEndOfSimTime:
	    أرجع "cbAtEndOfSimTime";
	  افتراضي:
	    أرجع "unrecognised";
      }
}

ساكن منطقي check_callback_time(p_cb_data data, منطقي allow_suppress)
{
      أكد(data);
      إذا (!data->time) {
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  أرجع خطأ;
	    }
	    أرجع true;
      }
      بدل (data->time->type) {
	  حالة vpiSimTime:
	    اكسر;
	  حالة vpiScaledRealTime:
	    اكسر;
	  حالة vpiSuppressTime:
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  أرجع خطأ;
	    }
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    أرجع خطأ;
      }
      أرجع true;
}

ساكن فراغ set_callback_time(p_cb_data data)
{
      أكد(data && data->time);
      data->time->نزم  = 0;
      data->time->بەرز = 0;
      data->time->real = 0.0;
      بدل (data->time->type) {
	  حالة vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    اكسر;
	  حالة vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    اكسر;
	  حالة vpiSuppressTime:
	    اكسر;
	  افتراضي:
	    أكد(0);
	    اكسر;
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

صنف sync_callback;

بنية sync_cb  : عام vvp_gen_event_s {
      sync_callback*handle;
      منطقي sync_flag;

      ~sync_cb () override { }

      virtual فراغ run_run() override;
};

مضمن __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحيح __vpiCallback::get_type_code(فراغ) ثابت
{ أرجع vpiCallback; }


بەها_callback::بەها_callback(p_cb_data data)
{
      cb_data = *data;
      إذا (data->time) {
	    cb_time = *(data->time);
      } وإلا {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      إذا (data->value) {
	    cb_بەها = *(data->value);
      } وإلا {
	    cb_بەها.format = vpiSuppressVal;
      }
      cb_data.value = &cb_بەها;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
منطقي بەها_callback::test_بەها_callback_ready(فراغ)
{
      أرجع true;
}

ساكن فراغ vpip_real_بەها_change(بەها_callback*cbh, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      أكد(obj);

      obj->add_vpi_callback(cbh);
}

صنف بەها_part_callback : عام بەها_callback {
    عام:
      explicit بەها_part_callback(p_cb_data data);
      ~بەها_part_callback() override;

      بەها_part_callback(ثابت بەها_part_callback&) = delete;
      بەها_part_callback& operator=(ثابت بەها_part_callback&) = delete;

      منطقي test_بەها_callback_ready(فراغ) override;

    خاص:
      حرف*بەها_bits;
      size_t بەها_off;
};

مضمن بەها_part_callback::بەها_part_callback(p_cb_data data)
: بەها_callback(data)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->add_vpi_callback(هذا);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_بەها tmp_بەها;
      tmp_بەها.format = vpiBinStrVal;
      sig_fil->get_بەها(&tmp_بەها);

      بەها_bits = جديد حرف[pobj->width+1];
      بەها_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(بەها_bits, tmp_بەها.value.str + بەها_off, pobj->width);
      بەها_bits[pobj->width] = 0;
}

بەها_part_callback::~بەها_part_callback()
{
      delete[]بەها_bits;
}

منطقي بەها_part_callback::test_بەها_callback_ready(فراغ)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_بەها tmp_بەها;
      tmp_بەها.format = vpiBinStrVal;
      sig_fil->get_بەها(&tmp_بەها);

      إذا (memcmp(بەها_bits, tmp_بەها.value.str + بەها_off, pobj->width) == 0)
	    أرجع خطأ;

      memcpy(بەها_bits, tmp_بەها.value.str + بەها_off, pobj->width);
      أرجع true;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ساكن بەها_callback*make_بەها_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      بەها_callback*cbh = جديد بەها_part_callback(data);
      أرجع cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ساكن بەها_callback* make_بەها_change(p_cb_data data)
{
      إذا (!check_callback_time(data, true))
	    أرجع 0;

      إذا (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            أرجع 0;
      }

	// Special case: the target object is a vpiPartSelect
      إذا (data->obj->get_type_code() == vpiPartSelect) {
            إذا (data->obj->vpi_handle(vpiArray))
	        أرجع vpip_array_word_change(data);
            وإلا
	        أرجع make_بەها_change_part(data);
      }

      إذا (data->obj->get_type_code() == vpiMemoryWord)
	    أرجع vpip_array_word_change(data);

      إذا (data->obj->get_type_code() == vpiMemory)
	    أرجع vpip_array_change(data);

      بەها_callback*obj = جديد بەها_callback(data);

      أكد(data->obj);
      بدل (data->obj->get_type_code()) {

	  حالة vpiReg:
	  حالة vpiNet:
	  حالة vpiIntegerVar:
	  حالة vpiBitVar:
	  حالة vpiByteVar:
	  حالة vpiShortIntVar:
	  حالة vpiIntVar:
	  حالة vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    بنية __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->گرێ->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    اكسر;

	  حالة vpiRealVar:
	    vpip_real_بەها_change(obj, data->obj);
	    اكسر;

	  حالة vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    اكسر;

	  حالة vpiModule:
	  حالة vpiConstant:
	  حالة vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    أرجع 0;
      }

      أرجع obj;
}

صنف sync_callback : عام __vpiCallback {
    عام:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = delete;
      sync_callback& operator=(ثابت sync_callback&) = delete;

    عام:
	// scheduled event
      بنية sync_cb* cb_sync;
	// user supplied callback data
      بنية t_vpi_time cb_time;

    خاص:
};

مضمن sync_callback::sync_callback(p_cb_data data)
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

فراغ sync_cb::run_run()
{
      إذا (handle == 0)
	    أرجع;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      إذا (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    أكد(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

ساكن vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      بدل (obj->cb_time.type) {
	  حالة vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    اكسر;

	  حالة vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحيح)obj->cb_time.type);
	    أكد(0);
	    اكسر;
      }
      أرجع tv;
}

ساكن sync_callback* make_sync(p_cb_data data, منطقي readonly_flag)
{
      إذا (!check_callback_time(data, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(data);

      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = readonly_flag? true : خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, true, readonly_flag);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_afterdelay(p_cb_data data)
{
      إذا (!check_callback_time(data, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(data);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, خطأ);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      إذا (!check_callback_time(data, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(data);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      إذا (!check_callback_time(data, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(data);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      أرجع obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

صنف simulator_callback : عام __vpiCallback {
    عام:
      explicit simulator_callback(ثابت بنية t_cb_data*data);

    عام:
      بنية t_vpi_time cb_time;
};

مضمن simulator_callback::simulator_callback(ثابت بنية t_cb_data*data)
{
      cb_data = *data;
      إذا ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } وإلا إذا (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } وإلا {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

ساكن simulator_callback*NextSimTime = 0;
ساكن simulator_callback*EndOfCompile = 0;
ساكن simulator_callback*StartOfSimulation = 0;
ساكن simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
فراغ simulator_cb_delete(فراغ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      طالما (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      طالما (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      طالما (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

فراغ vpiEndOfCompile(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      طالما (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    إذا (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

فراغ vpiStartOfSim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      طالما (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    إذا (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

فراغ vpiPostsim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      طالما (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    إذا (cur->cb_data.cb_rtn != 0) {
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
فراغ vpiNextSimTime(فراغ)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      أكد(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      طالما (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            إذا (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ساكن simulator_callback* make_prepost(p_cb_data data)
{
      إذا ((data->reason == cbNextSimTime) && !check_callback_time(data, true))
	    أرجع 0;

      simulator_callback*obj = جديد simulator_callback(data);

      /* Insert at head of list */
      بدل (data->reason) {
	  حالة cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    اكسر;
	  حالة cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    اكسر;
	  حالة cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    اكسر;
	  حالة cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    اكسر;
      }

      أرجع obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      بنية __vpiCallback*obj = 0;

      أكد(data);
      بدل (data->reason) {

	  حالة cb_بەها_Change:
	    obj = make_بەها_change(data);
	    اكسر;

	  حالة cbReadOnlySynch:
	    obj = make_sync(data, true);
	    اكسر;

	  حالة cbReadWriteSynch:
	    obj = make_sync(data, خطأ);
	    اكسر;

	  حالة cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    اكسر;

	  حالة cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    اكسر;

	  حالة cbAfterDelay:
	    obj = make_afterdelay(data);
	    اكسر;

	  حالة cbEndOfCompile:
	  حالة cbStartOfSimulation:
	  حالة cbEndOfSimulation:
	  حالة cbNextSimTime:
	    obj = make_prepost(data);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (صحيح)data->reason);
	    اكسر;
      }

      أرجع obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      بنية __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      أكد(obj);
      obj->cb_data.cb_rtn = 0;

      أرجع 1;
}

فراغ callback_execute(بنية __vpiCallback*cur)
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
بنية __vpi_array_word {
      بنية __vpi_array_word* next;
      بنية __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_وشەکان = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      أكد(vpi_callbacks_ == 0);
      أكد(array_وشەکان == 0);
}

فراغ vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      بنية __vpi_array_word*tmp = جديد __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_وشەکان;
      array_وشەکان = tmp;
}

فراغ vvp_vpi_callback::add_vpi_callback(بەها_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_vpi_callback::clear_all_callbacks()
{
      طالما (vpi_callbacks_) {
	    بەها_callback *tmp = dynamic_cast<بەها_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      طالما (array_وشەکان) {
	    بنية __vpi_array_word*tmp = array_وشەکان->next;
	    delete array_وشەکان;
	    array_وشەکان = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
فراغ vvp_vpi_callback::run_vpi_callbacks()
{
      بنية __vpi_array_word*array_word = array_وشەکان;
      طالما (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      بەها_callback *next = vpi_callbacks_;
      بەها_callback *prev = 0;

      طالما (next) {
	    بەها_callback*cur = next;
	    next = dynamic_cast<بەها_callback*>(cur->next);

	    إذا (cur->cb_data.cb_rtn != 0) {
		  إذا (cur->test_بەها_callback_ready()) {
			إذا (cur->cb_data.value)
			      get_بەها(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } وإلا {
		  أكد(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

فراغ vvp_signal_بەها::get_signal_بەها(بنية t_vpi_بەها*vp)
{
      بدل (vp->format) {
	  حالة vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    اكسر;

	  حالة vpiBinStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiHexStrVal:
	  حالة vpiIntVal:
	  حالة vpiVectorVal:
	  حالة vpiStringVal:
	  حالة vpiRealVal: {
	    unsigned wid = بەها_size();
	    vvp_vector4_t vec4(wid);
	    لكل (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_بەها(vec4, wid, خطأ, vp);
	    اكسر;
	  }

	  حالة vpiSuppressVal:
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (صحيح)vp->format);
      }
}

ساكن فراغ real_signal_بەها(بنية t_vpi_بەها*vp, مزدوج rval)
{
      ساكن ثابت size_t RBUF_SIZE = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالة vpiRealVal:
	    vp->value.real = rval;
	    اكسر;

	  حالة vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    إذا (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } وإلا {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		طالما (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		لكل (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		إذا (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		اكسر;
	  }

	  حالة vpiSuppressVal:
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (صحيح)vp->format);
      }
}

فراغ vvp_fun_signal_real_aa::get_signal_بەها(بنية t_vpi_بەها*vp)
{
      real_signal_بەها(vp, real_بەها());
}

فراغ vvp_wire_real::get_signal_بەها(بنية t_vpi_بەها*vp)
{
      real_signal_بەها(vp, real_بەها());
}

فراغ vvp_fun_signal_string_aa::get_signal_بەها(بنية t_vpi_بەها*)
{
      أكد(0);
}
#إذا 0
فراغ vvp_wire_string::get_signal_بەها(بنية t_vpi_بەها*vp)
{
      أكد(0);
}
#endif
فراغ vvp_wire_vec4::get_بەها(بنية t_vpi_بەها*val)
{
      get_signal_بەها(val);
}

فراغ vvp_wire_vec8::get_بەها(بنية t_vpi_بەها*val)
{
      get_signal_بەها(val);
}

فراغ vvp_wire_real::get_بەها(بنية t_vpi_بەها*val)
{
      get_signal_بەها(val);
}
#إذا 0
فراغ vvp_wire_string::get_بەها(بنية t_vpi_بەها*val)
{
      أكد(0);
}
#endif
