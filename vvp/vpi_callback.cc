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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*cb_reason_name(PLI_INT32 reason)
{
      ಆಯ್ಕೆ (reason) {
	  ಸ್ಥಿತಿ cb_ಮೌಲ್ಯ_Change:
	    ಹಿಂತಿರುಗಿ "cbValueChange";
	  ಸ್ಥಿತಿ cbStmt:
	    ಹಿಂತಿರುಗಿ "cbStmt";
	  ಸ್ಥಿತಿ cbForce:
	    ಹಿಂತಿರುಗಿ "cbForce";
	  ಸ್ಥಿತಿ cbRelease:
	    ಹಿಂತಿರುಗಿ "cbRelease";
	  ಸ್ಥಿತಿ cb_At_ಆರಂಭ_ದ_Sim_ಸಮಯ:
	    ಹಿಂತಿರುಗಿ "cbAtStartOfSimTime";
	  ಸ್ಥಿತಿ cbReadWriteSynch:
	    ಹಿಂತಿರುಗಿ "cbReadWriteSynch";
	  ಸ್ಥಿತಿ cbReadOnlySynch:
	    ಹಿಂತಿರುಗಿ "cbReadOnlySynch";
	  ಸ್ಥಿತಿ cb_ಮುಂದಿನ_Sim_ಸಮಯ:
	    ಹಿಂತಿರುಗಿ "cbNextSimTime";
	  ಸ್ಥಿತಿ cbAfterDelay:
	    ಹಿಂತಿರುಗಿ "cbAfterDelay";
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Compile:
	    ಹಿಂತಿರುಗಿ "cbEndOfCompile";
	  ಸ್ಥಿತಿ cb_ಆರಂಭ_ದ_Simulation:
	    ಹಿಂತಿರುಗಿ "cbStartOfSimulation";
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Simulation:
	    ಹಿಂತಿರುಗಿ "cbEndOfSimulation";
	  ಸ್ಥಿತಿ cbError:
	    ಹಿಂತಿರುಗಿ "cbError";
	  ಸ್ಥಿತಿ cbTchkViolation:
	    ಹಿಂತಿರುಗಿ "cbTchkViolation";
	  ಸ್ಥಿತಿ cb_ಆರಂಭ_ದ_Save:
	    ಹಿಂತಿರುಗಿ "cbStartOfSave";
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Save:
	    ಹಿಂತಿರುಗಿ "cbEndOfSave";
	  ಸ್ಥಿತಿ cb_ಆರಂಭ_ದ_Restart:
	    ಹಿಂತಿರುಗಿ "cbStartOfRestart";
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Restart:
	    ಹಿಂತಿರುಗಿ "cbEndOfRestart";
	  ಸ್ಥಿತಿ cb_ಆರಂಭ_ದ_Reset:
	    ಹಿಂತಿರುಗಿ "cbStartOfReset";
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Reset:
	    ಹಿಂತಿರುಗಿ "cbEndOfReset";
	  ಸ್ಥಿತಿ cbEnterInteractive:
	    ಹಿಂತಿರುಗಿ "cbEnterInteractive";
	  ಸ್ಥಿತಿ cbExitInteractive:
	    ಹಿಂತಿರುಗಿ "cbExitInteractive";
	  ಸ್ಥಿತಿ cbInteractiveScopeChange:
	    ಹಿಂತಿರುಗಿ "cbInteractiveScopeChange";
	  ಸ್ಥಿತಿ cbUnresolvedSystf:
	    ಹಿಂತಿರುಗಿ "cbUnresolvedSystf";
	  ಸ್ಥಿತಿ cb_At_ಕೊನೆ_ದ_Sim_ಸಮಯ:
	    ಹಿಂತಿರುಗಿ "cbAtEndOfSimTime";
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ "unrecognised";
      }
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ check_callback_ಸಮಯ(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ, ತಾರ್ಕಿಕ allow_suppress)
{
      ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ);
      ಆದರೆ (!ದತ್ತಾಂಶ->time) {
	    ಆದರೆ (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(ದತ್ತಾಂಶ->reason));
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    }
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಆಯ್ಕೆ (ದತ್ತಾಂಶ->time->type) {
	  ಸ್ಥಿತಿ vpi_Sim_ಸಮಯ:
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi_Scaled_Real_ಸಮಯ:
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi_Suppress_ಸಮಯ:
	    ಆದರೆ (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(ದತ್ತಾಂಶ->reason));
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    }
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(ದತ್ತಾಂಶ->reason));
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_callback_ಸಮಯ(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->time);
      ದತ್ತಾಂಶ->time->ಕೆಳಗಿನ  = 0;
      ದತ್ತಾಂಶ->time->ಮೇಲಿನ = 0;
      ದತ್ತಾಂಶ->time->real = 0.0;
      ಆಯ್ಕೆ (ದತ್ತಾಂಶ->time->type) {
	  ಸ್ಥಿತಿ vpi_Sim_ಸಮಯ:
	    vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(ದತ್ತಾಂಶ->time, ವೇಳಾಪಟ್ಟಿ_simtime());
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi_Scaled_Real_ಸಮಯ:
	    ದತ್ತಾಂಶ->time->real = vpip_scaled_ಸಮಯ_from_handle(ವೇಳಾಪಟ್ಟಿ_simtime(), ದತ್ತಾಂಶ->obj);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi_Suppress_ಸಮಯ:
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
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

ರಚನೆ sync_cb  : ಸಾರ್ವಜನಿಕ vvp_gen_event_s {
      sync_callback*handle;
      ತಾರ್ಕಿಕ sync_flag;

      ~sync_cb () override { }

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() override;
};

ಸಾಲೊಳಗೆ __vpiCallback::__vpiCallback()
{
      ಮುಂದಿನ_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

ಪೂರ್ಣಾಂಕ __vpiCallback::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiCallback; }


ಮೌಲ್ಯ_callback::ಮೌಲ್ಯ_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      cb_ದತ್ತಾಂಶ = *ದತ್ತಾಂಶ;
      ಆದರೆ (ದತ್ತಾಂಶ->time) {
	    cb_ಸಮಯ = *(ದತ್ತಾಂಶ->time);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    cb_ಸಮಯ.type = vpi_Suppress_ಸಮಯ;
      }
      cb_ದತ್ತಾಂಶ.time = &cb_ಸಮಯ;
      ಆದರೆ (ದತ್ತಾಂಶ->value) {
	    cb_ಮೌಲ್ಯ = *(ದತ್ತಾಂಶ->value);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    cb_ಮೌಲ್ಯ.format = vpiSuppressVal;
      }
      cb_ದತ್ತಾಂಶ.value = &cb_ಮೌಲ್ಯ;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
ತಾರ್ಕಿಕ ಮೌಲ್ಯ_callback::test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ vpip_real_ಮೌಲ್ಯ_change(ಮೌಲ್ಯ_callback*cbh, vpiHandle ref)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(obj);

      obj->ಕೂಡಿಸು_vpi_callback(cbh);
}

