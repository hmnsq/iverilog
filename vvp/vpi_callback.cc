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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
#ਅਨ੍ਤਰ੍ਭਾਵ  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਸਥਿਰ ਅਚਲ ਅੱਖਰ*cb_reason_name(PLI_INT32 reason)
{
      ਚੋਣ (reason) {
	  ਹਾਲਤ cb_ਮੁੱਲ_Change:
	    ਵਾਪਸ "cbValueChange";
	  ਹਾਲਤ cbStmt:
	    ਵਾਪਸ "cbStmt";
	  ਹਾਲਤ cbForce:
	    ਵਾਪਸ "cbForce";
	  ਹਾਲਤ cbRelease:
	    ਵਾਪਸ "cbRelease";
	  ਹਾਲਤ cb_At_ਸ਼ੁਰੂ_ਦਾ_Sim_ਸਮਾਂ:
	    ਵਾਪਸ "cbAtStartOfSimTime";
	  ਹਾਲਤ cbReadWriteSynch:
	    ਵਾਪਸ "cbReadWriteSynch";
	  ਹਾਲਤ cbReadOnlySynch:
	    ਵਾਪਸ "cbReadOnlySynch";
	  ਹਾਲਤ cb_ਅਗਲਾ_Sim_ਸਮਾਂ:
	    ਵਾਪਸ "cbNextSimTime";
	  ਹਾਲਤ cbAfterDelay:
	    ਵਾਪਸ "cbAfterDelay";
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Compile:
	    ਵਾਪਸ "cbEndOfCompile";
	  ਹਾਲਤ cb_ਸ਼ੁਰੂ_ਦਾ_Simulation:
	    ਵਾਪਸ "cbStartOfSimulation";
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Simulation:
	    ਵਾਪਸ "cbEndOfSimulation";
	  ਹਾਲਤ cbError:
	    ਵਾਪਸ "cbError";
	  ਹਾਲਤ cbTchkViolation:
	    ਵਾਪਸ "cbTchkViolation";
	  ਹਾਲਤ cb_ਸ਼ੁਰੂ_ਦਾ_Save:
	    ਵਾਪਸ "cbStartOfSave";
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Save:
	    ਵਾਪਸ "cbEndOfSave";
	  ਹਾਲਤ cb_ਸ਼ੁਰੂ_ਦਾ_Restart:
	    ਵਾਪਸ "cbStartOfRestart";
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Restart:
	    ਵਾਪਸ "cbEndOfRestart";
	  ਹਾਲਤ cb_ਸ਼ੁਰੂ_ਦਾ_Reset:
	    ਵਾਪਸ "cbStartOfReset";
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Reset:
	    ਵਾਪਸ "cbEndOfReset";
	  ਹਾਲਤ cbEnterInteractive:
	    ਵਾਪਸ "cbEnterInteractive";
	  ਹਾਲਤ cbExitInteractive:
	    ਵਾਪਸ "cbExitInteractive";
	  ਹਾਲਤ cbInteractiveScopeChange:
	    ਵਾਪਸ "cbInteractiveScopeChange";
	  ਹਾਲਤ cbUnresolvedSystf:
	    ਵਾਪਸ "cbUnresolvedSystf";
	  ਹਾਲਤ cb_At_ਸਿਰਾ_ਦਾ_Sim_ਸਮਾਂ:
	    ਵਾਪਸ "cbAtEndOfSimTime";
	  ਮੂਲ:
	    ਵਾਪਸ "unrecognised";
      }
}

