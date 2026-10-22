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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
#شامل  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

استعمال نامخلا std;

ساکن ثابت حرف*cb_reason_name(PLI_INT32 reason)
{
      انتخاب (reason) {
	  حالت cb_قدر_Change:
	    واپس "cbValueChange";
	  حالت cbStmt:
	    واپس "cbStmt";
	  حالت cbForce:
	    واپس "cbForce";
	  حالت cbRelease:
	    واپس "cbRelease";
	  حالت cb_At_آغاز_کا_Sim_وقت:
	    واپس "cbAtStartOfSimTime";
	  حالت cbReadWriteSynch:
	    واپس "cbReadWriteSynch";
	  حالت cbReadOnlySynch:
	    واپس "cbReadOnlySynch";
	  حالت cb_اگلا_Sim_وقت:
	    واپس "cbNextSimTime";
	  حالت cbAfterDelay:
	    واپس "cbAfterDelay";
	  حالت cb_انتہا_کا_Compile:
	    واپس "cbEndOfCompile";
	  حالت cb_آغاز_کا_Simulation:
	    واپس "cbStartOfSimulation";
	  حالت cb_انتہا_کا_Simulation:
	    واپس "cbEndOfSimulation";
	  حالت cbError:
	    واپس "cbError";
	  حالت cbTchkViolation:
	    واپس "cbTchkViolation";
	  حالت cb_آغاز_کا_Save:
	    واپس "cbStartOfSave";
	  حالت cb_انتہا_کا_Save:
	    واپس "cbEndOfSave";
	  حالت cb_آغاز_کا_Restart:
	    واپس "cbStartOfRestart";
	  حالت cb_انتہا_کا_Restart:
	    واپس "cbEndOfRestart";
	  حالت cb_آغاز_کا_Reset:
	    واپس "cbStartOfReset";
	  حالت cb_انتہا_کا_Reset:
	    واپس "cbEndOfReset";
	  حالت cbEnterInteractive:
	    واپس "cbEnterInteractive";
	  حالت cbExitInteractive:
	    واپس "cbExitInteractive";
	  حالت cbInteractiveScopeChange:
	    واپس "cbInteractiveScopeChange";
	  حالت cbUnresolvedSystf:
	    واپس "cbUnresolvedSystf";
	  حالت cb_At_انتہا_کا_Sim_وقت:
	    واپس "cbAtEndOfSimTime";
	  طےشدہ:
	    واپس "unrecognised";
      }
}

ساکن منطقی check_callback_وقت(p_cb_ڈیٹا ڈیٹا, منطقی allow_suppress)
{
      أكد(ڈیٹا);
      اگر (!ڈیٹا->time) {
	    اگر (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(ڈیٹا->reason));
		  واپس جھوٹ;
	    }
	    واپس سچ;
      }
      انتخاب (ڈیٹا->time->type) {
	  حالت vpi_Sim_وقت:
	    توڑو;
	  حالت vpi_Scaled_Real_وقت:
	    توڑو;
	  حالت vpi_Suppress_وقت:
	    اگر (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(ڈیٹا->reason));
		  واپس جھوٹ;
	    }
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(ڈیٹا->reason));
	    واپس جھوٹ;
      }
      واپس سچ;
}

