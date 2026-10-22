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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
#అన్తర్భావ  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

ఉపయోగించి నామాకాశ std;

స్థిర స్థిరరాశి అక్షరం*cb_reason_name(PLI_INT32 reason)
{
      ఎంపిక (reason) {
	  స్థితి cb_విలువ_Change:
	    తిరిగి "cbValueChange";
	  స్థితి cbStmt:
	    తిరిగి "cbStmt";
	  స్థితి cbForce:
	    తిరిగి "cbForce";
	  స్థితి cbRelease:
	    తిరిగి "cbRelease";
	  స్థితి cb_At_ప్రారంభం_యొక్క_Sim_సమయం:
	    తిరిగి "cbAtStartOfSimTime";
	  స్థితి cbReadWriteSynch:
	    తిరిగి "cbReadWriteSynch";
	  స్థితి cbReadOnlySynch:
	    తిరిగి "cbReadOnlySynch";
	  స్థితి cb_తదుపరి_Sim_సమయం:
	    తిరిగి "cbNextSimTime";
	  స్థితి cbAfterDelay:
	    తిరిగి "cbAfterDelay";
	  స్థితి cb_చివర_యొక్క_Compile:
	    తిరిగి "cbEndOfCompile";
	  స్థితి cb_ప్రారంభం_యొక్క_Simulation:
	    తిరిగి "cbStartOfSimulation";
	  స్థితి cb_చివర_యొక్క_Simulation:
	    తిరిగి "cbEndOfSimulation";
	  స్థితి cbError:
	    తిరిగి "cbError";
	  స్థితి cbTchkViolation:
	    తిరిగి "cbTchkViolation";
	  స్థితి cb_ప్రారంభం_యొక్క_Save:
	    తిరిగి "cbStartOfSave";
	  స్థితి cb_చివర_యొక్క_Save:
	    తిరిగి "cbEndOfSave";
	  స్థితి cb_ప్రారంభం_యొక్క_Restart:
	    తిరిగి "cbStartOfRestart";
	  స్థితి cb_చివర_యొక్క_Restart:
	    తిరిగి "cbEndOfRestart";
	  స్థితి cb_ప్రారంభం_యొక్క_Reset:
	    తిరిగి "cbStartOfReset";
	  స్థితి cb_చివర_యొక్క_Reset:
	    తిరిగి "cbEndOfReset";
	  స్థితి cbEnterInteractive:
	    తిరిగి "cbEnterInteractive";
	  స్థితి cbExitInteractive:
	    తిరిగి "cbExitInteractive";
	  స్థితి cbInteractiveScopeChange:
	    తిరిగి "cbInteractiveScopeChange";
	  స్థితి cbUnresolvedSystf:
	    తిరిగి "cbUnresolvedSystf";
	  స్థితి cb_At_చివర_యొక్క_Sim_సమయం:
	    తిరిగి "cbAtEndOfSimTime";
	  అప్రమేయ:
	    తిరిగి "unrecognised";
      }
}

స్థిర తార్కిక check_callback_సమయం(p_cb_దత్తాంశం దత్తాంశం, తార్కిక allow_suppress)
{
      ప్రతిజ్ఞా(దత్తాంశం);
      ఒకవేళ (!దత్తాంశం->time) {
	    ఒకవేళ (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(దత్తాంశం->reason));
		  తిరిగి అబద్ధం;
	    }
	    తిరిగి నిజం;
      }
      ఎంపిక (దత్తాంశం->time->type) {
	  స్థితి vpi_Sim_సమయం:
	    ఆపు;
	  స్థితి vpi_Scaled_Real_సమయం:
	    ఆపు;
	  స్థితి vpi_Suppress_సమయం:
	    ఒకవేళ (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(దత్తాంశం->reason));
		  తిరిగి అబద్ధం;
	    }
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(దత్తాంశం->reason));
	    తిరిగి అబద్ధం;
      }
      తిరిగి నిజం;
}