ਸਥਿਰ ਤਾਰਕਿਕ check_callback_ਸਮਾਂ(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ, ਤਾਰਕਿਕ allow_suppress)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ);
      ਜੇ (!ਅੰਕੜੇ->time) {
	    ਜੇ (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(ਅੰਕੜੇ->reason));
		  ਵਾਪਸ ਝੂਠ;
	    }
	    ਵਾਪਸ ਸੱਚ;
      }
      ਚੋਣ (ਅੰਕੜੇ->time->type) {
	  ਹਾਲਤ vpi_Sim_ਸਮਾਂ:
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi_Scaled_Real_ਸਮਾਂ:
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi_Suppress_ਸਮਾਂ:
	    ਜੇ (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(ਅੰਕੜੇ->reason));
		  ਵਾਪਸ ਝੂਠ;
	    }
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(ਅੰਕੜੇ->reason));
	    ਵਾਪਸ ਝੂਠ;
      }
      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_callback_ਸਮਾਂ(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ && ਅੰਕੜੇ->time);
      ਅੰਕੜੇ->time->ਨੀਵਾਂ  = 0;
      ਅੰਕੜੇ->time->ਉੱਚਾ = 0;
      ਅੰਕੜੇ->time->real = 0.0;
      ਚੋਣ (ਅੰਕੜੇ->time->type) {
	  ਹਾਲਤ vpi_Sim_ਸਮਾਂ:
	    vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(ਅੰਕੜੇ->time, ਸਮਾਂ_ਸਾਰਣੀ_simtime());
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi_Scaled_Real_ਸਮਾਂ:
	    ਅੰਕੜੇ->time->real = vpip_scaled_ਸਮਾਂ_from_handle(ਸਮਾਂ_ਸਾਰਣੀ_simtime(), ਅੰਕੜੇ->obj);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi_Suppress_ਸਮਾਂ:
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
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

ਵਰਗ sync_callback;

ਬਣਤਰ sync_cb  : ਜਨਤਕ vvp_gen_event_s {
      sync_callback*handle;
      ਤਾਰਕਿਕ sync_flag;

      ~sync_cb () override { }

      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
};

ਸਤਰਅੰਦਰ __vpiCallback::__vpiCallback()
{
      ਅਗਲਾ_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

ਪੂਰਨ __vpiCallback::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiCallback; }


ਮੁੱਲ_callback::ਮੁੱਲ_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      cb_ਅੰਕੜੇ = *ਅੰਕੜੇ;
      ਜੇ (ਅੰਕੜੇ->time) {
	    cb_ਸਮਾਂ = *(ਅੰਕੜੇ->time);
      } ਨਹੀਂਤਾਂ {
	    cb_ਸਮਾਂ.type = vpi_Suppress_ਸਮਾਂ;
      }
      cb_ਅੰਕੜੇ.time = &cb_ਸਮਾਂ;
      ਜੇ (ਅੰਕੜੇ->value) {
	    cb_ਮੁੱਲ = *(ਅੰਕੜੇ->value);
      } ਨਹੀਂਤਾਂ {
	    cb_ਮੁੱਲ.format = vpiSuppressVal;
      }
      cb_ਅੰਕੜੇ.value = &cb_ਮੁੱਲ;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
ਤਾਰਕਿਕ ਮੁੱਲ_callback::test_ਮੁੱਲ_callback_ready(ਖਾਲੀ)
{
      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ vpip_real_ਮੁੱਲ_change(ਮੁੱਲ_callback*cbh, vpiHandle ref)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      obj->ਜੋੜੋ_vpi_callback(cbh);
}

ਵਰਗ ਮੁੱਲ_part_callback : ਜਨਤਕ ਮੁੱਲ_callback {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ ਮੁੱਲ_part_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ);
      ~ਮੁੱਲ_part_callback() override;

      ਮੁੱਲ_part_callback(ਅਚਲ ਮੁੱਲ_part_callback&) = ਮਿਟਾਓ;
      ਮੁੱਲ_part_callback& ਸੰਚਾਲਕ=(ਅਚਲ ਮੁੱਲ_part_callback&) = ਮਿਟਾਓ;

      ਤਾਰਕਿਕ test_ਮੁੱਲ_callback_ready(ਖਾਲੀ) override;

    ਨਿਜੀ:
      ਅੱਖਰ*ਮੁੱਲ_ਬਿੱਟ;
      size_t ਮੁੱਲ_off;
};

ਸਤਰਅੰਦਰ ਮੁੱਲ_part_callback::ਮੁੱਲ_part_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
: ਮੁੱਲ_callback(ਅੰਕੜੇ)
{
      ਬਣਤਰ __vpiPV*pobj = dynamic_cast<__vpiPV*>(ਅੰਕੜੇ->obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig_fil);

      sig_fil->ਜੋੜੋ_vpi_callback(ਇਹ);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_ਮੁੱਲ tmp_ਮੁੱਲ;
      tmp_ਮੁੱਲ.format = vpiBinStrVal;
      sig_fil->get_ਮੁੱਲ(&tmp_ਮੁੱਲ);

      ਮੁੱਲ_ਬਿੱਟ = ਨਵਾਂ ਅੱਖਰ[pobj->width+1];
      ਮੁੱਲ_off = pobj->ਮਾਪਾ->vpi_get(vpi_ਮਾਪ) - pobj->width - pobj->tbase;

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੁੱਲ_ਬਿੱਟ, tmp_ਮੁੱਲ.value.str + ਮੁੱਲ_off, pobj->width);
      ਮੁੱਲ_ਬਿੱਟ[pobj->width] = 0;
}

