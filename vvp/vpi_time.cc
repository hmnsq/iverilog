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
ساكن صحيح vpi_ویلا_precision = 0;

ساكن بنية __vpiSystemTime global_simtime;

فراغ vpip_ویلا_منزل_timestruct(بنية t_vpi_ویلا*ts, vvp_time64_t ti)
{
      ts->کم  = ti & 0xFFFFFFFF;
      ts->بلند = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_منزل_ویلا(ثابت بنية t_vpi_ویلا*ts)
{
      vvp_time64_t ti = ts->بلند;
      ti <<= 32;
      ti += ts->کم & 0xffffffff;
      أرجع ti;
}

مزدوج vpip_ویلا_منزل_scaled_real(vvp_time64_t ti, ثابت __vpiScope*scope)
{
      مزدوج val;
      صحيح scale = 0;
      إذا (scope) scale = vpi_ویلا_precision - scope->ویلا_units;

      إذا (scale >= 0) val = (مزدوج)ti * قوت(10.0, scale);
      وإلا val = (مزدوج)ti / قوت(10.0, -scale);

      أرجع val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_منزل_time64(مزدوج val, ثابت __vpiScope*scope)
{
      صحيح کھسکاؤ = 0;
      إذا (scope) کھسکاؤ = scope->ویلا_units - scope->ویلا_precision;
      أكد(کھسکاؤ >= 0);

      أكد(val >= 0);

	// Scale to the local precision and then round away from zero.
      مزدوج scale = قوت(10.0L, کھسکاؤ);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      إذا (scope) {
	    کھسکاؤ = scope->ویلا_precision - vpi_ویلا_precision;
	    أكد(کھسکاؤ >= 0);
	    لكل (صحيح lp = 0; lp <  کھسکاؤ; lp += 1) delay *= 10;
      }

      أرجع delay;
}

ساكن فراغ timevar_get_قدر(vpiHandle ref, s_vpi_قدر*vp, منطقي is_int_func,
                              منطقي is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ساكن بنية t_vpi_ویلا ویلا_قدر;

      بنية __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      بنانشانی لما عدد_بٹ;
      vvp_time64_t x, simtime = ویلا_سارنی_simtime();
      صحيح units = rfp->scope? rfp->scope->ویلا_units : vpi_ویلا_precision;

      ساكن ثابت size_t RBUF_USE_ناپ = 128;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_ناپ, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t مقسوم_علیہ = 1;
      طالما (units > vpi_ویلا_precision) {
	    مقسوم_علیہ *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % مقسوم_علیہ;
      simtime /= مقسوم_علیہ;

      إذا ((مقسوم_علیہ >= 10) && (simtime_fraction >= (مقسوم_علیہ/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      إذا (is_stime) simtime &= 0xffffffff;

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpi_ویلا_Val;
	    // fallthrough
	  حالة vpi_ویلا_Val:
	    vp->value.time = &ویلا_قدر;
	    vp->value.time->type = vpi_Sim_ویلا;
	    vpip_ویلا_منزل_timestruct(vp->value.time, simtime);
	    اكسر;

	  حالة vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    إذا (is_int_func)  vp->value.real = مزدوج (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    وإلا vp->value.real = vpip_ویلا_منزل_scaled_real(ویلا_سارنی_simtime(),
						           rfp->scope);
	    اكسر;

	  حالة vpiBinStrVal:
	    x = simtime;
	    عدد_بٹ = 8 * حجم(vvp_time64_t);

	    rbuf[عدد_بٹ] = 0;
	    لكل (بنانشانی i = 1; i <= عدد_بٹ; i++) {
	      rbuf[عدد_بٹ-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_ناپ, "%" ویلا_FMT_U, simtime);
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_ناپ, "%" ویلا_FMT_O, simtime);
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_ناپ, "%" ویلا_FMT_X, simtime);
	    vp->value.str = rbuf;
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (صحيح)vp->format);
	    أكد(0);
      }
}

ساكن فراغ timevar_get_ivalue(vpiHandle ref, s_vpi_قدر*vp)
{
      timevar_get_قدر(ref, vp, سچ, خطأ);
}

ساكن فراغ timevar_get_svalue(vpiHandle ref, s_vpi_قدر*vp)
{
      timevar_get_قدر(ref, vp, سچ, سچ);
}

ساكن فراغ timevar_get_rvalue(vpiHandle ref, s_vpi_قدر*vp)
{
      timevar_get_قدر(ref, vp, خطأ, خطأ);
}

__vpiScopedTime::__vpiScopedTime()
{ }

حرف* __vpiScopedTime::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع simple_سیٹ_rbuf_str("$time");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiScopedTime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_ivalue(هذا, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

صحيح __vpiScopedSTime::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpi_ناپ:
	    أرجع 32;

	  افتراضي:
	    أرجع __vpiSystemTime::vpi_get(code);
      }
}


حرف* __vpiScopedSTime::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع simple_سیٹ_rbuf_str("$stime");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiScopedSTime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_svalue(هذا, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

صحيح __vpiSystemTime::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiSysFuncCall; }

صحيح __vpiSystemTime::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpi_ناپ:
	    أرجع 64;

          حالة vpiSigned:
	    أرجع 0;

	  حالة vpiFuncType:
	    أرجع vpi_ویلا_Func;

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
	    أرجع simple_سیٹ_rbuf_str("$simtime");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiSystemTime::vpi_get_قدر(p_vpi_قدر val)
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
          حالة vpi_ناپ:
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
	    أرجع simple_سیٹ_rbuf_str("$realtime");
	  افتراضي:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiScopedRealtime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_rvalue(هذا, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_ویلا(__vpiScope*scope, منطقي is_stime)
{
      إذا (scope) {
	    إذا (is_stime) {
		  scope->scoped_stime.scope = scope;
		  أرجع &scope->scoped_stime;
	    } وإلا {
		  scope->scoped_ویلا.scope = scope;
		  أرجع &scope->scoped_ویلا;
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

صحيح vpip_get_ویلا_precision(فراغ)
{
      أرجع vpi_ویلا_precision;
}

فراغ vpip_سیٹ_ویلا_precision(صحيح pre)
{
      vpi_ویلا_precision = pre;
}
