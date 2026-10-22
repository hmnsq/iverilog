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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

استعمال نطاق std;

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
	  حالة cb_At_شروعات_جو_Sim_وقت:
	    أرجع "cbAtStartOfSimTime";
	  حالة cbReadWriteSynch:
	    أرجع "cbReadWriteSynch";
	  حالة cbReadOnlySynch:
	    أرجع "cbReadOnlySynch";
	  حالة cb_ايندڙ_Sim_وقت:
	    أرجع "cbNextSimTime";
	  حالة cbAfterDelay:
	    أرجع "cbAfterDelay";
	  حالة cb_پڇاڙي_جو_Compile:
	    أرجع "cbEndOfCompile";
	  حالة cb_شروعات_جو_Simulation:
	    أرجع "cbStartOfSimulation";
	  حالة cb_پڇاڙي_جو_Simulation:
	    أرجع "cbEndOfSimulation";
	  حالة cbError:
	    أرجع "cbError";
	  حالة cbTchkViolation:
	    أرجع "cbTchkViolation";
	  حالة cb_شروعات_جو_Save:
	    أرجع "cbStartOfSave";
	  حالة cb_پڇاڙي_جو_Save:
	    أرجع "cbEndOfSave";
	  حالة cb_شروعات_جو_Restart:
	    أرجع "cbStartOfRestart";
	  حالة cb_پڇاڙي_جو_Restart:
	    أرجع "cbEndOfRestart";
	  حالة cb_شروعات_جو_Reset:
	    أرجع "cbStartOfReset";
	  حالة cb_پڇاڙي_جو_Reset:
	    أرجع "cbEndOfReset";
	  حالة cbEnterInteractive:
	    أرجع "cbEnterInteractive";
	  حالة cbExitInteractive:
	    أرجع "cbExitInteractive";
	  حالة cbInteractiveScopeChange:
	    أرجع "cbInteractiveScopeChange";
	  حالة cbUnresolvedSystf:
	    أرجع "cbUnresolvedSystf";
	  حالة cb_At_پڇاڙي_جو_Sim_وقت:
	    أرجع "cbAtEndOfSimTime";
	  افتراضي:
	    أرجع "unrecognised";
      }
}

ساكن منطقي check_callback_وقت(p_cb_معلومات معلومات, منطقي allow_suppress)
{
      أكد(معلومات);
      إذا (!معلومات->time) {
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(معلومات->reason));
		  أرجع خطأ;
	    }
	    أرجع سچ;
      }
      بدل (معلومات->time->type) {
	  حالة vpi_Sim_وقت:
	    اكسر;
	  حالة vpi_Scaled_Real_وقت:
	    اكسر;
	  حالة vpi_Suppress_وقت:
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
      أرجع سچ;
}

