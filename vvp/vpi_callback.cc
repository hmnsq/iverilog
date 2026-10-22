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
#অন্তর্ভাব  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using নামাকাশ std;

স্থির ধ্রুব বর্ণ*cb_reason_name(PLI_INT32 reason)
{
      বিকল্প (reason) {
	  পক্ষ cb_মান_Change:
	    প্রত্যাগম "cbValueChange";
	  পক্ষ cbStmt:
	    প্রত্যাগম "cbStmt";
	  পক্ষ cbForce:
	    প্রত্যাগম "cbForce";
	  পক্ষ cbRelease:
	    প্রত্যাগম "cbRelease";
	  পক্ষ cbAtStartOfSimTime:
	    প্রত্যাগম "cbAtStartOfSimTime";
	  পক্ষ cbReadWriteSynch:
	    প্রত্যাগম "cbReadWriteSynch";
	  পক্ষ cbReadOnlySynch:
	    প্রত্যাগম "cbReadOnlySynch";
	  পক্ষ cbNextSimTime:
	    প্রত্যাগম "cbNextSimTime";
	  পক্ষ cbAfterDelay:
	    প্রত্যাগম "cbAfterDelay";
	  পক্ষ cbEndOfCompile:
	    প্রত্যাগম "cbEndOfCompile";
	  পক্ষ cbStartOfSimulation:
	    প্রত্যাগম "cbStartOfSimulation";
	  পক্ষ cbEndOfSimulation:
	    প্রত্যাগম "cbEndOfSimulation";
	  পক্ষ cbError:
	    প্রত্যাগম "cbError";
	  পক্ষ cbTchkViolation:
	    প্রত্যাগম "cbTchkViolation";
	  পক্ষ cbStartOfSave:
	    প্রত্যাগম "cbStartOfSave";
	  পক্ষ cbEndOfSave:
	    প্রত্যাগম "cbEndOfSave";
	  পক্ষ cbStartOfRestart:
	    প্রত্যাগম "cbStartOfRestart";
	  পক্ষ cbEndOfRestart:
	    প্রত্যাগম "cbEndOfRestart";
	  পক্ষ cbStartOfReset:
	    প্রত্যাগম "cbStartOfReset";
	  পক্ষ cbEndOfReset:
	    প্রত্যাগম "cbEndOfReset";
	  পক্ষ cbEnterInteractive:
	    প্রত্যাগম "cbEnterInteractive";
	  পক্ষ cbExitInteractive:
	    প্রত্যাগম "cbExitInteractive";
	  পক্ষ cbInteractiveScopeChange:
	    প্রত্যাগম "cbInteractiveScopeChange";
	  পক্ষ cbUnresolvedSystf:
	    প্রত্যাগম "cbUnresolvedSystf";
	  পক্ষ cbAtEndOfSimTime:
	    প্রত্যাগম "cbAtEndOfSimTime";
	  স্বভাব:
	    প্রত্যাগম "unrecognised";
      }
}

স্থির তার্কিক check_callback_time(p_cb_data data, তার্কিক allow_suppress)
{
      প্রতিজ্ঞা(data);
      যদি (!data->time) {
	    যদি (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  প্রত্যাগম অসত্য;
	    }
	    প্রত্যাগম সত্য;
      }
      বিকল্প (data->time->type) {
	  পক্ষ vpiSimTime:
	    ভঙ্গ;
	  পক্ষ vpiScaledRealTime:
	    ভঙ্গ;
	  পক্ষ vpiSuppressTime:
	    যদি (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  প্রত্যাগম অসত্য;
	    }
	    ভঙ্গ;
	  স্বভাব:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    প্রত্যাগম অসত্য;
      }
      প্রত্যাগম সত্য;
}