స్థిర శూన్య సమితి_callback_సమయం(p_cb_దత్తాంశం దత్తాంశం)
{
      ప్రతిజ్ఞా(దత్తాంశం && దత్తాంశం->time);
      దత్తాంశం->time->దిగువ  = 0;
      దత్తాంశం->time->ఎగువ = 0;
      దత్తాంశం->time->real = 0.0;
      ఎంపిక (దత్తాంశం->time->type) {
	  స్థితి vpi_Sim_సమయం:
	    vpip_సమయం_గమ్యం_timestruct(దత్తాంశం->time, కాలపట్టిక_simtime());
	    ఆపు;
	  స్థితి vpi_Scaled_Real_సమయం:
	    దత్తాంశం->time->real = vpip_scaled_సమయం_from_handle(కాలపట్టిక_simtime(), దత్తాంశం->obj);
	    ఆపు;
	  స్థితి vpi_Suppress_సమయం:
	    ఆపు;
	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
	    ఆపు;
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

తరగతి sync_callback;

నిర్మాణం sync_cb  : బహిరంగ vvp_gen_event_s {
      sync_callback*handle;
      తార్కిక sync_flag;

      ~sync_cb () override { }

      మాయా శూన్య నిరంతరం_నిరంతరం() override;
};

అంతఃవరుస __vpiCallback::__vpiCallback()
{
      తదుపరి_2 = 0;
}

__vpiCallback::~__vpiCallback()
{
}

పూర్ణాంకం __vpiCallback::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiCallback; }


విలువ_callback::విలువ_callback(p_cb_దత్తాంశం దత్తాంశం)
{
      cb_దత్తాంశం = *దత్తాంశం;
      ఒకవేళ (దత్తాంశం->time) {
	    cb_సమయం = *(దత్తాంశం->time);
      } లేకపోతే {
	    cb_సమయం.type = vpi_Suppress_సమయం;
      }
      cb_దత్తాంశం.time = &cb_సమయం;
      ఒకవేళ (దత్తాంశం->value) {
	    cb_విలువ = *(దత్తాంశం->value);
      } లేకపోతే {
	    cb_విలువ.format = vpiSuppressVal;
      }
      cb_దత్తాంశం.value = &cb_విలువ;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
తార్కిక విలువ_callback::test_విలువ_callback_ready(శూన్య)
{
      తిరిగి నిజం;
}

స్థిర శూన్య vpip_real_విలువ_change(విలువ_callback*cbh, vpiHandle ref)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      ప్రతిజ్ఞా(obj);

      obj->కూడు_vpi_callback(cbh);
}

తరగతి విలువ_part_callback : బహిరంగ విలువ_callback {
    బహిరంగ:
      స్పష్టమైన విలువ_part_callback(p_cb_దత్తాంశం దత్తాంశం);
      ~విలువ_part_callback() override;

      విలువ_part_callback(స్థిరరాశి విలువ_part_callback&) = తొలగించు;
      విలువ_part_callback& క్రియాకారకం=(స్థిరరాశి విలువ_part_callback&) = తొలగించు;

      తార్కిక test_విలువ_callback_ready(శూన్య) override;

    అంతరంగ:
      అక్షరం*విలువ_బిట్లు;
      size_t విలువ_off;
};

అంతఃవరుస విలువ_part_callback::విలువ_part_callback(p_cb_దత్తాంశం దత్తాంశం)
: విలువ_callback(దత్తాంశం)
{
      నిర్మాణం __vpiPV*pobj = dynamic_cast<__vpiPV*>(దత్తాంశం->obj);
      ప్రతిజ్ఞా(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ప్రతిజ్ఞా(sig_fil);

      sig_fil->కూడు_vpi_callback(ఇది);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_విలువ tmp_విలువ;
      tmp_విలువ.format = vpiBinStrVal;
      sig_fil->get_విలువ(&tmp_విలువ);

      విలువ_బిట్లు = కొత్త అక్షరం[pobj->width+1];
      విలువ_off = pobj->మాతృ->vpi_get(vpi_కొలత) - pobj->width - pobj->tbase;

      జ్ఞాపకం_నకలు(విలువ_బిట్లు, tmp_విలువ.value.str + విలువ_off, pobj->width);
      విలువ_బిట్లు[pobj->width] = 0;
}

విలువ_part_callback::~విలువ_part_callback()
{
      తొలగించు[]విలువ_బిట్లు;
}

తార్కిక విలువ_part_callback::test_విలువ_callback_ready(శూన్య)
{
      నిర్మాణం __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_దత్తాంశం.obj);
      ప్రతిజ్ఞా(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      ప్రతిజ్ఞా(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_విలువ tmp_విలువ;
      tmp_విలువ.format = vpiBinStrVal;
      sig_fil->get_విలువ(&tmp_విలువ);

      ఒకవేళ (memcmp(విలువ_బిట్లు, tmp_విలువ.value.str + విలువ_off, pobj->width) == 0)
	    తిరిగి అబద్ధం;

      జ్ఞాపకం_నకలు(విలువ_బిట్లు, tmp_విలువ.value.str + విలువ_off, pobj->width);
      తిరిగి నిజం;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
స్థిర విలువ_callback*make_విలువ_change_part(p_cb_దత్తాంశం దత్తాంశం)
{
	/* Attach the __vpiCallback object to the signal. */
      విలువ_callback*cbh = కొత్త విలువ_part_callback(దత్తాంశం);
      తిరిగి cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
స్థిర విలువ_callback* make_విలువ_change(p_cb_దత్తాంశం దత్తాంశం)
{
      ఒకవేళ (!check_callback_సమయం(దత్తాంశం, నిజం))
	    తిరిగి 0;

      ఒకవేళ (vpi_get(vpiAutomatic, దత్తాంశం->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, దత్తాంశం->obj));
            తిరిగి 0;
      }

	// Special case: the target object is a vpiPartSelect
      ఒకవేళ (దత్తాంశం->obj->get_type_సంకేతం() == vpiPartSelect) {
            ఒకవేళ (దత్తాంశం->obj->vpi_handle(vpiArray))
	        తిరిగి vpip_array_మాట_change(దత్తాంశం);
            లేకపోతే
	        తిరిగి make_విలువ_change_part(దత్తాంశం);
      }

      ఒకవేళ (దత్తాంశం->obj->get_type_సంకేతం() == vpi_జ్ఞాపకం_మాట)
	    తిరిగి vpip_array_మాట_change(దత్తాంశం);

      ఒకవేళ (దత్తాంశం->obj->get_type_సంకేతం() == vpi_జ్ఞాపకం)
	    తిరిగి vpip_array_change(దత్తాంశం);

      విలువ_callback*obj = కొత్త విలువ_callback(దత్తాంశం);

      ప్రతిజ్ఞా(దత్తాంశం->obj);
      ఎంపిక (దత్తాంశం->obj->get_type_సంకేతం()) {

	  స్థితి vpiReg:
	  స్థితి vpiNet:
	  స్థితి vpiIntegerVar:
	  స్థితి vpi_బిట్టు_Var:
	  స్థితి vpiByteVar:
	  స్థితి vpiShortIntVar:
	  స్థితి vpiIntVar:
	  స్థితి vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    నిర్మాణం __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(దత్తాంశం->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->కణుపు->fil);
	    ప్రతిజ్ఞా(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->కూడు_vpi_callback(obj);
	    ఆపు;

	  స్థితి vpiRealVar:
	    vpip_real_విలువ_change(obj, దత్తాంశం->obj);
	    ఆపు;

	  స్థితి vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(దత్తాంశం->obj);
	    nev->కూడు_vpi_callback(obj);
	    ఆపు;

	  స్థితి vpiModule:
	  స్థితి vpiConstant:
	  స్థితి vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    దత్తాంశం->obj->get_type_సంకేతం());
	    తొలగించు obj;
	    తిరిగి 0;
      }

      తిరిగి obj;
}

తరగతి sync_callback : బహిరంగ __vpiCallback {
    బహిరంగ:
      స్పష్టమైన sync_callback(p_cb_దత్తాంశం దత్తాంశం);
      ~sync_callback() override;

      sync_callback(స్థిరరాశి sync_callback&) = తొలగించు;
      sync_callback& క్రియాకారకం=(స్థిరరాశి sync_callback&) = తొలగించు;

    బహిరంగ:
	// scheduled event
      నిర్మాణం sync_cb* cb_sync;
	// user supplied callback data
      నిర్మాణం t_vpi_సమయం cb_సమయం;

    అంతరంగ:
};

అంతఃవరుస sync_callback::sync_callback(p_cb_దత్తాంశం దత్తాంశం)
{
      cb_sync = 0;

      cb_దత్తాంశం = *దత్తాంశం;
      ప్రతిజ్ఞా(దత్తాంశం->time);
      cb_సమయం = *(దత్తాంశం->time);
      cb_దత్తాంశం.time = &cb_సమయం;
}

sync_callback::~sync_callback()
{
      తొలగించు cb_sync;
}

శూన్య sync_cb::నిరంతరం_నిరంతరం()
{
      ఒకవేళ (handle == 0)
	    తిరిగి;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
	    సమితి_callback_సమయం(&cur->cb_దత్తాంశం);
	    ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
	    vpi_బహుళకం_flag = sync_flag? VPI_బహుళకం_ROSYNC : VPI_బహుళకం_RWSYNC;
	    (cur->cb_దత్తాంశం.cb_rtn)(&cur->cb_దత్తాంశం);
	    vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
      }

      తొలగించు cur;
}

స్థిర vvp_time64_t get_sync_cb_సమయం(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      ఎంపిక (obj->cb_సమయం.type) {
	  స్థితి vpi_Sim_సమయం:
	    tv = vpip_timestruct_గమ్యం_సమయం(&obj->cb_సమయం);
	    ఆపు;

	  స్థితి vpi_Scaled_Real_సమయం:
	    tv = vpip_scaled_real_గమ్యం_time64(obj->cb_సమయం.real,
		     vpip_timescale_scope_from_handle(obj->cb_దత్తాంశం.obj));
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (పూర్ణాంకం)obj->cb_సమయం.type);
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }
      తిరిగి tv;
}

స్థిర sync_callback* make_sync(p_cb_దత్తాంశం దత్తాంశం, తార్కిక readonly_flag)
{
      ఒకవేళ (!check_callback_సమయం(దత్తాంశం, అబద్ధం))
	    తిరిగి 0;

      sync_callback*obj = కొత్త sync_callback(దత్తాంశం);

      నిర్మాణం sync_cb*cb = కొత్త sync_cb;
      cb->sync_flag = readonly_flag? నిజం : అబద్ధం;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_సమయం(obj);
      కాలపట్టిక_generic(cb, tv, నిజం, readonly_flag);

      తిరిగి obj;
}

స్థిర నిర్మాణం __vpiCallback* make_afterdelay(p_cb_దత్తాంశం దత్తాంశం)
{
      ఒకవేళ (!check_callback_సమయం(దత్తాంశం, అబద్ధం))
	    తిరిగి 0;

      sync_callback*obj = కొత్త sync_callback(దత్తాంశం);
      నిర్మాణం sync_cb*cb = కొత్త sync_cb;
      cb->sync_flag = అబద్ధం;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_సమయం(obj);
      కాలపట్టిక_generic(cb, tv, అబద్ధం);

      తిరిగి obj;
}

స్థిర నిర్మాణం __vpiCallback* make_at_ప్రారంభం_యొక్క_sim_సమయం(p_cb_దత్తాంశం దత్తాంశం)
{
      ఒకవేళ (!check_callback_సమయం(దత్తాంశం, అబద్ధం))
	    తిరిగి 0;

      sync_callback*obj = కొత్త sync_callback(దత్తాంశం);
      నిర్మాణం sync_cb*cb = కొత్త sync_cb;
      cb->sync_flag = అబద్ధం;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_సమయం(obj);
      vvp_time64_t cur = కాలపట్టిక_simtime();
      ఒకవేళ (cur > tv) {
	    tv = 0;
	    ప్రతిజ్ఞా(0);
      } లేకపోతే ఒకవేళ (cur == tv) {
	    tv = 0;
      } లేకపోతే {
	    tv -= cur;
      }
      కాలపట్టిక_at_ప్రారంభం_యొక్క_simtime(cb, tv);

      తిరిగి obj;
}

స్థిర నిర్మాణం __vpiCallback* make_at_చివర_యొక్క_sim_సమయం(p_cb_దత్తాంశం దత్తాంశం)
{
      ఒకవేళ (!check_callback_సమయం(దత్తాంశం, అబద్ధం))
	    తిరిగి 0;

      sync_callback*obj = కొత్త sync_callback(దత్తాంశం);
      నిర్మాణం sync_cb*cb = కొత్త sync_cb;
      cb->sync_flag = అబద్ధం;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_సమయం(obj);
      vvp_time64_t cur = కాలపట్టిక_simtime();
      ఒకవేళ (cur > tv) {
	    tv = 0;
	    ప్రతిజ్ఞా(0);
      } లేకపోతే ఒకవేళ (cur == tv) {
	    tv = 0;
      } లేకపోతే {
	    tv -= cur;
      }
      కాలపట్టిక_at_చివర_యొక్క_simtime(cb, tv);

      తిరిగి obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

తరగతి simulator_callback : బహిరంగ __vpiCallback {
    బహిరంగ:
      స్పష్టమైన simulator_callback(స్థిరరాశి నిర్మాణం t_cb_దత్తాంశం*దత్తాంశం);

    బహిరంగ:
      నిర్మాణం t_vpi_సమయం cb_సమయం;
};

అంతఃవరుస simulator_callback::simulator_callback(స్థిరరాశి నిర్మాణం t_cb_దత్తాంశం*దత్తాంశం)
{
      cb_దత్తాంశం = *దత్తాంశం;
      ఒకవేళ ((దత్తాంశం->reason == cb_తదుపరి_Sim_సమయం) && దత్తాంశం->time) {
	    cb_సమయం = *(దత్తాంశం->time);
      } లేకపోతే ఒకవేళ (దత్తాంశం->reason == cb_చివర_యొక్క_Simulation) {
	    cb_సమయం.type = vpi_Sim_సమయం;
      } లేకపోతే {
	    cb_సమయం.type = vpi_Suppress_సమయం;
      }
      cb_దత్తాంశం.time = &cb_సమయం;
}

స్థిర simulator_callback*తదుపరి_Sim_సమయం = 0;
స్థిర simulator_callback*చివర_యొక్క_Compile = 0;
స్థిర simulator_callback*ప్రారంభం_యొక్క_Simulation = 0;
స్థిర simulator_callback*చివర_యొక్క_Simulation = 0;

#నిర్వచితమైతే CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
శూన్య simulator_cb_delete(శూన్య)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      అయితే (చివర_యొక్క_Compile) {
	    cur = చివర_యొక్క_Compile;
	    చివర_యొక్క_Compile = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
	    తొలగించు cur;
      }

	/* Delete all the start of simulation callbacks. */
      అయితే (ప్రారంభం_యొక్క_Simulation) {
	    cur = ప్రారంభం_యొక్క_Simulation;
	    ప్రారంభం_యొక్క_Simulation = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
	    తొలగించు cur;
      }

	/* Delete all the end of simulation callbacks. */
      అయితే (చివర_యొక్క_Simulation) {
	    cur = చివర_యొక్క_Simulation;
	    చివర_యొక్క_Simulation = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
	    తొలగించు cur;
      }
}
#అయితే_ముగింపు

శూన్య vpi_చివర_యొక్క_Compile(శూన్య) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
      vpi_బహుళకం_flag = VPI_బహుళకం_RWSYNC;

      అయితే (చివర_యొక్క_Compile) {
	    cur = చివర_యొక్క_Compile;
	    చివర_యొక్క_Compile = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
	    ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
		  (cur->cb_దత్తాంశం.cb_rtn)(&cur->cb_దత్తాంశం);
	    }
	    తొలగించు cur;
      }

      vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
}