ساكن فراغ سيٽ_callback_وقت(p_cb_معلومات معلومات)
{
      أكد(معلومات && معلومات->time);
      معلومات->time->هيٺيون  = 0;
      معلومات->time->مٿاهون = 0;
      معلومات->time->real = 0.0;
      بدل (معلومات->time->type) {
	  حالة vpi_Sim_وقت:
	    vpip_وقت_منزل_timestruct(معلومات->time, وقت_جدول_simtime());
	    اكسر;
	  حالة vpi_Scaled_Real_وقت:
	    معلومات->time->real = vpip_scaled_وقت_from_handle(وقت_جدول_simtime(), معلومات->obj);
	    اكسر;
	  حالة vpi_Suppress_وقت:
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

      مجازي فراغ لاڳيتو_لاڳيتو() override;
};

مضمن __vpiCallback::__vpiCallback()
{
      ايندڙ_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحيح __vpiCallback::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiCallback; }


قدر_callback::قدر_callback(p_cb_معلومات معلومات)
{
      cb_معلومات = *معلومات;
      إذا (معلومات->time) {
	    cb_وقت = *(معلومات->time);
      } وإلا {
	    cb_وقت.type = vpi_Suppress_وقت;
      }
      cb_معلومات.time = &cb_وقت;
      إذا (معلومات->value) {
	    cb_قدر = *(معلومات->value);
      } وإلا {
	    cb_قدر.format = vpiSuppressVal;
      }
      cb_معلومات.value = &cb_قدر;
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

      obj->جوڙيو_vpi_callback(cbh);
}

صنف قدر_part_callback : عام قدر_callback {
    عام:
      واضح قدر_part_callback(p_cb_معلومات معلومات);
      ~قدر_part_callback() override;

      قدر_part_callback(ثابت قدر_part_callback&) = ميٽيو;
      قدر_part_callback& عامل=(ثابت قدر_part_callback&) = ميٽيو;

      منطقي test_قدر_callback_ready(فراغ) override;

    خاص:
      حرف*قدر_بٽ;
      size_t قدر_off;
};

مضمن قدر_part_callback::قدر_part_callback(p_cb_معلومات معلومات)
: قدر_callback(معلومات)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(معلومات->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->جوڙيو_vpi_callback(هذا);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      sig_fil->get_قدر(&tmp_قدر);

      قدر_بٽ = جديد حرف[pobj->width+1];
      قدر_off = pobj->والد->vpi_get(vpi_سائيز) - pobj->width - pobj->tbase;

      يادگيري_نقل(قدر_بٽ, tmp_قدر.value.str + قدر_off, pobj->width);
      قدر_بٽ[pobj->width] = 0;
}

قدر_part_callback::~قدر_part_callback()
{
      ميٽيو[]قدر_بٽ;
}

منطقي قدر_part_callback::test_قدر_callback_ready(فراغ)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_معلومات.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      sig_fil->get_قدر(&tmp_قدر);

      إذا (memcmp(قدر_بٽ, tmp_قدر.value.str + قدر_off, pobj->width) == 0)
	    أرجع خطأ;

      يادگيري_نقل(قدر_بٽ, tmp_قدر.value.str + قدر_off, pobj->width);
      أرجع سچ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ساكن قدر_callback*make_قدر_change_part(p_cb_معلومات معلومات)
{
	/* Attach the __vpiCallback object to the signal. */
      قدر_callback*cbh = جديد قدر_part_callback(معلومات);
      أرجع cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ساكن قدر_callback* make_قدر_change(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وقت(معلومات, سچ))
	    أرجع 0;

      إذا (vpi_get(vpiAutomatic, معلومات->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, معلومات->obj));
            أرجع 0;
      }

	// Special case: the target object is a vpiPartSelect
      إذا (معلومات->obj->get_type_ڪوڊ() == vpiPartSelect) {
            إذا (معلومات->obj->vpi_handle(vpiArray))
	        أرجع vpip_array_لفظ_change(معلومات);
            وإلا
	        أرجع make_قدر_change_part(معلومات);
      }

      إذا (معلومات->obj->get_type_ڪوڊ() == vpi_يادگيري_لفظ)
	    أرجع vpip_array_لفظ_change(معلومات);

      إذا (معلومات->obj->get_type_ڪوڊ() == vpi_يادگيري)
	    أرجع vpip_array_change(معلومات);

      قدر_callback*obj = جديد قدر_callback(معلومات);

      أكد(معلومات->obj);
      بدل (معلومات->obj->get_type_ڪوڊ()) {

	  حالة vpiReg:
	  حالة vpiNet:
	  حالة vpiIntegerVar:
	  حالة vpi_ٻٽو_انگ_Var:
	  حالة vpiByteVar:
	  حالة vpiShortIntVar:
	  حالة vpiIntVar:
	  حالة vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    بنية __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(معلومات->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ڳنڍ->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->جوڙيو_vpi_callback(obj);
	    اكسر;

	  حالة vpiRealVar:
	    vpip_real_قدر_change(obj, معلومات->obj);
	    اكسر;

	  حالة vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(معلومات->obj);
	    nev->جوڙيو_vpi_callback(obj);
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
		    معلومات->obj->get_type_ڪوڊ());
	    ميٽيو obj;
	    أرجع 0;
      }

      أرجع obj;
}

صنف sync_callback : عام __vpiCallback {
    عام:
      واضح sync_callback(p_cb_معلومات معلومات);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = ميٽيو;
      sync_callback& عامل=(ثابت sync_callback&) = ميٽيو;

    عام:
	// scheduled event
      بنية sync_cb* cb_sync;
	// user supplied callback data
      بنية t_vpi_وقت cb_وقت;

    خاص:
};

مضمن sync_callback::sync_callback(p_cb_معلومات معلومات)
{
      cb_sync = 0;

      cb_معلومات = *معلومات;
      أكد(معلومات->time);
      cb_وقت = *(معلومات->time);
      cb_معلومات.time = &cb_وقت;
}

