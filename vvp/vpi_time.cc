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
ساكن صحيح vpi_time_precision = 0;

ساكن بنية __vpiSystemTime global_simtime;

فراغ vpip_time_to_timestruct(بنية t_vpi_time*ts, vvp_time64_t ti)
{
      ts->low  = ti & 0xFFFFFFFF;
      ts->high = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ثابت بنية t_vpi_time*ts)
{
      vvp_time64_t ti = ts->high;
      ti <<= 32;
      ti += ts->low & 0xffffffff;
      أرجع ti;
}

مزدوج vpip_time_to_scaled_real(vvp_time64_t ti, ثابت __vpiScope*scope)
{
      مزدوج val;
      صحيح scale = 0;
      𐤀𐤌 (scope) scale = vpi_time_precision - scope->time_units;

      𐤀𐤌 (scale >= 0) val = (مزدوج)ti * pow(10.0, scale);
      وإلا val = (مزدوج)ti / pow(10.0, -scale);

      أرجع val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(مزدوج val, ثابت __vpiScope*scope)
{
      صحيح shift = 0;
      𐤀𐤌 (scope) shift = scope->time_units - scope->time_precision;
      أكد(shift >= 0);

      أكد(val >= 0);

	// Scale to the local precision and then round away from zero.
      مزدوج scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      𐤀𐤌 (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    أكد(shift >= 0);
	    لكل (صحيح lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      أرجع delay;
}

ساكن فراغ timevar_get_value(vpiHandle ref, s_vpi_value*vp, منطقي is_int_func,
                              منطقي is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ساكن بنية t_vpi_time time_value;

      بنية __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long num_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      صحيح units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      ساكن ثابت size_t RBUF_USE_SIZE = 128;
      حرف*rbuf = static_cast<حرف *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t divisor = 1;
      طالما (units > vpi_time_precision) {
	    divisor *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % divisor;
      simtime /= divisor;

      𐤀𐤌 ((divisor >= 10) && (simtime_fraction >= (divisor/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      𐤀𐤌 (is_stime) simtime &= 0xffffffff;

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  حالة vpiTimeVal:
	    vp->value.time = &time_value;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    اكسر;

	  حالة vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    𐤀𐤌 (is_int_func)  vp->value.real = مزدوج (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    وإلا vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    اكسر;

	  حالة vpiBinStrVal:
	    x = simtime;
	    num_bits = 8 * حجم(vvp_time64_t);

	    rbuf[num_bits] = 0;
	    لكل (unsigned i = 1; i <= num_bits; i++) {
	      rbuf[num_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (صحيح)vp->format);
	    أكد(0);
      }
}

ساكن فراغ timevar_get_ivalue(vpiHandle ref, s_vpi_value*vp)
{
      timevar_get_value(ref, vp, true, خطأ);
}

ساكن فراغ timevar_get_svalue(vpiHandle ref, s_vpi_value*vp)
{
      timevar_get_value(ref, vp, true, true);
}

ساكن فراغ timevar_get_rvalue(vpiHandle ref, s_vpi_value*vp)
{
      timevar_get_value(ref, vp, خطأ, خطأ);
}

__vpiScopedTime::__vpiScopedTime()
{ }

حرف* __vpiScopedTime::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع simple_set_rbuf_str("$time");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiScopedTime::vpi_get_value(p_vpi_value val)
{ timevar_get_ivalue(هذا, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

صحيح __vpiScopedSTime::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpiSize:
	    أرجع 32;

	  افتراضي:
	    أرجع __vpiSystemTime::vpi_get(code);
      }
}


حرف* __vpiScopedSTime::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع simple_set_rbuf_str("$stime");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiScopedSTime::vpi_get_value(p_vpi_value val)
{ timevar_get_svalue(هذا, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

صحيح __vpiSystemTime::get_type_code(فراغ) ثابت
{ أرجع vpiSysFuncCall; }

صحيح __vpiSystemTime::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpiSize:
	    أرجع 64;

          حالة vpiSigned:
	    أرجع 0;

	  حالة vpiFuncType:
	    أرجع vpiTimeFunc;

          حالة vpiAutomatic:
	    أرجع 0;

	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


حرف* __vpiSystemTime::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع simple_set_rbuf_str("$simtime");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiSystemTime::vpi_get_value(p_vpi_value val)
{ timevar_get_ivalue(هذا, val); }

vpiHandle __vpiSystemTime::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiScope:
	    أرجع scope;
	  افتراضي:
	    أرجع 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

صحيح __vpiScopedRealtime::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpiSize:
	    أرجع 1;

          حالة vpiSigned:
	    أرجع 0;

	  حالة vpiFuncType:
	    أرجع vpiRealFunc;

          حالة vpiAutomatic:
	    أرجع 0;

	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


حرف* __vpiScopedRealtime::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع simple_set_rbuf_str("$realtime");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiScopedRealtime::vpi_get_value(p_vpi_value val)
{ timevar_get_rvalue(هذا, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, منطقي is_stime)
{
      𐤀𐤌 (scope) {
	    𐤀𐤌 (is_stime) {
		  scope->scoped_stime.scope = scope;
		  أرجع &scope->scoped_stime;
	    } وإلا {
		  scope->scoped_time.scope = scope;
		  أرجع &scope->scoped_time;
	    }
      } وإلا {
	    أرجع &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      أرجع &scope->scoped_realtime;
}

صحيح vpip_get_time_precision(فراغ)
{
      أرجع vpi_time_precision;
}

فراغ vpip_set_time_precision(صحيح pre)
{
      vpi_time_precision = pre;
}
