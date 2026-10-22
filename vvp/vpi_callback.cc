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
#ଅନ୍ତର୍ଭାଵ  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using ନାମାକାଶ std;

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର*cb_reason_name(PLI_INT32 reason)
{
      ବାଛ (reason) {
	  ସ୍ଥିତି cb_ମୂଲ୍ୟ_Change:
	    ଫେରନ୍ତୁ "cbValueChange";
	  ସ୍ଥିତି cbStmt:
	    ଫେରନ୍ତୁ "cbStmt";
	  ସ୍ଥିତି cbForce:
	    ଫେରନ୍ତୁ "cbForce";
	  ସ୍ଥିତି cbRelease:
	    ଫେରନ୍ତୁ "cbRelease";
	  ସ୍ଥିତି cbAtStartOfSimTime:
	    ଫେରନ୍ତୁ "cbAtStartOfSimTime";
	  ସ୍ଥିତି cbReadWriteSynch:
	    ଫେରନ୍ତୁ "cbReadWriteSynch";
	  ସ୍ଥିତି cbReadOnlySynch:
	    ଫେରନ୍ତୁ "cbReadOnlySynch";
	  ସ୍ଥିତି cbNextSimTime:
	    ଫେରନ୍ତୁ "cbNextSimTime";
	  ସ୍ଥିତି cbAfterDelay:
	    ଫେରନ୍ତୁ "cbAfterDelay";
	  ସ୍ଥିତି cbEndOfCompile:
	    ଫେରନ୍ତୁ "cbEndOfCompile";
	  ସ୍ଥିତି cbStartOfSimulation:
	    ଫେରନ୍ତୁ "cbStartOfSimulation";
	  ସ୍ଥିତି cbEndOfSimulation:
	    ଫେରନ୍ତୁ "cbEndOfSimulation";
	  ସ୍ଥିତି cbError:
	    ଫେରନ୍ତୁ "cbError";
	  ସ୍ଥିତି cbTchkViolation:
	    ଫେରନ୍ତୁ "cbTchkViolation";
	  ସ୍ଥିତି cbStartOfSave:
	    ଫେରନ୍ତୁ "cbStartOfSave";
	  ସ୍ଥିତି cbEndOfSave:
	    ଫେରନ୍ତୁ "cbEndOfSave";
	  ସ୍ଥିତି cbStartOfRestart:
	    ଫେରନ୍ତୁ "cbStartOfRestart";
	  ସ୍ଥିତି cbEndOfRestart:
	    ଫେରନ୍ତୁ "cbEndOfRestart";
	  ସ୍ଥିତି cbStartOfReset:
	    ଫେରନ୍ତୁ "cbStartOfReset";
	  ସ୍ଥିତି cbEndOfReset:
	    ଫେରନ୍ତୁ "cbEndOfReset";
	  ସ୍ଥିତି cbEnterInteractive:
	    ଫେରନ୍ତୁ "cbEnterInteractive";
	  ସ୍ଥିତି cbExitInteractive:
	    ଫେରନ୍ତୁ "cbExitInteractive";
	  ସ୍ଥିତି cbInteractiveScopeChange:
	    ଫେରନ୍ତୁ "cbInteractiveScopeChange";
	  ସ୍ଥିତି cbUnresolvedSystf:
	    ଫେରନ୍ତୁ "cbUnresolvedSystf";
	  ସ୍ଥିତି cbAtEndOfSimTime:
	    ଫେରନ୍ତୁ "cbAtEndOfSimTime";
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ "unrecognised";
      }
}

