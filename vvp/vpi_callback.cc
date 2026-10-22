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
#અન્તર્ભાવ  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using નામાકાશ std;

સ્થિર અચલ અક્ષર*cb_reason_name(PLI_INT32 reason)
{
      પસંદગી (reason) {
	  સ્થિતિ cb_કિંમત_Change:
	    પાછા "cbValueChange";
	  સ્થિતિ cbStmt:
	    પાછા "cbStmt";
	  સ્થિતિ cbForce:
	    પાછા "cbForce";
	  સ્થિતિ cbRelease:
	    પાછા "cbRelease";
	  સ્થિતિ cbAtStartOfSimTime:
	    પાછા "cbAtStartOfSimTime";
	  સ્થિતિ cbReadWriteSynch:
	    પાછા "cbReadWriteSynch";
	  સ્થિતિ cbReadOnlySynch:
	    પાછા "cbReadOnlySynch";
	  સ્થિતિ cbNextSimTime:
	    પાછા "cbNextSimTime";
	  સ્થિતિ cbAfterDelay:
	    પાછા "cbAfterDelay";
	  સ્થિતિ cbEndOfCompile:
	    પાછા "cbEndOfCompile";
	  સ્થિતિ cbStartOfSimulation:
	    પાછા "cbStartOfSimulation";
	  સ્થિતિ cbEndOfSimulation:
	    પાછા "cbEndOfSimulation";
	  સ્થિતિ cbError:
	    પાછા "cbError";
	  સ્થિતિ cbTchkViolation:
	    પાછા "cbTchkViolation";
	  સ્થિતિ cbStartOfSave:
	    પાછા "cbStartOfSave";
	  સ્થિતિ cbEndOfSave:
	    પાછા "cbEndOfSave";
	  સ્થિતિ cbStartOfRestart:
	    પાછા "cbStartOfRestart";
	  સ્થિતિ cbEndOfRestart:
	    પાછા "cbEndOfRestart";
	  સ્થિતિ cbStartOfReset:
	    પાછા "cbStartOfReset";
	  સ્થિતિ cbEndOfReset:
	    પાછા "cbEndOfReset";
	  સ્થિતિ cbEnterInteractive:
	    પાછા "cbEnterInteractive";
	  સ્થિતિ cbExitInteractive:
	    પાછા "cbExitInteractive";
	  સ્થિતિ cbInteractiveScopeChange:
	    પાછા "cbInteractiveScopeChange";
	  સ્થિતિ cbUnresolvedSystf:
	    પાછા "cbUnresolvedSystf";
	  સ્થિતિ cbAtEndOfSimTime:
	    પાછા "cbAtEndOfSimTime";
	  મૂળભૂત:
	    પાછા "unrecognised";
      }
}

સ્થિર તાર્કિક check_callback_time(p_cb_data data, તાર્કિક allow_suppress)
{
      પ્રતિજ્ઞા(data);
      જો (!data->time) {
	    જો (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  પાછા ખોટું;
	    }
	    પાછા સાચું;
      }
      પસંદગી (data->time->type) {
	  સ્થિતિ vpiSimTime:
	    તોડો;
	  સ્થિતિ vpiScaledRealTime:
	    તોડો;
	  સ્થિતિ vpiSuppressTime:
	    જો (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  પાછા ખોટું;
	    }
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    પાછા ખોટું;
      }
      પાછા સાચું;
}

