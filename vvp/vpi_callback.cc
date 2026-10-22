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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
#अन्तर्भाव  "vvp_cleanup.h"
#यदि_अन्तः
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

उपयुज्य नामाकाश std;

स्थिर ध्रुव वर्ण*cb_reason_name(PLI_INT32 reason)
{
      विकल्प (reason) {
	  पक्ष cb_मानम्_Change:
	    प्रत्यागम "cbValueChange";
	  पक्ष cbStmt:
	    प्रत्यागम "cbStmt";
	  पक्ष cbForce:
	    प्रत्यागम "cbForce";
	  पक्ष cbRelease:
	    प्रत्यागम "cbRelease";
	  पक्ष cb_At_Start_सम्बन्धि_Sim_Time:
	    प्रत्यागम "cbAtStartOfSimTime";
	  पक्ष cbReadWriteSynch:
	    प्रत्यागम "cbReadWriteSynch";
	  पक्ष cbReadOnlySynch:
	    प्रत्यागम "cbReadOnlySynch";
	  पक्ष cb_अग्र_Sim_Time:
	    प्रत्यागम "cbNextSimTime";
	  पक्ष cbAfterDelay:
	    प्रत्यागम "cbAfterDelay";
	  पक्ष cb_अन्तः_सम्बन्धि_Compile:
	    प्रत्यागम "cbEndOfCompile";
	  पक्ष cb_Start_सम्बन्धि_Simulation:
	    प्रत्यागम "cbStartOfSimulation";
	  पक्ष cb_अन्तः_सम्बन्धि_Simulation:
	    प्रत्यागम "cbEndOfSimulation";
	  पक्ष cbError:
	    प्रत्यागम "cbError";
	  पक्ष cbTchkViolation:
	    प्रत्यागम "cbTchkViolation";
	  पक्ष cb_Start_सम्बन्धि_Save:
	    प्रत्यागम "cbStartOfSave";
	  पक्ष cb_अन्तः_सम्बन्धि_Save:
	    प्रत्यागम "cbEndOfSave";
	  पक्ष cb_Start_सम्बन्धि_Restart:
	    प्रत्यागम "cbStartOfRestart";
	  पक्ष cb_अन्तः_सम्बन्धि_Restart:
	    प्रत्यागम "cbEndOfRestart";
	  पक्ष cb_Start_सम्बन्धि_Reset:
	    प्रत्यागम "cbStartOfReset";
	  पक्ष cb_अन्तः_सम्बन्धि_Reset:
	    प्रत्यागम "cbEndOfReset";
	  पक्ष cbEnterInteractive:
	    प्रत्यागम "cbEnterInteractive";
	  पक्ष cbExitInteractive:
	    प्रत्यागम "cbExitInteractive";
	  पक्ष cbInteractiveScopeChange:
	    प्रत्यागम "cbInteractiveScopeChange";
	  पक्ष cbUnresolvedSystf:
	    प्रत्यागम "cbUnresolvedSystf";
	  पक्ष cb_At_अन्तः_सम्बन्धि_Sim_Time:
	    प्रत्यागम "cbAtEndOfSimTime";
	  स्वभाव:
	    प्रत्यागम "unrecognised";
      }
}

स्थिर तार्किक check_callback_time(p_cb_दत्त दत्त, तार्किक allow_suppress)
{
      प्रतिज्ञा(दत्त);
      यदि (!दत्त->time) {
	    यदि (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(दत्त->reason));
		  प्रत्यागम असत्य;
	    }
	    प्रत्यागम सत्य;
      }
      विकल्प (दत्त->time->type) {
	  पक्ष vpiSimTime:
	    भङ्ग;
	  पक्ष vpiScaledRealTime:
	    भङ्ग;
	  पक्ष vpiSuppressTime:
	    यदि (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(दत्त->reason));
		  प्रत्यागम असत्य;
	    }
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(दत्त->reason));
	    प्रत्यागम असत्य;
      }
      प्रत्यागम सत्य;
}