ਮੁੱਲ_part_callback::~ਮੁੱਲ_part_callback()
{
      ਮਿਟਾਓ[]ਮੁੱਲ_ਬਿੱਟ;
}

ਤਾਰਕਿਕ ਮੁੱਲ_part_callback::test_ਮੁੱਲ_callback_ready(ਖਾਲੀ)
{
      ਬਣਤਰ __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_ਅੰਕੜੇ.obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ਮੁੱਲ tmp_ਮੁੱਲ;
      tmp_ਮੁੱਲ.format = vpiBinStrVal;
      sig_fil->get_ਮੁੱਲ(&tmp_ਮੁੱਲ);

      ਜੇ (memcmp(ਮੁੱਲ_ਬਿੱਟ, tmp_ਮੁੱਲ.value.str + ਮੁੱਲ_off, pobj->width) == 0)
	    ਵਾਪਸ ਝੂਠ;

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੁੱਲ_ਬਿੱਟ, tmp_ਮੁੱਲ.value.str + ਮੁੱਲ_off, pobj->width);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ਸਥਿਰ ਮੁੱਲ_callback*make_ਮੁੱਲ_change_part(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
	/* Attach the __vpiCallback object to the signal. */
      ਮੁੱਲ_callback*cbh = ਨਵਾਂ ਮੁੱਲ_part_callback(ਅੰਕੜੇ);
      ਵਾਪਸ cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ਸਥਿਰ ਮੁੱਲ_callback* make_ਮੁੱਲ_change(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਜੇ (!check_callback_ਸਮਾਂ(ਅੰਕੜੇ, ਸੱਚ))
	    ਵਾਪਸ 0;

      ਜੇ (vpi_get(vpiAutomatic, ਅੰਕੜੇ->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, ਅੰਕੜੇ->obj));
            ਵਾਪਸ 0;
      }

	// Special case: the target object is a vpiPartSelect
      ਜੇ (ਅੰਕੜੇ->obj->get_type_ਕੋਡ() == vpiPartSelect) {
            ਜੇ (ਅੰਕੜੇ->obj->vpi_handle(vpiArray))
	        ਵਾਪਸ vpip_array_ਸ਼ਬਦ_change(ਅੰਕੜੇ);
            ਨਹੀਂਤਾਂ
	        ਵਾਪਸ make_ਮੁੱਲ_change_part(ਅੰਕੜੇ);
      }

      ਜੇ (ਅੰਕੜੇ->obj->get_type_ਕੋਡ() == vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ)
	    ਵਾਪਸ vpip_array_ਸ਼ਬਦ_change(ਅੰਕੜੇ);

      ਜੇ (ਅੰਕੜੇ->obj->get_type_ਕੋਡ() == vpi_ਸਮ੍ਰਿਤੀ)
	    ਵਾਪਸ vpip_array_change(ਅੰਕੜੇ);

      ਮੁੱਲ_callback*obj = ਨਵਾਂ ਮੁੱਲ_callback(ਅੰਕੜੇ);

      ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ->obj);
      ਚੋਣ (ਅੰਕੜੇ->obj->get_type_ਕੋਡ()) {

	  ਹਾਲਤ vpiReg:
	  ਹਾਲਤ vpiNet:
	  ਹਾਲਤ vpiIntegerVar:
	  ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var:
	  ਹਾਲਤ vpiByteVar:
	  ਹਾਲਤ vpiShortIntVar:
	  ਹਾਲਤ vpiIntVar:
	  ਹਾਲਤ vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    ਬਣਤਰ __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(ਅੰਕੜੇ->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ਗੰਢ->fil);
	    ਪ੍ਰਤਿਜ੍ਞਾ(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->ਜੋੜੋ_vpi_callback(obj);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiRealVar:
	    vpip_real_ਮੁੱਲ_change(obj, ਅੰਕੜੇ->obj);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(ਅੰਕੜੇ->obj);
	    nev->ਜੋੜੋ_vpi_callback(obj);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiModule:
	  ਹਾਲਤ vpiConstant:
	  ਹਾਲਤ vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    ਅੰਕੜੇ->obj->get_type_ਕੋਡ());
	    ਮਿਟਾਓ obj;
	    ਵਾਪਸ 0;
      }

      ਵਾਪਸ obj;
}

ਵਰਗ sync_callback : ਜਨਤਕ __vpiCallback {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ sync_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ);
      ~sync_callback() override;

      sync_callback(ਅਚਲ sync_callback&) = ਮਿਟਾਓ;
      sync_callback& ਸੰਚਾਲਕ=(ਅਚਲ sync_callback&) = ਮਿਟਾਓ;

    ਜਨਤਕ:
	// scheduled event
      ਬਣਤਰ sync_cb* cb_sync;
	// user supplied callback data
      ਬਣਤਰ t_vpi_ਸਮਾਂ cb_ਸਮਾਂ;

    ਨਿਜੀ:
};

