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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#پایان_اگر
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

استفاده نطاق std;

ساكن ثابت حرف*cb_reason_name(PLI_INT32 reason)
{
      بدل (reason) {
	  حالة cb_مقدار_Change:
	    أرجع "cbValueChange";
	  حالة cbStmt:
	    أرجع "cbStmt";
	  حالة cbForce:
	    أرجع "cbForce";
	  حالة cbRelease:
	    أرجع "cbRelease";
	  حالة cb_At_شروع_از_Sim_زمان:
	    أرجع "cbAtStartOfSimTime";
	  حالة cbReadWriteSynch:
	    أرجع "cbReadWriteSynch";
	  حالة cbReadOnlySynch:
	    أرجع "cbReadOnlySynch";
	  حالة cb_بعدی_Sim_زمان:
	    أرجع "cbNextSimTime";
	  حالة cbAfterDelay:
	    أرجع "cbAfterDelay";
	  حالة cb_انتها_از_Compile:
	    أرجع "cbEndOfCompile";
	  حالة cb_شروع_از_Simulation:
	    أرجع "cbStartOfSimulation";
	  حالة cb_انتها_از_Simulation:
	    أرجع "cbEndOfSimulation";
	  حالة cbError:
	    أرجع "cbError";
	  حالة cbTchkViolation:
	    أرجع "cbTchkViolation";
	  حالة cb_شروع_از_Save:
	    أرجع "cbStartOfSave";
	  حالة cb_انتها_از_Save:
	    أرجع "cbEndOfSave";
	  حالة cb_شروع_از_Restart:
	    أرجع "cbStartOfRestart";
	  حالة cb_انتها_از_Restart:
	    أرجع "cbEndOfRestart";
	  حالة cb_شروع_از_Reset:
	    أرجع "cbStartOfReset";
	  حالة cb_انتها_از_Reset:
	    أرجع "cbEndOfReset";
	  حالة cbEnterInteractive:
	    أرجع "cbEnterInteractive";
	  حالة cbExitInteractive:
	    أرجع "cbExitInteractive";
	  حالة cbInteractiveScopeChange:
	    أرجع "cbInteractiveScopeChange";
	  حالة cbUnresolvedSystf:
	    أرجع "cbUnresolvedSystf";
	  حالة cb_At_انتها_از_Sim_زمان:
	    أرجع "cbAtEndOfSimTime";
	  افتراضي:
	    أرجع "unrecognised";
      }
}

ساكن منطقي check_callback_زمان(p_cb_داده داده, منطقي allow_suppress)
{
      أكد(داده);
      إذا (!داده->time) {
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(داده->reason));
		  أرجع خطأ;
	    }
	    أرجع درست;
      }
      بدل (داده->time->type) {
	  حالة vpi_Sim_زمان:
	    اكسر;
	  حالة vpi_Scaled_Real_زمان:
	    اكسر;
	  حالة vpi_Suppress_زمان:
	    إذا (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(داده->reason));
		  أرجع خطأ;
	    }
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(داده->reason));
	    أرجع خطأ;
      }
      أرجع درست;
}