శూన్య vpi_ప్రారంభం_యొక్క_Sim(శూన్య) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
      vpi_బహుళకం_flag = VPI_బహుళకం_RWSYNC;

      అయితే (ప్రారంభం_యొక్క_Simulation) {
	    cur = ప్రారంభం_యొక్క_Simulation;
	    ప్రారంభం_యొక్క_Simulation = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
	    ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
		  (cur->cb_దత్తాంశం.cb_rtn)(&cur->cb_దత్తాంశం);
	    }
	    తొలగించు cur;
      }

      vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
}

శూన్య vpiPostsim(శూన్య) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
      vpi_బహుళకం_flag = VPI_బహుళకం_ROSYNC;

      అయితే (చివర_యొక్క_Simulation) {
	    cur = చివర_యొక్క_Simulation;
	    చివర_యొక్క_Simulation = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
	    ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
		  సమితి_callback_సమయం(&cur->cb_దత్తాంశం);
		  (cur->cb_దత్తాంశం.cb_rtn)(&cur->cb_దత్తాంశం);
	    }
	    తొలగించు cur;
      }

      vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
శూన్య vpi_తదుపరి_Sim_సమయం(శూన్య)
{
      simulator_callback* cur;
      simulator_callback* తదుపరి_2 = తదుపరి_Sim_సమయం;
      తదుపరి_Sim_సమయం = 0;

      ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
      vpi_బహుళకం_flag = VPI_బహుళకం_RWSYNC;

      అయితే (తదుపరి_2) {
            cur = తదుపరి_2;
            తదుపరి_2 = dynamic_cast<simulator_callback*>(cur->తదుపరి_2);
            ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
                  సమితి_callback_సమయం(&cur->cb_దత్తాంశం);
                  (cur->cb_దత్తాంశం.cb_rtn)(&cur->cb_దత్తాంశం);
            }
            తొలగించు cur;
      }

      vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
}