ਸਤਰਅੰਦਰ sync_callback::sync_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      cb_sync = 0;

      cb_ਅੰਕੜੇ = *ਅੰਕੜੇ;
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ->time);
      cb_ਸਮਾਂ = *(ਅੰਕੜੇ->time);
      cb_ਅੰਕੜੇ.time = &cb_ਸਮਾਂ;
}

sync_callback::~sync_callback()
{
      ਮਿਟਾਓ cb_sync;
}

ਖਾਲੀ sync_cb::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      ਜੇ (handle == 0)
	    ਵਾਪਸ;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
	    ਸੈੱਟ_callback_ਸਮਾਂ(&cur->cb_ਅੰਕੜੇ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
	    vpi_ਬਹੁਲਕ_flag = sync_flag? VPI_ਬਹੁਲਕ_ROSYNC : VPI_ਬਹੁਲਕ_RWSYNC;
	    (cur->cb_ਅੰਕੜੇ.cb_rtn)(&cur->cb_ਅੰਕੜੇ);
	    vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
      }

      ਮਿਟਾਓ cur;
}

ਸਥਿਰ vvp_time64_t get_sync_cb_ਸਮਾਂ(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      ਚੋਣ (obj->cb_ਸਮਾਂ.type) {
	  ਹਾਲਤ vpi_Sim_ਸਮਾਂ:
	    tv = vpip_timestruct_ਮੰਜ਼ਿਲ_ਸਮਾਂ(&obj->cb_ਸਮਾਂ);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpi_Scaled_Real_ਸਮਾਂ:
	    tv = vpip_scaled_real_ਮੰਜ਼ਿਲ_time64(obj->cb_ਸਮਾਂ.real,
		     vpip_timescale_scope_from_handle(obj->cb_ਅੰਕੜੇ.obj));
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (ਪੂਰਨ)obj->cb_ਸਮਾਂ.type);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }
      ਵਾਪਸ tv;
}

ਸਥਿਰ sync_callback* make_sync(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ, ਤਾਰਕਿਕ readonly_flag)
{
      ਜੇ (!check_callback_ਸਮਾਂ(ਅੰਕੜੇ, ਝੂਠ))
	    ਵਾਪਸ 0;

      sync_callback*obj = ਨਵਾਂ sync_callback(ਅੰਕੜੇ);

      ਬਣਤਰ sync_cb*cb = ਨਵਾਂ sync_cb;
      cb->sync_flag = readonly_flag? ਸੱਚ : ਝੂਠ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ਸਮਾਂ(obj);
      ਸਮਾਂ_ਸਾਰਣੀ_generic(cb, tv, ਸੱਚ, readonly_flag);

      ਵਾਪਸ obj;
}

ਸਥਿਰ ਬਣਤਰ __vpiCallback* make_afterdelay(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਜੇ (!check_callback_ਸਮਾਂ(ਅੰਕੜੇ, ਝੂਠ))
	    ਵਾਪਸ 0;

      sync_callback*obj = ਨਵਾਂ sync_callback(ਅੰਕੜੇ);
      ਬਣਤਰ sync_cb*cb = ਨਵਾਂ sync_cb;
      cb->sync_flag = ਝੂਠ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ਸਮਾਂ(obj);
      ਸਮਾਂ_ਸਾਰਣੀ_generic(cb, tv, ਝੂਠ);

      ਵਾਪਸ obj;
}