ساکن خالی سیٹ_callback_وقت(p_cb_ڈیٹا ڈیٹا)
{
      أكد(ڈیٹا && ڈیٹا->time);
      ڈیٹا->time->کم  = 0;
      ڈیٹا->time->بلند = 0;
      ڈیٹا->time->real = 0.0;
      انتخاب (ڈیٹا->time->type) {
	  حالت vpi_Sim_وقت:
	    vpip_وقت_مقصد_timestruct(ڈیٹا->time, جدول_وقت_simtime());
	    توڑو;
	  حالت vpi_Scaled_Real_وقت:
	    ڈیٹا->time->real = vpip_scaled_وقت_from_handle(جدول_وقت_simtime(), ڈیٹا->obj);
	    توڑو;
	  حالت vpi_Suppress_وقت:
	    توڑو;
	  طےشدہ:
	    أكد(0);
	    توڑو;
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

جماعت sync_callback;

ساخت sync_cb  : عوامی vvp_gen_event_s {
      sync_callback*handle;
      منطقی sync_flag;

      ~sync_cb () override { }

      مجازی خالی تسلسل_تسلسل() override;
};

اندرونی __vpiCallback::__vpiCallback()
{
      اگلا_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

صحیح __vpiCallback::get_type_رمز(خالی) ثابت
{ واپس vpiCallback; }


قدر_callback::قدر_callback(p_cb_ڈیٹا ڈیٹا)
{
      cb_ڈیٹا = *ڈیٹا;
      اگر (ڈیٹا->time) {
	    cb_وقت = *(ڈیٹا->time);
      } ورنہ {
	    cb_وقت.type = vpi_Suppress_وقت;
      }
      cb_ڈیٹا.time = &cb_وقت;
      اگر (ڈیٹا->value) {
	    cb_قدر = *(ڈیٹا->value);
      } ورنہ {
	    cb_قدر.format = vpiSuppressVal;
      }
      cb_ڈیٹا.value = &cb_قدر;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
منطقی قدر_callback::test_قدر_callback_ready(خالی)
{
      واپس سچ;
}

ساکن خالی vpip_real_قدر_change(قدر_callback*cbh, vpiHandle ref)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      أكد(obj);

      obj->جوڑو_vpi_callback(cbh);
}

جماعت قدر_part_callback : عوامی قدر_callback {
    عوامی:
      صریح قدر_part_callback(p_cb_ڈیٹا ڈیٹا);
      ~قدر_part_callback() override;

      قدر_part_callback(ثابت قدر_part_callback&) = حذف;
      قدر_part_callback& عامل=(ثابت قدر_part_callback&) = حذف;

      منطقی test_قدر_callback_ready(خالی) override;

    نجی:
      حرف*قدر_بٹ;
      size_t قدر_off;
};

اندرونی قدر_part_callback::قدر_part_callback(p_cb_ڈیٹا ڈیٹا)
: قدر_callback(ڈیٹا)
{
      ساخت __vpiPV*pobj = dynamic_cast<__vpiPV*>(ڈیٹا->obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

      sig_fil->جوڑو_vpi_callback(یہ);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      sig_fil->get_قدر(&tmp_قدر);

      قدر_بٹ = نیا حرف[pobj->width+1];
      قدر_off = pobj->والد->vpi_get(vpi_سائز) - pobj->width - pobj->tbase;

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str + قدر_off, pobj->width);
      قدر_بٹ[pobj->width] = 0;
}

قدر_part_callback::~قدر_part_callback()
{
      حذف[]قدر_بٹ;
}

منطقی قدر_part_callback::test_قدر_callback_ready(خالی)
{
      ساخت __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_ڈیٹا.obj);
      أكد(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      أكد(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      sig_fil->get_قدر(&tmp_قدر);

      اگر (memcmp(قدر_بٹ, tmp_قدر.value.str + قدر_off, pobj->width) == 0)
	    واپس جھوٹ;

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str + قدر_off, pobj->width);
      واپس سچ;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
ساکن قدر_callback*make_قدر_change_part(p_cb_ڈیٹا ڈیٹا)
{
	/* Attach the __vpiCallback object to the signal. */
      قدر_callback*cbh = نیا قدر_part_callback(ڈیٹا);
      واپس cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
ساکن قدر_callback* make_قدر_change(p_cb_ڈیٹا ڈیٹا)
{
      اگر (!check_callback_وقت(ڈیٹا, سچ))
	    واپس 0;

      اگر (vpi_get(vpiAutomatic, ڈیٹا->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, ڈیٹا->obj));
            واپس 0;
      }

	// Special case: the target object is a vpiPartSelect
      اگر (ڈیٹا->obj->get_type_رمز() == vpiPartSelect) {
            اگر (ڈیٹا->obj->vpi_handle(vpiArray))
	        واپس vpip_array_لفظ_change(ڈیٹا);
            ورنہ
	        واپس make_قدر_change_part(ڈیٹا);
      }

      اگر (ڈیٹا->obj->get_type_رمز() == vpi_حافظہ_لفظ)
	    واپس vpip_array_لفظ_change(ڈیٹا);

      اگر (ڈیٹا->obj->get_type_رمز() == vpi_حافظہ)
	    واپس vpip_array_change(ڈیٹا);

      قدر_callback*obj = نیا قدر_callback(ڈیٹا);

      أكد(ڈیٹا->obj);
      انتخاب (ڈیٹا->obj->get_type_رمز()) {

	  حالت vpiReg:
	  حالت vpiNet:
	  حالت vpiIntegerVar:
	  حالت vpi_ثنائی_ہندسہ_Var:
	  حالت vpiByteVar:
	  حالت vpiShortIntVar:
	  حالت vpiIntVar:
	  حالت vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    ساخت __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(ڈیٹا->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->گرہ->fil);
	    أكد(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->جوڑو_vpi_callback(obj);
	    توڑو;

	  حالت vpiRealVar:
	    vpip_real_قدر_change(obj, ڈیٹا->obj);
	    توڑو;

	  حالت vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(ڈیٹا->obj);
	    nev->جوڑو_vpi_callback(obj);
	    توڑو;

	  حالت vpiModule:
	  حالت vpiConstant:
	  حالت vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    ڈیٹا->obj->get_type_رمز());
	    حذف obj;
	    واپس 0;
      }

      واپس obj;
}

جماعت sync_callback : عوامی __vpiCallback {
    عوامی:
      صریح sync_callback(p_cb_ڈیٹا ڈیٹا);
      ~sync_callback() override;

      sync_callback(ثابت sync_callback&) = حذف;
      sync_callback& عامل=(ثابت sync_callback&) = حذف;

    عوامی:
	// scheduled event
      ساخت sync_cb* cb_sync;
	// user supplied callback data
      ساخت t_vpi_وقت cb_وقت;

    نجی:
};

اندرونی sync_callback::sync_callback(p_cb_ڈیٹا ڈیٹا)
{
      cb_sync = 0;

      cb_ڈیٹا = *ڈیٹا;
      أكد(ڈیٹا->time);
      cb_وقت = *(ڈیٹا->time);
      cb_ڈیٹا.time = &cb_وقت;
}

sync_callback::~sync_callback()
{
      حذف cb_sync;
}

خالی sync_cb::تسلسل_تسلسل()
{
      اگر (handle == 0)
	    واپس;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
	    سیٹ_callback_وقت(&cur->cb_ڈیٹا);
	    أكد(vpi_منوال_flag == VPI_منوال_NONE);
	    vpi_منوال_flag = sync_flag? VPI_منوال_ROSYNC : VPI_منوال_RWSYNC;
	    (cur->cb_ڈیٹا.cb_rtn)(&cur->cb_ڈیٹا);
	    vpi_منوال_flag = VPI_منوال_NONE;
      }

      حذف cur;
}

ساکن vvp_time64_t get_sync_cb_وقت(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      انتخاب (obj->cb_وقت.type) {
	  حالت vpi_Sim_وقت:
	    tv = vpip_timestruct_مقصد_وقت(&obj->cb_وقت);
	    توڑو;

	  حالت vpi_Scaled_Real_وقت:
	    tv = vpip_scaled_real_مقصد_time64(obj->cb_وقت.real,
		     vpip_timescale_scope_from_handle(obj->cb_ڈیٹا.obj));
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (صحیح)obj->cb_وقت.type);
	    أكد(0);
	    توڑو;
      }
      واپس tv;
}

ساکن sync_callback* make_sync(p_cb_ڈیٹا ڈیٹا, منطقی readonly_flag)
{
      اگر (!check_callback_وقت(ڈیٹا, جھوٹ))
	    واپس 0;

      sync_callback*obj = نیا sync_callback(ڈیٹا);

      ساخت sync_cb*cb = نیا sync_cb;
      cb->sync_flag = readonly_flag? سچ : جھوٹ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      جدول_وقت_generic(cb, tv, سچ, readonly_flag);

      واپس obj;
}

ساکن ساخت __vpiCallback* make_afterdelay(p_cb_ڈیٹا ڈیٹا)
{
      اگر (!check_callback_وقت(ڈیٹا, جھوٹ))
	    واپس 0;

      sync_callback*obj = نیا sync_callback(ڈیٹا);
      ساخت sync_cb*cb = نیا sync_cb;
      cb->sync_flag = جھوٹ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      جدول_وقت_generic(cb, tv, جھوٹ);

      واپس obj;
}

ساکن ساخت __vpiCallback* make_at_آغاز_کا_sim_وقت(p_cb_ڈیٹا ڈیٹا)
{
      اگر (!check_callback_وقت(ڈیٹا, جھوٹ))
	    واپس 0;

      sync_callback*obj = نیا sync_callback(ڈیٹا);
      ساخت sync_cb*cb = نیا sync_cb;
      cb->sync_flag = جھوٹ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      vvp_time64_t cur = جدول_وقت_simtime();
      اگر (cur > tv) {
	    tv = 0;
	    أكد(0);
      } ورنہ اگر (cur == tv) {
	    tv = 0;
      } ورنہ {
	    tv -= cur;
      }
      جدول_وقت_at_آغاز_کا_simtime(cb, tv);

      واپس obj;
}

ساکن ساخت __vpiCallback* make_at_انتہا_کا_sim_وقت(p_cb_ڈیٹا ڈیٹا)
{
      اگر (!check_callback_وقت(ڈیٹا, جھوٹ))
	    واپس 0;

      sync_callback*obj = نیا sync_callback(ڈیٹا);
      ساخت sync_cb*cb = نیا sync_cb;
      cb->sync_flag = جھوٹ;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_وقت(obj);
      vvp_time64_t cur = جدول_وقت_simtime();
      اگر (cur > tv) {
	    tv = 0;
	    أكد(0);
      } ورنہ اگر (cur == tv) {
	    tv = 0;
      } ورنہ {
	    tv -= cur;
      }
      جدول_وقت_at_انتہا_کا_simtime(cb, tv);

      واپس obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

جماعت simulator_callback : عوامی __vpiCallback {
    عوامی:
      صریح simulator_callback(ثابت ساخت t_cb_ڈیٹا*ڈیٹا);

    عوامی:
      ساخت t_vpi_وقت cb_وقت;
};

اندرونی simulator_callback::simulator_callback(ثابت ساخت t_cb_ڈیٹا*ڈیٹا)
{
      cb_ڈیٹا = *ڈیٹا;
      اگر ((ڈیٹا->reason == cb_اگلا_Sim_وقت) && ڈیٹا->time) {
	    cb_وقت = *(ڈیٹا->time);
      } ورنہ اگر (ڈیٹا->reason == cb_انتہا_کا_Simulation) {
	    cb_وقت.type = vpi_Sim_وقت;
      } ورنہ {
	    cb_وقت.type = vpi_Suppress_وقت;
      }
      cb_ڈیٹا.time = &cb_وقت;
}

ساکن simulator_callback*اگلا_Sim_وقت = 0;
ساکن simulator_callback*انتہا_کا_Compile = 0;
ساکن simulator_callback*آغاز_کا_Simulation = 0;
ساکن simulator_callback*انتہا_کا_Simulation = 0;

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
خالی simulator_cb_delete(خالی)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      جب (انتہا_کا_Compile) {
	    cur = انتہا_کا_Compile;
	    انتہا_کا_Compile = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    حذف cur;
      }

	/* Delete all the start of simulation callbacks. */
      جب (آغاز_کا_Simulation) {
	    cur = آغاز_کا_Simulation;
	    آغاز_کا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    حذف cur;
      }

	/* Delete all the end of simulation callbacks. */
      جب (انتہا_کا_Simulation) {
	    cur = انتہا_کا_Simulation;
	    انتہا_کا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    حذف cur;
      }
}
#اگر_ختم

خالی vpi_انتہا_کا_Compile(خالی) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      جب (انتہا_کا_Compile) {
	    cur = انتہا_کا_Compile;
	    انتہا_کا_Compile = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
		  (cur->cb_ڈیٹا.cb_rtn)(&cur->cb_ڈیٹا);
	    }
	    حذف cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

خالی vpi_آغاز_کا_Sim(خالی) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      جب (آغاز_کا_Simulation) {
	    cur = آغاز_کا_Simulation;
	    آغاز_کا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
		  (cur->cb_ڈیٹا.cb_rtn)(&cur->cb_ڈیٹا);
	    }
	    حذف cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

خالی vpiPostsim(خالی) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_ROSYNC;

      جب (انتہا_کا_Simulation) {
	    cur = انتہا_کا_Simulation;
	    انتہا_کا_Simulation = dynamic_cast<simulator_callback*>(cur->اگلا_2);
	    اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
		  سیٹ_callback_وقت(&cur->cb_ڈیٹا);
		  (cur->cb_ڈیٹا.cb_rtn)(&cur->cb_ڈیٹا);
	    }
	    حذف cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
خالی vpi_اگلا_Sim_وقت(خالی)
{
      simulator_callback* cur;
      simulator_callback* اگلا_2 = اگلا_Sim_وقت;
      اگلا_Sim_وقت = 0;

      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      جب (اگلا_2) {
            cur = اگلا_2;
            اگلا_2 = dynamic_cast<simulator_callback*>(cur->اگلا_2);
            اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
                  سیٹ_callback_وقت(&cur->cb_ڈیٹا);
                  (cur->cb_ڈیٹا.cb_rtn)(&cur->cb_ڈیٹا);
            }
            حذف cur;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

ساکن simulator_callback* make_prepost(p_cb_ڈیٹا ڈیٹا)
{
      اگر ((ڈیٹا->reason == cb_اگلا_Sim_وقت) && !check_callback_وقت(ڈیٹا, سچ))
	    واپس 0;

      simulator_callback*obj = نیا simulator_callback(ڈیٹا);

      /* Insert at head of list */
      انتخاب (ڈیٹا->reason) {
	  حالت cb_انتہا_کا_Compile:
	    obj->اگلا_2 = انتہا_کا_Compile;
	    انتہا_کا_Compile = obj;
	    توڑو;
	  حالت cb_آغاز_کا_Simulation:
	    obj->اگلا_2 = آغاز_کا_Simulation;
	    آغاز_کا_Simulation = obj;
	    توڑو;
	  حالت cb_انتہا_کا_Simulation:
	    obj->اگلا_2 = انتہا_کا_Simulation;
	    انتہا_کا_Simulation = obj;
	    توڑو;
	  حالت cb_اگلا_Sim_وقت:
	    obj->اگلا_2 = اگلا_Sim_وقت;
	    اگلا_Sim_وقت = obj;
	    توڑو;
      }

      واپس obj;
}

vpiHandle vpi_رجسٹر_cb(p_cb_ڈیٹا ڈیٹا)
{
      ساخت __vpiCallback*obj = 0;

      أكد(ڈیٹا);
      انتخاب (ڈیٹا->reason) {

	  حالت cb_قدر_Change:
	    obj = make_قدر_change(ڈیٹا);
	    توڑو;

	  حالت cbReadOnlySynch:
	    obj = make_sync(ڈیٹا, سچ);
	    توڑو;

	  حالت cbReadWriteSynch:
	    obj = make_sync(ڈیٹا, جھوٹ);
	    توڑو;

	  حالت cb_At_آغاز_کا_Sim_وقت:
	    obj = make_at_آغاز_کا_sim_وقت(ڈیٹا);
	    توڑو;

	  حالت cb_At_انتہا_کا_Sim_وقت:
	    obj = make_at_انتہا_کا_sim_وقت(ڈیٹا);
	    توڑو;

	  حالت cbAfterDelay:
	    obj = make_afterdelay(ڈیٹا);
	    توڑو;

	  حالت cb_انتہا_کا_Compile:
	  حالت cb_آغاز_کا_Simulation:
	  حالت cb_انتہا_کا_Simulation:
	  حالت cb_اگلا_Sim_وقت:
	    obj = make_prepost(ڈیٹا);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (صحیح)ڈیٹا->reason);
	    توڑو;
      }

      واپس obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      ساخت __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      أكد(obj);
      obj->cb_ڈیٹا.cb_rtn = 0;

      واپس 1;
}

خالی callback_execute(ساخت __vpiCallback*cur)
{
      ثابت vpi_منوال_t save_منوال = vpi_منوال_flag;
      vpi_منوال_flag = VPI_منوال_RWSYNC;

      أكد(cur->cb_ڈیٹا.cb_rtn);
      سیٹ_callback_وقت(&cur->cb_ڈیٹا);
      (cur->cb_ڈیٹا.cb_rtn)(&cur->cb_ڈیٹا);

      vpi_منوال_flag = save_منوال;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
ساخت __vpi_array_word {
      ساخت __vpi_array_word* اگلا_2;
      ساخت __vpiArray* array;
      غیرعلامتی طویل لفظ;
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

خالی vvp_vpi_callback::attach_as_لفظ(vvp_array_t arr, غیرعلامتی طویل addr)
{
      ساخت __vpi_array_word*tmp = نیا __vpi_array_word;
      tmp->array = arr;
      tmp->لفظ = addr;
      tmp->اگلا_2 = array_الفاظ;
      array_الفاظ = tmp;
}

خالی vvp_vpi_callback::جوڑو_vpi_callback(قدر_callback*cb)
{
      cb->اگلا_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_vpi_callback::clear_all_callbacks()
{
      جب (vpi_callbacks_) {
	    قدر_callback *tmp = dynamic_cast<قدر_callback*>
	                            (vpi_callbacks_->اگلا_2);
	    حذف vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      جب (array_الفاظ) {
	    ساخت __vpi_array_word*tmp = array_الفاظ->اگلا_2;
	    حذف array_الفاظ;
	    array_الفاظ = tmp;
      }
}
#اگر_ختم

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
خالی vvp_vpi_callback::تسلسل_vpi_callbacks()
{
      ساخت __vpi_array_word*array_لفظ = array_الفاظ;
      جب (array_لفظ) {
	    array_لفظ->array->لفظ_change(array_لفظ->لفظ);
	    array_لفظ = array_لفظ->اگلا_2;
      }

      قدر_callback *اگلا_2 = vpi_callbacks_;
      قدر_callback *prev = 0;

      جب (اگلا_2) {
	    قدر_callback*cur = اگلا_2;
	    اگلا_2 = dynamic_cast<قدر_callback*>(cur->اگلا_2);

	    اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
		  اگر (cur->test_قدر_callback_ready()) {
			اگر (cur->cb_ڈیٹا.value)
			      get_قدر(cur->cb_ڈیٹا.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } ورنہ اگر (prev == 0) {

		  vpi_callbacks_ = اگلا_2;
		  cur->اگلا_2 = 0;
		  حذف cur;

	    } ورنہ {
		  أكد(prev->اگلا_2 == cur);
		  prev->اگلا_2 = اگلا_2;
		  cur->اگلا_2 = 0;
		  حذف cur;
	    }
      }
}

خالی vvp_signal_قدر::get_signal_قدر(ساخت t_vpi_قدر*vp)
{
      انتخاب (vp->format) {
	  حالت vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    توڑو;

	  حالت vpiBinStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiHexStrVal:
	  حالت vpiIntVal:
	  حالت vpi_سمتیہ_Val:
	  حالت vpiStringVal:
	  حالت vpiRealVal: {
	    غیرعلامتی wid = قدر_سائز();
	    vvp_vector4_t vec4(wid);
	    برائے (غیرعلامتی idx = 0; idx < wid; idx += 1) {
		  vec4.سیٹ_ثنائی_ہندسہ(idx, value(idx));
	    }
	    vpip_vec4_get_قدر(vec4, wid, جھوٹ, vp);
	    توڑو;
	  }

	  حالت vpiSuppressVal:
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (صحیح)vp->format);
      }
}

ساکن خالی real_signal_قدر(ساخت t_vpi_قدر*vp, دوہرا rval)
{
      ساکن ثابت size_t RBUF_سائز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_سائز, RBUF_VAL));

      انتخاب (vp->format) {
	  حالت vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالت vpiRealVal:
	    vp->value.real = rval;
	    توڑو;

	  حالت vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    اگر (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } ورنہ {
		  rval = std::باری_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    توڑو;

	  حالت vpiDecStrVal:
	    اگر (std::isnan(rval))
		  snprintf(rbuf, RBUF_سائز, "%s", "nan");
	    ورنہ
		  snprintf(rbuf, RBUF_سائز, "%0.0f", std::باری_2(rval));
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiHexStrVal:
	    snprintf(rbuf, RBUF_سائز, "%" PRIx64, vlg_باری_مقصد_u64(rval));
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiBinStrVal: {
		uint64_t val = vlg_باری_مقصد_u64(rval);
		غیرعلامتی len = 0;

		جب (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_باری_مقصد_u64(rval);
		برائے (غیرعلامتی idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		اگر (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		توڑو;
	  }

	  حالت vpiSuppressVal:
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (صحیح)vp->format);
      }
}

خالی vvp_fun_signal_real_aa::get_signal_قدر(ساخت t_vpi_قدر*vp)
{
      real_signal_قدر(vp, real_قدر());
}

خالی vvp_wire_real::get_signal_قدر(ساخت t_vpi_قدر*vp)
{
      real_signal_قدر(vp, real_قدر());
}

خالی vvp_fun_signal_string_aa::get_signal_قدر(ساخت t_vpi_قدر*)
{
      أكد(0);
}
#اگر 0
خالی vvp_wire_string::get_signal_قدر(ساخت t_vpi_قدر*vp)
{
      أكد(0);
}
#اگر_ختم
خالی vvp_wire_vec4::get_قدر(ساخت t_vpi_قدر*val)
{
      get_signal_قدر(val);
}

خالی vvp_wire_vec8::get_قدر(ساخت t_vpi_قدر*val)
{
      get_signal_قدر(val);
}

خالی vvp_wire_real::get_قدر(ساخت t_vpi_قدر*val)
{
      get_signal_قدر(val);
}
#اگر 0
خالی vvp_wire_string::get_قدر(ساخت t_vpi_قدر*val)
{
      أكد(0);
}
#اگر_ختم