ವರ್ಗ ಮೌಲ್ಯ_part_callback : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_callback {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ ಮೌಲ್ಯ_part_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ);
      ~ಮೌಲ್ಯ_part_callback() override;

      ಮೌಲ್ಯ_part_callback(ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_part_callback&) = ಅಳಿಸು;
      ಮೌಲ್ಯ_part_callback& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_part_callback&) = ಅಳಿಸು;

      ತಾರ್ಕಿಕ test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      ಅಕ್ಷರ*ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;
      size_t ಮೌಲ್ಯ_off;
};

ಸಾಲೊಳಗೆ ಮೌಲ್ಯ_part_callback::ಮೌಲ್ಯ_part_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
: ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ)
{
      ರಚನೆ __vpiPV*pobj = dynamic_cast<__vpiPV*>(ದತ್ತಾಂಶ->obj);
      ಪ್ರತಿಜ್ಞಾ(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig_fil);

      sig_fil->ಕೂಡಿಸು_vpi_callback(ಇದು);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      sig_fil->get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು = ಹೊಸ ಅಕ್ಷರ[pobj->width+1];
      ಮೌಲ್ಯ_off = pobj->ಪೋಷಕ->vpi_get(vpi_ಗಾತ್ರ) - pobj->width - pobj->tbase;

      ಸ್ಮರಣೆ_ನಕಲು(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, tmp_ಮೌಲ್ಯ.value.str + ಮೌಲ್ಯ_off, pobj->width);
      ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು[pobj->width] = 0;
}

ಮೌಲ್ಯ_part_callback::~ಮೌಲ್ಯ_part_callback()
{
      ಅಳಿಸು[]ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;
}