ਸਥਿਰ ਬਣਤਰ __vpiCallback* make_at_ਸ਼ੁਰੂ_ਦਾ_sim_ਸਮਾਂ(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਜੇ (!check_callback_ਸਮਾਂ(ਅੰਕੜੇ, ਝੂਠ))
	    ਵਾਪਸ 0;

      sync_callback*obj = ਨਵਾਂ sync_callback(ਅੰਕੜੇ);
      ਬਣਤਰ sync_cb*cb = ਨਵਾਂ sync_cb;
      cb->sync_flag = ਝੂਠ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ਸਮਾਂ(obj);
      vvp_time64_t cur = ਸਮਾਂ_ਸਾਰਣੀ_simtime();
      ਜੇ (cur > tv) {
	    tv = 0;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (cur == tv) {
	    tv = 0;
      } ਨਹੀਂਤਾਂ {
	    tv -= cur;
      }
      ਸਮਾਂ_ਸਾਰਣੀ_at_ਸ਼ੁਰੂ_ਦਾ_simtime(cb, tv);

      ਵਾਪਸ obj;
}

ਸਥਿਰ ਬਣਤਰ __vpiCallback* make_at_ਸਿਰਾ_ਦਾ_sim_ਸਮਾਂ(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਜੇ (!check_callback_ਸਮਾਂ(ਅੰਕੜੇ, ਝੂਠ))
	    ਵਾਪਸ 0;

      sync_callback*obj = ਨਵਾਂ sync_callback(ਅੰਕੜੇ);
      ਬਣਤਰ sync_cb*cb = ਨਵਾਂ sync_cb;
      cb->sync_flag = ਝੂਠ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ਸਮਾਂ(obj);
      vvp_time64_t cur = ਸਮਾਂ_ਸਾਰਣੀ_simtime();
      ਜੇ (cur > tv) {
	    tv = 0;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (cur == tv) {
	    tv = 0;
      } ਨਹੀਂਤਾਂ {
	    tv -= cur;
      }
      ਸਮਾਂ_ਸਾਰਣੀ_at_ਸਿਰਾ_ਦਾ_simtime(cb, tv);

      ਵਾਪਸ obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

ਵਰਗ simulator_callback : ਜਨਤਕ __vpiCallback {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ simulator_callback(ਅਚਲ ਬਣਤਰ t_cb_ਅੰਕੜੇ*ਅੰਕੜੇ);

    ਜਨਤਕ:
      ਬਣਤਰ t_vpi_ਸਮਾਂ cb_ਸਮਾਂ;
};

ਸਤਰਅੰਦਰ simulator_callback::simulator_callback(ਅਚਲ ਬਣਤਰ t_cb_ਅੰਕੜੇ*ਅੰਕੜੇ)
{
      cb_ਅੰਕੜੇ = *ਅੰਕੜੇ;
      ਜੇ ((ਅੰਕੜੇ->reason == cb_ਅਗਲਾ_Sim_ਸਮਾਂ) && ਅੰਕੜੇ->time) {
	    cb_ਸਮਾਂ = *(ਅੰਕੜੇ->time);
      } ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕੜੇ->reason == cb_ਸਿਰਾ_ਦਾ_Simulation) {
	    cb_ਸਮਾਂ.type = vpi_Sim_ਸਮਾਂ;
      } ਨਹੀਂਤਾਂ {
	    cb_ਸਮਾਂ.type = vpi_Suppress_ਸਮਾਂ;
      }
      cb_ਅੰਕੜੇ.time = &cb_ਸਮਾਂ;
}

ਸਥਿਰ simulator_callback*ਅਗਲਾ_Sim_ਸਮਾਂ = 0;
ਸਥਿਰ simulator_callback*ਸਿਰਾ_ਦਾ_Compile = 0;
ਸਥਿਰ simulator_callback*ਸ਼ੁਰੂ_ਦਾ_Simulation = 0;
ਸਥਿਰ simulator_callback*ਸਿਰਾ_ਦਾ_Simulation = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
ਖਾਲੀ simulator_cb_delete(ਖਾਲੀ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      ਜਦੋਂ (ਸਿਰਾ_ਦਾ_Compile) {
	    cur = ਸਿਰਾ_ਦਾ_Compile;
	    ਸਿਰਾ_ਦਾ_Compile = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
	    ਮਿਟਾਓ cur;
      }

	/* Delete all the start of simulation callbacks. */
      ਜਦੋਂ (ਸ਼ੁਰੂ_ਦਾ_Simulation) {
	    cur = ਸ਼ੁਰੂ_ਦਾ_Simulation;
	    ਸ਼ੁਰੂ_ਦਾ_Simulation = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
	    ਮਿਟਾਓ cur;
      }

	/* Delete all the end of simulation callbacks. */
      ਜਦੋਂ (ਸਿਰਾ_ਦਾ_Simulation) {
	    cur = ਸਿਰਾ_ਦਾ_Simulation;
	    ਸਿਰਾ_ਦਾ_Simulation = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
	    ਮਿਟਾਓ cur;
      }
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vpi_ਸਿਰਾ_ਦਾ_Compile(ਖਾਲੀ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_RWSYNC;

      ਜਦੋਂ (ਸਿਰਾ_ਦਾ_Compile) {
	    cur = ਸਿਰਾ_ਦਾ_Compile;
	    ਸਿਰਾ_ਦਾ_Compile = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
	    ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
		  (cur->cb_ਅੰਕੜੇ.cb_rtn)(&cur->cb_ਅੰਕੜੇ);
	    }
	    ਮਿਟਾਓ cur;
      }

      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
}

ਖਾਲੀ vpi_ਸ਼ੁਰੂ_ਦਾ_Sim(ਖਾਲੀ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_RWSYNC;

      ਜਦੋਂ (ਸ਼ੁਰੂ_ਦਾ_Simulation) {
	    cur = ਸ਼ੁਰੂ_ਦਾ_Simulation;
	    ਸ਼ੁਰੂ_ਦਾ_Simulation = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
	    ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
		  (cur->cb_ਅੰਕੜੇ.cb_rtn)(&cur->cb_ਅੰਕੜੇ);
	    }
	    ਮਿਟਾਓ cur;
      }

      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
}

ਖਾਲੀ vpiPostsim(ਖਾਲੀ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_ROSYNC;

      ਜਦੋਂ (ਸਿਰਾ_ਦਾ_Simulation) {
	    cur = ਸਿਰਾ_ਦਾ_Simulation;
	    ਸਿਰਾ_ਦਾ_Simulation = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
	    ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
		  ਸੈੱਟ_callback_ਸਮਾਂ(&cur->cb_ਅੰਕੜੇ);
		  (cur->cb_ਅੰਕੜੇ.cb_rtn)(&cur->cb_ਅੰਕੜੇ);
	    }
	    ਮਿਟਾਓ cur;
      }

      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
ਖਾਲੀ vpi_ਅਗਲਾ_Sim_ਸਮਾਂ(ਖਾਲੀ)
{
      simulator_callback* cur;
      simulator_callback* ਅਗਲਾ_2 = ਅਗਲਾ_Sim_ਸਮਾਂ;
      ਅਗਲਾ_Sim_ਸਮਾਂ = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_RWSYNC;

      ਜਦੋਂ (ਅਗਲਾ_2) {
            cur = ਅਗਲਾ_2;
            ਅਗਲਾ_2 = dynamic_cast<simulator_callback*>(cur->ਅਗਲਾ_2);
            ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
                  ਸੈੱਟ_callback_ਸਮਾਂ(&cur->cb_ਅੰਕੜੇ);
                  (cur->cb_ਅੰਕੜੇ.cb_rtn)(&cur->cb_ਅੰਕੜੇ);
            }
            ਮਿਟਾਓ cur;
      }

      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
}