ساكن فراغ مجموعه_callback_زمان(p_cb_داده داده)
{
      أكد(داده && داده->time);
      داده->time->کم  = 0;
      داده->time->بالا = 0;
      داده->time->real = 0.0;
      بدل (داده->time->type) {
	  حالة vpi_Sim_زمان:
	    vpip_زمان_مقصد_timestruct(داده->time, زمانبندی_simtime());
	    اكسر;
	  حالة vpi_Scaled_Real_زمان:
	    داده->time->real = vpip_scaled_زمان_from_handle(زمانبندی_simtime(), داده->obj);
	    اكسر;
	  حالة vpi_Suppress_زمان:
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

      مجازی فراغ پیاپی_پیاپی() override;
};

مضمن __vpiCallback::__vpiCallback()
{
      بعدی_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحيح __vpiCallback::get_type_کد(فراغ) ثابت
{ أرجع vpiCallback; }


مقدار_callback::مقدار_callback(p_cb_داده داده)
{
      cb_داده = *داده;
      إذا (داده->time) {
	    cb_زمان = *(داده->time);
      } وإلا {
	    cb_زمان.type = vpi_Suppress_زمان;
      }
      cb_داده.time = &cb_زمان;
      إذا (داده->value) {
	    cb_مقدار = *(داده->value);
      } وإلا {
	    cb_مقدار.format = vpiSuppressVal;
      }
      cb_داده.value = &cb_مقدار;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
منطقي مقدار_callback::test_مقدار_callback_ready(فراغ)
{
      أرجع درست;
}

ساكن فراغ vpip_real_مقدار_change(مقدار_callback*cbh, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      أكد(obj);

      obj->جمع_کن_vpi_callback(cbh);
}

صنف مقدار_part_callback : عام مقدار_callback {
    عام:
      صریح مقدار_part_callback(p_cb_داده داده);
      ~مقدار_part_callback() override;

      مقدار_part_callback(ثابت مقدار_part_callback&) = حذف;
      مقدار_part_callback& عملگر=(ثابت مقدار_part_callback&) = حذف;

      منطقي test_مقدار_callback_ready(فراغ) override;

    خاص:
      حرف*مقدار_بیتها;
      size_t مقدار_off;
};

مضمن مقدار_part_callback::مقدار_part_callback(p_cb_داده داده)
: مقدار_callback(داده)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(داده->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->جمع_کن_vpi_callback(هذا);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      sig_fil->get_مقدار(&tmp_مقدار);

      مقدار_بیتها = جديد حرف[pobj->width+1];
      مقدار_off = pobj->والد->vpi_get(vpi_اندازه) - pobj->width - pobj->tbase;

      کپی_حافظه(مقدار_بیتها, tmp_مقدار.value.str + مقدار_off, pobj->width);
      مقدار_بیتها[pobj->width] = 0;
}

مقدار_part_callback::~مقدار_part_callback()
{
      حذف[]مقدار_بیتها;
}

منطقي مقدار_part_callback::test_مقدار_callback_ready(فراغ)
{
      بنية __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_داده.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      sig_fil->get_مقدار(&tmp_مقدار);

      إذا (memcmp(مقدار_بیتها, tmp_مقدار.value.str + مقدار_off, pobj->width) == 0)
	    أرجع خطأ;

      کپی_حافظه(مقدار_بیتها, tmp_مقدار.value.str + مقدار_off, pobj->width);
      أرجع درست;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ساكن مقدار_callback*make_مقدار_change_part(p_cb_داده داده)
{
	/* Attach the __vpiCallback object to the signal. */
      مقدار_callback*cbh = جديد مقدار_part_callback(داده);
      أرجع cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ساكن مقدار_callback* make_مقدار_change(p_cb_داده داده)
{
      إذا (!check_callback_زمان(داده, درست))
	    أرجع 0;

      إذا (vpi_get(vpiAutomatic, داده->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, داده->obj));
            أرجع 0;
      }

	// Special case: the target object is a vpiPartSelect
      إذا (داده->obj->get_type_کد() == vpiPartSelect) {
            إذا (داده->obj->vpi_handle(vpiArray))
	        أرجع vpip_array_کلمه_change(داده);
            وإلا
	        أرجع make_مقدار_change_part(داده);
      }

      إذا (داده->obj->get_type_کد() == vpi_حافظه_کلمه)
	    أرجع vpip_array_کلمه_change(داده);

      إذا (داده->obj->get_type_کد() == vpi_حافظه)
	    أرجع vpip_array_change(داده);

      مقدار_callback*obj = جديد مقدار_callback(داده);

      أكد(داده->obj);
      بدل (داده->obj->get_type_کد()) {

	  حالة vpiReg:
	  حالة vpiNet:
	  حالة vpiIntegerVar:
	  حالة vpi_بیت_Var:
	  حالة vpiByteVar:
	  حالة vpiShortIntVar:
	  حالة vpiIntVar:
	  حالة vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    بنية __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(داده->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->گره->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->جمع_کن_vpi_callback(obj);
	    اكسر;

	  حالة vpiRealVar:
	    vpip_real_مقدار_change(obj, داده->obj);
	    اكسر;

	  حالة vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(داده->obj);
	    nev->جمع_کن_vpi_callback(obj);
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
		    داده->obj->get_type_کد());
	    حذف obj;
	    أرجع 0;
      }

      أرجع obj;
}

صنف sync_callback : عام __vpiCallback {
    عام:
      صریح sync_callback(p_cb_داده داده);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = حذف;
      sync_callback& عملگر=(ثابت sync_callback&) = حذف;

    عام:
	// scheduled event
      بنية sync_cb* cb_sync;
	// user supplied callback data
      بنية t_vpi_زمان cb_زمان;

    خاص:
};

مضمن sync_callback::sync_callback(p_cb_داده داده)
{
      cb_sync = 0;

      cb_داده = *داده;
      أكد(داده->time);
      cb_زمان = *(داده->time);
      cb_داده.time = &cb_زمان;
}

sync_callback::~sync_callback()
{
      حذف cb_sync;
}

فراغ sync_cb::پیاپی_پیاپی()
{
      إذا (handle == 0)
	    أرجع;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      إذا (cur->cb_داده.cb_rtn != 0) {
	    مجموعه_callback_زمان(&cur->cb_داده);
	    أكد(vpi_نما_flag == VPI_نما_NONE);
	    vpi_نما_flag = sync_flag? VPI_نما_ROSYNC : VPI_نما_RWSYNC;
	    (cur->cb_داده.cb_rtn)(&cur->cb_داده);
	    vpi_نما_flag = VPI_نما_NONE;
      }

      حذف cur;
}

ساكن vvp_time64_t get_sync_cb_زمان(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      بدل (obj->cb_زمان.type) {
	  حالة vpi_Sim_زمان:
	    tv = vpip_timestruct_مقصد_زمان(&obj->cb_زمان);
	    اكسر;

	  حالة vpi_Scaled_Real_زمان:
	    tv = vpip_scaled_real_مقصد_time64(obj->cb_زمان.real,
		     vpip_timescale_scope_from_handle(obj->cb_داده.obj));
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحيح)obj->cb_زمان.type);
	    أكد(0);
	    اكسر;
      }
      أرجع tv;
}

ساكن sync_callback* make_sync(p_cb_داده داده, منطقي readonly_flag)
{
      إذا (!check_callback_زمان(داده, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(داده);

      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = readonly_flag? درست : خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_زمان(obj);
      زمانبندی_generic(cb, tv, درست, readonly_flag);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_afterdelay(p_cb_داده داده)
{
      إذا (!check_callback_زمان(داده, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(داده);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_زمان(obj);
      زمانبندی_generic(cb, tv, خطأ);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_شروع_از_sim_زمان(p_cb_داده داده)
{
      إذا (!check_callback_زمان(داده, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(داده);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_زمان(obj);
      vvp_time64_t cur = زمانبندی_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      زمانبندی_at_شروع_از_simtime(cb, tv);

      أرجع obj;
}

ساكن بنية __vpiCallback* make_at_انتها_از_sim_زمان(p_cb_داده داده)
{
      إذا (!check_callback_زمان(داده, خطأ))
	    أرجع 0;

      sync_callback*obj = جديد sync_callback(داده);
      بنية sync_cb*cb = جديد sync_cb;
      cb->sync_flag = خطأ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_زمان(obj);
      vvp_time64_t cur = زمانبندی_simtime();
      إذا (cur > tv) {
	    tv = 0;
	    أكد(0);
      } وإلا إذا (cur == tv) {
	    tv = 0;
      } وإلا {
	    tv -= cur;
      }
      زمانبندی_at_انتها_از_simtime(cb, tv);

      أرجع obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

صنف simulator_callback : عام __vpiCallback {
    عام:
      صریح simulator_callback(ثابت بنية t_cb_داده*داده);

    عام:
      بنية t_vpi_زمان cb_زمان;
};

مضمن simulator_callback::simulator_callback(ثابت بنية t_cb_داده*داده)
{
      cb_داده = *داده;
      إذا ((داده->reason == cb_بعدی_Sim_زمان) && داده->time) {
	    cb_زمان = *(داده->time);
      } وإلا إذا (داده->reason == cb_انتها_از_Simulation) {
	    cb_زمان.type = vpi_Sim_زمان;
      } وإلا {
	    cb_زمان.type = vpi_Suppress_زمان;
      }
      cb_داده.time = &cb_زمان;
}

ساكن simulator_callback*بعدی_Sim_زمان = 0;
ساكن simulator_callback*انتها_از_Compile = 0;
ساكن simulator_callback*شروع_از_Simulation = 0;
ساكن simulator_callback*انتها_از_Simulation = 0;

#اگر_تعریف_شده CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
فراغ simulator_cb_delete(فراغ)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      طالما (انتها_از_Compile) {
	    cur = انتها_از_Compile;
	    انتها_از_Compile = dynamic_cast<simulator_callback*>(cur->بعدی_2);
	    حذف cur;
      }

	/* Delete all the start of simulation callbacks. */
      طالما (شروع_از_Simulation) {
	    cur = شروع_از_Simulation;
	    شروع_از_Simulation = dynamic_cast<simulator_callback*>(cur->بعدی_2);
	    حذف cur;
      }

	/* Delete all the end of simulation callbacks. */
      طالما (انتها_از_Simulation) {
	    cur = انتها_از_Simulation;
	    انتها_از_Simulation = dynamic_cast<simulator_callback*>(cur->بعدی_2);
	    حذف cur;
      }
}
#پایان_اگر

فراغ vpi_انتها_از_Compile(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_نما_flag == VPI_نما_NONE);
      vpi_نما_flag = VPI_نما_RWSYNC;

      طالما (انتها_از_Compile) {
	    cur = انتها_از_Compile;
	    انتها_از_Compile = dynamic_cast<simulator_callback*>(cur->بعدی_2);
	    إذا (cur->cb_داده.cb_rtn != 0) {
		  (cur->cb_داده.cb_rtn)(&cur->cb_داده);
	    }
	    حذف cur;
      }

      vpi_نما_flag = VPI_نما_NONE;
}

فراغ vpi_شروع_از_Sim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_نما_flag == VPI_نما_NONE);
      vpi_نما_flag = VPI_نما_RWSYNC;

      طالما (شروع_از_Simulation) {
	    cur = شروع_از_Simulation;
	    شروع_از_Simulation = dynamic_cast<simulator_callback*>(cur->بعدی_2);
	    إذا (cur->cb_داده.cb_rtn != 0) {
		  (cur->cb_داده.cb_rtn)(&cur->cb_داده);
	    }
	    حذف cur;
      }

      vpi_نما_flag = VPI_نما_NONE;
}

فراغ vpiPostsim(فراغ) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_نما_flag == VPI_نما_NONE);
      vpi_نما_flag = VPI_نما_ROSYNC;

      طالما (انتها_از_Simulation) {
	    cur = انتها_از_Simulation;
	    انتها_از_Simulation = dynamic_cast<simulator_callback*>(cur->بعدی_2);
	    إذا (cur->cb_داده.cb_rtn != 0) {
		  مجموعه_callback_زمان(&cur->cb_داده);
		  (cur->cb_داده.cb_rtn)(&cur->cb_داده);
	    }
	    حذف cur;
      }

      vpi_نما_flag = VPI_نما_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
فراغ vpi_بعدی_Sim_زمان(فراغ)
{
      simulator_callback* cur;
      simulator_callback* بعدی_2 = بعدی_Sim_زمان;
      بعدی_Sim_زمان = 0;

      أكد(vpi_نما_flag == VPI_نما_NONE);
      vpi_نما_flag = VPI_نما_RWSYNC;

      طالما (بعدی_2) {
            cur = بعدی_2;
            بعدی_2 = dynamic_cast<simulator_callback*>(cur->بعدی_2);
            إذا (cur->cb_داده.cb_rtn != 0) {
                  مجموعه_callback_زمان(&cur->cb_داده);
                  (cur->cb_داده.cb_rtn)(&cur->cb_داده);
            }
            حذف cur;
      }

      vpi_نما_flag = VPI_نما_NONE;
}

ساكن simulator_callback* make_prepost(p_cb_داده داده)
{
      إذا ((داده->reason == cb_بعدی_Sim_زمان) && !check_callback_زمان(داده, درست))
	    أرجع 0;

      simulator_callback*obj = جديد simulator_callback(داده);

      /* Insert at head of list */
      بدل (داده->reason) {
	  حالة cb_انتها_از_Compile:
	    obj->بعدی_2 = انتها_از_Compile;
	    انتها_از_Compile = obj;
	    اكسر;
	  حالة cb_شروع_از_Simulation:
	    obj->بعدی_2 = شروع_از_Simulation;
	    شروع_از_Simulation = obj;
	    اكسر;
	  حالة cb_انتها_از_Simulation:
	    obj->بعدی_2 = انتها_از_Simulation;
	    انتها_از_Simulation = obj;
	    اكسر;
	  حالة cb_بعدی_Sim_زمان:
	    obj->بعدی_2 = بعدی_Sim_زمان;
	    بعدی_Sim_زمان = obj;
	    اكسر;
      }

      أرجع obj;
}

vpiHandle vpi_ثبات_cb(p_cb_داده داده)
{
      بنية __vpiCallback*obj = 0;

      أكد(داده);
      بدل (داده->reason) {

	  حالة cb_مقدار_Change:
	    obj = make_مقدار_change(داده);
	    اكسر;

	  حالة cbReadOnlySynch:
	    obj = make_sync(داده, درست);
	    اكسر;

	  حالة cbReadWriteSynch:
	    obj = make_sync(داده, خطأ);
	    اكسر;

	  حالة cb_At_شروع_از_Sim_زمان:
	    obj = make_at_شروع_از_sim_زمان(داده);
	    اكسر;

	  حالة cb_At_انتها_از_Sim_زمان:
	    obj = make_at_انتها_از_sim_زمان(داده);
	    اكسر;

	  حالة cbAfterDelay:
	    obj = make_afterdelay(داده);
	    اكسر;

	  حالة cb_انتها_از_Compile:
	  حالة cb_شروع_از_Simulation:
	  حالة cb_انتها_از_Simulation:
	  حالة cb_بعدی_Sim_زمان:
	    obj = make_prepost(داده);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (صحيح)داده->reason);
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
      obj->cb_داده.cb_rtn = 0;

      أرجع 1;
}

فراغ callback_execute(بنية __vpiCallback*cur)
{
      ثابت vpi_نما_t save_نما = vpi_نما_flag;
      vpi_نما_flag = VPI_نما_RWSYNC;

      أكد(cur->cb_داده.cb_rtn);
      مجموعه_callback_زمان(&cur->cb_داده);
      (cur->cb_داده.cb_rtn)(&cur->cb_داده);

      vpi_نما_flag = save_نما;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
بنية __vpi_array_word {
      بنية __vpi_array_word* بعدی_2;
      بنية __vpiArray* array;
      بیعلامت دراز کلمه;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_واژگان = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      أكد(vpi_callbacks_ == 0);
      أكد(array_واژگان == 0);
}

فراغ vvp_vpi_callback::attach_as_کلمه(vvp_array_t arr, بیعلامت دراز addr)
{
      بنية __vpi_array_word*tmp = جديد __vpi_array_word;
      tmp->array = arr;
      tmp->کلمه = addr;
      tmp->بعدی_2 = array_واژگان;
      array_واژگان = tmp;
}

فراغ vvp_vpi_callback::جمع_کن_vpi_callback(مقدار_callback*cb)
{
      cb->بعدی_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_vpi_callback::clear_all_callbacks()
{
      طالما (vpi_callbacks_) {
	    مقدار_callback *tmp = dynamic_cast<مقدار_callback*>
	                            (vpi_callbacks_->بعدی_2);
	    حذف vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      طالما (array_واژگان) {
	    بنية __vpi_array_word*tmp = array_واژگان->بعدی_2;
	    حذف array_واژگان;
	    array_واژگان = tmp;
      }
}
#پایان_اگر

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
فراغ vvp_vpi_callback::پیاپی_vpi_callbacks()
{
      بنية __vpi_array_word*array_کلمه = array_واژگان;
      طالما (array_کلمه) {
	    array_کلمه->array->کلمه_change(array_کلمه->کلمه);
	    array_کلمه = array_کلمه->بعدی_2;
      }

      مقدار_callback *بعدی_2 = vpi_callbacks_;
      مقدار_callback *prev = 0;

      طالما (بعدی_2) {
	    مقدار_callback*cur = بعدی_2;
	    بعدی_2 = dynamic_cast<مقدار_callback*>(cur->بعدی_2);

	    إذا (cur->cb_داده.cb_rtn != 0) {
		  إذا (cur->test_مقدار_callback_ready()) {
			إذا (cur->cb_داده.value)
			      get_مقدار(cur->cb_داده.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks_ = بعدی_2;
		  cur->بعدی_2 = 0;
		  حذف cur;

	    } وإلا {
		  أكد(prev->بعدی_2 == cur);
		  prev->بعدی_2 = بعدی_2;
		  cur->بعدی_2 = 0;
		  حذف cur;
	    }
      }
}

فراغ vvp_signal_مقدار::get_signal_مقدار(بنية t_vpi_مقدار*vp)
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
	  حالة vpi_وکتور_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal: {
	    بیعلامت wid = مقدار_اندازه();
	    vvp_vector4_t vec4(wid);
	    لكل (بیعلامت idx = 0; idx < wid; idx += 1) {
		  vec4.مجموعه_بیت(idx, value(idx));
	    }
	    vpip_vec4_get_مقدار(vec4, wid, خطأ, vp);
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

ساكن فراغ real_signal_مقدار(بنية t_vpi_مقدار*vp, مزدوج rval)
{
      ساكن ثابت size_t RBUF_اندازه = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجه_buf(RBUF_اندازه, RBUF_VAL));

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
		  rval = std::دور_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(rval))
		  snprintf(rbuf, RBUF_اندازه, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_اندازه, "%0.0f", std::دور_2(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_اندازه, "%" PRIx64, vlg_دور_مقصد_u64(rval));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t val = vlg_دور_مقصد_u64(rval);
		بیعلامت len = 0;

		طالما (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_دور_مقصد_u64(rval);
		لكل (بیعلامت idx = 0 ;  idx < len ;  idx += 1) {
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

فراغ vvp_fun_signal_real_aa::get_signal_مقدار(بنية t_vpi_مقدار*vp)
{
      real_signal_مقدار(vp, real_مقدار());
}

فراغ vvp_wire_real::get_signal_مقدار(بنية t_vpi_مقدار*vp)
{
      real_signal_مقدار(vp, real_مقدار());
}

فراغ vvp_fun_signal_string_aa::get_signal_مقدار(بنية t_vpi_مقدار*)
{
      أكد(0);
}
#إذا 0
فراغ vvp_wire_string::get_signal_مقدار(بنية t_vpi_مقدار*vp)
{
      أكد(0);
}
#پایان_اگر
فراغ vvp_wire_vec4::get_مقدار(بنية t_vpi_مقدار*val)
{
      get_signal_مقدار(val);
}

فراغ vvp_wire_vec8::get_مقدار(بنية t_vpi_مقدار*val)
{
      get_signal_مقدار(val);
}

فراغ vvp_wire_real::get_مقدار(بنية t_vpi_مقدار*val)
{
      get_signal_مقدار(val);
}
#إذا 0
فراغ vvp_wire_string::get_مقدار(بنية t_vpi_مقدار*val)
{
      أكد(0);
}
#پایان_اگر
