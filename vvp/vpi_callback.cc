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

স্থির ধ্রুবক অক্ষর*cb_reason_name(PLI_INT32 reason)
{
      নির্বাচন (reason) {
	  ক্ষেত্র cb_মান_Change:
	    ফেরত "cbValueChange";
	  ক্ষেত্র cbStmt:
	    ফেরত "cbStmt";
	  ক্ষেত্র cbForce:
	    ফেরত "cbForce";
	  ক্ষেত্র cbRelease:
	    ফেরত "cbRelease";
	  ক্ষেত্র cbAtStartOfSimTime:
	    ফেরত "cbAtStartOfSimTime";
	  ক্ষেত্র cbReadWriteSynch:
	    ফেরত "cbReadWriteSynch";
	  ক্ষেত্র cbReadOnlySynch:
	    ফেরত "cbReadOnlySynch";
	  ক্ষেত্র cb_পরবর্তী_Sim_Time:
	    ফেরত "cbNextSimTime";
	  ক্ষেত্র cbAfterDelay:
	    ফেরত "cbAfterDelay";
	  ক্ষেত্র cbEndOfCompile:
	    ফেরত "cbEndOfCompile";
	  ক্ষেত্র cbStartOfSimulation:
	    ফেরত "cbStartOfSimulation";
	  ক্ষেত্র cbEndOfSimulation:
	    ফেরত "cbEndOfSimulation";
	  ক্ষেত্র cbError:
	    ফেরত "cbError";
	  ক্ষেত্র cbTchkViolation:
	    ফেরত "cbTchkViolation";
	  ক্ষেত্র cbStartOfSave:
	    ফেরত "cbStartOfSave";
	  ক্ষেত্র cbEndOfSave:
	    ফেরত "cbEndOfSave";
	  ক্ষেত্র cbStartOfRestart:
	    ফেরত "cbStartOfRestart";
	  ক্ষেত্র cbEndOfRestart:
	    ফেরত "cbEndOfRestart";
	  ক্ষেত্র cbStartOfReset:
	    ফেরত "cbStartOfReset";
	  ক্ষেত্র cbEndOfReset:
	    ফেরত "cbEndOfReset";
	  ক্ষেত্র cbEnterInteractive:
	    ফেরত "cbEnterInteractive";
	  ক্ষেত্র cbExitInteractive:
	    ফেরত "cbExitInteractive";
	  ক্ষেত্র cbInteractiveScopeChange:
	    ফেরত "cbInteractiveScopeChange";
	  ক্ষেত্র cbUnresolvedSystf:
	    ফেরত "cbUnresolvedSystf";
	  ক্ষেত্র cbAtEndOfSimTime:
	    ফেরত "cbAtEndOfSimTime";
	  পূর্বনির্ধারিত:
	    ফেরত "unrecognised";
      }
}

স্থির যৌক্তিক check_callback_time(p_cb_তথ্য তথ্য, যৌক্তিক allow_suppress)
{
      প্রতিজ্ঞা(তথ্য);
      যদি (!তথ্য->time) {
	    যদি (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(তথ্য->reason));
		  ফেরত মিথ্যা;
	    }
	    ফেরত সত্য;
      }
      নির্বাচন (তথ্য->time->type) {
	  ক্ষেত্র vpiSimTime:
	    বিরতি;
	  ক্ষেত্র vpiScaledRealTime:
	    বিরতি;
	  ক্ষেত্র vpiSuppressTime:
	    যদি (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(তথ্য->reason));
		  ফেরত মিথ্যা;
	    }
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(তথ্য->reason));
	    ফেরত মিথ্যা;
      }
      ফেরত সত্য;
}