ਸਥਿਰ simulator_callback* make_prepost(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਜੇ ((ਅੰਕੜੇ->reason == cb_ਅਗਲਾ_Sim_ਸਮਾਂ) && !check_callback_ਸਮਾਂ(ਅੰਕੜੇ, ਸੱਚ))
	    ਵਾਪਸ 0;

      simulator_callback*obj = ਨਵਾਂ simulator_callback(ਅੰਕੜੇ);

      /* Insert at head of list */
      ਚੋਣ (ਅੰਕੜੇ->reason) {
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Compile:
	    obj->ਅਗਲਾ_2 = ਸਿਰਾ_ਦਾ_Compile;
	    ਸਿਰਾ_ਦਾ_Compile = obj;
	    ਰੋਕੋ;
	  ਹਾਲਤ cb_ਸ਼ੁਰੂ_ਦਾ_Simulation:
	    obj->ਅਗਲਾ_2 = ਸ਼ੁਰੂ_ਦਾ_Simulation;
	    ਸ਼ੁਰੂ_ਦਾ_Simulation = obj;
	    ਰੋਕੋ;
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Simulation:
	    obj->ਅਗਲਾ_2 = ਸਿਰਾ_ਦਾ_Simulation;
	    ਸਿਰਾ_ਦਾ_Simulation = obj;
	    ਰੋਕੋ;
	  ਹਾਲਤ cb_ਅਗਲਾ_Sim_ਸਮਾਂ:
	    obj->ਅਗਲਾ_2 = ਅਗਲਾ_Sim_ਸਮਾਂ;
	    ਅਗਲਾ_Sim_ਸਮਾਂ = obj;
	    ਰੋਕੋ;
      }

      ਵਾਪਸ obj;
}