స్థిర simulator_callback* make_prepost(p_cb_దత్తాంశం దత్తాంశం)
{
      ఒకవేళ ((దత్తాంశం->reason == cb_తదుపరి_Sim_సమయం) && !check_callback_సమయం(దత్తాంశం, నిజం))
	    తిరిగి 0;

      simulator_callback*obj = కొత్త simulator_callback(దత్తాంశం);

      /* Insert at head of list */
      ఎంపిక (దత్తాంశం->reason) {
	  స్థితి cb_చివర_యొక్క_Compile:
	    obj->తదుపరి_2 = చివర_యొక్క_Compile;
	    చివర_యొక్క_Compile = obj;
	    ఆపు;
	  స్థితి cb_ప్రారంభం_యొక్క_Simulation:
	    obj->తదుపరి_2 = ప్రారంభం_యొక్క_Simulation;
	    ప్రారంభం_యొక్క_Simulation = obj;
	    ఆపు;
	  స్థితి cb_చివర_యొక్క_Simulation:
	    obj->తదుపరి_2 = చివర_యొక్క_Simulation;
	    చివర_యొక్క_Simulation = obj;
	    ఆపు;
	  స్థితి cb_తదుపరి_Sim_సమయం:
	    obj->తదుపరి_2 = తదుపరి_Sim_సమయం;
	    తదుపరి_Sim_సమయం = obj;
	    ఆపు;
      }

      తిరిగి obj;
}