sync_callback::~sync_callback()
{
      ميٽيو cb_sync;
}

فراغ sync_cb::لاڳيتو_لاڳيتو()
{
      إذا (handle == 0)
	    أرجع;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      إذا (cur->cb_معلومات.cb_rtn != 0) {
	    سيٽ_callback_وقت(&cur->cb_معلومات);
	    أكد(vpi_گهڻائي_flag == VPI_گهڻائي_NONE);
	    vpi_گهڻائي_flag = sync_flag? VPI_گهڻائي_ROSYNC : VPI_گهڻائي_RWSYNC;
	    (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    vpi_گهڻائي_flag = VPI_گهڻائي_NONE;
      }

      ميٽيو cur;
}

ساكن vvp_time64_t get_sync_cb_وقت(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      بدل (obj->cb_وقت.type) {
	  حالة vpi_Sim_وقت:
	    tv = vpip_timestruct_منزل_وقت(&obj->cb_وقت);
	    اكسر;

	  حالة vpi_Scaled_Real_وقت:
	    tv = vpip_scaled_real_منزل_time64(obj->cb_وقت.real,
		     vpip_timescale_scope_from_handle(obj->cb_معلومات.obj));
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحيح)obj->cb_وقت.type);
	    أكد(0);
	    اكسر;
      }
      أرجع tv;
}

ساكن sync_callback* make_sync(p_cb_معلومات معلومات, منطقي readonly_flag)
{
      إذا (!check_callback_وقت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);

      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = readonly_flag? سچ : خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      وقت_جدول_generic(cb, tv, سچ, readonly_flag);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_afterdelay(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وقت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      وقت_جدول_generic(cb, tv, خطأ);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_شروعات_جو_sim_وقت(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وقت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      vvp_time64_t cur = وقت_جدول_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      وقت_جدول_at_شروعات_جو_simtime(cb, tv);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_پڇاڙي_جو_sim_وقت(p_cb_معلومات معلومات)
{
      إذا (!check_callback_وقت(معلومات, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(معلومات);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      vvp_time64_t cur = وقت_جدول_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      وقت_جدول_at_پڇاڙي_جو_simtime(cb, tv);

      أرجع obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

صنف simulator_callback : عام __vpiCallback {
    عام:
      واضح simulator_callback(ثابت بنية t_cb_معلومات*معلومات);

    عام:
      بنية t_vpi_وقت cb_وقت;
};

مضمن simulator_callback::simulator_callback(ثابت بنية t_cb_معلومات*معلومات)
{
      cb_معلومات = *معلومات;
      إذا ((معلومات->reason == cb_ايندڙ_Sim_وقت) && معلومات->time) {
	    cb_وقت = *(معلومات->time);
      } وإلا إذا (معلومات->reason == cb_پڇاڙي_جو_Simulation) {
	    cb_وقت.type = vpi_Sim_وقت;
      } وإلا {
	    cb_وقت.type = vpi_Suppress_وقت;
      }
      cb_معلومات.time = &cb_وقت;
}

ساكن simulator_callback*ايندڙ_Sim_وقت = 0;
ساكن simulator_callback*پڇاڙي_جو_Compile = 0;
ساكن simulator_callback*شروعات_جو_Simulation = 0;
ساكن simulator_callback*پڇاڙي_جو_Simulation = 0;

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
فراغ simulator_cb_delete(فراغ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      طالما (پڇاڙي_جو_Compile) {
	    cur = پڇاڙي_جو_Compile;
	    پڇاڙي_جو_Compile = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
	    ميٽيو cur;
      }

	/* Delete all the start of simulation callbacks. */
      طالما (شروعات_جو_Simulation) {
	    cur = شروعات_جو_Simulation;
	    شروعات_جو_Simulation = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
	    ميٽيو cur;
      }

	/* Delete all the end of simulation callbacks. */
      طالما (پڇاڙي_جو_Simulation) {
	    cur = پڇاڙي_جو_Simulation;
	    پڇاڙي_جو_Simulation = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
	    ميٽيو cur;
      }
}
#جيڪڏهن_پورو

فراغ vpi_پڇاڙي_جو_Compile(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_گهڻائي_flag == VPI_گهڻائي_NONE);
      vpi_گهڻائي_flag = VPI_گهڻائي_RWSYNC;

      طالما (پڇاڙي_جو_Compile) {
	    cur = پڇاڙي_جو_Compile;
	    پڇاڙي_جو_Compile = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    }
	    ميٽيو cur;
      }

      vpi_گهڻائي_flag = VPI_گهڻائي_NONE;
}

فراغ vpi_شروعات_جو_Sim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_گهڻائي_flag == VPI_گهڻائي_NONE);
      vpi_گهڻائي_flag = VPI_گهڻائي_RWSYNC;

      طالما (شروعات_جو_Simulation) {
	    cur = شروعات_جو_Simulation;
	    شروعات_جو_Simulation = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    }
	    ميٽيو cur;
      }

      vpi_گهڻائي_flag = VPI_گهڻائي_NONE;
}

فراغ vpiPostsim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_گهڻائي_flag == VPI_گهڻائي_NONE);
      vpi_گهڻائي_flag = VPI_گهڻائي_ROSYNC;

      طالما (پڇاڙي_جو_Simulation) {
	    cur = پڇاڙي_جو_Simulation;
	    پڇاڙي_جو_Simulation = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  سيٽ_callback_وقت(&cur->cb_معلومات);
		  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
	    }
	    ميٽيو cur;
      }

      vpi_گهڻائي_flag = VPI_گهڻائي_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