ସ୍ଥିର ତାର୍କିକ check_callback_time(p_cb_data data, ତାର୍କିକ allow_suppress)
{
      ପ୍ରତିଜ୍ଞା(data);
      ଯଦି (!data->time) {
	    ଯଦି (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    }
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ବାଛ (data->time->type) {
	  ସ୍ଥିତି vpiSimTime:
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiScaledRealTime:
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiSuppressTime:
	    ଯଦି (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    }
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ set_callback_time(p_cb_data data)
{
      ପ୍ରତିଜ୍ଞା(data && data->time);
      data->time->ତଳ  = 0;
      data->time->ଉପର = 0;
      data->time->real = 0.0;
      ବାଛ (data->time->type) {
	  ସ୍ଥିତି vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiSuppressTime:
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
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

ଶ୍ରେଣୀ sync_callback;

ସଂରଚନା sync_cb  : ସର୍ବସାଧାରଣ vvp_gen_event_s {
      sync_callback*handle;
      ତାର୍କିକ sync_flag;

      ~sync_cb () override { }

      ଆଭାସୀ ଶୂନ୍ୟ run_run() override;
};

ଧାଡ଼ିଭିତରେ __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiCallback::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiCallback; }


ମୂଲ୍ୟ_callback::ମୂଲ୍ୟ_callback(p_cb_data data)
{
      cb_data = *data;
      ଯଦି (data->time) {
	    cb_time = *(data->time);
      } ଅନ୍ୟଥା {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      ଯଦି (data->value) {
	    cb_ମୂଲ୍ୟ = *(data->value);
      } ଅନ୍ୟଥା {
	    cb_ମୂଲ୍ୟ.format = vpiSuppressVal;
      }
      cb_data.value = &cb_ମୂଲ୍ୟ;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
ତାର୍କିକ ମୂଲ୍ୟ_callback::test_ମୂଲ୍ୟ_callback_ready(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ vpip_real_ମୂଲ୍ୟ_change(ମୂଲ୍ୟ_callback*cbh, vpiHandle ref)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      ପ୍ରତିଜ୍ଞା(obj);

      obj->add_vpi_callback(cbh);
}

ଶ୍ରେଣୀ ମୂଲ୍ୟ_part_callback : ସର୍ବସାଧାରଣ ମୂଲ୍ୟ_callback {
    ସର୍ବସାଧାରଣ:
      explicit ମୂଲ୍ୟ_part_callback(p_cb_data data);
      ~ମୂଲ୍ୟ_part_callback() override;

      ମୂଲ୍ୟ_part_callback(ଅଚଳ ମୂଲ୍ୟ_part_callback&) = delete;
      ମୂଲ୍ୟ_part_callback& operator=(ଅଚଳ ମୂଲ୍ୟ_part_callback&) = delete;

      ତାର୍କିକ test_ମୂଲ୍ୟ_callback_ready(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      ଅକ୍ଷର*ମୂଲ୍ୟ_bits;
      size_t ମୂଲ୍ୟ_off;
};

ଧାଡ଼ିଭିତରେ ମୂଲ୍ୟ_part_callback::ମୂଲ୍ୟ_part_callback(p_cb_data data)
: ମୂଲ୍ୟ_callback(data)
{
      ସଂରଚନା __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      ପ୍ରତିଜ୍ଞା(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ପ୍ରତିଜ୍ଞା(sig_fil);

      sig_fil->add_vpi_callback(ଏହି);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_ମୂଲ୍ୟ tmp_ମୂଲ୍ୟ;
      tmp_ମୂଲ୍ୟ.format = vpiBinStrVal;
      sig_fil->get_ମୂଲ୍ୟ(&tmp_ମୂଲ୍ୟ);

      ମୂଲ୍ୟ_bits = ନୂତନ ଅକ୍ଷର[pobj->width+1];
      ମୂଲ୍ୟ_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(ମୂଲ୍ୟ_bits, tmp_ମୂଲ୍ୟ.value.str + ମୂଲ୍ୟ_off, pobj->width);
      ମୂଲ୍ୟ_bits[pobj->width] = 0;
}

ମୂଲ୍ୟ_part_callback::~ମୂଲ୍ୟ_part_callback()
{
      delete[]ମୂଲ୍ୟ_bits;
}

ତାର୍କିକ ମୂଲ୍ୟ_part_callback::test_ମୂଲ୍ୟ_callback_ready(ଶୂନ୍ୟ)
{
      ସଂରଚନା __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      ପ୍ରତିଜ୍ଞା(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ପ୍ରତିଜ୍ଞା(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ମୂଲ୍ୟ tmp_ମୂଲ୍ୟ;
      tmp_ମୂଲ୍ୟ.format = vpiBinStrVal;
      sig_fil->get_ମୂଲ୍ୟ(&tmp_ମୂଲ୍ୟ);

      ଯଦି (memcmp(ମୂଲ୍ୟ_bits, tmp_ମୂଲ୍ୟ.value.str + ମୂଲ୍ୟ_off, pobj->width) == 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      memcpy(ମୂଲ୍ୟ_bits, tmp_ମୂଲ୍ୟ.value.str + ମୂଲ୍ୟ_off, pobj->width);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ସ୍ଥିର ମୂଲ୍ୟ_callback*make_ମୂଲ୍ୟ_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      ମୂଲ୍ୟ_callback*cbh = ନୂତନ ମୂଲ୍ୟ_part_callback(data);
      ଫେରନ୍ତୁ cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ସ୍ଥିର ମୂଲ୍ୟ_callback* make_ମୂଲ୍ୟ_change(p_cb_data data)
{
      ଯଦି (!check_callback_time(data, ସତ୍ୟ))
	    ଫେରନ୍ତୁ 0;

      ଯଦି (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            ଫେରନ୍ତୁ 0;
      }

	// Special case: the target object is a vpiPartSelect
      ଯଦି (data->obj->get_type_code() == vpiPartSelect) {
            ଯଦି (data->obj->vpi_handle(vpiArray))
	        ଫେରନ୍ତୁ vpip_array_word_change(data);
            ଅନ୍ୟଥା
	        ଫେରନ୍ତୁ make_ମୂଲ୍ୟ_change_part(data);
      }

      ଯଦି (data->obj->get_type_code() == vpiMemoryWord)
	    ଫେରନ୍ତୁ vpip_array_word_change(data);

      ଯଦି (data->obj->get_type_code() == vpiMemory)
	    ଫେରନ୍ତୁ vpip_array_change(data);

      ମୂଲ୍ୟ_callback*obj = ନୂତନ ମୂଲ୍ୟ_callback(data);

      ପ୍ରତିଜ୍ଞା(data->obj);
      ବାଛ (data->obj->get_type_code()) {

	  ସ୍ଥିତି vpiReg:
	  ସ୍ଥିତି vpiNet:
	  ସ୍ଥିତି vpiIntegerVar:
	  ସ୍ଥିତି vpiBitVar:
	  ସ୍ଥିତି vpiByteVar:
	  ସ୍ଥିତି vpiShortIntVar:
	  ସ୍ଥିତି vpiIntVar:
	  ସ୍ଥିତି vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    ସଂରଚନା __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ଗଣ୍ଠି->fil);
	    ପ୍ରତିଜ୍ଞା(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiRealVar:
	    vpip_real_ମୂଲ୍ୟ_change(obj, data->obj);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiModule:
	  ସ୍ଥିତି vpiConstant:
	  ସ୍ଥିତି vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    ଫେରନ୍ତୁ 0;
      }

      ଫେରନ୍ତୁ obj;
}

ଶ୍ରେଣୀ sync_callback : ସର୍ବସାଧାରଣ __vpiCallback {
    ସର୍ବସାଧାରଣ:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(ଅଚଳ sync_callback&) = delete;
      sync_callback& operator=(ଅଚଳ sync_callback&) = delete;

    ସର୍ବସାଧାରଣ:
	// scheduled event
      ସଂରଚନା sync_cb* cb_sync;
	// user supplied callback data
      ସଂରଚନା t_vpi_time cb_time;

    ବ୍ୟକ୍ତିଗତ:
};

ଧାଡ଼ିଭିତରେ sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      ପ୍ରତିଜ୍ଞା(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

ଶୂନ୍ୟ sync_cb::run_run()
{
      ଯଦି (handle == 0)
	    ଫେରନ୍ତୁ;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      ଯଦି (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    ପ୍ରତିଜ୍ଞା(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

ସ୍ଥିର vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      ବାଛ (obj->cb_time.type) {
	  ସ୍ଥିତି vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)obj->cb_time.type);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }
      ଫେରନ୍ତୁ tv;
}

ସ୍ଥିର sync_callback* make_sync(p_cb_data data, ତାର୍କିକ readonly_flag)
{
      ଯଦି (!check_callback_time(data, ମିଥ୍ୟା))
	    ଫେରନ୍ତୁ 0;

      sync_callback*obj = ନୂତନ sync_callback(data);

      ସଂରଚନା sync_cb*cb = ନୂତନ sync_cb;
      cb->sync_flag = readonly_flag? ସତ୍ୟ : ମିଥ୍ୟା;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, ସତ୍ୟ, readonly_flag);

      ଫେରନ୍ତୁ obj;
}

ସ୍ଥିର ସଂରଚନା __vpiCallback* make_afterdelay(p_cb_data data)
{
      ଯଦି (!check_callback_time(data, ମିଥ୍ୟା))
	    ଫେରନ୍ତୁ 0;

      sync_callback*obj = ନୂତନ sync_callback(data);
      ସଂରଚନା sync_cb*cb = ନୂତନ sync_cb;
      cb->sync_flag = ମିଥ୍ୟା;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, ମିଥ୍ୟା);

      ଫେରନ୍ତୁ obj;
}

ସ୍ଥିର ସଂରଚନା __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      ଯଦି (!check_callback_time(data, ମିଥ୍ୟା))
	    ଫେରନ୍ତୁ 0;

      sync_callback*obj = ନୂତନ sync_callback(data);
      ସଂରଚନା sync_cb*cb = ନୂତନ sync_cb;
      cb->sync_flag = ମିଥ୍ୟା;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      ଯଦି (cur > tv) {
	    tv = 0;
	    ପ୍ରତିଜ୍ଞା(0);
      } ଅନ୍ୟଥା ଯଦି (cur == tv) {
	    tv = 0;
      } ଅନ୍ୟଥା {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      ଫେରନ୍ତୁ obj;
}

ସ୍ଥିର ସଂରଚନା __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      ଯଦି (!check_callback_time(data, ମିଥ୍ୟା))
	    ଫେରନ୍ତୁ 0;

      sync_callback*obj = ନୂତନ sync_callback(data);
      ସଂରଚନା sync_cb*cb = ନୂତନ sync_cb;
      cb->sync_flag = ମିଥ୍ୟା;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      ଯଦି (cur > tv) {
	    tv = 0;
	    ପ୍ରତିଜ୍ଞା(0);
      } ଅନ୍ୟଥା ଯଦି (cur == tv) {
	    tv = 0;
      } ଅନ୍ୟଥା {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      ଫେରନ୍ତୁ obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

ଶ୍ରେଣୀ simulator_callback : ସର୍ବସାଧାରଣ __vpiCallback {
    ସର୍ବସାଧାରଣ:
      explicit simulator_callback(ଅଚଳ ସଂରଚନା t_cb_data*data);

    ସର୍ବସାଧାରଣ:
      ସଂରଚନା t_vpi_time cb_time;
};

ଧାଡ଼ିଭିତରେ simulator_callback::simulator_callback(ଅଚଳ ସଂରଚନା t_cb_data*data)
{
      cb_data = *data;
      ଯଦି ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } ଅନ୍ୟଥା ଯଦି (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } ଅନ୍ୟଥା {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

ସ୍ଥିର simulator_callback*NextSimTime = 0;
ସ୍ଥିର simulator_callback*EndOfCompile = 0;
ସ୍ଥିର simulator_callback*StartOfSimulation = 0;
ସ୍ଥିର simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
ଶୂନ୍ୟ simulator_cb_delete(ଶୂନ୍ୟ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      ଯେତେବେଳେ (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      ଯେତେବେଳେ (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      ଯେତେବେଳେ (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

ଶୂନ୍ୟ vpiEndOfCompile(ଶୂନ୍ୟ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ପ୍ରତିଜ୍ଞା(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ଯେତେବେଳେ (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    ଯଦି (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ଶୂନ୍ୟ vpiStartOfSim(ଶୂନ୍ୟ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ପ୍ରତିଜ୍ଞା(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ଯେତେବେଳେ (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    ଯଦି (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ଶୂନ୍ୟ vpiPostsim(ଶୂନ୍ୟ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      ପ୍ରତିଜ୍ଞା(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      ଯେତେବେଳେ (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    ଯଦି (cur->cb_data.cb_rtn != 0) {
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
ଶୂନ୍ୟ vpiNextSimTime(ଶୂନ୍ୟ)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      ପ୍ରତିଜ୍ଞା(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ଯେତେବେଳେ (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            ଯଦି (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ସ୍ଥିର simulator_callback* make_prepost(p_cb_data data)
{
      ଯଦି ((data->reason == cbNextSimTime) && !check_callback_time(data, ସତ୍ୟ))
	    ଫେରନ୍ତୁ 0;

      simulator_callback*obj = ନୂତନ simulator_callback(data);

      /* Insert at head of list */
      ବାଛ (data->reason) {
	  ସ୍ଥିତି cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      ସଂରଚନା __vpiCallback*obj = 0;

      ପ୍ରତିଜ୍ଞା(data);
      ବାଛ (data->reason) {

	  ସ୍ଥିତି cb_ମୂଲ୍ୟ_Change:
	    obj = make_ମୂଲ୍ୟ_change(data);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି cbReadOnlySynch:
	    obj = make_sync(data, ସତ୍ୟ);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି cbReadWriteSynch:
	    obj = make_sync(data, ମିଥ୍ୟା);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି cbAfterDelay:
	    obj = make_afterdelay(data);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି cbEndOfCompile:
	  ସ୍ଥିତି cbStartOfSimulation:
	  ସ୍ଥିତି cbEndOfSimulation:
	  ସ୍ଥିତି cbNextSimTime:
	    obj = make_prepost(data);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)data->reason);
	    ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      ସଂରଚନା __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      ପ୍ରତିଜ୍ଞା(obj);
      obj->cb_data.cb_rtn = 0;

      ଫେରନ୍ତୁ 1;
}

ଶୂନ୍ୟ callback_execute(ସଂରଚନା __vpiCallback*cur)
{
      ଅଚଳ vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ପ୍ରତିଜ୍ଞା(cur->cb_data.cb_rtn);
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
ସଂରଚନା __vpi_array_word {
      ସଂରଚନା __vpi_array_word* next;
      ସଂରଚନା __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_ଶବ୍ଦଗୁଡ଼ିକ = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      ପ୍ରତିଜ୍ଞା(vpi_callbacks_ == 0);
      ପ୍ରତିଜ୍ଞା(array_ଶବ୍ଦଗୁଡ଼ିକ == 0);
}

ଶୂନ୍ୟ vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      ସଂରଚନା __vpi_array_word*tmp = ନୂତନ __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_ଶବ୍ଦଗୁଡ଼ିକ;
      array_ଶବ୍ଦଗୁଡ଼ିକ = tmp;
}

ଶୂନ୍ୟ vvp_vpi_callback::add_vpi_callback(ମୂଲ୍ୟ_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_vpi_callback::clear_all_callbacks()
{
      ଯେତେବେଳେ (vpi_callbacks_) {
	    ମୂଲ୍ୟ_callback *tmp = dynamic_cast<ମୂଲ୍ୟ_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      ଯେତେବେଳେ (array_ଶବ୍ଦଗୁଡ଼ିକ) {
	    ସଂରଚନା __vpi_array_word*tmp = array_ଶବ୍ଦଗୁଡ଼ିକ->next;
	    delete array_ଶବ୍ଦଗୁଡ଼ିକ;
	    array_ଶବ୍ଦଗୁଡ଼ିକ = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
ଶୂନ୍ୟ vvp_vpi_callback::run_vpi_callbacks()
{
      ସଂରଚନା __vpi_array_word*array_word = array_ଶବ୍ଦଗୁଡ଼ିକ;
      ଯେତେବେଳେ (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      ମୂଲ୍ୟ_callback *next = vpi_callbacks_;
      ମୂଲ୍ୟ_callback *prev = 0;

      ଯେତେବେଳେ (next) {
	    ମୂଲ୍ୟ_callback*cur = next;
	    next = dynamic_cast<ମୂଲ୍ୟ_callback*>(cur->next);

	    ଯଦି (cur->cb_data.cb_rtn != 0) {
		  ଯଦି (cur->test_ମୂଲ୍ୟ_callback_ready()) {
			ଯଦି (cur->cb_data.value)
			      get_ମୂଲ୍ୟ(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } ଅନ୍ୟଥା ଯଦି (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } ଅନ୍ୟଥା {
		  ପ୍ରତିଜ୍ଞା(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

ଶୂନ୍ୟ vvp_signal_ମୂଲ୍ୟ::get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp)
{
      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal:
	  ସ୍ଥିତି vpiOctStrVal:
	  ସ୍ଥିତି vpiDecStrVal:
	  ସ୍ଥିତି vpiHexStrVal:
	  ସ୍ଥିତି vpiIntVal:
	  ସ୍ଥିତି vpiVectorVal:
	  ସ୍ଥିତି vpiStringVal:
	  ସ୍ଥିତି vpiRealVal: {
	    unsigned wid = ମୂଲ୍ୟ_size();
	    vvp_vector4_t vec4(wid);
	    ପାଇଁ (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_ମୂଲ୍ୟ(vec4, wid, ମିଥ୍ୟା, vp);
	    ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiSuppressVal:
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
      }
}

ସ୍ଥିର ଶୂନ୍ୟ real_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp, ଦ୍ୱିଗୁଣ rval)
{
      ସ୍ଥିର ଅଚଳ size_t RBUF_SIZE = 64 + 1;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ସ୍ଥିତି vpiRealVal:
	    vp->value.real = rval;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ଯଦି (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } ଅନ୍ୟଥା {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    ଯଦି (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    ଅନ୍ୟଥା
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		ଯେତେବେଳେ (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		ପାଇଁ (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		ଯଦି (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiSuppressVal:
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
      }
}

ଶୂନ୍ୟ vvp_fun_signal_real_aa::get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp)
{
      real_signal_ମୂଲ୍ୟ(vp, real_ମୂଲ୍ୟ());
}

ଶୂନ୍ୟ vvp_wire_real::get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp)
{
      real_signal_ମୂଲ୍ୟ(vp, real_ମୂଲ୍ୟ());
}

ଶୂନ୍ୟ vvp_fun_signal_string_aa::get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}
#ଯଦି 0
ଶୂନ୍ୟ vvp_wire_string::get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp)
{
      ପ୍ରତିଜ୍ଞା(0);
}
#endif
ଶୂନ୍ୟ vvp_wire_vec4::get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*val)
{
      get_signal_ମୂଲ୍ୟ(val);
}

ଶୂନ୍ୟ vvp_wire_vec8::get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*val)
{
      get_signal_ମୂଲ୍ୟ(val);
}

ଶୂନ୍ୟ vvp_wire_real::get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*val)
{
      get_signal_ମୂଲ୍ୟ(val);
}
#ଯଦି 0
ଶୂନ୍ୟ vvp_wire_string::get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*val)
{
      ପ୍ରତିଜ୍ଞା(0);
}
#endif