ತಾರ್ಕಿಕ ಮೌಲ್ಯ_part_callback::test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ)
{
      ರಚನೆ __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_ದತ್ತಾಂಶ.obj);
      ಪ್ರತಿಜ್ಞಾ(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      sig_fil->get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಆದರೆ (memcmp(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, tmp_ಮೌಲ್ಯ.value.str + ಮೌಲ್ಯ_off, pobj->width) == 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಸ್ಮರಣೆ_ನಕಲು(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, tmp_ಮೌಲ್ಯ.value.str + ಮೌಲ್ಯ_off, pobj->width);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ಸ್ಥಿರ ಮೌಲ್ಯ_callback*make_ಮೌಲ್ಯ_change_part(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
	/* Attach the __vpiCallback object to the signal. */
      ಮೌಲ್ಯ_callback*cbh = ಹೊಸ ಮೌಲ್ಯ_part_callback(ದತ್ತಾಂಶ);
      ಹಿಂತಿರುಗಿ cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ಸ್ಥಿರ ಮೌಲ್ಯ_callback* make_ಮೌಲ್ಯ_change(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ಆದರೆ (!check_callback_ಸಮಯ(ದತ್ತಾಂಶ, ನಿಜ))
	    ಹಿಂತಿರುಗಿ 0;

      ಆದರೆ (vpi_get(vpiAutomatic, ದತ್ತಾಂಶ->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, ದತ್ತಾಂಶ->obj));
            ಹಿಂತಿರುಗಿ 0;
      }

	// Special case: the target object is a vpiPartSelect
      ಆದರೆ (ದತ್ತಾಂಶ->obj->get_type_ಸಂಕೇತ() == vpiPartSelect) {
            ಆದರೆ (ದತ್ತಾಂಶ->obj->vpi_handle(vpiArray))
	        ಹಿಂತಿರುಗಿ vpip_array_ಶಬ್ದ_change(ದತ್ತಾಂಶ);
            ಇಲ್ಲದಿದ್ದರೆ
	        ಹಿಂತಿರುಗಿ make_ಮೌಲ್ಯ_change_part(ದತ್ತಾಂಶ);
      }

      ಆದರೆ (ದತ್ತಾಂಶ->obj->get_type_ಸಂಕೇತ() == vpi_ಸ್ಮರಣೆ_ಶಬ್ದ)
	    ಹಿಂತಿರುಗಿ vpip_array_ಶಬ್ದ_change(ದತ್ತಾಂಶ);

      ಆದರೆ (ದತ್ತಾಂಶ->obj->get_type_ಸಂಕೇತ() == vpi_ಸ್ಮರಣೆ)
	    ಹಿಂತಿರುಗಿ vpip_array_change(ದತ್ತಾಂಶ);

      ಮೌಲ್ಯ_callback*obj = ಹೊಸ ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ);

      ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ->obj);
      ಆಯ್ಕೆ (ದತ್ತಾಂಶ->obj->get_type_ಸಂಕೇತ()) {

	  ಸ್ಥಿತಿ vpiReg:
	  ಸ್ಥಿತಿ vpiNet:
	  ಸ್ಥಿತಿ vpiIntegerVar:
	  ಸ್ಥಿತಿ vpi_ಬಿಟ್_Var:
	  ಸ್ಥಿತಿ vpiByteVar:
	  ಸ್ಥಿತಿ vpiShortIntVar:
	  ಸ್ಥಿತಿ vpiIntVar:
	  ಸ್ಥಿತಿ vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    ರಚನೆ __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(ದತ್ತಾಂಶ->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ಗಂಟು->fil);
	    ಪ್ರತಿಜ್ಞಾ(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->ಕೂಡಿಸು_vpi_callback(obj);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiRealVar:
	    vpip_real_ಮೌಲ್ಯ_change(obj, ದತ್ತಾಂಶ->obj);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(ದತ್ತಾಂಶ->obj);
	    nev->ಕೂಡಿಸು_vpi_callback(obj);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiModule:
	  ಸ್ಥಿತಿ vpiConstant:
	  ಸ್ಥಿತಿ vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    ದತ್ತಾಂಶ->obj->get_type_ಸಂಕೇತ());
	    ಅಳಿಸು obj;
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಹಿಂತಿರುಗಿ obj;
}

ವರ್ಗ sync_callback : ಸಾರ್ವಜನಿಕ __vpiCallback {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ sync_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ);
      ~sync_callback() override;

      sync_callback(ಸ್ಥಿರಾಂಕ sync_callback&) = ಅಳಿಸು;
      sync_callback& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ sync_callback&) = ಅಳಿಸು;

    ಸಾರ್ವಜನಿಕ:
	// scheduled event
      ರಚನೆ sync_cb* cb_sync;
	// user supplied callback data
      ರಚನೆ t_vpi_ಸಮಯ cb_ಸಮಯ;

    ಖಾಸಗಿ:
};