vpiHandle vpi_ਰਜਿਸਟਰ_cb(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਬਣਤਰ __vpiCallback*obj = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ);
      ਚੋਣ (ਅੰਕੜੇ->reason) {

	  ਹਾਲਤ cb_ਮੁੱਲ_Change:
	    obj = make_ਮੁੱਲ_change(ਅੰਕੜੇ);
	    ਰੋਕੋ;

	  ਹਾਲਤ cbReadOnlySynch:
	    obj = make_sync(ਅੰਕੜੇ, ਸੱਚ);
	    ਰੋਕੋ;

	  ਹਾਲਤ cbReadWriteSynch:
	    obj = make_sync(ਅੰਕੜੇ, ਝੂਠ);
	    ਰੋਕੋ;

	  ਹਾਲਤ cb_At_ਸ਼ੁਰੂ_ਦਾ_Sim_ਸਮਾਂ:
	    obj = make_at_ਸ਼ੁਰੂ_ਦਾ_sim_ਸਮਾਂ(ਅੰਕੜੇ);
	    ਰੋਕੋ;

	  ਹਾਲਤ cb_At_ਸਿਰਾ_ਦਾ_Sim_ਸਮਾਂ:
	    obj = make_at_ਸਿਰਾ_ਦਾ_sim_ਸਮਾਂ(ਅੰਕੜੇ);
	    ਰੋਕੋ;

	  ਹਾਲਤ cbAfterDelay:
	    obj = make_afterdelay(ਅੰਕੜੇ);
	    ਰੋਕੋ;

	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Compile:
	  ਹਾਲਤ cb_ਸ਼ੁਰੂ_ਦਾ_Simulation:
	  ਹਾਲਤ cb_ਸਿਰਾ_ਦਾ_Simulation:
	  ਹਾਲਤ cb_ਅਗਲਾ_Sim_ਸਮਾਂ:
	    obj = make_prepost(ਅੰਕੜੇ);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (ਪੂਰਨ)ਅੰਕੜੇ->reason);
	    ਰੋਕੋ;
      }

      ਵਾਪਸ obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      ਬਣਤਰ __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      obj->cb_ਅੰਕੜੇ.cb_rtn = 0;

      ਵਾਪਸ 1;
}

ਖਾਲੀ callback_execute(ਬਣਤਰ __vpiCallback*cur)
{
      ਅਚਲ vpi_ਬਹੁਲਕ_t save_ਬਹੁਲਕ = vpi_ਬਹੁਲਕ_flag;
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_RWSYNC;

      ਪ੍ਰਤਿਜ੍ਞਾ(cur->cb_ਅੰਕੜੇ.cb_rtn);
      ਸੈੱਟ_callback_ਸਮਾਂ(&cur->cb_ਅੰਕੜੇ);
      (cur->cb_ਅੰਕੜੇ.cb_rtn)(&cur->cb_ਅੰਕੜੇ);

      vpi_ਬਹੁਲਕ_flag = save_ਬਹੁਲਕ;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
ਬਣਤਰ __vpi_array_word {
      ਬਣਤਰ __vpi_array_word* ਅਗਲਾ_2;
      ਬਣਤਰ __vpiArray* array;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ਬਦ_2;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_ਸ਼ਬਦ = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_callbacks_ == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(array_ਸ਼ਬਦ == 0);
}

ਖਾਲੀ vvp_vpi_callback::attach_as_ਸ਼ਬਦ(vvp_array_t arr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
      ਬਣਤਰ __vpi_array_word*tmp = ਨਵਾਂ __vpi_array_word;
      tmp->array = arr;
      tmp->ਸ਼ਬਦ_2 = addr;
      tmp->ਅਗਲਾ_2 = array_ਸ਼ਬਦ;
      array_ਸ਼ਬਦ = tmp;
}

ਖਾਲੀ vvp_vpi_callback::ਜੋੜੋ_vpi_callback(ਮੁੱਲ_callback*cb)
{
      cb->ਅਗਲਾ_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_vpi_callback::clear_all_callbacks()
{
      ਜਦੋਂ (vpi_callbacks_) {
	    ਮੁੱਲ_callback *tmp = dynamic_cast<ਮੁੱਲ_callback*>
	                            (vpi_callbacks_->ਅਗਲਾ_2);
	    ਮਿਟਾਓ vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      ਜਦੋਂ (array_ਸ਼ਬਦ) {
	    ਬਣਤਰ __vpi_array_word*tmp = array_ਸ਼ਬਦ->ਅਗਲਾ_2;
	    ਮਿਟਾਓ array_ਸ਼ਬਦ;
	    array_ਸ਼ਬਦ = tmp;
      }
}
#ਜੇ_ਅੰਤ

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
ਖਾਲੀ vvp_vpi_callback::ਲੜੀਵਾਰ_vpi_callbacks()
{
      ਬਣਤਰ __vpi_array_word*array_ਸ਼ਬਦ_2 = array_ਸ਼ਬਦ;
      ਜਦੋਂ (array_ਸ਼ਬਦ_2) {
	    array_ਸ਼ਬਦ_2->array->ਸ਼ਬਦ_change(array_ਸ਼ਬਦ_2->ਸ਼ਬਦ_2);
	    array_ਸ਼ਬਦ_2 = array_ਸ਼ਬਦ_2->ਅਗਲਾ_2;
      }

      ਮੁੱਲ_callback *ਅਗਲਾ_2 = vpi_callbacks_;
      ਮੁੱਲ_callback *prev = 0;

      ਜਦੋਂ (ਅਗਲਾ_2) {
	    ਮੁੱਲ_callback*cur = ਅਗਲਾ_2;
	    ਅਗਲਾ_2 = dynamic_cast<ਮੁੱਲ_callback*>(cur->ਅਗਲਾ_2);

	    ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
		  ਜੇ (cur->test_ਮੁੱਲ_callback_ready()) {
			ਜੇ (cur->cb_ਅੰਕੜੇ.value)
			      get_ਮੁੱਲ(cur->cb_ਅੰਕੜੇ.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } ਨਹੀਂਤਾਂ ਜੇ (prev == 0) {

		  vpi_callbacks_ = ਅਗਲਾ_2;
		  cur->ਅਗਲਾ_2 = 0;
		  ਮਿਟਾਓ cur;

	    } ਨਹੀਂਤਾਂ {
		  ਪ੍ਰਤਿਜ੍ਞਾ(prev->ਅਗਲਾ_2 == cur);
		  prev->ਅਗਲਾ_2 = ਅਗਲਾ_2;
		  cur->ਅਗਲਾ_2 = 0;
		  ਮਿਟਾਓ cur;
	    }
      }
}

ਖਾਲੀ vvp_signal_ਮੁੱਲ::get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp)
{
      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal:
	  ਹਾਲਤ vpiOctStrVal:
	  ਹਾਲਤ vpiDecStrVal:
	  ਹਾਲਤ vpiHexStrVal:
	  ਹਾਲਤ vpiIntVal:
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	  ਹਾਲਤ vpiStringVal:
	  ਹਾਲਤ vpiRealVal: {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ਮੁੱਲ_ਮਾਪ();
	    vvp_vector4_t vec4(wid);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < wid; idx += 1) {
		  vec4.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, value(idx));
	    }
	    vpip_vec4_get_ਮੁੱਲ(vec4, wid, ਝੂਠ, vp);
	    ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpiSuppressVal:
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (ਪੂਰਨ)vp->format);
      }
}