فراغ vpi_ايندڙ_Sim_وقت(فراغ)
{
      simulator_callback* cur;
      simulator_callback* ايندڙ_2 = ايندڙ_Sim_وقت;
      ايندڙ_Sim_وقت = 0;

      أكد(vpi_گهڻائي_flag == VPI_گهڻائي_NONE);
      vpi_گهڻائي_flag = VPI_گهڻائي_RWSYNC;

      طالما (ايندڙ_2) {
            cur = ايندڙ_2;
            ايندڙ_2 = dynamic_cast<simulator_callback*>(cur->ايندڙ_2);
            إذا (cur->cb_معلومات.cb_rtn != 0) {
                  سيٽ_callback_وقت(&cur->cb_معلومات);
                  (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);
            }
            ميٽيو cur;
      }

      vpi_گهڻائي_flag = VPI_گهڻائي_NONE;
}

ساكن simulator_callback* make_prepost(p_cb_معلومات معلومات)
{
      إذا ((معلومات->reason == cb_ايندڙ_Sim_وقت) && !check_callback_وقت(معلومات, سچ))
	    أرجع 0;

      simulator_callback*obj = جديد simulator_callback(معلومات);

      /* Insert at head of list */
      بدل (معلومات->reason) {
	  حالة cb_پڇاڙي_جو_Compile:
	    obj->ايندڙ_2 = پڇاڙي_جو_Compile;
	    پڇاڙي_جو_Compile = obj;
	    اكسر;
	  حالة cb_شروعات_جو_Simulation:
	    obj->ايندڙ_2 = شروعات_جو_Simulation;
	    شروعات_جو_Simulation = obj;
	    اكسر;
	  حالة cb_پڇاڙي_جو_Simulation:
	    obj->ايندڙ_2 = پڇاڙي_جو_Simulation;
	    پڇاڙي_جو_Simulation = obj;
	    اكسر;
	  حالة cb_ايندڙ_Sim_وقت:
	    obj->ايندڙ_2 = ايندڙ_Sim_وقت;
	    ايندڙ_Sim_وقت = obj;
	    اكسر;
      }

      أرجع obj;
}

