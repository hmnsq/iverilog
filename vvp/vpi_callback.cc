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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#جے_انت
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

ورتوں نطاق std;

ساكن ثابت حرف*cb_reason_name(PLI_INT32 reason)
{
      بدل (reason) {
	  حالة cb_قدر_Change:
	    أرجع "cbValueChange";
	  حالة cbStmt:
	    أرجع "cbStmt";
	  حالة cbForce:
	    أرجع "cbForce";
	  حالة cbRelease:
	    أرجع "cbRelease";
	  حالة cb_At_شروع_دا_Sim_ویلا:
	    أرجع "cbAtStartOfSimTime";
	  حالة cbReadWriteSynch:
	    أرجع "cbReadWriteSynch";
	  حالة cbReadOnlySynch:
	    أرجع "cbReadOnlySynch";
	  حالة cb_اگلا_Sim_ویلا:
	    أرجع "cbNextSimTime";
	  حالة cbAfterDelay:
	    أرجع "cbAfterDelay";
	  حالة cb_سرا_دا_Compile:
	    أرجع "cbEndOfCompile";
	  حالة cb_شروع_دا_Simulation:
	    أرجع "cbStartOfSimulation";
	  حالة cb_سرا_دا_Simulation:
	    أرجع "cbEndOfSimulation";
	  حالة cbError:
	    أرجع "cbError";
	  حالة cbTchkViolation:
	    أرجع "cbTchkViolation";
	  حالة cb_شروع_دا_Save:
	    أرجع "cbStartOfSave";
	  حالة cb_سرا_دا_Save:
	    أرجع "cbEndOfSave";
	  حالة cb_شروع_دا_Restart:
	    أرجع "cbStartOfRestart";
	  حالة cb_سرا_دا_Restart:
	    أرجع "cbEndOfRestart";
	  حالة cb_شروع_دا_Reset:
	    أرجع "cbStartOfReset";
	  حالة cb_سرا_دا_Reset:
	    أرجع "cbEndOfReset";
	  حالة cbEnterInteractive:
	    أرجع "cbEnterInteractive";
	  حالة cbExitInteractive:
	    أرجع "cbExitInteractive";
	  حالة cbInteractiveScopeChange:
	    أرجع "cbInteractiveScopeChange";
	  حالة cbUnresolvedSystf:
	    أرجع "cbUnresolvedSystf";
	  حالة cb_At_سرا_دا_Sim_ویلا:
	    أرجع "cbAtEndOfSimTime";
	  افتراضي:
	    أرجع "unrecognised";
      }
}

ساكن منطقي check_callback_ویلا(p_cb_انکڑے انکڑے, منطقي allow_suppress)
{
      أكد(انکڑے);
      إذا (!انکڑے->time) {
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(انکڑے->reason));
		  أرجع خطأ;
	    }
	    أرجع سچ;
      }
      بدل (انکڑے->time->type) {
	  حالة vpi_Sim_ویلا:
	    اكسر;
	  حالة vpi_Scaled_Real_ویلا:
	    اكسر;
	  حالة vpi_Suppress_ویلا:
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(انکڑے->reason));
		  أرجع خطأ;
	    }
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(انکڑے->reason));
	    أرجع خطأ;
      }
      أرجع سچ;
}