vpiHandle vpi_రిజిస్టరు_cb(p_cb_దత్తాంశం దత్తాంశం)
{
      నిర్మాణం __vpiCallback*obj = 0;

      ప్రతిజ్ఞా(దత్తాంశం);
      ఎంపిక (దత్తాంశం->reason) {

	  స్థితి cb_విలువ_Change:
	    obj = make_విలువ_change(దత్తాంశం);
	    ఆపు;

	  స్థితి cbReadOnlySynch:
	    obj = make_sync(దత్తాంశం, నిజం);
	    ఆపు;

	  స్థితి cbReadWriteSynch:
	    obj = make_sync(దత్తాంశం, అబద్ధం);
	    ఆపు;

	  స్థితి cb_At_ప్రారంభం_యొక్క_Sim_సమయం:
	    obj = make_at_ప్రారంభం_యొక్క_sim_సమయం(దత్తాంశం);
	    ఆపు;

	  స్థితి cb_At_చివర_యొక్క_Sim_సమయం:
	    obj = make_at_చివర_యొక్క_sim_సమయం(దత్తాంశం);
	    ఆపు;

	  స్థితి cbAfterDelay:
	    obj = make_afterdelay(దత్తాంశం);
	    ఆపు;

	  స్థితి cb_చివర_యొక్క_Compile:
	  స్థితి cb_ప్రారంభం_యొక్క_Simulation:
	  స్థితి cb_చివర_యొక్క_Simulation:
	  స్థితి cb_తదుపరి_Sim_సమయం:
	    obj = make_prepost(దత్తాంశం);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (పూర్ణాంకం)దత్తాంశం->reason);
	    ఆపు;
      }

      తిరిగి obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      నిర్మాణం __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      ప్రతిజ్ఞా(obj);
      obj->cb_దత్తాంశం.cb_rtn = 0;

      తిరిగి 1;
}

