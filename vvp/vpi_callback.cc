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
#ಅನ್ತರ್ಭಾವ  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using ನಾಮಾಕಾಶ std;

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ*cb_reason_name(PLI_INT32 reason)
{
      ವಿಕಲ್ಪ (reason) {
	  ಪಕ್ಷ cb_ಮೌಲ್ಯ_Change:
	    ಪ್ರತ್ಯಾಗಮ "cbValueChange";
	  ಪಕ್ಷ cbStmt:
	    ಪ್ರತ್ಯಾಗಮ "cbStmt";
	  ಪಕ್ಷ cbForce:
	    ಪ್ರತ್ಯಾಗಮ "cbForce";
	  ಪಕ್ಷ cbRelease:
	    ಪ್ರತ್ಯಾಗಮ "cbRelease";
	  ಪಕ್ಷ cbAtStartOfSimTime:
	    ಪ್ರತ್ಯಾಗಮ "cbAtStartOfSimTime";
	  ಪಕ್ಷ cbReadWriteSynch:
	    ಪ್ರತ್ಯಾಗಮ "cbReadWriteSynch";
	  ಪಕ್ಷ cbReadOnlySynch:
	    ಪ್ರತ್ಯಾಗಮ "cbReadOnlySynch";
	  ಪಕ್ಷ cbNextSimTime:
	    ಪ್ರತ್ಯಾಗಮ "cbNextSimTime";
	  ಪಕ್ಷ cbAfterDelay:
	    ಪ್ರತ್ಯಾಗಮ "cbAfterDelay";
	  ಪಕ್ಷ cbEndOfCompile:
	    ಪ್ರತ್ಯಾಗಮ "cbEndOfCompile";
	  ಪಕ್ಷ cbStartOfSimulation:
	    ಪ್ರತ್ಯಾಗಮ "cbStartOfSimulation";
	  ಪಕ್ಷ cbEndOfSimulation:
	    ಪ್ರತ್ಯಾಗಮ "cbEndOfSimulation";
	  ಪಕ್ಷ cbError:
	    ಪ್ರತ್ಯಾಗಮ "cbError";
	  ಪಕ್ಷ cbTchkViolation:
	    ಪ್ರತ್ಯಾಗಮ "cbTchkViolation";
	  ಪಕ್ಷ cbStartOfSave:
	    ಪ್ರತ್ಯಾಗಮ "cbStartOfSave";
	  ಪಕ್ಷ cbEndOfSave:
	    ಪ್ರತ್ಯಾಗಮ "cbEndOfSave";
	  ಪಕ್ಷ cbStartOfRestart:
	    ಪ್ರತ್ಯಾಗಮ "cbStartOfRestart";
	  ಪಕ್ಷ cbEndOfRestart:
	    ಪ್ರತ್ಯಾಗಮ "cbEndOfRestart";
	  ಪಕ್ಷ cbStartOfReset:
	    ಪ್ರತ್ಯಾಗಮ "cbStartOfReset";
	  ಪಕ್ಷ cbEndOfReset:
	    ಪ್ರತ್ಯಾಗಮ "cbEndOfReset";
	  ಪಕ್ಷ cbEnterInteractive:
	    ಪ್ರತ್ಯಾಗಮ "cbEnterInteractive";
	  ಪಕ್ಷ cbExitInteractive:
	    ಪ್ರತ್ಯಾಗಮ "cbExitInteractive";
	  ಪಕ್ಷ cbInteractiveScopeChange:
	    ಪ್ರತ್ಯಾಗಮ "cbInteractiveScopeChange";
	  ಪಕ್ಷ cbUnresolvedSystf:
	    ಪ್ರತ್ಯಾಗಮ "cbUnresolvedSystf";
	  ಪಕ್ಷ cbAtEndOfSimTime:
	    ಪ್ರತ್ಯಾಗಮ "cbAtEndOfSimTime";
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ "unrecognised";
      }
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ check_callback_time(p_cb_data data, ತಾರ್ಕಿಕ allow_suppress)
{
      ಪ್ರತಿಜ್ಞಾ(data);
      ಯದಿ (!data->time) {
	    ಯದಿ (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    }
	    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }
      ವಿಕಲ್ಪ (data->time->type) {
	  ಪಕ್ಷ vpiSimTime:
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiScaledRealTime:
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiSuppressTime:
	    ಯದಿ (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    }
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ set_callback_time(p_cb_data data)
{
      ಪ್ರತಿಜ್ಞಾ(data && data->time);
      data->time->ಕೆಳಗಿನ  = 0;
      data->time->ಮೇಲಿನ = 0;
      data->time->real = 0.0;
      ವಿಕಲ್ಪ (data->time->type) {
	  ಪಕ್ಷ vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiSuppressTime:
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
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

ವರ್ಗ sync_callback;

ಸಂರಚನಾ sync_cb  : ಸಾರ್ವಜನಿಕ vvp_gen_event_s {
      sync_callback*handle;
      ತಾರ್ಕಿಕ sync_flag;

      ~sync_cb () override { }

      ಆಭಾಸ ಶೂನ್ಯ run_run() override;
};

ಅನ್ತಃಸ್ಥ __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

ಪೂರ್ಣಾಙ್ಕ __vpiCallback::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiCallback; }


ಮೌಲ್ಯ_callback::ಮೌಲ್ಯ_callback(p_cb_data data)
{
      cb_data = *data;
      ಯದಿ (data->time) {
	    cb_time = *(data->time);
      } ಅನ್ಯಥಾ {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      ಯದಿ (data->value) {
	    cb_ಮೌಲ್ಯ = *(data->value);
      } ಅನ್ಯಥಾ {
	    cb_ಮೌಲ್ಯ.format = vpiSuppressVal;
      }
      cb_data.value = &cb_ಮೌಲ್ಯ;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
ತಾರ್ಕಿಕ ಮೌಲ್ಯ_callback::test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ)
{
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ vpip_real_ಮೌಲ್ಯ_change(ಮೌಲ್ಯ_callback*cbh, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(obj);

      obj->add_vpi_callback(cbh);
}

ವರ್ಗ ಮೌಲ್ಯ_part_callback : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_callback {
    ಸಾರ್ವಜನಿಕ:
      explicit ಮೌಲ್ಯ_part_callback(p_cb_data data);
      ~ಮೌಲ್ಯ_part_callback() override;

      ಮೌಲ್ಯ_part_callback(ಧ್ರುವ ಮೌಲ್ಯ_part_callback&) = delete;
      ಮೌಲ್ಯ_part_callback& operator=(ಧ್ರುವ ಮೌಲ್ಯ_part_callback&) = delete;

      ತಾರ್ಕಿಕ test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      ವರ್ಣ*ಮೌಲ್ಯ_bits;
      size_t ಮೌಲ್ಯ_off;
};

ಅನ್ತಃಸ್ಥ ಮೌಲ್ಯ_part_callback::ಮೌಲ್ಯ_part_callback(p_cb_data data)
: ಮೌಲ್ಯ_callback(data)
{
      ಸಂರಚನಾ __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      ಪ್ರತಿಜ್ಞಾ(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig_fil);

      sig_fil->add_vpi_callback(ಏತತ್);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      sig_fil->get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಮೌಲ್ಯ_bits = ನೂತನ ವರ್ಣ[pobj->width+1];
      ಮೌಲ್ಯ_off = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(ಮೌಲ್ಯ_bits, tmp_ಮೌಲ್ಯ.value.str + ಮೌಲ್ಯ_off, pobj->width);
      ಮೌಲ್ಯ_bits[pobj->width] = 0;
}

ಮೌಲ್ಯ_part_callback::~ಮೌಲ್ಯ_part_callback()
{
      delete[]ಮೌಲ್ಯ_bits;
}

ತಾರ್ಕಿಕ ಮೌಲ್ಯ_part_callback::test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ)
{
      ಸಂರಚನಾ __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      ಪ್ರತಿಜ್ಞಾ(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      sig_fil->get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಯದಿ (memcmp(ಮೌಲ್ಯ_bits, tmp_ಮೌಲ್ಯ.value.str + ಮೌಲ್ಯ_off, pobj->width) == 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      memcpy(ಮೌಲ್ಯ_bits, tmp_ಮೌಲ್ಯ.value.str + ಮೌಲ್ಯ_off, pobj->width);
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ಸ್ಥಿರ ಮೌಲ್ಯ_callback*make_ಮೌಲ್ಯ_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      ಮೌಲ್ಯ_callback*cbh = ನೂತನ ಮೌಲ್ಯ_part_callback(data);
      ಪ್ರತ್ಯಾಗಮ cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ಸ್ಥಿರ ಮೌಲ್ಯ_callback* make_ಮೌಲ್ಯ_change(p_cb_data data)
{
      ಯದಿ (!check_callback_time(data, ಸತ್ಯ))
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಯದಿ (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            ಪ್ರತ್ಯಾಗಮ 0;
      }

	// Special case: the target object is a vpiPartSelect
      ಯದಿ (data->obj->get_type_code() == vpiPartSelect) {
            ಯದಿ (data->obj->vpi_handle(vpiArray))
	        ಪ್ರತ್ಯಾಗಮ vpip_array_word_change(data);
            ಅನ್ಯಥಾ
	        ಪ್ರತ್ಯಾಗಮ make_ಮೌಲ್ಯ_change_part(data);
      }

      ಯದಿ (data->obj->get_type_code() == vpiMemoryWord)
	    ಪ್ರತ್ಯಾಗಮ vpip_array_word_change(data);

      ಯದಿ (data->obj->get_type_code() == vpiMemory)
	    ಪ್ರತ್ಯಾಗಮ vpip_array_change(data);

      ಮೌಲ್ಯ_callback*obj = ನೂತನ ಮೌಲ್ಯ_callback(data);

      ಪ್ರತಿಜ್ಞಾ(data->obj);
      ವಿಕಲ್ಪ (data->obj->get_type_code()) {

	  ಪಕ್ಷ vpiReg:
	  ಪಕ್ಷ vpiNet:
	  ಪಕ್ಷ vpiIntegerVar:
	  ಪಕ್ಷ vpiBitVar:
	  ಪಕ್ಷ vpiByteVar:
	  ಪಕ್ಷ vpiShortIntVar:
	  ಪಕ್ಷ vpiIntVar:
	  ಪಕ್ಷ vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    ಸಂರಚನಾ __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ಗಂಟು->fil);
	    ಪ್ರತಿಜ್ಞಾ(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiRealVar:
	    vpip_real_ಮೌಲ್ಯ_change(obj, data->obj);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiModule:
	  ಪಕ್ಷ vpiConstant:
	  ಪಕ್ಷ vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ಪ್ರತ್ಯಾಗಮ obj;
}

ವರ್ಗ sync_callback : ಸಾರ್ವಜನಿಕ __vpiCallback {
    ಸಾರ್ವಜನಿಕ:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(ಧ್ರುವ sync_callback&) = delete;
      sync_callback& operator=(ಧ್ರುವ sync_callback&) = delete;

    ಸಾರ್ವಜನಿಕ:
	// scheduled event
      ಸಂರಚನಾ sync_cb* cb_sync;
	// user supplied callback data
      ಸಂರಚನಾ t_vpi_time cb_time;

    ಗುಪ್ತ:
};

ಅನ್ತಃಸ್ಥ sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      ಪ್ರತಿಜ್ಞಾ(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

ಶೂನ್ಯ sync_cb::run_run()
{
      ಯದಿ (handle == 0)
	    ಪ್ರತ್ಯಾಗಮ;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      ಯದಿ (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

ಸ್ಥಿರ vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      ವಿಕಲ್ಪ (obj->cb_time.type) {
	  ಪಕ್ಷ vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (ಪೂರ್ಣಾಙ್ಕ)obj->cb_time.type);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }
      ಪ್ರತ್ಯಾಗಮ tv;
}

ಸ್ಥಿರ sync_callback* make_sync(p_cb_data data, ತಾರ್ಕಿಕ readonly_flag)
{
      ಯದಿ (!check_callback_time(data, ಅಸತ್ಯ))
	    ಪ್ರತ್ಯಾಗಮ 0;

      sync_callback*obj = ನೂತನ sync_callback(data);

      ಸಂರಚನಾ sync_cb*cb = ನೂತನ sync_cb;
      cb->sync_flag = readonly_flag? ಸತ್ಯ : ಅಸತ್ಯ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, ಸತ್ಯ, readonly_flag);

      ಪ್ರತ್ಯಾಗಮ obj;
}

ಸ್ಥಿರ ಸಂರಚನಾ __vpiCallback* make_afterdelay(p_cb_data data)
{
      ಯದಿ (!check_callback_time(data, ಅಸತ್ಯ))
	    ಪ್ರತ್ಯಾಗಮ 0;

      sync_callback*obj = ನೂತನ sync_callback(data);
      ಸಂರಚನಾ sync_cb*cb = ನೂತನ sync_cb;
      cb->sync_flag = ಅಸತ್ಯ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, ಅಸತ್ಯ);

      ಪ್ರತ್ಯಾಗಮ obj;
}

ಸ್ಥಿರ ಸಂರಚನಾ __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      ಯದಿ (!check_callback_time(data, ಅಸತ್ಯ))
	    ಪ್ರತ್ಯಾಗಮ 0;

      sync_callback*obj = ನೂತನ sync_callback(data);
      ಸಂರಚನಾ sync_cb*cb = ನೂತನ sync_cb;
      cb->sync_flag = ಅಸತ್ಯ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      ಯದಿ (cur > tv) {
	    tv = 0;
	    ಪ್ರತಿಜ್ಞಾ(0);
      } ಅನ್ಯಥಾ ಯದಿ (cur == tv) {
	    tv = 0;
      } ಅನ್ಯಥಾ {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      ಪ್ರತ್ಯಾಗಮ obj;
}

ಸ್ಥಿರ ಸಂರಚನಾ __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      ಯದಿ (!check_callback_time(data, ಅಸತ್ಯ))
	    ಪ್ರತ್ಯಾಗಮ 0;

      sync_callback*obj = ನೂತನ sync_callback(data);
      ಸಂರಚನಾ sync_cb*cb = ನೂತನ sync_cb;
      cb->sync_flag = ಅಸತ್ಯ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      ಯದಿ (cur > tv) {
	    tv = 0;
	    ಪ್ರತಿಜ್ಞಾ(0);
      } ಅನ್ಯಥಾ ಯದಿ (cur == tv) {
	    tv = 0;
      } ಅನ್ಯಥಾ {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      ಪ್ರತ್ಯಾಗಮ obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

ವರ್ಗ simulator_callback : ಸಾರ್ವಜನಿಕ __vpiCallback {
    ಸಾರ್ವಜನಿಕ:
      explicit simulator_callback(ಧ್ರುವ ಸಂರಚನಾ t_cb_data*data);

    ಸಾರ್ವಜನಿಕ:
      ಸಂರಚನಾ t_vpi_time cb_time;
};

ಅನ್ತಃಸ್ಥ simulator_callback::simulator_callback(ಧ್ರುವ ಸಂರಚನಾ t_cb_data*data)
{
      cb_data = *data;
      ಯದಿ ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } ಅನ್ಯಥಾ ಯದಿ (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } ಅನ್ಯಥಾ {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

ಸ್ಥಿರ simulator_callback*NextSimTime = 0;
ಸ್ಥಿರ simulator_callback*EndOfCompile = 0;
ಸ್ಥಿರ simulator_callback*StartOfSimulation = 0;
ಸ್ಥಿರ simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
ಶೂನ್ಯ simulator_cb_delete(ಶೂನ್ಯ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      ಯಾವತ್ (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      ಯಾವತ್ (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      ಯಾವತ್ (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

ಶೂನ್ಯ vpiEndOfCompile(ಶೂನ್ಯ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ಯಾವತ್ (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    ಯದಿ (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ಶೂನ್ಯ vpiStartOfSim(ಶೂನ್ಯ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ಯಾವತ್ (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    ಯದಿ (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ಶೂನ್ಯ vpiPostsim(ಶೂನ್ಯ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      ಯಾವತ್ (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    ಯದಿ (cur->cb_data.cb_rtn != 0) {
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
ಶೂನ್ಯ vpiNextSimTime(ಶೂನ್ಯ)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ಯಾವತ್ (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            ಯದಿ (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ಸ್ಥಿರ simulator_callback* make_prepost(p_cb_data data)
{
      ಯದಿ ((data->reason == cbNextSimTime) && !check_callback_time(data, ಸತ್ಯ))
	    ಪ್ರತ್ಯಾಗಮ 0;

      simulator_callback*obj = ನೂತನ simulator_callback(data);

      /* Insert at head of list */
      ವಿಕಲ್ಪ (data->reason) {
	  ಪಕ್ಷ cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      ಸಂರಚನಾ __vpiCallback*obj = 0;

      ಪ್ರತಿಜ್ಞಾ(data);
      ವಿಕಲ್ಪ (data->reason) {

	  ಪಕ್ಷ cb_ಮೌಲ್ಯ_Change:
	    obj = make_ಮೌಲ್ಯ_change(data);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ cbReadOnlySynch:
	    obj = make_sync(data, ಸತ್ಯ);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ cbReadWriteSynch:
	    obj = make_sync(data, ಅಸತ್ಯ);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ cbAfterDelay:
	    obj = make_afterdelay(data);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ cbEndOfCompile:
	  ಪಕ್ಷ cbStartOfSimulation:
	  ಪಕ್ಷ cbEndOfSimulation:
	  ಪಕ್ಷ cbNextSimTime:
	    obj = make_prepost(data);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (ಪೂರ್ಣಾಙ್ಕ)data->reason);
	    ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      ಸಂರಚನಾ __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      obj->cb_data.cb_rtn = 0;

      ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ callback_execute(ಸಂರಚನಾ __vpiCallback*cur)
{
      ಧ್ರುವ vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      ಪ್ರತಿಜ್ಞಾ(cur->cb_data.cb_rtn);
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
ಸಂರಚನಾ __vpi_array_word {
      ಸಂರಚನಾ __vpi_array_word* next;
      ಸಂರಚನಾ __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_ಪದಗಳು = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      ಪ್ರತಿಜ್ಞಾ(vpi_callbacks_ == 0);
      ಪ್ರತಿಜ್ಞಾ(array_ಪದಗಳು == 0);
}

ಶೂನ್ಯ vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      ಸಂರಚನಾ __vpi_array_word*tmp = ನೂತನ __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_ಪದಗಳು;
      array_ಪದಗಳು = tmp;
}

ಶೂನ್ಯ vvp_vpi_callback::add_vpi_callback(ಮೌಲ್ಯ_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_vpi_callback::clear_all_callbacks()
{
      ಯಾವತ್ (vpi_callbacks_) {
	    ಮೌಲ್ಯ_callback *tmp = dynamic_cast<ಮೌಲ್ಯ_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      ಯಾವತ್ (array_ಪದಗಳು) {
	    ಸಂರಚನಾ __vpi_array_word*tmp = array_ಪದಗಳು->next;
	    delete array_ಪದಗಳು;
	    array_ಪದಗಳು = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
ಶೂನ್ಯ vvp_vpi_callback::run_vpi_callbacks()
{
      ಸಂರಚನಾ __vpi_array_word*array_word = array_ಪದಗಳು;
      ಯಾವತ್ (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      ಮೌಲ್ಯ_callback *next = vpi_callbacks_;
      ಮೌಲ್ಯ_callback *prev = 0;

      ಯಾವತ್ (next) {
	    ಮೌಲ್ಯ_callback*cur = next;
	    next = dynamic_cast<ಮೌಲ್ಯ_callback*>(cur->next);

	    ಯದಿ (cur->cb_data.cb_rtn != 0) {
		  ಯದಿ (cur->test_ಮೌಲ್ಯ_callback_ready()) {
			ಯದಿ (cur->cb_data.value)
			      get_ಮೌಲ್ಯ(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } ಅನ್ಯಥಾ ಯದಿ (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } ಅನ್ಯಥಾ {
		  ಪ್ರತಿಜ್ಞಾ(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

ಶೂನ್ಯ vvp_signal_ಮೌಲ್ಯ::get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp)
{
      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal:
	  ಪಕ್ಷ vpiOctStrVal:
	  ಪಕ್ಷ vpiDecStrVal:
	  ಪಕ್ಷ vpiHexStrVal:
	  ಪಕ್ಷ vpiIntVal:
	  ಪಕ್ಷ vpiVectorVal:
	  ಪಕ್ಷ vpiStringVal:
	  ಪಕ್ಷ vpiRealVal: {
	    unsigned wid = ಮೌಲ್ಯ_size();
	    vvp_vector4_t vec4(wid);
	    ಪ್ರತಿ (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_ಮೌಲ್ಯ(vec4, wid, ಅಸತ್ಯ, vp);
	    ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiSuppressVal:
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (ಪೂರ್ಣಾಙ್ಕ)vp->format);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ real_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp, ದ್ವಿಗುಣ rval)
{
      ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_SIZE = 64 + 1;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ಪಕ್ಷ vpiRealVal:
	    vp->value.real = rval;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ಯದಿ (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } ಅನ್ಯಥಾ {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    ಯದಿ (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    ಅನ್ಯಥಾ
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		ಯಾವತ್ (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		ಪ್ರತಿ (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		ಯದಿ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiSuppressVal:
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (ಪೂರ್ಣಾಙ್ಕ)vp->format);
      }
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp)
{
      real_signal_ಮೌಲ್ಯ(vp, real_ಮೌಲ್ಯ());
}

ಶೂನ್ಯ vvp_wire_real::get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp)
{
      real_signal_ಮೌಲ್ಯ(vp, real_ಮೌಲ್ಯ());
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
#ಯದಿ 0
ಶೂನ್ಯ vvp_wire_string::get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
#endif
ಶೂನ್ಯ vvp_wire_vec4::get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*val)
{
      get_signal_ಮೌಲ್ಯ(val);
}

ಶೂನ್ಯ vvp_wire_vec8::get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*val)
{
      get_signal_ಮೌಲ್ಯ(val);
}

ಶೂನ್ಯ vvp_wire_real::get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*val)
{
      get_signal_ಮೌಲ್ಯ(val);
}
#ಯದಿ 0
ಶೂನ್ಯ vvp_wire_string::get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*val)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
#endif