স্থির শূন্য set_callback_time(p_cb_তথ্য তথ্য)
{
      প্রতিজ্ঞা(তথ্য && তথ্য->time);
      তথ্য->time->নিচের  = 0;
      তথ্য->time->উপরের = 0;
      তথ্য->time->real = 0.0;
      নির্বাচন (তথ্য->time->type) {
	  ক্ষেত্র vpiSimTime:
	    vpip_time_to_timestruct(তথ্য->time, schedule_simtime());
	    বিরতি;
	  ক্ষেত্র vpiScaledRealTime:
	    তথ্য->time->real = vpip_scaled_time_from_handle(schedule_simtime(), তথ্য->obj);
	    বিরতি;
	  ক্ষেত্র vpiSuppressTime:
	    বিরতি;
	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
	    বিরতি;
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

শ্রেণী sync_callback;

কাঠামো sync_cb  : সর্বজনীন vvp_gen_event_s {
      sync_callback*handle;
      যৌক্তিক sync_flag;

      ~sync_cb () override { }

      আভাসী শূন্য run_run() override;
};

অন্তঃরেখ __vpiCallback::__vpiCallback()
{
      পরবর্তী_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

পূর্ণসংখ্যা __vpiCallback::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiCallback; }


মান_callback::মান_callback(p_cb_তথ্য তথ্য)
{
      cb_তথ্য = *তথ্য;
      যদি (তথ্য->time) {
	    cb_time = *(তথ্য->time);
      } অন্যথা {
	    cb_time.type = vpiSuppressTime;
      }
      cb_তথ্য.time = &cb_time;
      যদি (তথ্য->value) {
	    cb_মান = *(তথ্য->value);
      } অন্যথা {
	    cb_মান.format = vpiSuppressVal;
      }
      cb_তথ্য.value = &cb_মান;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
যৌক্তিক মান_callback::test_মান_callback_ready(শূন্য)
{
      ফেরত সত্য;
}

স্থির শূন্য vpip_real_মান_change(মান_callback*cbh, vpiHandle ref)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      প্রতিজ্ঞা(obj);

      obj->add_vpi_callback(cbh);
}

শ্রেণী মান_part_callback : সর্বজনীন মান_callback {
    সর্বজনীন:
      explicit মান_part_callback(p_cb_তথ্য তথ্য);
      ~মান_part_callback() override;

      মান_part_callback(ধ্রুবক মান_part_callback&) = delete;
      মান_part_callback& operator=(ধ্রুবক মান_part_callback&) = delete;

      যৌক্তিক test_মান_callback_ready(শূন্য) override;

    ব্যক্তিগত:
      অক্ষর*মান_bits;
      size_t মান_off;
};

অন্তঃরেখ মান_part_callback::মান_part_callback(p_cb_তথ্য তথ্য)
: মান_callback(তথ্য)
{
      কাঠামো __vpiPV*pobj = dynamic_cast<__vpiPV*>(তথ্য->obj);
      প্রতিজ্ঞা(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      প্রতিজ্ঞা(sig_fil);

      sig_fil->add_vpi_callback(এই);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      sig_fil->get_মান(&tmp_মান);

      মান_bits = নতুন অক্ষর[pobj->width+1];
      মান_off = pobj->parent->vpi_get(vpi_আকার) - pobj->width - pobj->tbase;

      memcpy(মান_bits, tmp_মান.value.str + মান_off, pobj->width);
      মান_bits[pobj->width] = 0;
}

মান_part_callback::~মান_part_callback()
{
      delete[]মান_bits;
}

যৌক্তিক মান_part_callback::test_মান_callback_ready(শূন্য)
{
      কাঠামো __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_তথ্য.obj);
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
	    ফেরত মিথ্যা;

      memcpy(মান_bits, tmp_মান.value.str + মান_off, pobj->width);
      ফেরত সত্য;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
স্থির মান_callback*make_মান_change_part(p_cb_তথ্য তথ্য)
{
	/* Attach the __vpiCallback object to the signal. */
      মান_callback*cbh = নতুন মান_part_callback(তথ্য);
      ফেরত cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
স্থির মান_callback* make_মান_change(p_cb_তথ্য তথ্য)
{
      যদি (!check_callback_time(তথ্য, সত্য))
	    ফেরত 0;

      যদি (vpi_get(vpiAutomatic, তথ্য->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, তথ্য->obj));
            ফেরত 0;
      }

	// Special case: the target object is a vpiPartSelect
      যদি (তথ্য->obj->get_type_code() == vpiPartSelect) {
            যদি (তথ্য->obj->vpi_handle(vpiArray))
	        ফেরত vpip_array_word_change(তথ্য);
            অন্যথা
	        ফেরত make_মান_change_part(তথ্য);
      }

      যদি (তথ্য->obj->get_type_code() == vpiMemoryWord)
	    ফেরত vpip_array_word_change(তথ্য);

      যদি (তথ্য->obj->get_type_code() == vpiMemory)
	    ফেরত vpip_array_change(তথ্য);

      মান_callback*obj = নতুন মান_callback(তথ্য);

      প্রতিজ্ঞা(তথ্য->obj);
      নির্বাচন (তথ্য->obj->get_type_code()) {

	  ক্ষেত্র vpiReg:
	  ক্ষেত্র vpiNet:
	  ক্ষেত্র vpiIntegerVar:
	  ক্ষেত্র vpiBitVar:
	  ক্ষেত্র vpiByteVar:
	  ক্ষেত্র vpiShortIntVar:
	  ক্ষেত্র vpiIntVar:
	  ক্ষেত্র vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    কাঠামো __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(তথ্য->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->গ্রন্থি->fil);
	    প্রতিজ্ঞা(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    বিরতি;

	  ক্ষেত্র vpiRealVar:
	    vpip_real_মান_change(obj, তথ্য->obj);
	    বিরতি;

	  ক্ষেত্র vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(তথ্য->obj);
	    nev->add_vpi_callback(obj);
	    বিরতি;

	  ক্ষেত্র vpiModule:
	  ক্ষেত্র vpiConstant:
	  ক্ষেত্র vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    তথ্য->obj->get_type_code());
	    delete obj;
	    ফেরত 0;
      }

      ফেরত obj;
}

শ্রেণী sync_callback : সর্বজনীন __vpiCallback {
    সর্বজনীন:
      explicit sync_callback(p_cb_তথ্য তথ্য);
      ~sync_callback() override;

      sync_callback(ধ্রুবক sync_callback&) = delete;
      sync_callback& operator=(ধ্রুবক sync_callback&) = delete;

    সর্বজনীন:
	// scheduled event
      কাঠামো sync_cb* cb_sync;
	// user supplied callback data
      কাঠামো t_vpi_time cb_time;

    ব্যক্তিগত:
};

অন্তঃরেখ sync_callback::sync_callback(p_cb_তথ্য তথ্য)
{
      cb_sync = 0;

      cb_তথ্য = *তথ্য;
      প্রতিজ্ঞা(তথ্য->time);
      cb_time = *(তথ্য->time);
      cb_তথ্য.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

শূন্য sync_cb::run_run()
{
      যদি (handle == 0)
	    ফেরত;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      যদি (cur->cb_তথ্য.cb_rtn != 0) {
	    set_callback_time(&cur->cb_তথ্য);
	    প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_তথ্য.cb_rtn)(&cur->cb_তথ্য);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

স্থির vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      নির্বাচন (obj->cb_time.type) {
	  ক্ষেত্র vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    বিরতি;

	  ক্ষেত্র vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_তথ্য.obj));
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (পূর্ণসংখ্যা)obj->cb_time.type);
	    প্রতিজ্ঞা(0);
	    বিরতি;
      }
      ফেরত tv;
}

স্থির sync_callback* make_sync(p_cb_তথ্য তথ্য, যৌক্তিক readonly_flag)
{
      যদি (!check_callback_time(তথ্য, মিথ্যা))
	    ফেরত 0;

      sync_callback*obj = নতুন sync_callback(তথ্য);

      কাঠামো sync_cb*cb = নতুন sync_cb;
      cb->sync_flag = readonly_flag? সত্য : মিথ্যা;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, সত্য, readonly_flag);

      ফেরত obj;
}