স্থির শূন্য set_callback_time(p_cb_data data)
{
      প্রতিজ্ঞা(data && data->time);
      data->time->নিচের  = 0;
      data->time->উপরের = 0;
      data->time->real = 0.0;
      বিকল্প (data->time->type) {
	  পক্ষ vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    ভঙ্গ;
	  পক্ষ vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    ভঙ্গ;
	  পক্ষ vpiSuppressTime:
	    ভঙ্গ;
	  স্বভাব:
	    প্রতিজ্ঞা(0);
	    ভঙ্গ;
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

বর্গ sync_callback;

সংরচনা sync_cb  : সার্বজনিক vvp_gen_event_s {
      sync_callback*handle;
      তার্কিক sync_flag;

      ~sync_cb () override { }

      আভাস শূন্য run_run() override;
};

অন্তঃস্থ __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

পূর্ণাঙ্ক __vpiCallback::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiCallback; }


মান_callback::মান_callback(p_cb_data data)
{
      cb_data = *data;
      যদি (data->time) {
	    cb_time = *(data->time);
      } অন্যথা {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      যদি (data->value) {
	    cb_মান = *(data->value);
      } অন্যথা {
	    cb_মান.format = vpiSuppressVal;
      }
      cb_data.value = &cb_মান;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
তার্কিক মান_callback::test_মান_callback_ready(শূন্য)
{
      প্রত্যাগম সত্য;
}

স্থির শূন্য vpip_real_মান_change(মান_callback*cbh, vpiHandle ref)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      প্রতিজ্ঞা(obj);

      obj->add_vpi_callback(cbh);
}

বর্গ মান_part_callback : সার্বজনিক মান_callback {
    সার্বজনিক:
      explicit মান_part_callback(p_cb_data data);
      ~মান_part_callback() override;

      মান_part_callback(ধ্রুব মান_part_callback&) = delete;
      মান_part_callback& operator=(ধ্রুব মান_part_callback&) = delete;

      তার্কিক test_মান_callback_ready(শূন্য) override;

    গুপ্ত:
      বর্ণ*মান_bits;
      size_t মান_off;
};

অন্তঃস্থ মান_part_callback::মান_part_callback(p_cb_data data)
: মান_callback(data)
{
      সংরচনা __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      প্রতিজ্ঞা(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      প্রতিজ্ঞা(sig_fil);

      sig_fil->add_vpi_callback(এতত্);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      sig_fil->get_মান(&tmp_মান);

      মান_bits = নূতন বর্ণ[pobj->width+1];
      মান_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(মান_bits, tmp_মান.value.str + মান_off, pobj->width);
      মান_bits[pobj->width] = 0;
}

মান_part_callback::~মান_part_callback()
{
      delete[]মান_bits;
}

তার্কিক মান_part_callback::test_মান_callback_ready(শূন্য)
{
      সংরচনা __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      প্রতিজ্ঞা(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      প্রতিজ্ঞা(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      sig_fil->get_মান(&tmp_মান);

      যদি (memcmp(মান_bits, tmp_মান.value.str + মান_off, pobj->width) == 0)
	    প্রত্যাগম অসত্য;

      memcpy(মান_bits, tmp_মান.value.str + মান_off, pobj->width);
      প্রত্যাগম সত্য;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
স্থির মান_callback*make_মান_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      মান_callback*cbh = নূতন মান_part_callback(data);
      প্রত্যাগম cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
স্থির মান_callback* make_মান_change(p_cb_data data)
{
      যদি (!check_callback_time(data, সত্য))
	    প্রত্যাগম 0;

      যদি (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            প্রত্যাগম 0;
      }

	// Special case: the target object is a vpiPartSelect
      যদি (data->obj->get_type_code() == vpiPartSelect) {
            যদি (data->obj->vpi_handle(vpiArray))
	        প্রত্যাগম vpip_array_word_change(data);
            অন্যথা
	        প্রত্যাগম make_মান_change_part(data);
      }

      যদি (data->obj->get_type_code() == vpiMemoryWord)
	    প্রত্যাগম vpip_array_word_change(data);

      যদি (data->obj->get_type_code() == vpiMemory)
	    প্রত্যাগম vpip_array_change(data);

      মান_callback*obj = নূতন মান_callback(data);

      প্রতিজ্ঞা(data->obj);
      বিকল্প (data->obj->get_type_code()) {

	  পক্ষ vpiReg:
	  পক্ষ vpiNet:
	  পক্ষ vpiIntegerVar:
	  পক্ষ vpiBitVar:
	  পক্ষ vpiByteVar:
	  পক্ষ vpiShortIntVar:
	  পক্ষ vpiIntVar:
	  পক্ষ vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    সংরচনা __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->গ্রন্থি->fil);
	    প্রতিজ্ঞা(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    ভঙ্গ;

	  পক্ষ vpiRealVar:
	    vpip_real_মান_change(obj, data->obj);
	    ভঙ্গ;

	  পক্ষ vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    ভঙ্গ;

	  পক্ষ vpiModule:
	  পক্ষ vpiConstant:
	  পক্ষ vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    প্রত্যাগম 0;
      }

      প্রত্যাগম obj;
}

বর্গ sync_callback : সার্বজনিক __vpiCallback {
    সার্বজনিক:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(ধ্রুব sync_callback&) = delete;
      sync_callback& operator=(ধ্রুব sync_callback&) = delete;

    সার্বজনিক:
	// scheduled event
      সংরচনা sync_cb* cb_sync;
	// user supplied callback data
      সংরচনা t_vpi_time cb_time;

    গুপ্ত:
};

অন্তঃস্থ sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      প্রতিজ্ঞা(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

শূন্য sync_cb::run_run()
{
      যদি (handle == 0)
	    প্রত্যাগম;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      যদি (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

স্থির vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      বিকল্প (obj->cb_time.type) {
	  পক্ষ vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    ভঙ্গ;

	  পক্ষ vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (পূর্ণাঙ্ক)obj->cb_time.type);
	    প্রতিজ্ঞা(0);
	    ভঙ্গ;
      }
      প্রত্যাগম tv;
}

স্থির sync_callback* make_sync(p_cb_data data, তার্কিক readonly_flag)
{
      যদি (!check_callback_time(data, অসত্য))
	    প্রত্যাগম 0;

      sync_callback*obj = নূতন sync_callback(data);

      সংরচনা sync_cb*cb = নূতন sync_cb;
      cb->sync_flag = readonly_flag? সত্য : অসত্য;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, সত্য, readonly_flag);

      প্রত্যাগম obj;
}

স্থির সংরচনা __vpiCallback* make_afterdelay(p_cb_data data)
{
      যদি (!check_callback_time(data, অসত্য))
	    প্রত্যাগম 0;

      sync_callback*obj = নূতন sync_callback(data);
      সংরচনা sync_cb*cb = নূতন sync_cb;
      cb->sync_flag = অসত্য;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, অসত্য);

      প্রত্যাগম obj;
}

স্থির সংরচনা __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      যদি (!check_callback_time(data, অসত্য))
	    প্রত্যাগম 0;

      sync_callback*obj = নূতন sync_callback(data);
      সংরচনা sync_cb*cb = নূতন sync_cb;
      cb->sync_flag = অসত্য;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      যদি (cur > tv) {
	    tv = 0;
	    প্রতিজ্ঞা(0);
      } অন্যথা যদি (cur == tv) {
	    tv = 0;
      } অন্যথা {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      প্রত্যাগম obj;
}

স্থির সংরচনা __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      যদি (!check_callback_time(data, অসত্য))
	    প্রত্যাগম 0;

      sync_callback*obj = নূতন sync_callback(data);
      সংরচনা sync_cb*cb = নূতন sync_cb;
      cb->sync_flag = অসত্য;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      যদি (cur > tv) {
	    tv = 0;
	    প্রতিজ্ঞা(0);
      } অন্যথা যদি (cur == tv) {
	    tv = 0;
      } অন্যথা {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      প্রত্যাগম obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

বর্গ simulator_callback : সার্বজনিক __vpiCallback {
    সার্বজনিক:
      explicit simulator_callback(ধ্রুব সংরচনা t_cb_data*data);

    সার্বজনিক:
      সংরচনা t_vpi_time cb_time;
};

অন্তঃস্থ simulator_callback::simulator_callback(ধ্রুব সংরচনা t_cb_data*data)
{
      cb_data = *data;
      যদি ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } অন্যথা যদি (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } অন্যথা {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

স্থির simulator_callback*NextSimTime = 0;
স্থির simulator_callback*EndOfCompile = 0;
স্থির simulator_callback*StartOfSimulation = 0;
স্থির simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
শূন্য simulator_cb_delete(শূন্য)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      যাবত্ (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      যাবত্ (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      যাবত্ (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

শূন্য vpiEndOfCompile(শূন্য) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      যাবত্ (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    যদি (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

শূন্য vpiStartOfSim(শূন্য) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      যাবত্ (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    যদি (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

শূন্য vpiPostsim(শূন্য) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      যাবত্ (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    যদি (cur->cb_data.cb_rtn != 0) {
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
শূন্য vpiNextSimTime(শূন্য)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      যাবত্ (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            যদি (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

স্থির simulator_callback* make_prepost(p_cb_data data)
{
      যদি ((data->reason == cbNextSimTime) && !check_callback_time(data, সত্য))
	    প্রত্যাগম 0;

      simulator_callback*obj = নূতন simulator_callback(data);

      /* Insert at head of list */
      বিকল্প (data->reason) {
	  পক্ষ cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    ভঙ্গ;
	  পক্ষ cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    ভঙ্গ;
	  পক্ষ cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    ভঙ্গ;
	  পক্ষ cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    ভঙ্গ;
      }

      প্রত্যাগম obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      সংরচনা __vpiCallback*obj = 0;

      প্রতিজ্ঞা(data);
      বিকল্প (data->reason) {

	  পক্ষ cb_মান_Change:
	    obj = make_মান_change(data);
	    ভঙ্গ;

	  পক্ষ cbReadOnlySynch:
	    obj = make_sync(data, সত্য);
	    ভঙ্গ;

	  পক্ষ cbReadWriteSynch:
	    obj = make_sync(data, অসত্য);
	    ভঙ্গ;

	  পক্ষ cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    ভঙ্গ;

	  পক্ষ cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    ভঙ্গ;

	  পক্ষ cbAfterDelay:
	    obj = make_afterdelay(data);
	    ভঙ্গ;

	  পক্ষ cbEndOfCompile:
	  পক্ষ cbStartOfSimulation:
	  পক্ষ cbEndOfSimulation:
	  পক্ষ cbNextSimTime:
	    obj = make_prepost(data);
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (পূর্ণাঙ্ক)data->reason);
	    ভঙ্গ;
      }

      প্রত্যাগম obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      সংরচনা __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      প্রতিজ্ঞা(obj);
      obj->cb_data.cb_rtn = 0;

      প্রত্যাগম 1;
}

শূন্য callback_execute(সংরচনা __vpiCallback*cur)
{
      ধ্রুব vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      প্রতিজ্ঞা(cur->cb_data.cb_rtn);
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
সংরচনা __vpi_array_word {
      সংরচনা __vpi_array_word* next;
      সংরচনা __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_শব্দগুলি = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      প্রতিজ্ঞা(vpi_callbacks_ == 0);
      প্রতিজ্ঞা(array_শব্দগুলি == 0);
}

শূন্য vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      সংরচনা __vpi_array_word*tmp = নূতন __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_শব্দগুলি;
      array_শব্দগুলি = tmp;
}

শূন্য vvp_vpi_callback::add_vpi_callback(মান_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_vpi_callback::clear_all_callbacks()
{
      যাবত্ (vpi_callbacks_) {
	    মান_callback *tmp = dynamic_cast<মান_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      যাবত্ (array_শব্দগুলি) {
	    সংরচনা __vpi_array_word*tmp = array_শব্দগুলি->next;
	    delete array_শব্দগুলি;
	    array_শব্দগুলি = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
শূন্য vvp_vpi_callback::run_vpi_callbacks()
{
      সংরচনা __vpi_array_word*array_word = array_শব্দগুলি;
      যাবত্ (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      মান_callback *next = vpi_callbacks_;
      মান_callback *prev = 0;

      যাবত্ (next) {
	    মান_callback*cur = next;
	    next = dynamic_cast<মান_callback*>(cur->next);

	    যদি (cur->cb_data.cb_rtn != 0) {
		  যদি (cur->test_মান_callback_ready()) {
			যদি (cur->cb_data.value)
			      get_মান(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } অন্যথা যদি (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } অন্যথা {
		  প্রতিজ্ঞা(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

শূন্য vvp_signal_মান::get_signal_মান(সংরচনা t_vpi_মান*vp)
{
      বিকল্প (vp->format) {
	  পক্ষ vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal:
	  পক্ষ vpiOctStrVal:
	  পক্ষ vpiDecStrVal:
	  পক্ষ vpiHexStrVal:
	  পক্ষ vpiIntVal:
	  পক্ষ vpiVectorVal:
	  পক্ষ vpiStringVal:
	  পক্ষ vpiRealVal: {
	    unsigned wid = মান_size();
	    vvp_vector4_t vec4(wid);
	    প্রতি (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_মান(vec4, wid, অসত্য, vp);
	    ভঙ্গ;
	  }

	  পক্ষ vpiSuppressVal:
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (পূর্ণাঙ্ক)vp->format);
      }
}

স্থির শূন্য real_signal_মান(সংরচনা t_vpi_মান*vp, দ্বিগুণ rval)
{
      স্থির ধ্রুব size_t RBUF_SIZE = 64 + 1;
      বর্ণ*rbuf = static_cast<বর্ণ *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      বিকল্প (vp->format) {
	  পক্ষ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  পক্ষ vpiRealVal:
	    vp->value.real = rval;
	    ভঙ্গ;

	  পক্ষ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    যদি (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } অন্যথা {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    যদি (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    অন্যথা
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		যাবত্ (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		প্রতি (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		যদি (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ভঙ্গ;
	  }

	  পক্ষ vpiSuppressVal:
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (পূর্ণাঙ্ক)vp->format);
      }
}

শূন্য vvp_fun_signal_real_aa::get_signal_মান(সংরচনা t_vpi_মান*vp)
{
      real_signal_মান(vp, real_মান());
}

শূন্য vvp_wire_real::get_signal_মান(সংরচনা t_vpi_মান*vp)
{
      real_signal_মান(vp, real_মান());
}

শূন্য vvp_fun_signal_string_aa::get_signal_মান(সংরচনা t_vpi_মান*)
{
      প্রতিজ্ঞা(0);
}
#যদি 0
শূন্য vvp_wire_string::get_signal_মান(সংরচনা t_vpi_মান*vp)
{
      প্রতিজ্ঞা(0);
}
#endif
শূন্য vvp_wire_vec4::get_মান(সংরচনা t_vpi_মান*val)
{
      get_signal_মান(val);
}

শূন্য vvp_wire_vec8::get_মান(সংরচনা t_vpi_মান*val)
{
      get_signal_মান(val);
}

শূন্য vvp_wire_real::get_মান(সংরচনা t_vpi_মান*val)
{
      get_signal_মান(val);
}
#যদি 0
শূন্য vvp_wire_string::get_মান(সংরচনা t_vpi_মান*val)
{
      প্রতিজ্ঞা(0);
}
#endif