vpiHandle vpi_رجسٽر_cb(p_cb_معلومات معلومات)
{
      بنية __vpiCallback*obj = 0;

      أكد(معلومات);
      بدل (معلومات->reason) {

	  حالة cb_قدر_Change:
	    obj = make_قدر_change(معلومات);
	    اكسر;

	  حالة cbReadOnlySynch:
	    obj = make_sync(معلومات, سچ);
	    اكسر;

	  حالة cbReadWriteSynch:
	    obj = make_sync(معلومات, خطأ);
	    اكسر;

	  حالة cb_At_شروعات_جو_Sim_وقت:
	    obj = make_at_شروعات_جو_sim_وقت(معلومات);
	    اكسر;

	  حالة cb_At_پڇاڙي_جو_Sim_وقت:
	    obj = make_at_پڇاڙي_جو_sim_وقت(معلومات);
	    اكسر;

	  حالة cbAfterDelay:
	    obj = make_afterdelay(معلومات);
	    اكسر;

	  حالة cb_پڇاڙي_جو_Compile:
	  حالة cb_شروعات_جو_Simulation:
	  حالة cb_پڇاڙي_جو_Simulation:
	  حالة cb_ايندڙ_Sim_وقت:
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
      ثابت vpi_گهڻائي_t save_گهڻائي = vpi_گهڻائي_flag;
      vpi_گهڻائي_flag = VPI_گهڻائي_RWSYNC;

      أكد(cur->cb_معلومات.cb_rtn);
      سيٽ_callback_وقت(&cur->cb_معلومات);
      (cur->cb_معلومات.cb_rtn)(&cur->cb_معلومات);

      vpi_گهڻائي_flag = save_گهڻائي;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
بنية __vpi_array_word {
      بنية __vpi_array_word* ايندڙ_2;
      بنية __vpiArray* array;
      بنانشان ڊگهو لفظ_2;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_لفظ = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      أكد(vpi_callbacks_ == 0);
      أكد(array_لفظ == 0);
}

فراغ vvp_vpi_callback::attach_as_لفظ(vvp_array_t arr, بنانشان ڊگهو addr)
{
      بنية __vpi_array_word*tmp = جديد __vpi_array_word;
      tmp->array = arr;
      tmp->لفظ_2 = addr;
      tmp->ايندڙ_2 = array_لفظ;
      array_لفظ = tmp;
}

فراغ vvp_vpi_callback::جوڙيو_vpi_callback(قدر_callback*cb)
{
      cb->ايندڙ_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_vpi_callback::clear_all_callbacks()
{
      طالما (vpi_callbacks_) {
	    قدر_callback *tmp = dynamic_cast<قدر_callback*>
	                            (vpi_callbacks_->ايندڙ_2);
	    ميٽيو vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      طالما (array_لفظ) {
	    بنية __vpi_array_word*tmp = array_لفظ->ايندڙ_2;
	    ميٽيو array_لفظ;
	    array_لفظ = tmp;
      }
}
#جيڪڏهن_پورو

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
فراغ vvp_vpi_callback::لاڳيتو_vpi_callbacks()
{
      بنية __vpi_array_word*array_لفظ_2 = array_لفظ;
      طالما (array_لفظ_2) {
	    array_لفظ_2->array->لفظ_change(array_لفظ_2->لفظ_2);
	    array_لفظ_2 = array_لفظ_2->ايندڙ_2;
      }

      قدر_callback *ايندڙ_2 = vpi_callbacks_;
      قدر_callback *prev = 0;

      طالما (ايندڙ_2) {
	    قدر_callback*cur = ايندڙ_2;
	    ايندڙ_2 = dynamic_cast<قدر_callback*>(cur->ايندڙ_2);

	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  إذا (cur->test_قدر_callback_ready()) {
			إذا (cur->cb_معلومات.value)
			      get_قدر(cur->cb_معلومات.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks_ = ايندڙ_2;
		  cur->ايندڙ_2 = 0;
		  ميٽيو cur;

	    } وإلا {
		  أكد(prev->ايندڙ_2 == cur);
		  prev->ايندڙ_2 = ايندڙ_2;
		  cur->ايندڙ_2 = 0;
		  ميٽيو cur;
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
	  حالة vpi_ويڪٽر_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal: {
	    بنانشان wid = قدر_سائيز();
	    vvp_vector4_t vec4(wid);
	    لكل (بنانشان idx = 0; idx < wid; idx += 1) {
		  vec4.سيٽ_ٻٽو_انگ(idx, value(idx));
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
      ساكن ثابت size_t RBUF_سائيز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتيجو_buf(RBUF_سائيز, RBUF_VAL));

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
		  rval = std::ڦيرو_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(rval))
		  snprintf(rbuf, RBUF_سائيز, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_سائيز, "%0.0f", std::ڦيرو_2(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_سائيز, "%" PRIx64, vlg_ڦيرو_منزل_u64(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t val = vlg_ڦيرو_منزل_u64(rval);
		بنانشان len = 0;

		طالما (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_ڦيرو_منزل_u64(rval);
		لكل (بنانشان idx = 0 ;  idx < len ;  idx += 1) {
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
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو
