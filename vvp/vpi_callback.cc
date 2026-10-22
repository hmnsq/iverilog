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
#که_تعریف_شوی CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#د_که_پای
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

کارول نطاق std;

ساكن ثابت حرف*cb_reason_name(PLI_INT32 reason)
{
      بدل (reason) {
	  حالة cb_ارزښت_Change:
	    أرجع "cbValueChange";
	  حالة cbStmt:
	    أرجع "cbStmt";
	  حالة cbForce:
	    أرجع "cbForce";
	  حالة cbRelease:
	    أرجع "cbRelease";
	  حالة cb_At_پیل_د_Sim_وخت:
	    أرجع "cbAtStartOfSimTime";
	  حالة cbReadWriteSynch:
	    أرجع "cbReadWriteSynch";
	  حالة cbReadOnlySynch:
	    أرجع "cbReadOnlySynch";
	  حالة cb_بل_Sim_وخت:
	    أرجع "cbNextSimTime";
	  حالة cbAfterDelay:
	    أرجع "cbAfterDelay";
	  حالة cb_پای_ټکی_د_Compile:
	    أرجع "cbEndOfCompile";
	  حالة cb_پیل_د_Simulation:
	    أرجع "cbStartOfSimulation";
	  حالة cb_پای_ټکی_د_Simulation:
	    أرجع "cbEndOfSimulation";
	  حالة cbError:
	    أرجع "cbError";
	  حالة cbTchkViolation:
	    أرجع "cbTchkViolation";
	  حالة cb_پیل_د_Save:
	    أرجع "cbStartOfSave";
	  حالة cb_پای_ټکی_د_Save:
	    أرجع "cbEndOfSave";
	  حالة cb_پیل_د_Restart:
	    أرجع "cbStartOfRestart";
	  حالة cb_پای_ټکی_د_Restart:
	    أرجع "cbEndOfRestart";
	  حالة cb_پیل_د_Reset:
	    أرجع "cbStartOfReset";
	  حالة cb_پای_ټکی_د_Reset:
	    أرجع "cbEndOfReset";
	  حالة cbEnterInteractive:
	    أرجع "cbEnterInteractive";
	  حالة cbExitInteractive:
	    أرجع "cbExitInteractive";
	  حالة cbInteractiveScopeChange:
	    أرجع "cbInteractiveScopeChange";
	  حالة cbUnresolvedSystf:
	    أرجع "cbUnresolvedSystf";
	  حالة cb_At_پای_ټکی_د_Sim_وخت:
	    أرجع "cbAtEndOfSimTime";
	  افتراضي:
	    أرجع "unrecognised";
      }
}

ساكن منطقي check_callback_وخت(p_cb_معلومات معلومات, منطقي allow_suppress)
{
      أكد(معلومات);
      إذا (!معلومات->time) {
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(معلومات->reason));
		  أرجع خطأ;
	    }
	    أرجع ریښتیا;
      }
      بدل (معلومات->time->type) {
	  حالة vpi_Sim_وخت:
	    اكسر;
	  حالة vpi_Scaled_Real_وخت:
	    اكسر;
	  حالة vpi_Suppress_وخت:
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(معلومات->reason));
		  أرجع خطأ;
	    }
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(معلومات->reason));
	    أرجع خطأ;
      }
      أرجع ریښتیا;
}

ساكن فراغ سېټ_callback_وخت(p_cb_معلومات معلومات)
{
      أكد(معلومات && معلومات->time);
      معلومات->time->ټیټ  = 0;
      معلومات->time->لوړ = 0;
      معلومات->time->real = 0.0;
      بدل (معلومات->time->type) {
	  حالة vpi_Sim_وخت:
	    vpip_وخت_منزل_timestruct(معلومات->time, مهالوېش_simtime());
	    اكسر;
	  حالة vpi_Scaled_Real_وخت:
	    معلومات->time->real = vpip_scaled_وخت_from_handle(مهالوېش_simtime(), معلومات->obj);
	    اكسر;
	  حالة vpi_Suppress_وخت:
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

      مجازي فراغ پرلپسې_پرلپسې() override;
};

مضمن __vpiCallback::__vpiCallback()
{
      بل_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحيح __vpiCallback::get_type_کوډ(فراغ) ثابت
{ أرجع vpiCallback; }


ارزښت_callback::ارزښت_callback(p_cb_معلومات معلومات)
{
      cb_معلومات = *معلومات;
      إذا (معلومات->time) {
	    cb_وخت = *(معلومات->time);
      } وإلا {
	    cb_وخت.type = vpi_Suppress_وخت;
      }
      cb_معلومات.time = &cb_وخت;
      إذا (معلومات->value) {
	    cb_ارزښت = *(معلومات->value);
      } وإلا {
	    cb_ارزښت.format = vpiSuppressVal;
      }
      cb_معلومات.value = &cb_ارزښت;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
منطقي ارزښت_callback::test_ارزښت_callback_ready(فراغ)
{
      أرجع ریښتیا;
}

ساكن فراغ vpip_real_ارزښت_change(ارزښت_callback*cbh, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      أكد(obj);

      obj->جمع_کړه_vpi_callback(cbh);
}

صنف ارزښت_part_callback : عام ارزښت_callback {
    عام:
      څرګند ارزښت_part_callback(p_cb_معلومات معلومات);
      ~ارزښت_part_callback() override;

      ارزښت_part_callback(ثابت ارزښت_part_callback&) = ړنګول;
      ارزښت_part_callback& چلوونکی=(ثابت ارزښت_part_callback&) = ړنګول;

      منطقي test_ارزښت_callback_ready(فراغ) override;

    خاص:
      حرف*ارزښت_بټونه;
      size_t ارزښت_off;
};

مضمن ارزښت_part_callback::ارزښت_part_callback(p_cb_معلومات معلومات)
: ارزښت_callback(معلومات)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(معلومات->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->جمع_کړه_vpi_callback(هذا);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_ارزښت tmp_ارزښت;
      tmp_ارزښت.format = vpiBinStrVal;
      sig_fil->get_ارزښت(&tmp_ارزښت);

      ارزښت_بټونه = جديد حرف[pobj->width+1];
      ارزښت_off = pobj->پلار->vpi_get(vpi_اندازه) - pobj->width - pobj->tbase;

      حافظه_نقل(ارزښت_بټونه, tmp_ارزښت.value.str + ارزښت_off, pobj->width);
      ارزښت_بټونه[pobj->width] = 0;
}

ارزښت_part_callback::~ارزښت_part_callback()
{
      ړنګول[]ارزښت_بټونه;
}

منطقي ارزښت_part_callback::test_ارزښت_callback_ready(فراغ)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_معلومات.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ارزښت tmp_ارزښت;
      tmp_ارزښت.format = vpiBinStrVal;
      sig_fil->get_ارزښت(&tmp_ارزښت);

      إذا (memcmp(ارزښت_بټونه, tmp_ارزښت.value.str + ارزښت_off, pobj->width) == 0)
	    أرجع خطأ;

      حافظه_نقل(ارزښت_بټونه, tmp_ارزښت.value.str + ارزښت_off, pobj->width);
      أرجع ریښتیا;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ساكن ارزښت_callback*make_ارزښت_change_part(p_cb_معلومات معلومات)
{
	/* Attach the __vpiCallback object to the signal. */
      ارزښت_callback*cbh = جديد ارزښت_part_callback(معلومات);
      أرجع cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ساكن ارزښت_callback* make_ارزښت_change(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وخت(معلومات, ریښتیا))
	    أرجع 0;

      إذا (vpi_get(vpiAutomatic, معلومات->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, معلومات->obj));
            أرجع 0;
      }

	// Special case: the target object is a vpiPartSelect
      إذا (معلومات->obj->get_type_کوډ() == vpiPartSelect) {
            إذا (معلومات->obj->vpi_handle(vpiArray))
	        أرجع vpip_array_کلمه_change(معلومات);
            وإلا
	        أرجع make_ارزښت_change_part(معلومات);
      }

      إذا (معلومات->obj->get_type_کوډ() == vpi_حافظه_کلمه)
	    أرجع vpip_array_کلمه_change(معلومات);

      إذا (معلومات->obj->get_type_کوډ() == vpi_حافظه)
	    أرجع vpip_array_change(معلومات);

      ارزښت_callback*obj = جديد ارزښت_callback(معلومات);

      أكد(معلومات->obj);
      بدل (معلومات->obj->get_type_کوډ()) {

	  حالة vpiReg:
	  حالة vpiNet:
	  حالة vpiIntegerVar:
	  حالة vpi_بټ_Var:
	  حالة vpiByteVar:
	  حالة vpiShortIntVar:
	  حالة vpiIntVar:
	  حالة vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    بنية __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(معلومات->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->غوټه->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->جمع_کړه_vpi_callback(obj);
	    اكسر;

	  حالة vpiRealVar:
	    vpip_real_ارزښت_change(obj, معلومات->obj);
	    اكسر;

	  حالة vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(معلومات->obj);
	    nev->جمع_کړه_vpi_callback(obj);
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
		    معلومات->obj->get_type_کوډ());
	    ړنګول obj;
	    أرجع 0;
      }

      أرجع obj;
}

صنف sync_callback : عام __vpiCallback {
    عام:
      څرګند sync_callback(p_cb_معلومات معلومات);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = ړنګول;
      sync_callback& چلوونکی=(ثابت sync_callback&) = ړنګول;

    عام:
	// scheduled event
      بنية sync_cb* cb_sync;
	// user supplied callback data
      بنية t_vpi_وخت cb_وخت;

    خاص:
};

مضمن sync_callback::sync_callback(p_cb_معلومات معلومات)
{
      cb_sync = 0;

      cb_معلومات = *معلومات;
      أكد(معلومات->time);
      cb_وخت = *(معلومات->time);
      cb_معلومات.time = &cb_وخت;
}

sync_callback::~sync_callback()
{
      ړنګول cb_sync;
}

فراغ sync_cb::پرلپسې_پرلپسې()
{
      إذا (handle == 0)
	    أرجع;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      إذا (cur->cb_معلومات.cb_rtn != 0) {
	    سېټ_callback_وخت(&cur->cb_معلومات);
	    أكد(vpi_ډېرپېښ_flag == VPI_ډېرپېښ_NONE);
	    vpi_ډېرپېښ_flag = sync_flag? VPI_ډېرپېښ_ROSYNC : VPI_ډېرپېښ_RWSYNC;
	    (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    vpi_ډېرپېښ_flag = VPI_ډېرپېښ_NONE;
      }

      ړنګول cur;
}

ساكن vvp_time64_t get_sync_cb_وخت(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      بدل (obj->cb_وخت.type) {
	  حالة vpi_Sim_وخت:
	    tv = vpip_timestruct_منزل_وخت(&obj->cb_وخت);
	    اكسر;

	  حالة vpi_Scaled_Real_وخت:
	    tv = vpip_scaled_real_منزل_time64(obj->cb_وخت.real,
		     vpip_timescale_scope_from_handle(obj->cb_معلومات.obj));
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحيح)obj->cb_وخت.type);
	    أكد(0);
	    اكسر;
      }
      أرجع tv;
}

ساكن sync_callback* make_sync(p_cb_معلومات معلومات, منطقي readonly_flag)
{
      إذا (!check_callback_وخت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);

      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = readonly_flag? ریښتیا : خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وخت(obj);
      مهالوېش_generic(cb, tv, ریښتیا, readonly_flag);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_afterdelay(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وخت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وخت(obj);
      مهالوېش_generic(cb, tv, خطأ);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_پیل_د_sim_وخت(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وخت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وخت(obj);
      vvp_time64_t cur = مهالوېش_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      مهالوېش_at_پیل_د_simtime(cb, tv);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_پای_ټکی_د_sim_وخت(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وخت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وخت(obj);
      vvp_time64_t cur = مهالوېش_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      مهالوېش_at_پای_ټکی_د_simtime(cb, tv);

      أرجع obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

صنف simulator_callback : عام __vpiCallback {
    عام:
      څرګند simulator_callback(ثابت بنية t_cb_معلومات*معلومات);

    عام:
      بنية t_vpi_وخت cb_وخت;
};

مضمن simulator_callback::simulator_callback(ثابت بنية t_cb_معلومات*معلومات)
{
      cb_معلومات = *معلومات;
      إذا ((معلومات->reason == cb_بل_Sim_وخت) && معلومات->time) {
	    cb_وخت = *(معلومات->time);
      } وإلا إذا (معلومات->reason == cb_پای_ټکی_د_Simulation) {
	    cb_وخت.type = vpi_Sim_وخت;
      } وإلا {
	    cb_وخت.type = vpi_Suppress_وخت;
      }
      cb_معلومات.time = &cb_وخت;
}

ساكن simulator_callback*بل_Sim_وخت = 0;
ساكن simulator_callback*پای_ټکی_د_Compile = 0;
ساكن simulator_callback*پیل_د_Simulation = 0;
ساكن simulator_callback*پای_ټکی_د_Simulation = 0;

#که_تعریف_شوی CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
فراغ simulator_cb_delete(فراغ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      طالما (پای_ټکی_د_Compile) {
	    cur = پای_ټکی_د_Compile;
	    پای_ټکی_د_Compile = dynamic_cast<simulator_callback*>(cur->بل_2);
	    ړنګول cur;
      }

	/* Delete all the start of simulation callbacks. */
      طالما (پیل_د_Simulation) {
	    cur = پیل_د_Simulation;
	    پیل_د_Simulation = dynamic_cast<simulator_callback*>(cur->بل_2);
	    ړنګول cur;
      }

	/* Delete all the end of simulation callbacks. */
      طالما (پای_ټکی_د_Simulation) {
	    cur = پای_ټکی_د_Simulation;
	    پای_ټکی_د_Simulation = dynamic_cast<simulator_callback*>(cur->بل_2);
	    ړنګول cur;
      }
}
#د_که_پای

فراغ vpi_پای_ټکی_د_Compile(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_ډېرپېښ_flag == VPI_ډېرپېښ_NONE);
      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_RWSYNC;

      طالما (پای_ټکی_د_Compile) {
	    cur = پای_ټکی_د_Compile;
	    پای_ټکی_د_Compile = dynamic_cast<simulator_callback*>(cur->بل_2);
	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    }
	    ړنګول cur;
      }

      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_NONE;
}

فراغ vpi_پیل_د_Sim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_ډېرپېښ_flag == VPI_ډېرپېښ_NONE);
      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_RWSYNC;

      طالما (پیل_د_Simulation) {
	    cur = پیل_د_Simulation;
	    پیل_د_Simulation = dynamic_cast<simulator_callback*>(cur->بل_2);
	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    }
	    ړنګول cur;
      }

      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_NONE;
}

فراغ vpiPostsim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_ډېرپېښ_flag == VPI_ډېرپېښ_NONE);
      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_ROSYNC;

      طالما (پای_ټکی_د_Simulation) {
	    cur = پای_ټکی_د_Simulation;
	    پای_ټکی_د_Simulation = dynamic_cast<simulator_callback*>(cur->بل_2);
	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  سېټ_callback_وخت(&cur->cb_معلومات);
		  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    }
	    ړنګول cur;
      }

      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
فراغ vpi_بل_Sim_وخت(فراغ)
{
      simulator_callback* cur;
      simulator_callback* بل_2 = بل_Sim_وخت;
      بل_Sim_وخت = 0;

      أكد(vpi_ډېرپېښ_flag == VPI_ډېرپېښ_NONE);
      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_RWSYNC;

      طالما (بل_2) {
            cur = بل_2;
            بل_2 = dynamic_cast<simulator_callback*>(cur->بل_2);
            إذا (cur->cb_معلومات.cb_rtn != 0) {
                  سېټ_callback_وخت(&cur->cb_معلومات);
                  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
            }
            ړنګول cur;
      }

      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_NONE;
}

ساكن simulator_callback* make_prepost(p_cb_معلومات معلومات)
{
      إذا ((معلومات->reason == cb_بل_Sim_وخت) && !check_callback_وخت(معلومات, ریښتیا))
	    أرجع 0;

      simulator_callback*obj = جديد simulator_callback(معلومات);

      /* Insert at head of list */
      بدل (معلومات->reason) {
	  حالة cb_پای_ټکی_د_Compile:
	    obj->بل_2 = پای_ټکی_د_Compile;
	    پای_ټکی_د_Compile = obj;
	    اكسر;
	  حالة cb_پیل_د_Simulation:
	    obj->بل_2 = پیل_د_Simulation;
	    پیل_د_Simulation = obj;
	    اكسر;
	  حالة cb_پای_ټکی_د_Simulation:
	    obj->بل_2 = پای_ټکی_د_Simulation;
	    پای_ټکی_د_Simulation = obj;
	    اكسر;
	  حالة cb_بل_Sim_وخت:
	    obj->بل_2 = بل_Sim_وخت;
	    بل_Sim_وخت = obj;
	    اكسر;
      }

      أرجع obj;
}