স্থির কাঠামো __vpiCallback* make_afterdelay(p_cb_তথ্য তথ্য)
{
      যদি (!check_callback_time(তথ্য, মিথ্যা))
	    ফেরত 0;

      sync_callback*obj = নতুন sync_callback(তথ্য);
      কাঠামো sync_cb*cb = নতুন sync_cb;
      cb->sync_flag = মিথ্যা;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, মিথ্যা);

      ফেরত obj;
}

স্থির কাঠামো __vpiCallback* make_at_start_of_sim_time(p_cb_তথ্য তথ্য)
{
      যদি (!check_callback_time(তথ্য, মিথ্যা))
	    ফেরত 0;

      sync_callback*obj = নতুন sync_callback(তথ্য);
      কাঠামো sync_cb*cb = নতুন sync_cb;
      cb->sync_flag = মিথ্যা;
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

      ফেরত obj;
}

স্থির কাঠামো __vpiCallback* make_at_end_of_sim_time(p_cb_তথ্য তথ্য)
{
      যদি (!check_callback_time(তথ্য, মিথ্যা))
	    ফেরত 0;

      sync_callback*obj = নতুন sync_callback(তথ্য);
      কাঠামো sync_cb*cb = নতুন sync_cb;
      cb->sync_flag = মিথ্যা;
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

      ফেরত obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

শ্রেণী simulator_callback : সর্বজনীন __vpiCallback {
    সর্বজনীন:
      explicit simulator_callback(ধ্রুবক কাঠামো t_cb_তথ্য*তথ্য);

    সর্বজনীন:
      কাঠামো t_vpi_time cb_time;
};

অন্তঃরেখ simulator_callback::simulator_callback(ধ্রুবক কাঠামো t_cb_তথ্য*তথ্য)
{
      cb_তথ্য = *তথ্য;
      যদি ((তথ্য->reason == cb_পরবর্তী_Sim_Time) && তথ্য->time) {
	    cb_time = *(তথ্য->time);
      } অন্যথা যদি (তথ্য->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } অন্যথা {
	    cb_time.type = vpiSuppressTime;
      }
      cb_তথ্য.time = &cb_time;
}

স্থির simulator_callback*পরবর্তী_Sim_Time = 0;
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
      যখন (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      যখন (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      যখন (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
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

      যখন (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
	    যদি (cur->cb_তথ্য.cb_rtn != 0) {
		  (cur->cb_তথ্য.cb_rtn)(&cur->cb_তথ্য);
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

      যখন (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
	    যদি (cur->cb_তথ্য.cb_rtn != 0) {
		  (cur->cb_তথ্য.cb_rtn)(&cur->cb_তথ্য);
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

      যখন (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
	    যদি (cur->cb_তথ্য.cb_rtn != 0) {
		  set_callback_time(&cur->cb_তথ্য);
		  (cur->cb_তথ্য.cb_rtn)(&cur->cb_তথ্য);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
শূন্য vpi_পরবর্তী_Sim_Time(শূন্য)
{
      simulator_callback* cur;
      simulator_callback* পরবর্তী_2 = পরবর্তী_Sim_Time;
      পরবর্তী_Sim_Time = 0;

      প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      যখন (পরবর্তী_2) {
            cur = পরবর্তী_2;
            পরবর্তী_2 = dynamic_cast<simulator_callback*>(cur->পরবর্তী_2);
            যদি (cur->cb_তথ্য.cb_rtn != 0) {
                  set_callback_time(&cur->cb_তথ্য);
                  (cur->cb_তথ্য.cb_rtn)(&cur->cb_তথ্য);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

স্থির simulator_callback* make_prepost(p_cb_তথ্য তথ্য)
{
      যদি ((তথ্য->reason == cb_পরবর্তী_Sim_Time) && !check_callback_time(তথ্য, সত্য))
	    ফেরত 0;

      simulator_callback*obj = নতুন simulator_callback(তথ্য);

      /* Insert at head of list */
      নির্বাচন (তথ্য->reason) {
	  ক্ষেত্র cbEndOfCompile:
	    obj->পরবর্তী_2 = EndOfCompile;
	    EndOfCompile = obj;
	    বিরতি;
	  ক্ষেত্র cbStartOfSimulation:
	    obj->পরবর্তী_2 = StartOfSimulation;
	    StartOfSimulation = obj;
	    বিরতি;
	  ক্ষেত্র cbEndOfSimulation:
	    obj->পরবর্তী_2 = EndOfSimulation;
	    EndOfSimulation = obj;
	    বিরতি;
	  ক্ষেত্র cb_পরবর্তী_Sim_Time:
	    obj->পরবর্তী_2 = পরবর্তী_Sim_Time;
	    পরবর্তী_Sim_Time = obj;
	    বিরতি;
      }

      ফেরত obj;
}

vpiHandle vpi_register_cb(p_cb_তথ্য তথ্য)
{
      কাঠামো __vpiCallback*obj = 0;

      প্রতিজ্ঞা(তথ্য);
      নির্বাচন (তথ্য->reason) {

	  ক্ষেত্র cb_মান_Change:
	    obj = make_মান_change(তথ্য);
	    বিরতি;

	  ক্ষেত্র cbReadOnlySynch:
	    obj = make_sync(তথ্য, সত্য);
	    বিরতি;

	  ক্ষেত্র cbReadWriteSynch:
	    obj = make_sync(তথ্য, মিথ্যা);
	    বিরতি;

	  ক্ষেত্র cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(তথ্য);
	    বিরতি;

	  ক্ষেত্র cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(তথ্য);
	    বিরতি;

	  ক্ষেত্র cbAfterDelay:
	    obj = make_afterdelay(তথ্য);
	    বিরতি;

	  ক্ষেত্র cbEndOfCompile:
	  ক্ষেত্র cbStartOfSimulation:
	  ক্ষেত্র cbEndOfSimulation:
	  ক্ষেত্র cb_পরবর্তী_Sim_Time:
	    obj = make_prepost(তথ্য);
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (পূর্ণসংখ্যা)তথ্য->reason);
	    বিরতি;
      }

      ফেরত obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      কাঠামো __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      প্রতিজ্ঞা(obj);
      obj->cb_তথ্য.cb_rtn = 0;

      ফেরত 1;
}

শূন্য callback_execute(কাঠামো __vpiCallback*cur)
{
      ধ্রুবক vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      প্রতিজ্ঞা(cur->cb_তথ্য.cb_rtn);
      set_callback_time(&cur->cb_তথ্য);
      (cur->cb_তথ্য.cb_rtn)(&cur->cb_তথ্য);

      vpi_mode_flag = save_mode;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
কাঠামো __vpi_array_word {
      কাঠামো __vpi_array_word* পরবর্তী_2;
      কাঠামো __vpiArray* array;
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
      কাঠামো __vpi_array_word*tmp = নতুন __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->পরবর্তী_2 = array_শব্দগুলি;
      array_শব্দগুলি = tmp;
}

শূন্য vvp_vpi_callback::add_vpi_callback(মান_callback*cb)
{
      cb->পরবর্তী_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_vpi_callback::clear_all_callbacks()
{
      যখন (vpi_callbacks_) {
	    মান_callback *tmp = dynamic_cast<মান_callback*>
	                            (vpi_callbacks_->পরবর্তী_2);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      যখন (array_শব্দগুলি) {
	    কাঠামো __vpi_array_word*tmp = array_শব্দগুলি->পরবর্তী_2;
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
      কাঠামো __vpi_array_word*array_word = array_শব্দগুলি;
      যখন (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->পরবর্তী_2;
      }

      মান_callback *পরবর্তী_2 = vpi_callbacks_;
      মান_callback *prev = 0;

      যখন (পরবর্তী_2) {
	    মান_callback*cur = পরবর্তী_2;
	    পরবর্তী_2 = dynamic_cast<মান_callback*>(cur->পরবর্তী_2);

	    যদি (cur->cb_তথ্য.cb_rtn != 0) {
		  যদি (cur->test_মান_callback_ready()) {
			যদি (cur->cb_তথ্য.value)
			      get_মান(cur->cb_তথ্য.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } অন্যথা যদি (prev == 0) {

		  vpi_callbacks_ = পরবর্তী_2;
		  cur->পরবর্তী_2 = 0;
		  delete cur;

	    } অন্যথা {
		  প্রতিজ্ঞা(prev->পরবর্তী_2 == cur);
		  prev->পরবর্তী_2 = পরবর্তী_2;
		  cur->পরবর্তী_2 = 0;
		  delete cur;
	    }
      }
}

শূন্য vvp_signal_মান::get_signal_মান(কাঠামো t_vpi_মান*vp)
{
      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    বিরতি;

	  ক্ষেত্র vpiBinStrVal:
	  ক্ষেত্র vpiOctStrVal:
	  ক্ষেত্র vpiDecStrVal:
	  ক্ষেত্র vpiHexStrVal:
	  ক্ষেত্র vpiIntVal:
	  ক্ষেত্র vpiVectorVal:
	  ক্ষেত্র vpiStringVal:
	  ক্ষেত্র vpiRealVal: {
	    unsigned wid = মান_আকার();
	    vvp_vector4_t vec4(wid);
	    জন্য (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_মান(vec4, wid, মিথ্যা, vp);
	    বিরতি;
	  }

	  ক্ষেত্র vpiSuppressVal:
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (পূর্ণসংখ্যা)vp->format);
      }
}

স্থির শূন্য real_signal_মান(কাঠামো t_vpi_মান*vp, দ্বিগুণ rval)
{
      স্থির ধ্রুবক size_t RBUF_আকার = 64 + 1;
      অক্ষর*rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_আকার, RBUF_VAL));

      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ক্ষেত্র vpiRealVal:
	    vp->value.real = rval;
	    বিরতি;

	  ক্ষেত্র vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    যদি (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } অন্যথা {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    বিরতি;

	  ক্ষেত্র vpiDecStrVal:
	    যদি (std::isnan(rval))
		  snprintf(rbuf, RBUF_আকার, "%s", "nan");
	    অন্যথা
		  snprintf(rbuf, RBUF_আকার, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiHexStrVal:
	    snprintf(rbuf, RBUF_আকার, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		যখন (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		জন্য (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		যদি (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		বিরতি;
	  }

	  ক্ষেত্র vpiSuppressVal:
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (পূর্ণসংখ্যা)vp->format);
      }
}

শূন্য vvp_fun_signal_real_aa::get_signal_মান(কাঠামো t_vpi_মান*vp)
{
      real_signal_মান(vp, real_মান());
}

শূন্য vvp_wire_real::get_signal_মান(কাঠামো t_vpi_মান*vp)
{
      real_signal_মান(vp, real_মান());
}

শূন্য vvp_fun_signal_string_aa::get_signal_মান(কাঠামো t_vpi_মান*)
{
      প্রতিজ্ঞা(0);
}
#যদি 0
শূন্য vvp_wire_string::get_signal_মান(কাঠামো t_vpi_মান*vp)
{
      প্রতিজ্ঞা(0);
}
#endif
শূন্য vvp_wire_vec4::get_মান(কাঠামো t_vpi_মান*val)
{
      get_signal_মান(val);
}

শূন্য vvp_wire_vec8::get_মান(কাঠামো t_vpi_মান*val)
{
      get_signal_মান(val);
}

শূন্য vvp_wire_real::get_মান(কাঠামো t_vpi_মান*val)
{
      get_signal_মান(val);
}
#যদি 0
শূন্য vvp_wire_string::get_মান(কাঠামো t_vpi_মান*val)
{
      প্রতিজ্ঞা(0);
}
#endif