ساكن فراغ سیٹ_callback_ویلا(p_cb_انکڑے انکڑے)
{
      أكد(انکڑے && انکڑے->time);
      انکڑے->time->کم  = 0;
      انکڑے->time->بلند = 0;
      انکڑے->time->real = 0.0;
      بدل (انکڑے->time->type) {
	  حالة vpi_Sim_ویلا:
	    vpip_ویلا_منزل_timestruct(انکڑے->time, ویلا_سارنی_simtime());
	    اكسر;
	  حالة vpi_Scaled_Real_ویلا:
	    انکڑے->time->real = vpip_scaled_ویلا_from_handle(ویلا_سارنی_simtime(), انکڑے->obj);
	    اكسر;
	  حالة vpi_Suppress_ویلا:
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

      مجازی فراغ لگاتار_لگاتار() override;
};

مضمن __vpiCallback::__vpiCallback()
{
      اگلا_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحيح __vpiCallback::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiCallback; }


قدر_callback::قدر_callback(p_cb_انکڑے انکڑے)
{
      cb_انکڑے = *انکڑے;
      إذا (انکڑے->time) {
	    cb_ویلا = *(انکڑے->time);
      } وإلا {
	    cb_ویلا.type = vpi_Suppress_ویلا;
      }
      cb_انکڑے.time = &cb_ویلا;
      إذا (انکڑے->value) {
	    cb_قدر = *(انکڑے->value);
      } وإلا {
	    cb_قدر.format = vpiSuppressVal;
      }
      cb_انکڑے.value = &cb_قدر;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
منطقي قدر_callback::test_قدر_callback_ready(فراغ)
{
      أرجع سچ;
}

ساكن فراغ vpip_real_قدر_change(قدر_callback*cbh, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      أكد(obj);

      obj->جوڑو_vpi_callback(cbh);
}

صنف قدر_part_callback : عام قدر_callback {
    عام:
      واضح قدر_part_callback(p_cb_انکڑے انکڑے);
      ~قدر_part_callback() override;

      قدر_part_callback(ثابت قدر_part_callback&) = مٹاؤ;
      قدر_part_callback& عامل=(ثابت قدر_part_callback&) = مٹاؤ;

      منطقي test_قدر_callback_ready(فراغ) override;

    خاص:
      حرف*قدر_بٹ;
      size_t قدر_off;
};

مضمن قدر_part_callback::قدر_part_callback(p_cb_انکڑے انکڑے)
: قدر_callback(انکڑے)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(انکڑے->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->جوڑو_vpi_callback(هذا);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      sig_fil->get_قدر(&tmp_قدر);

      قدر_بٹ = جديد حرف[pobj->width+1];
      قدر_off = pobj->ماپا->vpi_get(vpi_ناپ) - pobj->width - pobj->tbase;

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str + قدر_off, pobj->width);
      قدر_بٹ[pobj->width] = 0;
}

قدر_part_callback::~قدر_part_callback()
{
      مٹاؤ[]قدر_بٹ;
}

منطقي قدر_part_callback::test_قدر_callback_ready(فراغ)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_انکڑے.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      sig_fil->get_قدر(&tmp_قدر);

      إذا (memcmp(قدر_بٹ, tmp_قدر.value.str + قدر_off, pobj->width) == 0)
	    أرجع خطأ;

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str + قدر_off, pobj->width);
      أرجع سچ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ساكن قدر_callback*make_قدر_change_part(p_cb_انکڑے انکڑے)
{
	/* Attach the __vpiCallback object to the signal. */
      قدر_callback*cbh = جديد قدر_part_callback(انکڑے);
      أرجع cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ساكن قدر_callback* make_قدر_change(p_cb_انکڑے انکڑے)
{
      إذا (!check_callback_ویلا(انکڑے, سچ))
	    أرجع 0;

      إذا (vpi_get(vpiAutomatic, انکڑے->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, انکڑے->obj));
            أرجع 0;
      }

	// Special case: the target object is a vpiPartSelect
      إذا (انکڑے->obj->get_type_کوڈ() == vpiPartSelect) {
            إذا (انکڑے->obj->vpi_handle(vpiArray))
	        أرجع vpip_array_لفظ_change(انکڑے);
            وإلا
	        أرجع make_قدر_change_part(انکڑے);
      }

      إذا (انکڑے->obj->get_type_کوڈ() == vpi_حافظہ_لفظ)
	    أرجع vpip_array_لفظ_change(انکڑے);

      إذا (انکڑے->obj->get_type_کوڈ() == vpi_حافظہ)
	    أرجع vpip_array_change(انکڑے);

      قدر_callback*obj = جديد قدر_callback(انکڑے);

      أكد(انکڑے->obj);
      بدل (انکڑے->obj->get_type_کوڈ()) {

	  حالة vpiReg:
	  حالة vpiNet:
	  حالة vpiIntegerVar:
	  حالة vpi_ثنائی_ہندسہ_Var:
	  حالة vpiByteVar:
	  حالة vpiShortIntVar:
	  حالة vpiIntVar:
	  حالة vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    بنية __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(انکڑے->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->گرہ->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->جوڑو_vpi_callback(obj);
	    اكسر;

	  حالة vpiRealVar:
	    vpip_real_قدر_change(obj, انکڑے->obj);
	    اكسر;

	  حالة vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(انکڑے->obj);
	    nev->جوڑو_vpi_callback(obj);
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
		    انکڑے->obj->get_type_کوڈ());
	    مٹاؤ obj;
	    أرجع 0;
      }

      أرجع obj;
}