સ્થિર શૂન્ય set_callback_time(p_cb_data data)
{
      પ્રતિજ્ઞા(data && data->time);
      data->time->નીચલું  = 0;
      data->time->ઉપલું = 0;
      data->time->real = 0.0;
      પસંદગી (data->time->type) {
	  સ્થિતિ vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    તોડો;
	  સ્થિતિ vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    તોડો;
	  સ્થિતિ vpiSuppressTime:
	    તોડો;
	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
	    તોડો;
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

વર્ગ sync_callback;

સંરચના sync_cb  : જાહેર vvp_gen_event_s {
      sync_callback*handle;
      તાર્કિક sync_flag;

      ~sync_cb () override { }

      આભાસી શૂન્ય run_run() override;
};

અંતઃરેખ __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

પૂર્ણાંક __vpiCallback::get_type_code(શૂન્ય) અચલ
{ પાછા vpiCallback; }


કિંમત_callback::કિંમત_callback(p_cb_data data)
{
      cb_data = *data;
      જો (data->time) {
	    cb_time = *(data->time);
      } નહીંતર {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      જો (data->value) {
	    cb_કિંમત = *(data->value);
      } નહીંતર {
	    cb_કિંમત.format = vpiSuppressVal;
      }
      cb_data.value = &cb_કિંમત;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
તાર્કિક કિંમત_callback::test_કિંમત_callback_ready(શૂન્ય)
{
      પાછા સાચું;
}

સ્થિર શૂન્ય vpip_real_કિંમત_change(કિંમત_callback*cbh, vpiHandle ref)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      પ્રતિજ્ઞા(obj);

      obj->add_vpi_callback(cbh);
}

વર્ગ કિંમત_part_callback : જાહેર કિંમત_callback {
    જાહેર:
      explicit કિંમત_part_callback(p_cb_data data);
      ~કિંમત_part_callback() override;

      કિંમત_part_callback(અચલ કિંમત_part_callback&) = delete;
      કિંમત_part_callback& operator=(અચલ કિંમત_part_callback&) = delete;

      તાર્કિક test_કિંમત_callback_ready(શૂન્ય) override;

    ખાનગી:
      અક્ષર*કિંમત_bits;
      size_t કિંમત_off;
};

અંતઃરેખ કિંમત_part_callback::કિંમત_part_callback(p_cb_data data)
: કિંમત_callback(data)
{
      સંરચના __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      પ્રતિજ્ઞા(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      પ્રતિજ્ઞા(sig_fil);

      sig_fil->add_vpi_callback(આ);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_કિંમત tmp_કિંમત;
      tmp_કિંમત.format = vpiBinStrVal;
      sig_fil->get_કિંમત(&tmp_કિંમત);

      કિંમત_bits = નવું અક્ષર[pobj->width+1];
      કિંમત_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(કિંમત_bits, tmp_કિંમત.value.str + કિંમત_off, pobj->width);
      કિંમત_bits[pobj->width] = 0;
}

કિંમત_part_callback::~કિંમત_part_callback()
{
      delete[]કિંમત_bits;
}

તાર્કિક કિંમત_part_callback::test_કિંમત_callback_ready(શૂન્ય)
{
      સંરચના __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      પ્રતિજ્ઞા(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      પ્રતિજ્ઞા(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_કિંમત tmp_કિંમત;
      tmp_કિંમત.format = vpiBinStrVal;
      sig_fil->get_કિંમત(&tmp_કિંમત);

      જો (memcmp(કિંમત_bits, tmp_કિંમત.value.str + કિંમત_off, pobj->width) == 0)
	    પાછા ખોટું;

      memcpy(કિંમત_bits, tmp_કિંમત.value.str + કિંમત_off, pobj->width);
      પાછા સાચું;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
સ્થિર કિંમત_callback*make_કિંમત_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      કિંમત_callback*cbh = નવું કિંમત_part_callback(data);
      પાછા cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
સ્થિર કિંમત_callback* make_કિંમત_change(p_cb_data data)
{
      જો (!check_callback_time(data, સાચું))
	    પાછા 0;

      જો (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            પાછા 0;
      }

	// Special case: the target object is a vpiPartSelect
      જો (data->obj->get_type_code() == vpiPartSelect) {
            જો (data->obj->vpi_handle(vpiArray))
	        પાછા vpip_array_word_change(data);
            નહીંતર
	        પાછા make_કિંમત_change_part(data);
      }

      જો (data->obj->get_type_code() == vpiMemoryWord)
	    પાછા vpip_array_word_change(data);

      જો (data->obj->get_type_code() == vpiMemory)
	    પાછા vpip_array_change(data);

      કિંમત_callback*obj = નવું કિંમત_callback(data);

      પ્રતિજ્ઞા(data->obj);
      પસંદગી (data->obj->get_type_code()) {

	  સ્થિતિ vpiReg:
	  સ્થિતિ vpiNet:
	  સ્થિતિ vpiIntegerVar:
	  સ્થિતિ vpiBitVar:
	  સ્થિતિ vpiByteVar:
	  સ્થિતિ vpiShortIntVar:
	  સ્થિતિ vpiIntVar:
	  સ્થિતિ vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    સંરચના __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ગાંઠ->fil);
	    પ્રતિજ્ઞા(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    તોડો;

	  સ્થિતિ vpiRealVar:
	    vpip_real_કિંમત_change(obj, data->obj);
	    તોડો;

	  સ્થિતિ vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    તોડો;

	  સ્થિતિ vpiModule:
	  સ્થિતિ vpiConstant:
	  સ્થિતિ vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    પાછા 0;
      }

      પાછા obj;
}

વર્ગ sync_callback : જાહેર __vpiCallback {
    જાહેર:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(અચલ sync_callback&) = delete;
      sync_callback& operator=(અચલ sync_callback&) = delete;

    જાહેર:
	// scheduled event
      સંરચના sync_cb* cb_sync;
	// user supplied callback data
      સંરચના t_vpi_time cb_time;

    ખાનગી:
};

અંતઃરેખ sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      પ્રતિજ્ઞા(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

શૂન્ય sync_cb::run_run()
{
      જો (handle == 0)
	    પાછા;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      જો (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    પ્રતિજ્ઞા(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

સ્થિર vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      પસંદગી (obj->cb_time.type) {
	  સ્થિતિ vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    તોડો;

	  સ્થિતિ vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (પૂર્ણાંક)obj->cb_time.type);
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }
      પાછા tv;
}

સ્થિર sync_callback* make_sync(p_cb_data data, તાર્કિક readonly_flag)
{
      જો (!check_callback_time(data, ખોટું))
	    પાછા 0;

      sync_callback*obj = નવું sync_callback(data);

      સંરચના sync_cb*cb = નવું sync_cb;
      cb->sync_flag = readonly_flag? સાચું : ખોટું;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, સાચું, readonly_flag);

      પાછા obj;
}

સ્થિર સંરચના __vpiCallback* make_afterdelay(p_cb_data data)
{
      જો (!check_callback_time(data, ખોટું))
	    પાછા 0;

      sync_callback*obj = નવું sync_callback(data);
      સંરચના sync_cb*cb = નવું sync_cb;
      cb->sync_flag = ખોટું;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, ખોટું);

      પાછા obj;
}

સ્થિર સંરચના __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      જો (!check_callback_time(data, ખોટું))
	    પાછા 0;

      sync_callback*obj = નવું sync_callback(data);
      સંરચના sync_cb*cb = નવું sync_cb;
      cb->sync_flag = ખોટું;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      જો (cur > tv) {
	    tv = 0;
	    પ્રતિજ્ઞા(0);
      } નહીંતર જો (cur == tv) {
	    tv = 0;
      } નહીંતર {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      પાછા obj;
}

સ્થિર સંરચના __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      જો (!check_callback_time(data, ખોટું))
	    પાછા 0;

      sync_callback*obj = નવું sync_callback(data);
      સંરચના sync_cb*cb = નવું sync_cb;
      cb->sync_flag = ખોટું;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      જો (cur > tv) {
	    tv = 0;
	    પ્રતિજ્ઞા(0);
      } નહીંતર જો (cur == tv) {
	    tv = 0;
      } નહીંતર {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      પાછા obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

વર્ગ simulator_callback : જાહેર __vpiCallback {
    જાહેર:
      explicit simulator_callback(અચલ સંરચના t_cb_data*data);

    જાહેર:
      સંરચના t_vpi_time cb_time;
};

અંતઃરેખ simulator_callback::simulator_callback(અચલ સંરચના t_cb_data*data)
{
      cb_data = *data;
      જો ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } નહીંતર જો (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } નહીંતર {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

સ્થિર simulator_callback*NextSimTime = 0;
સ્થિર simulator_callback*EndOfCompile = 0;
સ્થિર simulator_callback*StartOfSimulation = 0;
સ્થિર simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
શૂન્ય simulator_cb_delete(શૂન્ય)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      જ્યારે (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      જ્યારે (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      જ્યારે (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

શૂન્ય vpiEndOfCompile(શૂન્ય) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      પ્રતિજ્ઞા(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      જ્યારે (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    જો (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

શૂન્ય vpiStartOfSim(શૂન્ય) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      પ્રતિજ્ઞા(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      જ્યારે (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    જો (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

શૂન્ય vpiPostsim(શૂન્ય) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      પ્રતિજ્ઞા(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      જ્યારે (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    જો (cur->cb_data.cb_rtn != 0) {
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
શૂન્ય vpiNextSimTime(શૂન્ય)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      પ્રતિજ્ઞા(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      જ્યારે (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            જો (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

સ્થિર simulator_callback* make_prepost(p_cb_data data)
{
      જો ((data->reason == cbNextSimTime) && !check_callback_time(data, સાચું))
	    પાછા 0;

      simulator_callback*obj = નવું simulator_callback(data);

      /* Insert at head of list */
      પસંદગી (data->reason) {
	  સ્થિતિ cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    તોડો;
	  સ્થિતિ cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    તોડો;
	  સ્થિતિ cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    તોડો;
	  સ્થિતિ cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    તોડો;
      }

      પાછા obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      સંરચના __vpiCallback*obj = 0;

      પ્રતિજ્ઞા(data);
      પસંદગી (data->reason) {

	  સ્થિતિ cb_કિંમત_Change:
	    obj = make_કિંમત_change(data);
	    તોડો;

	  સ્થિતિ cbReadOnlySynch:
	    obj = make_sync(data, સાચું);
	    તોડો;

	  સ્થિતિ cbReadWriteSynch:
	    obj = make_sync(data, ખોટું);
	    તોડો;

	  સ્થિતિ cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    તોડો;

	  સ્થિતિ cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    તોડો;

	  સ્થિતિ cbAfterDelay:
	    obj = make_afterdelay(data);
	    તોડો;

	  સ્થિતિ cbEndOfCompile:
	  સ્થિતિ cbStartOfSimulation:
	  સ્થિતિ cbEndOfSimulation:
	  સ્થિતિ cbNextSimTime:
	    obj = make_prepost(data);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (પૂર્ણાંક)data->reason);
	    તોડો;
      }

      પાછા obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      સંરચના __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      પ્રતિજ્ઞા(obj);
      obj->cb_data.cb_rtn = 0;

      પાછા 1;
}

શૂન્ય callback_execute(સંરચના __vpiCallback*cur)
{
      અચલ vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      પ્રતિજ્ઞા(cur->cb_data.cb_rtn);
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
સંરચના __vpi_array_word {
      સંરચના __vpi_array_word* next;
      સંરચના __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_શબ્દો = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      પ્રતિજ્ઞા(vpi_callbacks_ == 0);
      પ્રતિજ્ઞા(array_શબ્દો == 0);
}

શૂન્ય vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      સંરચના __vpi_array_word*tmp = નવું __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_શબ્દો;
      array_શબ્દો = tmp;
}

શૂન્ય vvp_vpi_callback::add_vpi_callback(કિંમત_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_vpi_callback::clear_all_callbacks()
{
      જ્યારે (vpi_callbacks_) {
	    કિંમત_callback *tmp = dynamic_cast<કિંમત_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      જ્યારે (array_શબ્દો) {
	    સંરચના __vpi_array_word*tmp = array_શબ્દો->next;
	    delete array_શબ્દો;
	    array_શબ્દો = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
શૂન્ય vvp_vpi_callback::run_vpi_callbacks()
{
      સંરચના __vpi_array_word*array_word = array_શબ્દો;
      જ્યારે (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      કિંમત_callback *next = vpi_callbacks_;
      કિંમત_callback *prev = 0;

      જ્યારે (next) {
	    કિંમત_callback*cur = next;
	    next = dynamic_cast<કિંમત_callback*>(cur->next);

	    જો (cur->cb_data.cb_rtn != 0) {
		  જો (cur->test_કિંમત_callback_ready()) {
			જો (cur->cb_data.value)
			      get_કિંમત(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } નહીંતર જો (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } નહીંતર {
		  પ્રતિજ્ઞા(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

શૂન્ય vvp_signal_કિંમત::get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp)
{
      પસંદગી (vp->format) {
	  સ્થિતિ vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    તોડો;

	  સ્થિતિ vpiBinStrVal:
	  સ્થિતિ vpiOctStrVal:
	  સ્થિતિ vpiDecStrVal:
	  સ્થિતિ vpiHexStrVal:
	  સ્થિતિ vpiIntVal:
	  સ્થિતિ vpiVectorVal:
	  સ્થિતિ vpiStringVal:
	  સ્થિતિ vpiRealVal: {
	    unsigned wid = કિંમત_size();
	    vvp_vector4_t vec4(wid);
	    માટે (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_કિંમત(vec4, wid, ખોટું, vp);
	    તોડો;
	  }

	  સ્થિતિ vpiSuppressVal:
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (પૂર્ણાંક)vp->format);
      }
}

સ્થિર શૂન્ય real_signal_કિંમત(સંરચના t_vpi_કિંમત*vp, દ્વિગુણ rval)
{
      સ્થિર અચલ size_t RBUF_SIZE = 64 + 1;
      અક્ષર*rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      પસંદગી (vp->format) {
	  સ્થિતિ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  સ્થિતિ vpiRealVal:
	    vp->value.real = rval;
	    તોડો;

	  સ્થિતિ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    જો (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } નહીંતર {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    જો (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    નહીંતર
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		જ્યારે (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		માટે (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		જો (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		તોડો;
	  }

	  સ્થિતિ vpiSuppressVal:
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (પૂર્ણાંક)vp->format);
      }
}

શૂન્ય vvp_fun_signal_real_aa::get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp)
{
      real_signal_કિંમત(vp, real_કિંમત());
}

શૂન્ય vvp_wire_real::get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp)
{
      real_signal_કિંમત(vp, real_કિંમત());
}

શૂન્ય vvp_fun_signal_string_aa::get_signal_કિંમત(સંરચના t_vpi_કિંમત*)
{
      પ્રતિજ્ઞા(0);
}
#જો 0
શૂન્ય vvp_wire_string::get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp)
{
      પ્રતિજ્ઞા(0);
}
#endif
શૂન્ય vvp_wire_vec4::get_કિંમત(સંરચના t_vpi_કિંમત*val)
{
      get_signal_કિંમત(val);
}

શૂન્ય vvp_wire_vec8::get_કિંમત(સંરચના t_vpi_કિંમત*val)
{
      get_signal_કિંમત(val);
}

શૂન્ય vvp_wire_real::get_કિંમત(સંરચના t_vpi_કિંમત*val)
{
      get_signal_કિંમત(val);
}
#જો 0
શૂન્ય vvp_wire_string::get_કિંમત(સંરચના t_vpi_કિંમત*val)
{
      પ્રતિજ્ઞા(0);
}
#endif