శూన్య callback_execute(నిర్మాణం __vpiCallback*cur)
{
      స్థిరరాశి vpi_బహుళకం_t save_బహుళకం = vpi_బహుళకం_flag;
      vpi_బహుళకం_flag = VPI_బహుళకం_RWSYNC;

      ప్రతిజ్ఞా(cur->cb_దత్తాంశం.cb_rtn);
      సమితి_callback_సమయం(&cur->cb_దత్తాంశం);
      (cur->cb_దత్తాంశం.cb_rtn)(&cur->cb_దత్తాంశం);

      vpi_బహుళకం_flag = save_బహుళకం;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
నిర్మాణం __vpi_array_word {
      నిర్మాణం __vpi_array_word* తదుపరి_2;
      నిర్మాణం __vpiArray* array;
      గుర్తులేని దీర్ఘ మాట;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_పదాలు = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      ప్రతిజ్ఞా(vpi_callbacks_ == 0);
      ప్రతిజ్ఞా(array_పదాలు == 0);
}

శూన్య vvp_vpi_callback::attach_as_మాట(vvp_array_t arr, గుర్తులేని దీర్ఘ addr)
{
      నిర్మాణం __vpi_array_word*tmp = కొత్త __vpi_array_word;
      tmp->array = arr;
      tmp->మాట = addr;
      tmp->తదుపరి_2 = array_పదాలు;
      array_పదాలు = tmp;
}

శూన్య vvp_vpi_callback::కూడు_vpi_callback(విలువ_callback*cb)
{
      cb->తదుపరి_2 = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_vpi_callback::clear_all_callbacks()
{
      అయితే (vpi_callbacks_) {
	    విలువ_callback *tmp = dynamic_cast<విలువ_callback*>
	                            (vpi_callbacks_->తదుపరి_2);
	    తొలగించు vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      అయితే (array_పదాలు) {
	    నిర్మాణం __vpi_array_word*tmp = array_పదాలు->తదుపరి_2;
	    తొలగించు array_పదాలు;
	    array_పదాలు = tmp;
      }
}
#అయితే_ముగింపు

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
శూన్య vvp_vpi_callback::నిరంతరం_vpi_callbacks()
{
      నిర్మాణం __vpi_array_word*array_మాట = array_పదాలు;
      అయితే (array_మాట) {
	    array_మాట->array->మాట_change(array_మాట->మాట);
	    array_మాట = array_మాట->తదుపరి_2;
      }

      విలువ_callback *తదుపరి_2 = vpi_callbacks_;
      విలువ_callback *prev = 0;

      అయితే (తదుపరి_2) {
	    విలువ_callback*cur = తదుపరి_2;
	    తదుపరి_2 = dynamic_cast<విలువ_callback*>(cur->తదుపరి_2);

	    ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
		  ఒకవేళ (cur->test_విలువ_callback_ready()) {
			ఒకవేళ (cur->cb_దత్తాంశం.value)
			      get_విలువ(cur->cb_దత్తాంశం.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } లేకపోతే ఒకవేళ (prev == 0) {

		  vpi_callbacks_ = తదుపరి_2;
		  cur->తదుపరి_2 = 0;
		  తొలగించు cur;

	    } లేకపోతే {
		  ప్రతిజ్ఞా(prev->తదుపరి_2 == cur);
		  prev->తదుపరి_2 = తదుపరి_2;
		  cur->తదుపరి_2 = 0;
		  తొలగించు cur;
	    }
      }
}

శూన్య vvp_signal_విలువ::get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp)
{
      ఎంపిక (vp->format) {
	  స్థితి vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    ఆపు;

	  స్థితి vpiBinStrVal:
	  స్థితి vpiOctStrVal:
	  స్థితి vpiDecStrVal:
	  స్థితి vpiHexStrVal:
	  స్థితి vpiIntVal:
	  స్థితి vpi_సదిశ_Val:
	  స్థితి vpiStringVal:
	  స్థితి vpiRealVal: {
	    గుర్తులేని wid = విలువ_కొలత();
	    vvp_vector4_t vec4(wid);
	    కొరకు (గుర్తులేని idx = 0; idx < wid; idx += 1) {
		  vec4.సమితి_బిట్టు(idx, value(idx));
	    }
	    vpip_vec4_get_విలువ(vec4, wid, అబద్ధం, vp);
	    ఆపు;
	  }

	  స్థితి vpiSuppressVal:
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (పూర్ణాంకం)vp->format);
      }
}

స్థిర శూన్య real_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp, ద్విగుణ rval)
{
      స్థిర స్థిరరాశి size_t RBUF_కొలత = 64 + 1;
      అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_కొలత, RBUF_VAL));

      ఎంపిక (vp->format) {
	  స్థితి vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  స్థితి vpiRealVal:
	    vp->value.real = rval;
	    ఆపు;

	  స్థితి vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ఒకవేళ (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } లేకపోతే {
		  rval = std::విడత_2(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    ఒకవేళ (std::isnan(rval))
		  snprintf(rbuf, RBUF_కొలత, "%s", "nan");
	    లేకపోతే
		  snprintf(rbuf, RBUF_కొలత, "%0.0f", std::విడత_2(rval));
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiHexStrVal:
	    snprintf(rbuf, RBUF_కొలత, "%" PRIx64, vlg_విడత_గమ్యం_u64(rval));
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiBinStrVal: {
		uint64_t val = vlg_విడత_గమ్యం_u64(rval);
		గుర్తులేని len = 0;

		అయితే (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_విడత_గమ్యం_u64(rval);
		కొరకు (గుర్తులేని idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		ఒకవేళ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ఆపు;
	  }

	  స్థితి vpiSuppressVal:
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (పూర్ణాంకం)vp->format);
      }
}

శూన్య vvp_fun_signal_real_aa::get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp)
{
      real_signal_విలువ(vp, real_విలువ());
}

శూన్య vvp_wire_real::get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp)
{
      real_signal_విలువ(vp, real_విలువ());
}

శూన్య vvp_fun_signal_string_aa::get_signal_విలువ(నిర్మాణం t_vpi_విలువ*)
{
      ప్రతిజ్ఞా(0);
}
#ఒకవేళ 0
శూన్య vvp_wire_string::get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp)
{
      ప్రతిజ్ఞా(0);
}
#అయితే_ముగింపు
శూన్య vvp_wire_vec4::get_విలువ(నిర్మాణం t_vpi_విలువ*val)
{
      get_signal_విలువ(val);
}

శూన్య vvp_wire_vec8::get_విలువ(నిర్మాణం t_vpi_విలువ*val)
{
      get_signal_విలువ(val);
}

శూన్య vvp_wire_real::get_విలువ(నిర్మాణం t_vpi_విలువ*val)
{
      get_signal_విలువ(val);
}
#ఒకవేళ 0
శూన్య vvp_wire_string::get_విలువ(నిర్మాణం t_vpi_విలువ*val)
{
      ప్రతిజ్ఞా(0);
}
#అయితే_ముగింపు