صنف sync_callback : عام __vpiCallback {
    عام:
      واضح sync_callback(p_cb_انکڑے انکڑے);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = مٹاؤ;
      sync_callback& عامل=(ثابت sync_callback&) = مٹاؤ;

    عام:
	// scheduled event
      بنية sync_cb* cb_sync;
	// user supplied callback data
      بنية t_vpi_ویلا cb_ویلا;

    خاص:
};

مضمن sync_callback::sync_callback(p_cb_انکڑے انکڑے)
{
      cb_sync = 0;

      cb_انکڑے = *انکڑے;
      أكد(انکڑے->time);
      cb_ویلا = *(انکڑے->time);
      cb_انکڑے.time = &cb_ویلا;
}

sync_callback::~sync_callback()
{
      مٹاؤ cb_sync;
}

فراغ sync_cb::لگاتار_لگاتار()
{
      إذا (handle == 0)
	    أرجع;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      إذا (cur->cb_انکڑے.cb_rtn != 0) {
	    سیٹ_callback_ویلا(&cur->cb_انکڑے);
	    أكد(vpi_منوال_flag == VPI_منوال_NONE);
	    vpi_منوال_flag = sync_flag? VPI_منوال_ROSYNC : VPI_منوال_RWSYNC;
	    (cur->cb_انکڑے.cb_rtn)(&cur->cb_انکڑے);
	    vpi_منوال_flag = VPI_منوال_NONE;
      }

      مٹاؤ cur;
}

ساكن vvp_time64_t get_sync_cb_ویلا(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      بدل (obj->cb_ویلا.type) {
	  حالة vpi_Sim_ویلا:
	    tv = vpip_timestruct_منزل_ویلا(&obj->cb_ویلا);
	    اكسر;

	  حالة vpi_Scaled_Real_ویلا:
	    tv = vpip_scaled_real_منزل_time64(obj->cb_ویلا.real,
		     vpip_timescale_scope_from_handle(obj->cb_انکڑے.obj));
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحيح)obj->cb_ویلا.type);
	    أكد(0);
	    اكسر;
      }
      أرجع tv;
}