ಸಾಲೊಳಗೆ sync_callback::sync_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      cb_sync = 0;

      cb_ದತ್ತಾಂಶ = *ದತ್ತಾಂಶ;
      ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ->time);
      cb_ಸಮಯ = *(ದತ್ತಾಂಶ->time);
      cb_ದತ್ತಾಂಶ.time = &cb_ಸಮಯ;
}

sync_callback::~sync_callback()
{
      ಅಳಿಸು cb_sync;
}

ಶೂನ್ಯ sync_cb::ಸತತ_ಸತತ()
{
      ಆದರೆ (handle == 0)
	    ಹಿಂತಿರುಗಿ;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
	    ಗಣ_callback_ಸಮಯ(&cur->cb_ದತ್ತಾಂಶ);
	    ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
	    vpi_ಬಹುಲಕ_flag = sync_flag? VPI_ಬಹುಲಕ_ROSYNC : VPI_ಬಹುಲಕ_RWSYNC;
	    (cur->cb_ದತ್ತಾಂಶ.cb_rtn)(&cur->cb_ದತ್ತಾಂಶ);
	    vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
      }

      ಅಳಿಸು cur;
}

ಸ್ಥಿರ vvp_time64_t get_sync_cb_ಸಮಯ(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      ಆಯ್ಕೆ (obj->cb_ಸಮಯ.type) {
	  ಸ್ಥಿತಿ vpi_Sim_ಸಮಯ:
	    tv = vpip_timestruct_ಗಮ್ಯ_ಸಮಯ(&obj->cb_ಸಮಯ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpi_Scaled_Real_ಸಮಯ:
	    tv = vpip_scaled_real_ಗಮ್ಯ_time64(obj->cb_ಸಮಯ.real,
		     vpip_timescale_scope_from_handle(obj->cb_ದತ್ತಾಂಶ.obj));
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (ಪೂರ್ಣಾಂಕ)obj->cb_ಸಮಯ.type);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }
      ಹಿಂತಿರುಗಿ tv;
}

ಸ್ಥಿರ sync_callback* make_sync(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ, ತಾರ್ಕಿಕ readonly_flag)
{
      ಆದರೆ (!check_callback_ಸಮಯ(ದತ್ತಾಂಶ, ಸುಳ್ಳು))
	    ಹಿಂತಿರುಗಿ 0;

      sync_callback*obj = ಹೊಸ sync_callback(ದತ್ತಾಂಶ);

      ರಚನೆ sync_cb*cb = ಹೊಸ sync_cb;
      cb->sync_flag = readonly_flag? ನಿಜ : ಸುಳ್ಳು;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ಸಮಯ(obj);
      ವೇಳಾಪಟ್ಟಿ_generic(cb, tv, ನಿಜ, readonly_flag);

      ಹಿಂತಿರುಗಿ obj;
}

ಸ್ಥಿರ ರಚನೆ __vpiCallback* make_afterdelay(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ಆದರೆ (!check_callback_ಸಮಯ(ದತ್ತಾಂಶ, ಸುಳ್ಳು))
	    ಹಿಂತಿರುಗಿ 0;

      sync_callback*obj = ಹೊಸ sync_callback(ದತ್ತಾಂಶ);
      ರಚನೆ sync_cb*cb = ಹೊಸ sync_cb;
      cb->sync_flag = ಸುಳ್ಳು;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ಸಮಯ(obj);
      ವೇಳಾಪಟ್ಟಿ_generic(cb, tv, ಸುಳ್ಳು);

      ಹಿಂತಿರುಗಿ obj;
}

