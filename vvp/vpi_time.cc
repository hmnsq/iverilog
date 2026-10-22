/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vpi_priv.h"
# include  "schedule.h"
# include  <cstdio>
# include  <cmath>
# include  <cassert>

/*
 * The $time system function is supported in VPI contexts (i.e. an
 * argument to a system task/function) as a vpiSysFuncCall object. The
 * $display function divines that this is a function call and uses a
 * vpi_get_value to get the value.
 */

/*
 * vpi_time_precision is the precision of the simulation clock. It is
 * set by the :vpi_time_precision directive in the vvp source file.
 */
స్థిర పూర్ణాంకం vpi_సమయం_precision = 0;

స్థిర నిర్మాణం __vpiSystemTime global_simtime;

శూన్య vpip_సమయం_గమ్యం_timestruct(నిర్మాణం t_vpi_సమయం*ts, vvp_time64_t ti)
{
      ts->దిగువ  = ti & 0xFFFFFFFF;
      ts->ఎగువ = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_గమ్యం_సమయం(స్థిరరాశి నిర్మాణం t_vpi_సమయం*ts)
{
      vvp_time64_t ti = ts->ఎగువ;
      ti <<= 32;
      ti += ts->దిగువ & 0xffffffff;
      తిరిగి ti;
}

ద్విగుణ vpip_సమయం_గమ్యం_scaled_real(vvp_time64_t ti, స్థిరరాశి __vpiScope*scope)
{
      ద్విగుణ val;
      పూర్ణాంకం scale = 0;
      ఒకవేళ (scope) scale = vpi_సమయం_precision - scope->సమయం_units;

      ఒకవేళ (scale >= 0) val = (ద్విగుణ)ti * ఘాతం(10.0, scale);
      లేకపోతే val = (ద్విగుణ)ti / ఘాతం(10.0, -scale);

      తిరిగి val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_గమ్యం_time64(ద్విగుణ val, స్థిరరాశి __vpiScope*scope)
{
      పూర్ణాంకం జరుపు = 0;
      ఒకవేళ (scope) జరుపు = scope->సమయం_units - scope->సమయం_precision;
      ప్రతిజ్ఞా(జరుపు >= 0);

      ప్రతిజ్ఞా(val >= 0);

	// Scale to the local precision and then round away from zero.
      ద్విగుణ scale = ఘాతం(10.0L, జరుపు);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      ఒకవేళ (scope) {
	    జరుపు = scope->సమయం_precision - vpi_సమయం_precision;
	    ప్రతిజ్ఞా(జరుపు >= 0);
	    కొరకు (పూర్ణాంకం lp = 0; lp <  జరుపు; lp += 1) delay *= 10;
      }

      తిరిగి delay;
}

స్థిర శూన్య timevar_get_విలువ(vpiHandle ref, s_vpi_విలువ*vp, తార్కిక is_int_func,
                              తార్కిక is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      స్థిర నిర్మాణం t_vpi_సమయం సమయం_విలువ;

      నిర్మాణం __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      గుర్తులేని దీర్ఘ అంకె_బిట్లు;
      vvp_time64_t x, simtime = కాలపట్టిక_simtime();
      పూర్ణాంకం units = rfp->scope? rfp->scope->సమయం_units : vpi_సమయం_precision;

      స్థిర స్థిరరాశి size_t RBUF_USE_కొలత = 128;
      అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_USE_కొలత, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t భాజకం = 1;
      అయితే (units > vpi_సమయం_precision) {
	    భాజకం *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % భాజకం;
      simtime /= భాజకం;

      ఒకవేళ ((భాజకం >= 10) && (simtime_fraction >= (భాజకం/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      ఒకవేళ (is_stime) simtime &= 0xffffffff;

      ఎంపిక (vp->format) {
	  స్థితి vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpi_సమయం_Val;
	    // fallthrough
	  స్థితి vpi_సమయం_Val:
	    vp->value.time = &సమయం_విలువ;
	    vp->value.time->type = vpi_Sim_సమయం;
	    vpip_సమయం_గమ్యం_timestruct(vp->value.time, simtime);
	    ఆపు;

	  స్థితి vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    ఒకవేళ (is_int_func)  vp->value.real = ద్విగుణ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    లేకపోతే vp->value.real = vpip_సమయం_గమ్యం_scaled_real(కాలపట్టిక_simtime(),
						           rfp->scope);
	    ఆపు;

	  స్థితి vpiBinStrVal:
	    x = simtime;
	    అంకె_బిట్లు = 8 * పరిమాణం(vvp_time64_t);

	    rbuf[అంకె_బిట్లు] = 0;
	    కొరకు (గుర్తులేని i = 1; i <= అంకె_బిట్లు; i++) {
	      rbuf[అంకె_బిట్లు-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "%" సమయం_FMT_U, simtime);
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "%" సమయం_FMT_O, simtime);
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "%" సమయం_FMT_X, simtime);
	    vp->value.str = rbuf;
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);
      }
}

స్థిర శూన్య timevar_get_ivalue(vpiHandle ref, s_vpi_విలువ*vp)
{
      timevar_get_విలువ(ref, vp, నిజం, అబద్ధం);
}

స్థిర శూన్య timevar_get_svalue(vpiHandle ref, s_vpi_విలువ*vp)
{
      timevar_get_విలువ(ref, vp, నిజం, నిజం);
}

స్థిర శూన్య timevar_get_rvalue(vpiHandle ref, s_vpi_విలువ*vp)
{
      timevar_get_విలువ(ref, vp, అబద్ధం, అబద్ధం);
}

__vpiScopedTime::__vpiScopedTime()
{ }

అక్షరం* __vpiScopedTime::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiName:
	    తిరిగి simple_సమితి_rbuf_str("$time");
	  అప్రమేయ:
	    fprintf(stderr, "Code: %d\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiScopedTime::vpi_get_విలువ(p_vpi_విలువ val)
{ timevar_get_ivalue(ఇది, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

పూర్ణాంకం __vpiScopedSTime::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpi_కొలత:
	    తిరిగి 32;

	  అప్రమేయ:
	    తిరిగి __vpiSystemTime::vpi_get(code);
      }
}


అక్షరం* __vpiScopedSTime::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiName:
	    తిరిగి simple_సమితి_rbuf_str("$stime");
	  అప్రమేయ:
	    fprintf(stderr, "Code: %d\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiScopedSTime::vpi_get_విలువ(p_vpi_విలువ val)
{ timevar_get_svalue(ఇది, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

పూర్ణాంకం __vpiSystemTime::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiSysFuncCall; }

పూర్ణాంకం __vpiSystemTime::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpi_కొలత:
	    తిరిగి 64;

          స్థితి vpiSigned:
	    తిరిగి 0;

	  స్థితి vpiFuncType:
	    తిరిగి vpi_సమయం_Func;

          స్థితి vpiAutomatic:
	    తిరిగి 0;

	  అప్రమేయ:
	    fprintf(stderr, "Code: %d\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


అక్షరం* __vpiSystemTime::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiName:
	    తిరిగి simple_సమితి_rbuf_str("$simtime");
	  అప్రమేయ:
	    fprintf(stderr, "Code: %d\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiSystemTime::vpi_get_విలువ(p_vpi_విలువ val)
{ timevar_get_ivalue(ఇది, val); }

vpiHandle __vpiSystemTime::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiScope:
	    తిరిగి scope;
	  అప్రమేయ:
	    తిరిగి 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

పూర్ణాంకం __vpiScopedRealtime::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpi_కొలత:
	    తిరిగి 1;

          స్థితి vpiSigned:
	    తిరిగి 0;

	  స్థితి vpiFuncType:
	    తిరిగి vpiRealFunc;

          స్థితి vpiAutomatic:
	    తిరిగి 0;

	  అప్రమేయ:
	    fprintf(stderr, "Code: %d\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


అక్షరం* __vpiScopedRealtime::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiName:
	    తిరిగి simple_సమితి_rbuf_str("$realtime");
	  అప్రమేయ:
	    fprintf(stderr, "Code: %d\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiScopedRealtime::vpi_get_విలువ(p_vpi_విలువ val)
{ timevar_get_rvalue(ఇది, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_సమయం(__vpiScope*scope, తార్కిక is_stime)
{
      ఒకవేళ (scope) {
	    ఒకవేళ (is_stime) {
		  scope->scoped_stime.scope = scope;
		  తిరిగి &scope->scoped_stime;
	    } లేకపోతే {
		  scope->scoped_సమయం.scope = scope;
		  తిరిగి &scope->scoped_సమయం;
	    }
      } లేకపోతే {
	    తిరిగి &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      తిరిగి &scope->scoped_realtime;
}

పూర్ణాంకం vpip_get_సమయం_precision(శూన్య)
{
      తిరిగి vpi_సమయం_precision;
}

శూన్య vpip_సమితి_సమయం_precision(పూర్ణాంకం pre)
{
      vpi_సమయం_precision = pre;
}