ساكن sync_callback* make_sync(p_cb_انکڑے انکڑے, منطقي readonly_flag)
{
      إذا (!check_callback_ویلا(انکڑے, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(انکڑے);

      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = readonly_flag? سچ : خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ویلا(obj);
      ویلا_سارنی_generic(cb, tv, سچ, readonly_flag);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_afterdelay(p_cb_انکڑے انکڑے)
{
      إذا (!check_callback_ویلا(انکڑے, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(انکڑے);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ویلا(obj);
      ویلا_سارنی_generic(cb, tv, خطأ);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_شروع_دا_sim_ویلا(p_cb_انکڑے انکڑے)
{
      إذا (!check_callback_ویلا(انکڑے, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(انکڑے);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ویلا(obj);
      vvp_time64_t cur = ویلا_سارنی_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      ویلا_سارنی_at_شروع_دا_simtime(cb, tv);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_سرا_دا_sim_ویلا(p_cb_انکڑے انکڑے)
{
      إذا (!check_callback_ویلا(انکڑے, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(انکڑے);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_ویلا(obj);
      vvp_time64_t cur = ویلا_سارنی_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      ویلا_سارنی_at_سرا_دا_simtime(cb, tv);

      أرجع obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

صنف simulator_callback : عام __vpiCallback {
    عام:
      واضح simulator_callback(ثابت بنية t_cb_انکڑے*انکڑے);

    عام:
      بنية t_vpi_ویلا cb_ویلا;
};

مضمن simulator_callback::simulator_callback(ثابت بنية t_cb_انکڑے*انکڑے)
{
      cb_انکڑے = *انکڑے;
      إذا ((انکڑے->reason == cb_اگلا_Sim_ویلا) && انکڑے->time) {
	    cb_ویلا = *(انکڑے->time);
      } وإلا إذا (انکڑے->reason == cb_سرا_دا_Simulation) {
	    cb_ویلا.type = vpi_Sim_ویلا;
      } وإلا {
	    cb_ویلا.type = vpi_Suppress_ویلا;
      }
      cb_انکڑے.time = &cb_ویلا;
}

ساكن simulator_callback*اگلا_Sim_ویلا = 0;
ساكن simulator_callback*سرا_دا_Compile = 0;
ساكن simulator_callback*شروع_دا_Simulation = 0;
ساكن simulator_callback*سرا_دا_Simulation = 0;

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
فراغ simulator_cb_delete(فراغ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      طالما (سرا_دا_Compile) {
	    cur = سرا_دا_Compile;
	    سرا_دا_Compile = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    مٹاؤ cur;
      }

	/* Delete all the start of simulation callbacks. */
      طالما (شروع_دا_Simulation) {
	    cur = شروع_دا_Simulation;
	    شروع_دا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    مٹاؤ cur;
      }

	/* Delete all the end of simulation callbacks. */
      طالما (سرا_دا_Simulation) {
	    cur = سرا_دا_Simulation;
	    سرا_دا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    مٹاؤ cur;
      }
}
#جے_انت

فراغ vpi_سرا_دا_Compile(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      طالما (سرا_دا_Compile) {
	    cur = سرا_دا_Compile;
	    سرا_دا_Compile = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    إذا (cur->cb_انکڑے.cb_rtn != 0) {
		  (cur->cb_انکڑے.cb_rtn)(&cur->cb_انکڑے);
	    }
	    مٹاؤ cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

فراغ vpi_شروع_دا_Sim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      طالما (شروع_دا_Simulation) {
	    cur = شروع_دا_Simulation;
	    شروع_دا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    إذا (cur->cb_انکڑے.cb_rtn != 0) {
		  (cur->cb_انکڑے.cb_rtn)(&cur->cb_انکڑے);
	    }
	    مٹاؤ cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

فراغ vpiPostsim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_ROSYNC;

      طالما (سرا_دا_Simulation) {
	    cur = سرا_دا_Simulation;
	    سرا_دا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    إذا (cur->cb_انکڑے.cb_rtn != 0) {
		  سیٹ_callback_ویلا(&cur->cb_انکڑے);
		  (cur->cb_انکڑے.cb_rtn)(&cur->cb_انکڑے);
	    }
	    مٹاؤ cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
فراغ vpi_اگلا_Sim_ویلا(فراغ)
{
      simulator_callback* cur;
      simulator_callback* اگلا_2 = اگلا_Sim_ویلا;
      اگلا_Sim_ویلا = 0;

      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      طالما (اگلا_2) {
            cur = اگلا_2;
            اگلا_2 = dynamic_cast<simulator_callback*>(cur->اگلا_2);
            إذا (cur->cb_انکڑے.cb_rtn != 0) {
                  سیٹ_callback_ویلا(&cur->cb_انکڑے);
                  (cur->cb_انکڑے.cb_rtn)(&cur->cb_انکڑے);
            }
            مٹاؤ cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

ساكن simulator_callback* make_prepost(p_cb_انکڑے انکڑے)
{
      إذا ((انکڑے->reason == cb_اگلا_Sim_ویلا) && !check_callback_ویلا(انکڑے, سچ))
	    أرجع 0;

      simulator_callback*obj = جديد simulator_callback(انکڑے);

      /* Insert at head of list */
      بدل (انکڑے->reason) {
	  حالة cb_سرا_دا_Compile:
	    obj->اگلا_2 = سرا_دا_Compile;
	    سرا_دا_Compile = obj;
	    اكسر;
	  حالة cb_شروع_دا_Simulation:
	    obj->اگلا_2 = شروع_دا_Simulation;
	    شروع_دا_Simulation = obj;
	    اكسر;
	  حالة cb_سرا_دا_Simulation:
	    obj->اگلا_2 = سرا_دا_Simulation;
	    سرا_دا_Simulation = obj;
	    اكسر;
	  حالة cb_اگلا_Sim_ویلا:
	    obj->اگلا_2 = اگلا_Sim_ویلا;
	    اگلا_Sim_ویلا = obj;
	    اكسر;
      }

      أرجع obj;
}

vpiHandle vpi_رجسٹر_cb(p_cb_انکڑے انکڑے)
{
      بنية __vpiCallback*obj = 0;

      أكد(انکڑے);
      بدل (انکڑے->reason) {

	  حالة cb_قدر_Change:
	    obj = make_قدر_change(انکڑے);
	    اكسر;

	  حالة cbReadOnlySynch:
	    obj = make_sync(انکڑے, سچ);
	    اكسر;

	  حالة cbReadWriteSynch:
	    obj = make_sync(انکڑے, خطأ);
	    اكسر;

	  حالة cb_At_شروع_دا_Sim_ویلا:
	    obj = make_at_شروع_دا_sim_ویلا(انکڑے);
	    اكسر;

	  حالة cb_At_سرا_دا_Sim_ویلا:
	    obj = make_at_سرا_دا_sim_ویلا(انکڑے);
	    اكسر;

	  حالة cbAfterDelay:
	    obj = make_afterdelay(انکڑے);
	    اكسر;

	  حالة cb_سرا_دا_Compile:
	  حالة cb_شروع_دا_Simulation:
	  حالة cb_سرا_دا_Simulation:
	  حالة cb_اگلا_Sim_ویلا:
	    obj = make_prepost(انکڑے);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (صحيح)انکڑے->reason);
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
      obj->cb_انکڑے.cb_rtn = 0;

      أرجع 1;
}

فراغ callback_execute(بنية __vpiCallback*cur)
{
      ثابت vpi_منوال_t save_منوال = vpi_منوال_flag;
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      أكد(cur->cb_انکڑے.cb_rtn);
      سیٹ_callback_ویلا(&cur->cb_انکڑے);
      (cur->cb_انکڑے.cb_rtn)(&cur->cb_انکڑے);

      vpi_منوال_flag = save_منوال;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
بنية __vpi_array_word {
      بنية __vpi_array_word* اگلا_2;
      بنية __vpiArray* array;
      بنانشانی لما لفظ;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_الفاظ = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      أكد(vpi_callbacks_ == 0);
      أكد(array_الفاظ == 0);
}

فراغ vvp_vpi_callback::attach_as_لفظ(vvp_array_t arr, بنانشانی لما addr)
{
      بنية __vpi_array_word*tmp = جديد __vpi_array_word;
      tmp->array = arr;
      tmp->لفظ = addr;
      tmp->اگلا_2 = array_الفاظ;
      array_الفاظ = tmp;
}

فراغ vvp_vpi_callback::جوڑو_vpi_callback(قدر_callback*cb)
{
      cb->اگلا_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_vpi_callback::clear_all_callbacks()
{
      طالما (vpi_callbacks_) {
	    قدر_callback *tmp = dynamic_cast<قدر_callback*>
	                            (vpi_callbacks_->اگلا_2);
	    مٹاؤ vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      طالما (array_الفاظ) {
	    بنية __vpi_array_word*tmp = array_الفاظ->اگلا_2;
	    مٹاؤ array_الفاظ;
	    array_الفاظ = tmp;
      }
}
#جے_انت

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
فراغ vvp_vpi_callback::لگاتار_vpi_callbacks()
{
      بنية __vpi_array_word*array_لفظ = array_الفاظ;
      طالما (array_لفظ) {
	    array_لفظ->array->لفظ_change(array_لفظ->لفظ);
	    array_لفظ = array_لفظ->اگلا_2;
      }

      قدر_callback *اگلا_2 = vpi_callbacks_;
      قدر_callback *prev = 0;

      طالما (اگلا_2) {
	    قدر_callback*cur = اگلا_2;
	    اگلا_2 = dynamic_cast<قدر_callback*>(cur->اگلا_2);

	    إذا (cur->cb_انکڑے.cb_rtn != 0) {
		  إذا (cur->test_قدر_callback_ready()) {
			إذا (cur->cb_انکڑے.value)
			      get_قدر(cur->cb_انکڑے.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks_ = اگلا_2;
		  cur->اگلا_2 = 0;
		  مٹاؤ cur;

	    } وإلا {
		  أكد(prev->اگلا_2 == cur);
		  prev->اگلا_2 = اگلا_2;
		  cur->اگلا_2 = 0;
		  مٹاؤ cur;
	    }
      }
}

فراغ vvp_signal_قدر::get_signal_قدر(بنية t_vpi_قدر*vp)
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
	  حالة vpi_سمتیہ_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal: {
	    بنانشانی wid = قدر_ناپ();
	    vvp_vector4_t vec4(wid);
	    لكل (بنانشانی idx = 0; idx < wid; idx += 1) {
		  vec4.سیٹ_ثنائی_ہندسہ(idx, value(idx));
	    }
	    vpip_vec4_get_قدر(vec4, wid, خطأ, vp);
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

ساكن فراغ real_signal_قدر(بنية t_vpi_قدر*vp, مزدوج rval)
{
      ساكن ثابت size_t RBUF_ناپ = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_ناپ, RBUF_VAL));

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
		  rval = std::گیڑا_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(rval))
		  snprintf(rbuf, RBUF_ناپ, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_ناپ, "%0.0f", std::گیڑا_2(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_ناپ, "%" PRIx64, vlg_گیڑا_منزل_u64(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t val = vlg_گیڑا_منزل_u64(rval);
		بنانشانی len = 0;

		طالما (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_گیڑا_منزل_u64(rval);
		لكل (بنانشانی idx = 0 ;  idx < len ;  idx += 1) {
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

فراغ vvp_fun_signal_real_aa::get_signal_قدر(بنية t_vpi_قدر*vp)
{
      real_signal_قدر(vp, real_قدر());
}

فراغ vvp_wire_real::get_signal_قدر(بنية t_vpi_قدر*vp)
{
      real_signal_قدر(vp, real_قدر());
}

فراغ vvp_fun_signal_string_aa::get_signal_قدر(بنية t_vpi_قدر*)
{
      أكد(0);
}
#إذا 0
فراغ vvp_wire_string::get_signal_قدر(بنية t_vpi_قدر*vp)
{
      أكد(0);
}
#جے_انت
فراغ vvp_wire_vec4::get_قدر(بنية t_vpi_قدر*val)
{
      get_signal_قدر(val);
}

فراغ vvp_wire_vec8::get_قدر(بنية t_vpi_قدر*val)
{
      get_signal_قدر(val);
}

فراغ vvp_wire_real::get_قدر(بنية t_vpi_قدر*val)
{
      get_signal_قدر(val);
}
#إذا 0
فراغ vvp_wire_string::get_قدر(بنية t_vpi_قدر*val)
{
      أكد(0);
}
#جے_انت