ਸਥਿਰ ਖਾਲੀ real_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp, ਦੁੱਗਣਾ rval)
{
      ਸਥਿਰ ਅਚਲ size_t RBUF_ਮਾਪ = 64 + 1;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_ਮਾਪ, RBUF_VAL));

      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ਹਾਲਤ vpiRealVal:
	    vp->value.real = rval;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ਜੇ (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } ਨਹੀਂਤਾਂ {
		  rval = std::ਗੇੜਾ_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    ਜੇ (std::isnan(rval))
		  snprintf(rbuf, RBUF_ਮਾਪ, "%s", "nan");
	    ਨਹੀਂਤਾਂ
		  snprintf(rbuf, RBUF_ਮਾਪ, "%0.0f", std::ਗੇੜਾ_2(rval));
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiHexStrVal:
	    snprintf(rbuf, RBUF_ਮਾਪ, "%" PRIx64, vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(rval));
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal: {
		uint64_t val = vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(rval);
		ਬਿਨਾਨਿਸ਼ਾਨੀ len = 0;

		ਜਦੋਂ (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(rval);
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		ਜੇ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpiSuppressVal:
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (ਪੂਰਨ)vp->format);
      }
}

ਖਾਲੀ vvp_fun_signal_real_aa::get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp)
{
      real_signal_ਮੁੱਲ(vp, real_ਮੁੱਲ());
}

ਖਾਲੀ vvp_wire_real::get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp)
{
      real_signal_ਮੁੱਲ(vp, real_ਮੁੱਲ());
}

ਖਾਲੀ vvp_fun_signal_string_aa::get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
#ਜੇ 0
ਖਾਲੀ vvp_wire_string::get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
#ਜੇ_ਅੰਤ
ਖਾਲੀ vvp_wire_vec4::get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*val)
{
      get_signal_ਮੁੱਲ(val);
}

ਖਾਲੀ vvp_wire_vec8::get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*val)
{
      get_signal_ਮੁੱਲ(val);
}

ਖਾਲੀ vvp_wire_real::get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*val)
{
      get_signal_ਮੁੱਲ(val);
}
#ਜੇ 0
ਖਾਲੀ vvp_wire_string::get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
#ਜੇ_ਅੰਤ