स्थिर शून्य समुच्चयः_callback_time(p_cb_दत्त दत्त)
{
      प्रतिज्ञा(दत्त && दत्त->time);
      दत्त->time->नीचः  = 0;
      दत्त->time->उच्चः = 0;
      दत्त->time->real = 0.0;
      विकल्प (दत्त->time->type) {
	  पक्ष vpiSimTime:
	    vpip_time_गन्तव्यम्_timestruct(दत्त->time, कालसारणी_simtime());
	    भङ्ग;
	  पक्ष vpiScaledRealTime:
	    दत्त->time->real = vpip_scaled_time_from_handle(कालसारणी_simtime(), दत्त->obj);
	    भङ्ग;
	  पक्ष vpiSuppressTime:
	    भङ्ग;
	  स्वभाव:
	    प्रतिज्ञा(0);
	    भङ्ग;
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

वर्ग sync_callback;

संरचना sync_cb  : सार्वजनिक vvp_gen_event_s {
      sync_callback*handle;
      तार्किक sync_flag;

      ~sync_cb () override { }

      आभास शून्य सततम्_सततम्() override;
};

अन्तःस्थ __vpiCallback::__vpiCallback()
{
      अग्र_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

पूर्णाङ्क __vpiCallback::get_type_संहिता(शून्य) ध्रुव
{ प्रत्यागम vpiCallback; }


मानम्_callback::मानम्_callback(p_cb_दत्त दत्त)
{
      cb_दत्त = *दत्त;
      यदि (दत्त->time) {
	    cb_time = *(दत्त->time);
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_दत्त.time = &cb_time;
      यदि (दत्त->value) {
	    cb_मानम् = *(दत्त->value);
      } अन्यथा {
	    cb_मानम्.format = vpiSuppressVal;
      }
      cb_दत्त.value = &cb_मानम्;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
तार्किक मानम्_callback::test_मानम्_callback_ready(शून्य)
{
      प्रत्यागम सत्य;
}

स्थिर शून्य vpip_real_मानम्_change(मानम्_callback*cbh, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      प्रतिज्ञा(obj);

      obj->सङ्कलय_vpi_callback(cbh);
}

वर्ग मानम्_part_callback : सार्वजनिक मानम्_callback {
    सार्वजनिक:
      स्पष्ट मानम्_part_callback(p_cb_दत्त दत्त);
      ~मानम्_part_callback() override;

      मानम्_part_callback(ध्रुव मानम्_part_callback&) = विलोपय;
      मानम्_part_callback& संक्रियकः=(ध्रुव मानम्_part_callback&) = विलोपय;

      तार्किक test_मानम्_callback_ready(शून्य) override;

    गुप्त:
      वर्ण*मानम्_द्वयङ्काः;
      size_t मानम्_off;
};

अन्तःस्थ मानम्_part_callback::मानम्_part_callback(p_cb_दत्त दत्त)
: मानम्_callback(दत्त)
{
      संरचना __vpiPV*pobj = dynamic_cast<__vpiPV*>(दत्त->obj);
      प्रतिज्ञा(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      प्रतिज्ञा(sig_fil);

      sig_fil->सङ्कलय_vpi_callback(एतत्);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_मानम् tmp_मानम्;
      tmp_मानम्.format = vpiBinStrVal;
      sig_fil->get_मानम्(&tmp_मानम्);

      मानम्_द्वयङ्काः = नूतन वर्ण[pobj->width+1];
      मानम्_off = pobj->parent->vpi_get(vpi_आकार) - pobj->width - pobj->tbase;

      स्मृतिं_प्रतिलिख(मानम्_द्वयङ्काः, tmp_मानम्.value.str + मानम्_off, pobj->width);
      मानम्_द्वयङ्काः[pobj->width] = 0;
}

मानम्_part_callback::~मानम्_part_callback()
{
      विलोपय[]मानम्_द्वयङ्काः;
}

तार्किक मानम्_part_callback::test_मानम्_callback_ready(शून्य)
{
      संरचना __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_दत्त.obj);
      प्रतिज्ञा(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      प्रतिज्ञा(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मानम् tmp_मानम्;
      tmp_मानम्.format = vpiBinStrVal;
      sig_fil->get_मानम्(&tmp_मानम्);

      यदि (memcmp(मानम्_द्वयङ्काः, tmp_मानम्.value.str + मानम्_off, pobj->width) == 0)
	    प्रत्यागम असत्य;

      स्मृतिं_प्रतिलिख(मानम्_द्वयङ्काः, tmp_मानम्.value.str + मानम्_off, pobj->width);
      प्रत्यागम सत्य;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
स्थिर मानम्_callback*make_मानम्_change_part(p_cb_दत्त दत्त)
{
	/* Attach the __vpiCallback object to the signal. */
      मानम्_callback*cbh = नूतन मानम्_part_callback(दत्त);
      प्रत्यागम cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
स्थिर मानम्_callback* make_मानम्_change(p_cb_दत्त दत्त)
{
      यदि (!check_callback_time(दत्त, सत्य))
	    प्रत्यागम 0;

      यदि (vpi_get(vpiAutomatic, दत्त->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, दत्त->obj));
            प्रत्यागम 0;
      }

	// Special case: the target object is a vpiPartSelect
      यदि (दत्त->obj->get_type_संहिता() == vpiPartSelect) {
            यदि (दत्त->obj->vpi_handle(vpiArray))
	        प्रत्यागम vpip_array_word_change(दत्त);
            अन्यथा
	        प्रत्यागम make_मानम्_change_part(दत्त);
      }

      यदि (दत्त->obj->get_type_संहिता() == vpi_स्मृतिः_Word)
	    प्रत्यागम vpip_array_word_change(दत्त);

      यदि (दत्त->obj->get_type_संहिता() == vpi_स्मृतिः)
	    प्रत्यागम vpip_array_change(दत्त);

      मानम्_callback*obj = नूतन मानम्_callback(दत्त);

      प्रतिज्ञा(दत्त->obj);
      विकल्प (दत्त->obj->get_type_संहिता()) {

	  पक्ष vpiReg:
	  पक्ष vpiNet:
	  पक्ष vpiIntegerVar:
	  पक्ष vpiBitVar:
	  पक्ष vpiByteVar:
	  पक्ष vpiShortIntVar:
	  पक्ष vpiIntVar:
	  पक्ष vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    संरचना __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(दत्त->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->ग्रन्थिः->fil);
	    प्रतिज्ञा(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->सङ्कलय_vpi_callback(obj);
	    भङ्ग;

	  पक्ष vpiRealVar:
	    vpip_real_मानम्_change(obj, दत्त->obj);
	    भङ्ग;

	  पक्ष vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(दत्त->obj);
	    nev->सङ्कलय_vpi_callback(obj);
	    भङ्ग;

	  पक्ष vpiModule:
	  पक्ष vpiConstant:
	  पक्ष vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    दत्त->obj->get_type_संहिता());
	    विलोपय obj;
	    प्रत्यागम 0;
      }

      प्रत्यागम obj;
}

वर्ग sync_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      स्पष्ट sync_callback(p_cb_दत्त दत्त);
      ~sync_callback() override;

      sync_callback(ध्रुव sync_callback&) = विलोपय;
      sync_callback& संक्रियकः=(ध्रुव sync_callback&) = विलोपय;

    सार्वजनिक:
	// scheduled event
      संरचना sync_cb* cb_sync;
	// user supplied callback data
      संरचना t_vpi_time cb_time;

    गुप्त:
};

अन्तःस्थ sync_callback::sync_callback(p_cb_दत्त दत्त)
{
      cb_sync = 0;

      cb_दत्त = *दत्त;
      प्रतिज्ञा(दत्त->time);
      cb_time = *(दत्त->time);
      cb_दत्त.time = &cb_time;
}

sync_callback::~sync_callback()
{
      विलोपय cb_sync;
}

शून्य sync_cb::सततम्_सततम्()
{
      यदि (handle == 0)
	    प्रत्यागम;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      यदि (cur->cb_दत्त.cb_rtn != 0) {
	    समुच्चयः_callback_time(&cur->cb_दत्त);
	    प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
	    vpi_बहुलकम्_flag = sync_flag? VPI_बहुलकम्_ROSYNC : VPI_बहुलकम्_RWSYNC;
	    (cur->cb_दत्त.cb_rtn)(&cur->cb_दत्त);
	    vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
      }

      विलोपय cur;
}

स्थिर vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      विकल्प (obj->cb_time.type) {
	  पक्ष vpiSimTime:
	    tv = vpip_timestruct_गन्तव्यम्_time(&obj->cb_time);
	    भङ्ग;

	  पक्ष vpiScaledRealTime:
	    tv = vpip_scaled_real_गन्तव्यम्_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_दत्त.obj));
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (पूर्णाङ्क)obj->cb_time.type);
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
      प्रत्यागम tv;
}

स्थिर sync_callback* make_sync(p_cb_दत्त दत्त, तार्किक readonly_flag)
{
      यदि (!check_callback_time(दत्त, असत्य))
	    प्रत्यागम 0;

      sync_callback*obj = नूतन sync_callback(दत्त);

      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = readonly_flag? सत्य : असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      कालसारणी_generic(cb, tv, सत्य, readonly_flag);

      प्रत्यागम obj;
}

स्थिर संरचना __vpiCallback* make_afterdelay(p_cb_दत्त दत्त)
{
      यदि (!check_callback_time(दत्त, असत्य))
	    प्रत्यागम 0;

      sync_callback*obj = नूतन sync_callback(दत्त);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      कालसारणी_generic(cb, tv, असत्य);

      प्रत्यागम obj;
}

स्थिर संरचना __vpiCallback* make_at_start_सम्बन्धि_sim_time(p_cb_दत्त दत्त)
{
      यदि (!check_callback_time(दत्त, असत्य))
	    प्रत्यागम 0;

      sync_callback*obj = नूतन sync_callback(दत्त);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = कालसारणी_simtime();
      यदि (cur > tv) {
	    tv = 0;
	    प्रतिज्ञा(0);
      } अन्यथा यदि (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      कालसारणी_at_start_सम्बन्धि_simtime(cb, tv);

      प्रत्यागम obj;
}

स्थिर संरचना __vpiCallback* make_at_अन्तः_सम्बन्धि_sim_time(p_cb_दत्त दत्त)
{
      यदि (!check_callback_time(दत्त, असत्य))
	    प्रत्यागम 0;

      sync_callback*obj = नूतन sync_callback(दत्त);
      संरचना sync_cb*cb = नूतन sync_cb;
      cb->sync_flag = असत्य;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = कालसारणी_simtime();
      यदि (cur > tv) {
	    tv = 0;
	    प्रतिज्ञा(0);
      } अन्यथा यदि (cur == tv) {
	    tv = 0;
      } अन्यथा {
	    tv -= cur;
      }
      कालसारणी_at_अन्तः_सम्बन्धि_simtime(cb, tv);

      प्रत्यागम obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

वर्ग simulator_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      स्पष्ट simulator_callback(ध्रुव संरचना t_cb_दत्त*दत्त);

    सार्वजनिक:
      संरचना t_vpi_time cb_time;
};

अन्तःस्थ simulator_callback::simulator_callback(ध्रुव संरचना t_cb_दत्त*दत्त)
{
      cb_दत्त = *दत्त;
      यदि ((दत्त->reason == cb_अग्र_Sim_Time) && दत्त->time) {
	    cb_time = *(दत्त->time);
      } अन्यथा यदि (दत्त->reason == cb_अन्तः_सम्बन्धि_Simulation) {
	    cb_time.type = vpiSimTime;
      } अन्यथा {
	    cb_time.type = vpiSuppressTime;
      }
      cb_दत्त.time = &cb_time;
}

स्थिर simulator_callback*अग्र_Sim_Time = 0;
स्थिर simulator_callback*अन्तः_सम्बन्धि_Compile = 0;
स्थिर simulator_callback*Start_सम्बन्धि_Simulation = 0;
स्थिर simulator_callback*अन्तः_सम्बन्धि_Simulation = 0;

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
शून्य simulator_cb_delete(शून्य)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      यावत् (अन्तः_सम्बन्धि_Compile) {
	    cur = अन्तः_सम्बन्धि_Compile;
	    अन्तः_सम्बन्धि_Compile = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    विलोपय cur;
      }

	/* Delete all the start of simulation callbacks. */
      यावत् (Start_सम्बन्धि_Simulation) {
	    cur = Start_सम्बन्धि_Simulation;
	    Start_सम्बन्धि_Simulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    विलोपय cur;
      }

	/* Delete all the end of simulation callbacks. */
      यावत् (अन्तः_सम्बन्धि_Simulation) {
	    cur = अन्तः_सम्बन्धि_Simulation;
	    अन्तः_सम्बन्धि_Simulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    विलोपय cur;
      }
}
#यदि_अन्तः

शून्य vpi_अन्तः_सम्बन्धि_Compile(शून्य) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
      vpi_बहुलकम्_flag = VPI_बहुलकम्_RWSYNC;

      यावत् (अन्तः_सम्बन्धि_Compile) {
	    cur = अन्तः_सम्बन्धि_Compile;
	    अन्तः_सम्बन्धि_Compile = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    यदि (cur->cb_दत्त.cb_rtn != 0) {
		  (cur->cb_दत्त.cb_rtn)(&cur->cb_दत्त);
	    }
	    विलोपय cur;
      }

      vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
}

शून्य vpi_Start_सम्बन्धि_Sim(शून्य) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
      vpi_बहुलकम्_flag = VPI_बहुलकम्_RWSYNC;

      यावत् (Start_सम्बन्धि_Simulation) {
	    cur = Start_सम्बन्धि_Simulation;
	    Start_सम्बन्धि_Simulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    यदि (cur->cb_दत्त.cb_rtn != 0) {
		  (cur->cb_दत्त.cb_rtn)(&cur->cb_दत्त);
	    }
	    विलोपय cur;
      }

      vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
}

शून्य vpiPostsim(शून्य) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
      vpi_बहुलकम्_flag = VPI_बहुलकम्_ROSYNC;

      यावत् (अन्तः_सम्बन्धि_Simulation) {
	    cur = अन्तः_सम्बन्धि_Simulation;
	    अन्तः_सम्बन्धि_Simulation = dynamic_cast<simulator_callback*>(cur->अग्र_2);
	    यदि (cur->cb_दत्त.cb_rtn != 0) {
		  समुच्चयः_callback_time(&cur->cb_दत्त);
		  (cur->cb_दत्त.cb_rtn)(&cur->cb_दत्त);
	    }
	    विलोपय cur;
      }

      vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
शून्य vpi_अग्र_Sim_Time(शून्य)
{
      simulator_callback* cur;
      simulator_callback* अग्र_2 = अग्र_Sim_Time;
      अग्र_Sim_Time = 0;

      प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
      vpi_बहुलकम्_flag = VPI_बहुलकम्_RWSYNC;

      यावत् (अग्र_2) {
            cur = अग्र_2;
            अग्र_2 = dynamic_cast<simulator_callback*>(cur->अग्र_2);
            यदि (cur->cb_दत्त.cb_rtn != 0) {
                  समुच्चयः_callback_time(&cur->cb_दत्त);
                  (cur->cb_दत्त.cb_rtn)(&cur->cb_दत्त);
            }
            विलोपय cur;
      }

      vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
}

स्थिर simulator_callback* make_prepost(p_cb_दत्त दत्त)
{
      यदि ((दत्त->reason == cb_अग्र_Sim_Time) && !check_callback_time(दत्त, सत्य))
	    प्रत्यागम 0;

      simulator_callback*obj = नूतन simulator_callback(दत्त);

      /* Insert at head of list */
      विकल्प (दत्त->reason) {
	  पक्ष cb_अन्तः_सम्बन्धि_Compile:
	    obj->अग्र_2 = अन्तः_सम्बन्धि_Compile;
	    अन्तः_सम्बन्धि_Compile = obj;
	    भङ्ग;
	  पक्ष cb_Start_सम्बन्धि_Simulation:
	    obj->अग्र_2 = Start_सम्बन्धि_Simulation;
	    Start_सम्बन्धि_Simulation = obj;
	    भङ्ग;
	  पक्ष cb_अन्तः_सम्बन्धि_Simulation:
	    obj->अग्र_2 = अन्तः_सम्बन्धि_Simulation;
	    अन्तः_सम्बन्धि_Simulation = obj;
	    भङ्ग;
	  पक्ष cb_अग्र_Sim_Time:
	    obj->अग्र_2 = अग्र_Sim_Time;
	    अग्र_Sim_Time = obj;
	    भङ्ग;
      }

      प्रत्यागम obj;
}

vpiHandle vpi_पञ्जिकाकोशः_cb(p_cb_दत्त दत्त)
{
      संरचना __vpiCallback*obj = 0;

      प्रतिज्ञा(दत्त);
      विकल्प (दत्त->reason) {

	  पक्ष cb_मानम्_Change:
	    obj = make_मानम्_change(दत्त);
	    भङ्ग;

	  पक्ष cbReadOnlySynch:
	    obj = make_sync(दत्त, सत्य);
	    भङ्ग;

	  पक्ष cbReadWriteSynch:
	    obj = make_sync(दत्त, असत्य);
	    भङ्ग;

	  पक्ष cb_At_Start_सम्बन्धि_Sim_Time:
	    obj = make_at_start_सम्बन्धि_sim_time(दत्त);
	    भङ्ग;

	  पक्ष cb_At_अन्तः_सम्बन्धि_Sim_Time:
	    obj = make_at_अन्तः_सम्बन्धि_sim_time(दत्त);
	    भङ्ग;

	  पक्ष cbAfterDelay:
	    obj = make_afterdelay(दत्त);
	    भङ्ग;

	  पक्ष cb_अन्तः_सम्बन्धि_Compile:
	  पक्ष cb_Start_सम्बन्धि_Simulation:
	  पक्ष cb_अन्तः_सम्बन्धि_Simulation:
	  पक्ष cb_अग्र_Sim_Time:
	    obj = make_prepost(दत्त);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (पूर्णाङ्क)दत्त->reason);
	    भङ्ग;
      }

      प्रत्यागम obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      संरचना __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      प्रतिज्ञा(obj);
      obj->cb_दत्त.cb_rtn = 0;

      प्रत्यागम 1;
}

शून्य callback_execute(संरचना __vpiCallback*cur)
{
      ध्रुव vpi_बहुलकम्_t save_बहुलकम् = vpi_बहुलकम्_flag;
      vpi_बहुलकम्_flag = VPI_बहुलकम्_RWSYNC;

      प्रतिज्ञा(cur->cb_दत्त.cb_rtn);
      समुच्चयः_callback_time(&cur->cb_दत्त);
      (cur->cb_दत्त.cb_rtn)(&cur->cb_दत्त);

      vpi_बहुलकम्_flag = save_बहुलकम्;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
संरचना __vpi_array_word {
      संरचना __vpi_array_word* अग्र_2;
      संरचना __vpiArray* array;
      अचिह्नित दीर्घ word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_शब्दाः = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      प्रतिज्ञा(vpi_callbacks_ == 0);
      प्रतिज्ञा(array_शब्दाः == 0);
}

शून्य vvp_vpi_callback::attach_as_word(vvp_array_t arr, अचिह्नित दीर्घ addr)
{
      संरचना __vpi_array_word*tmp = नूतन __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->अग्र_2 = array_शब्दाः;
      array_शब्दाः = tmp;
}

शून्य vvp_vpi_callback::सङ्कलय_vpi_callback(मानम्_callback*cb)
{
      cb->अग्र_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_vpi_callback::clear_all_callbacks()
{
      यावत् (vpi_callbacks_) {
	    मानम्_callback *tmp = dynamic_cast<मानम्_callback*>
	                            (vpi_callbacks_->अग्र_2);
	    विलोपय vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      यावत् (array_शब्दाः) {
	    संरचना __vpi_array_word*tmp = array_शब्दाः->अग्र_2;
	    विलोपय array_शब्दाः;
	    array_शब्दाः = tmp;
      }
}
#यदि_अन्तः

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
शून्य vvp_vpi_callback::सततम्_vpi_callbacks()
{
      संरचना __vpi_array_word*array_word = array_शब्दाः;
      यावत् (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->अग्र_2;
      }

      मानम्_callback *अग्र_2 = vpi_callbacks_;
      मानम्_callback *prev = 0;

      यावत् (अग्र_2) {
	    मानम्_callback*cur = अग्र_2;
	    अग्र_2 = dynamic_cast<मानम्_callback*>(cur->अग्र_2);

	    यदि (cur->cb_दत्त.cb_rtn != 0) {
		  यदि (cur->test_मानम्_callback_ready()) {
			यदि (cur->cb_दत्त.value)
			      get_मानम्(cur->cb_दत्त.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } अन्यथा यदि (prev == 0) {

		  vpi_callbacks_ = अग्र_2;
		  cur->अग्र_2 = 0;
		  विलोपय cur;

	    } अन्यथा {
		  प्रतिज्ञा(prev->अग्र_2 == cur);
		  prev->अग्र_2 = अग्र_2;
		  cur->अग्र_2 = 0;
		  विलोपय cur;
	    }
      }
}

शून्य vvp_signal_मानम्::get_signal_मानम्(संरचना t_vpi_मानम्*vp)
{
      विकल्प (vp->format) {
	  पक्ष vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	  पक्ष vpiOctStrVal:
	  पक्ष vpiDecStrVal:
	  पक्ष vpiHexStrVal:
	  पक्ष vpiIntVal:
	  पक्ष vpi_सदिशम्_Val:
	  पक्ष vpiStringVal:
	  पक्ष vpiRealVal: {
	    अचिह्नित wid = मानम्_आकार();
	    vvp_vector4_t vec4(wid);
	    प्रति (अचिह्नित idx = 0; idx < wid; idx += 1) {
		  vec4.समुच्चयः_bit(idx, value(idx));
	    }
	    vpip_vec4_get_मानम्(vec4, wid, असत्य, vp);
	    भङ्ग;
	  }

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (पूर्णाङ्क)vp->format);
      }
}

स्थिर शून्य real_signal_मानम्(संरचना t_vpi_मानम्*vp, द्विगुण rval)
{
      स्थिर ध्रुव size_t RBUF_आकार = 64 + 1;
      वर्ण*rbuf = static_cast<वर्ण *>(need_परिणाम_buf(RBUF_आकार, RBUF_VAL));

      विकल्प (vp->format) {
	  पक्ष vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  पक्ष vpiRealVal:
	    vp->value.real = rval;
	    भङ्ग;

	  पक्ष vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    यदि (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } अन्यथा {
		  rval = std::आवर्तनम्_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    यदि (std::isnan(rval))
		  snprintf(rbuf, RBUF_आकार, "%s", "nan");
	    अन्यथा
		  snprintf(rbuf, RBUF_आकार, "%0.0f", std::आवर्तनम्_2(rval));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    snprintf(rbuf, RBUF_आकार, "%" PRIx64, vlg_आवर्तनम्_गन्तव्यम्_u64(rval));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiBinStrVal: {
		uint64_t val = vlg_आवर्तनम्_गन्तव्यम्_u64(rval);
		अचिह्नित len = 0;

		यावत् (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_आवर्तनम्_गन्तव्यम्_u64(rval);
		प्रति (अचिह्नित idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		यदि (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		भङ्ग;
	  }

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (पूर्णाङ्क)vp->format);
      }
}

शून्य vvp_fun_signal_real_aa::get_signal_मानम्(संरचना t_vpi_मानम्*vp)
{
      real_signal_मानम्(vp, real_मानम्());
}

शून्य vvp_wire_real::get_signal_मानम्(संरचना t_vpi_मानम्*vp)
{
      real_signal_मानम्(vp, real_मानम्());
}

शून्य vvp_fun_signal_string_aa::get_signal_मानम्(संरचना t_vpi_मानम्*)
{
      प्रतिज्ञा(0);
}
#यदि 0
शून्य vvp_wire_string::get_signal_मानम्(संरचना t_vpi_मानम्*vp)
{
      प्रतिज्ञा(0);
}
#यदि_अन्तः
शून्य vvp_wire_vec4::get_मानम्(संरचना t_vpi_मानम्*val)
{
      get_signal_मानम्(val);
}

शून्य vvp_wire_vec8::get_मानम्(संरचना t_vpi_मानम्*val)
{
      get_signal_मानम्(val);
}

शून्य vvp_wire_real::get_मानम्(संरचना t_vpi_मानम्*val)
{
      get_signal_मानम्(val);
}
#यदि 0
शून्य vvp_wire_string::get_मानम्(संरचना t_vpi_मानम्*val)
{
      प्रतिज्ञा(0);
}
#यदि_अन्तः