ಸ್ಥಿರ ರಚನೆ __vpiCallback* make_at_ಆರಂಭ_ದ_sim_ಸಮಯ(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ಆದರೆ (!check_callback_ಸಮಯ(ದತ್ತಾಂಶ, ಸುಳ್ಳು))
	    ಹಿಂತಿರುಗಿ 0;

      sync_callback*obj = ಹೊಸ sync_callback(ದತ್ತಾಂಶ);
      ರಚನೆ sync_cb*cb = ಹೊಸ sync_cb;
      cb->sync_flag = ಸುಳ್ಳು;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ಸಮಯ(obj);
      vvp_time64_t cur = ವೇಳಾಪಟ್ಟಿ_simtime();
      ಆದರೆ (cur > tv) {
	    tv = 0;
	    ಪ್ರತಿಜ್ಞಾ(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur == tv) {
	    tv = 0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    tv -= cur;
      }
      ವೇಳಾಪಟ್ಟಿ_at_ಆರಂಭ_ದ_simtime(cb, tv);

      ಹಿಂತಿರುಗಿ obj;
}

ಸ್ಥಿರ ರಚನೆ __vpiCallback* make_at_ಕೊನೆ_ದ_sim_ಸಮಯ(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ಆದರೆ (!check_callback_ಸಮಯ(ದತ್ತಾಂಶ, ಸುಳ್ಳು))
	    ಹಿಂತಿರುಗಿ 0;

      sync_callback*obj = ಹೊಸ sync_callback(ದತ್ತಾಂಶ);
      ರಚನೆ sync_cb*cb = ಹೊಸ sync_cb;
      cb->sync_flag = ಸುಳ್ಳು;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ಸಮಯ(obj);
      vvp_time64_t cur = ವೇಳಾಪಟ್ಟಿ_simtime();
      ಆದರೆ (cur > tv) {
	    tv = 0;
	    ಪ್ರತಿಜ್ಞಾ(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur == tv) {
	    tv = 0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    tv -= cur;
      }
      ವೇಳಾಪಟ್ಟಿ_at_ಕೊನೆ_ದ_simtime(cb, tv);

      ಹಿಂತಿರುಗಿ obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

ವರ್ಗ simulator_callback : ಸಾರ್ವಜನಿಕ __vpiCallback {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ simulator_callback(ಸ್ಥಿರಾಂಕ ರಚನೆ t_cb_ದತ್ತಾಂಶ*ದತ್ತಾಂಶ);

    ಸಾರ್ವಜನಿಕ:
      ರಚನೆ t_vpi_ಸಮಯ cb_ಸಮಯ;
};

ಸಾಲೊಳಗೆ simulator_callback::simulator_callback(ಸ್ಥಿರಾಂಕ ರಚನೆ t_cb_ದತ್ತಾಂಶ*ದತ್ತಾಂಶ)
{
      cb_ದತ್ತಾಂಶ = *ದತ್ತಾಂಶ;
      ಆದರೆ ((ದತ್ತಾಂಶ->reason == cb_ಮುಂದಿನ_Sim_ಸಮಯ) && ದತ್ತಾಂಶ->time) {
	    cb_ಸಮಯ = *(ದತ್ತಾಂಶ->time);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದತ್ತಾಂಶ->reason == cb_ಕೊನೆ_ದ_Simulation) {
	    cb_ಸಮಯ.type = vpi_Sim_ಸಮಯ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    cb_ಸಮಯ.type = vpi_Suppress_ಸಮಯ;
      }
      cb_ದತ್ತಾಂಶ.time = &cb_ಸಮಯ;
}

ಸ್ಥಿರ simulator_callback*ಮುಂದಿನ_Sim_ಸಮಯ = 0;
ಸ್ಥಿರ simulator_callback*ಕೊನೆ_ದ_Compile = 0;
ಸ್ಥಿರ simulator_callback*ಆರಂಭ_ದ_Simulation = 0;
ಸ್ಥಿರ simulator_callback*ಕೊನೆ_ದ_Simulation = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
ಶೂನ್ಯ simulator_cb_delete(ಶೂನ್ಯ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      ಆಗ (ಕೊನೆ_ದ_Compile) {
	    cur = ಕೊನೆ_ದ_Compile;
	    ಕೊನೆ_ದ_Compile = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
	    ಅಳಿಸು cur;
      }

	/* Delete all the start of simulation callbacks. */
      ಆಗ (ಆರಂಭ_ದ_Simulation) {
	    cur = ಆರಂಭ_ದ_Simulation;
	    ಆರಂಭ_ದ_Simulation = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
	    ಅಳಿಸು cur;
      }

	/* Delete all the end of simulation callbacks. */
      ಆಗ (ಕೊನೆ_ದ_Simulation) {
	    cur = ಕೊನೆ_ದ_Simulation;
	    ಕೊನೆ_ದ_Simulation = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
	    ಅಳಿಸು cur;
      }
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vpi_ಕೊನೆ_ದ_Compile(ಶೂನ್ಯ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_RWSYNC;

      ಆಗ (ಕೊನೆ_ದ_Compile) {
	    cur = ಕೊನೆ_ದ_Compile;
	    ಕೊನೆ_ದ_Compile = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
	    ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
		  (cur->cb_ದತ್ತಾಂಶ.cb_rtn)(&cur->cb_ದತ್ತಾಂಶ);
	    }
	    ಅಳಿಸು cur;
      }

      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
}

ಶೂನ್ಯ vpi_ಆರಂಭ_ದ_Sim(ಶೂನ್ಯ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_RWSYNC;

      ಆಗ (ಆರಂಭ_ದ_Simulation) {
	    cur = ಆರಂಭ_ದ_Simulation;
	    ಆರಂಭ_ದ_Simulation = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
	    ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
		  (cur->cb_ದತ್ತಾಂಶ.cb_rtn)(&cur->cb_ದತ್ತಾಂಶ);
	    }
	    ಅಳಿಸು cur;
      }

      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
}

ಶೂನ್ಯ vpiPostsim(ಶೂನ್ಯ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_ROSYNC;

      ಆಗ (ಕೊನೆ_ದ_Simulation) {
	    cur = ಕೊನೆ_ದ_Simulation;
	    ಕೊನೆ_ದ_Simulation = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
	    ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
		  ಗಣ_callback_ಸಮಯ(&cur->cb_ದತ್ತಾಂಶ);
		  (cur->cb_ದತ್ತಾಂಶ.cb_rtn)(&cur->cb_ದತ್ತಾಂಶ);
	    }
	    ಅಳಿಸು cur;
      }

      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
ಶೂನ್ಯ vpi_ಮುಂದಿನ_Sim_ಸಮಯ(ಶೂನ್ಯ)
{
      simulator_callback* cur;
      simulator_callback* ಮುಂದಿನ_2 = ಮುಂದಿನ_Sim_ಸಮಯ;
      ಮುಂದಿನ_Sim_ಸಮಯ = 0;

      ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_RWSYNC;

      ಆಗ (ಮುಂದಿನ_2) {
            cur = ಮುಂದಿನ_2;
            ಮುಂದಿನ_2 = dynamic_cast<simulator_callback*>(cur->ಮುಂದಿನ_2);
            ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
                  ಗಣ_callback_ಸಮಯ(&cur->cb_ದತ್ತಾಂಶ);
                  (cur->cb_ದತ್ತಾಂಶ.cb_rtn)(&cur->cb_ದತ್ತಾಂಶ);
            }
            ಅಳಿಸು cur;
      }

      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
}

ಸ್ಥಿರ simulator_callback* make_prepost(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ಆದರೆ ((ದತ್ತಾಂಶ->reason == cb_ಮುಂದಿನ_Sim_ಸಮಯ) && !check_callback_ಸಮಯ(ದತ್ತಾಂಶ, ನಿಜ))
	    ಹಿಂತಿರುಗಿ 0;

      simulator_callback*obj = ಹೊಸ simulator_callback(ದತ್ತಾಂಶ);

      /* Insert at head of list */
      ಆಯ್ಕೆ (ದತ್ತಾಂಶ->reason) {
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Compile:
	    obj->ಮುಂದಿನ_2 = ಕೊನೆ_ದ_Compile;
	    ಕೊನೆ_ದ_Compile = obj;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ cb_ಆರಂಭ_ದ_Simulation:
	    obj->ಮುಂದಿನ_2 = ಆರಂಭ_ದ_Simulation;
	    ಆರಂಭ_ದ_Simulation = obj;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Simulation:
	    obj->ಮುಂದಿನ_2 = ಕೊನೆ_ದ_Simulation;
	    ಕೊನೆ_ದ_Simulation = obj;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ cb_ಮುಂದಿನ_Sim_ಸಮಯ:
	    obj->ಮುಂದಿನ_2 = ಮುಂದಿನ_Sim_ಸಮಯ;
	    ಮುಂದಿನ_Sim_ಸಮಯ = obj;
	    ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpi_ರಿಜಿಸ್ಟರ್_cb(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ರಚನೆ __vpiCallback*obj = 0;

      ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ);
      ಆಯ್ಕೆ (ದತ್ತಾಂಶ->reason) {

	  ಸ್ಥಿತಿ cb_ಮೌಲ್ಯ_Change:
	    obj = make_ಮೌಲ್ಯ_change(ದತ್ತಾಂಶ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ cbReadOnlySynch:
	    obj = make_sync(ದತ್ತಾಂಶ, ನಿಜ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ cbReadWriteSynch:
	    obj = make_sync(ದತ್ತಾಂಶ, ಸುಳ್ಳು);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ cb_At_ಆರಂಭ_ದ_Sim_ಸಮಯ:
	    obj = make_at_ಆರಂಭ_ದ_sim_ಸಮಯ(ದತ್ತಾಂಶ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ cb_At_ಕೊನೆ_ದ_Sim_ಸಮಯ:
	    obj = make_at_ಕೊನೆ_ದ_sim_ಸಮಯ(ದತ್ತಾಂಶ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ cbAfterDelay:
	    obj = make_afterdelay(ದತ್ತಾಂಶ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Compile:
	  ಸ್ಥಿತಿ cb_ಆರಂಭ_ದ_Simulation:
	  ಸ್ಥಿತಿ cb_ಕೊನೆ_ದ_Simulation:
	  ಸ್ಥಿತಿ cb_ಮುಂದಿನ_Sim_ಸಮಯ:
	    obj = make_prepost(ದತ್ತಾಂಶ);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->reason);
	    ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      ರಚನೆ __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      obj->cb_ದತ್ತಾಂಶ.cb_rtn = 0;

      ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ callback_execute(ರಚನೆ __vpiCallback*cur)
{
      ಸ್ಥಿರಾಂಕ vpi_ಬಹುಲಕ_t save_ಬಹುಲಕ = vpi_ಬಹುಲಕ_flag;
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_RWSYNC;

      ಪ್ರತಿಜ್ಞಾ(cur->cb_ದತ್ತಾಂಶ.cb_rtn);
      ಗಣ_callback_ಸಮಯ(&cur->cb_ದತ್ತಾಂಶ);
      (cur->cb_ದತ್ತಾಂಶ.cb_rtn)(&cur->cb_ದತ್ತಾಂಶ);

      vpi_ಬಹುಲಕ_flag = save_ಬಹುಲಕ;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
ರಚನೆ __vpi_array_word {
      ರಚನೆ __vpi_array_word* ಮುಂದಿನ_2;
      ರಚನೆ __vpiArray* array;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಶಬ್ದ;
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

ಶೂನ್ಯ vvp_vpi_callback::attach_as_ಶಬ್ದ(vvp_array_t arr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
      ರಚನೆ __vpi_array_word*tmp = ಹೊಸ __vpi_array_word;
      tmp->array = arr;
      tmp->ಶಬ್ದ = addr;
      tmp->ಮುಂದಿನ_2 = array_ಪದಗಳು;
      array_ಪದಗಳು = tmp;
}

ಶೂನ್ಯ vvp_vpi_callback::ಕೂಡಿಸು_vpi_callback(ಮೌಲ್ಯ_callback*cb)
{
      cb->ಮುಂದಿನ_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_vpi_callback::clear_all_callbacks()
{
      ಆಗ (vpi_callbacks_) {
	    ಮೌಲ್ಯ_callback *tmp = dynamic_cast<ಮೌಲ್ಯ_callback*>
	                            (vpi_callbacks_->ಮುಂದಿನ_2);
	    ಅಳಿಸು vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      ಆಗ (array_ಪದಗಳು) {
	    ರಚನೆ __vpi_array_word*tmp = array_ಪದಗಳು->ಮುಂದಿನ_2;
	    ಅಳಿಸು array_ಪದಗಳು;
	    array_ಪದಗಳು = tmp;
      }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
ಶೂನ್ಯ vvp_vpi_callback::ಸತತ_vpi_callbacks()
{
      ರಚನೆ __vpi_array_word*array_ಶಬ್ದ = array_ಪದಗಳು;
      ಆಗ (array_ಶಬ್ದ) {
	    array_ಶಬ್ದ->array->ಶಬ್ದ_change(array_ಶಬ್ದ->ಶಬ್ದ);
	    array_ಶಬ್ದ = array_ಶಬ್ದ->ಮುಂದಿನ_2;
      }

      ಮೌಲ್ಯ_callback *ಮುಂದಿನ_2 = vpi_callbacks_;
      ಮೌಲ್ಯ_callback *prev = 0;

      ಆಗ (ಮುಂದಿನ_2) {
	    ಮೌಲ್ಯ_callback*cur = ಮುಂದಿನ_2;
	    ಮುಂದಿನ_2 = dynamic_cast<ಮೌಲ್ಯ_callback*>(cur->ಮುಂದಿನ_2);

	    ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
		  ಆದರೆ (cur->test_ಮೌಲ್ಯ_callback_ready()) {
			ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.value)
			      get_ಮೌಲ್ಯ(cur->cb_ದತ್ತಾಂಶ.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prev == 0) {

		  vpi_callbacks_ = ಮುಂದಿನ_2;
		  cur->ಮುಂದಿನ_2 = 0;
		  ಅಳಿಸು cur;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಪ್ರತಿಜ್ಞಾ(prev->ಮುಂದಿನ_2 == cur);
		  prev->ಮುಂದಿನ_2 = ಮುಂದಿನ_2;
		  cur->ಮುಂದಿನ_2 = 0;
		  ಅಳಿಸು cur;
	    }
      }
}

ಶೂನ್ಯ vvp_signal_ಮೌಲ್ಯ::get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp)
{
      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal:
	  ಸ್ಥಿತಿ vpiOctStrVal:
	  ಸ್ಥಿತಿ vpiDecStrVal:
	  ಸ್ಥಿತಿ vpiHexStrVal:
	  ಸ್ಥಿತಿ vpiIntVal:
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	  ಸ್ಥಿತಿ vpiStringVal:
	  ಸ್ಥಿತಿ vpiRealVal: {
	    ಚಿಹ್ನೆರಹಿತ wid = ಮೌಲ್ಯ_ಗಾತ್ರ();
	    vvp_vector4_t vec4(wid);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < wid; idx += 1) {
		  vec4.ಗಣ_ಬಿಟ್(idx, value(idx));
	    }
	    vpip_vec4_get_ಮೌಲ್ಯ(vec4, wid, ಸುಳ್ಳು, vp);
	    ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpiSuppressVal:
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (ಪೂರ್ಣಾಂಕ)vp->format);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ real_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp, ದ್ವಿಗುಣ rval)
{
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_ಗಾತ್ರ = 64 + 1;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_ಗಾತ್ರ, RBUF_VAL));

      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ಸ್ಥಿತಿ vpiRealVal:
	    vp->value.real = rval;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ಆದರೆ (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  rval = std::ಸುತ್ತು_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    ಆದರೆ (std::isnan(rval))
		  snprintf(rbuf, RBUF_ಗಾತ್ರ, "%s", "nan");
	    ಇಲ್ಲದಿದ್ದರೆ
		  snprintf(rbuf, RBUF_ಗಾತ್ರ, "%0.0f", std::ಸುತ್ತು_2(rval));
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiHexStrVal:
	    snprintf(rbuf, RBUF_ಗಾತ್ರ, "%" PRIx64, vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(rval));
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal: {
		uint64_t val = vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(rval);
		ಚಿಹ್ನೆರಹಿತ len = 0;

		ಆಗ (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(rval);
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		ಆದರೆ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpiSuppressVal:
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (ಪೂರ್ಣಾಂಕ)vp->format);
      }
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp)
{
      real_signal_ಮೌಲ್ಯ(vp, real_ಮೌಲ್ಯ());
}

ಶೂನ್ಯ vvp_wire_real::get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp)
{
      real_signal_ಮೌಲ್ಯ(vp, real_ಮೌಲ್ಯ());
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
#ಆದರೆ 0
ಶೂನ್ಯ vvp_wire_string::get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ vvp_wire_vec4::get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*val)
{
      get_signal_ಮೌಲ್ಯ(val);
}

ಶೂನ್ಯ vvp_wire_vec8::get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*val)
{
      get_signal_ಮೌಲ್ಯ(val);
}

ಶೂನ್ಯ vvp_wire_real::get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*val)
{
      get_signal_ಮೌಲ್ಯ(val);
}
#ಆದರೆ 0
ಶೂನ್ಯ vvp_wire_string::get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*val)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
#ಆದರೆ_ಅಂತ್ಯ