vpiHandle vpi_ثبتوونکی_cb(p_cb_معلومات معلومات)
{
      بنية __vpiCallback*obj = 0;

      أكد(معلومات);
      بدل (معلومات->reason) {

	  حالة cb_ارزښت_Change:
	    obj = make_ارزښت_change(معلومات);
	    اكسر;

	  حالة cbReadOnlySynch:
	    obj = make_sync(معلومات, ریښتیا);
	    اكسر;

	  حالة cbReadWriteSynch:
	    obj = make_sync(معلومات, خطأ);
	    اكسر;

	  حالة cb_At_پیل_د_Sim_وخت:
	    obj = make_at_پیل_د_sim_وخت(معلومات);
	    اكسر;

	  حالة cb_At_پای_ټکی_د_Sim_وخت:
	    obj = make_at_پای_ټکی_د_sim_وخت(معلومات);
	    اكسر;

	  حالة cbAfterDelay:
	    obj = make_afterdelay(معلومات);
	    اكسر;

	  حالة cb_پای_ټکی_د_Compile:
	  حالة cb_پیل_د_Simulation:
	  حالة cb_پای_ټکی_د_Simulation:
	  حالة cb_بل_Sim_وخت:
	    obj = make_prepost(معلومات);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (صحيح)معلومات->reason);
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
      obj->cb_معلومات.cb_rtn = 0;

      أرجع 1;
}

فراغ callback_execute(بنية __vpiCallback*cur)
{
      ثابت vpi_ډېرپېښ_t save_ډېرپېښ = vpi_ډېرپېښ_flag;
      vpi_ډېرپېښ_flag = VPI_ډېرپېښ_RWSYNC;

      أكد(cur->cb_معلومات.cb_rtn);
      سېټ_callback_وخت(&cur->cb_معلومات);
      (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);

      vpi_ډېرپېښ_flag = save_ډېرپېښ;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
بنية __vpi_array_word {
      بنية __vpi_array_word* بل_2;
      بنية __vpiArray* array;
      بینښه اوږد کلمه;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_ټکي = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      أكد(vpi_callbacks_ == 0);
      أكد(array_ټکي == 0);
}

فراغ vvp_vpi_callback::attach_as_کلمه(vvp_array_t arr, بینښه اوږد addr)
{
      بنية __vpi_array_word*tmp = جديد __vpi_array_word;
      tmp->array = arr;
      tmp->کلمه = addr;
      tmp->بل_2 = array_ټکي;
      array_ټکي = tmp;
}

فراغ vvp_vpi_callback::جمع_کړه_vpi_callback(ارزښت_callback*cb)
{
      cb->بل_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_vpi_callback::clear_all_callbacks()
{
      طالما (vpi_callbacks_) {
	    ارزښت_callback *tmp = dynamic_cast<ارزښت_callback*>
	                            (vpi_callbacks_->بل_2);
	    ړنګول vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      طالما (array_ټکي) {
	    بنية __vpi_array_word*tmp = array_ټکي->بل_2;
	    ړنګول array_ټکي;
	    array_ټکي = tmp;
      }
}
#د_که_پای

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
فراغ vvp_vpi_callback::پرلپسې_vpi_callbacks()
{
      بنية __vpi_array_word*array_کلمه = array_ټکي;
      طالما (array_کلمه) {
	    array_کلمه->array->کلمه_change(array_کلمه->کلمه);
	    array_کلمه = array_کلمه->بل_2;
      }

      ارزښت_callback *بل_2 = vpi_callbacks_;
      ارزښت_callback *prev = 0;

      طالما (بل_2) {
	    ارزښت_callback*cur = بل_2;
	    بل_2 = dynamic_cast<ارزښت_callback*>(cur->بل_2);

	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  إذا (cur->test_ارزښت_callback_ready()) {
			إذا (cur->cb_معلومات.value)
			      get_ارزښت(cur->cb_معلومات.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks_ = بل_2;
		  cur->بل_2 = 0;
		  ړنګول cur;

	    } وإلا {
		  أكد(prev->بل_2 == cur);
		  prev->بل_2 = بل_2;
		  cur->بل_2 = 0;
		  ړنګول cur;
	    }
      }
}

فراغ vvp_signal_ارزښت::get_signal_ارزښت(بنية t_vpi_ارزښت*vp)
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
	  حالة vpi_ویکټور_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal: {
	    بینښه wid = ارزښت_اندازه();
	    vvp_vector4_t vec4(wid);
	    لكل (بینښه idx = 0; idx < wid; idx += 1) {
		  vec4.سېټ_بټ(idx, value(idx));
	    }
	    vpip_vec4_get_ارزښت(vec4, wid, خطأ, vp);
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

ساكن فراغ real_signal_ارزښت(بنية t_vpi_ارزښت*vp, مزدوج rval)
{
      ساكن ثابت size_t RBUF_اندازه = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_پایله_buf(RBUF_اندازه, RBUF_VAL));

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
		  rval = std::پړاو_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(rval))
		  snprintf(rbuf, RBUF_اندازه, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_اندازه, "%0.0f", std::پړاو_2(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_اندازه, "%" PRIx64, vlg_پړاو_منزل_u64(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t val = vlg_پړاو_منزل_u64(rval);
		بینښه len = 0;

		طالما (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_پړاو_منزل_u64(rval);
		لكل (بینښه idx = 0 ;  idx < len ;  idx += 1) {
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

فراغ vvp_fun_signal_real_aa::get_signal_ارزښت(بنية t_vpi_ارزښت*vp)
{
      real_signal_ارزښت(vp, real_ارزښت());
}

فراغ vvp_wire_real::get_signal_ارزښت(بنية t_vpi_ارزښت*vp)
{
      real_signal_ارزښت(vp, real_ارزښت());
}

فراغ vvp_fun_signal_string_aa::get_signal_ارزښت(بنية t_vpi_ارزښت*)
{
      أكد(0);
}
#إذا 0
فراغ vvp_wire_string::get_signal_ارزښت(بنية t_vpi_ارزښت*vp)
{
      أكد(0);
}
#د_که_پای
فراغ vvp_wire_vec4::get_ارزښت(بنية t_vpi_ارزښت*val)
{
      get_signal_ارزښت(val);
}

فراغ vvp_wire_vec8::get_ارزښت(بنية t_vpi_ارزښت*val)
{
      get_signal_ارزښت(val);
}

فراغ vvp_wire_real::get_ارزښت(بنية t_vpi_ارزښت*val)
{
      get_signal_ارزښت(val);
}
#إذا 0
فراغ vvp_wire_string::get_ارزښت(بنية t_vpi_ارزښت*val)
{
      أكد(0);
}
#د_که_پای
