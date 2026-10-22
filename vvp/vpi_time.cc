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
ساکن صحیح vpi_وقت_precision = 0;

ساکن ساخت __vpiSystemTime global_simtime;

خالی vpip_وقت_مقصد_timestruct(ساخت t_vpi_وقت*ts, vvp_time64_t ti)
{
      ts->کم  = ti & 0xFFFFFFFF;
      ts->بلند = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_مقصد_وقت(ثابت ساخت t_vpi_وقت*ts)
{
      vvp_time64_t ti = ts->بلند;
      ti <<= 32;
      ti += ts->کم & 0xffffffff;
      واپس ti;
}

دوہرا vpip_وقت_مقصد_scaled_real(vvp_time64_t ti, ثابت __vpiScope*scope)
{
      دوہرا val;
      صحیح scale = 0;
      اگر (scope) scale = vpi_وقت_precision - scope->وقت_units;

      اگر (scale >= 0) val = (دوہرا)ti * قوت(10.0, scale);
      ورنہ val = (دوہرا)ti / قوت(10.0, -scale);

      واپس val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_مقصد_time64(دوہرا val, ثابت __vpiScope*scope)
{
      صحیح سرکاؤ = 0;
      اگر (scope) سرکاؤ = scope->وقت_units - scope->وقت_precision;
      أكد(سرکاؤ >= 0);

      أكد(val >= 0);

	// Scale to the local precision and then round away from zero.
      دوہرا scale = قوت(10.0L, سرکاؤ);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      اگر (scope) {
	    سرکاؤ = scope->وقت_precision - vpi_وقت_precision;
	    أكد(سرکاؤ >= 0);
	    برائے (صحیح lp = 0; lp <  سرکاؤ; lp += 1) delay *= 10;
      }

      واپس delay;
}

ساکن خالی timevar_get_قدر(vpiHandle ref, s_vpi_قدر*vp, منطقی is_int_func,
                              منطقی is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ساکن ساخت t_vpi_وقت وقت_قدر;

      ساخت __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      غیرعلامتی طویل عدد_بٹ;
      vvp_time64_t x, simtime = جدول_وقت_simtime();
      صحیح units = rfp->scope? rfp->scope->وقت_units : vpi_وقت_precision;

      ساکن ثابت size_t RBUF_USE_سائز = 128;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_سائز, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t مقسوم_علیہ = 1;
      جب (units > vpi_وقت_precision) {
	    مقسوم_علیہ *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % مقسوم_علیہ;
      simtime /= مقسوم_علیہ;

      اگر ((مقسوم_علیہ >= 10) && (simtime_fraction >= (مقسوم_علیہ/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      اگر (is_stime) simtime &= 0xffffffff;

      انتخاب (vp->format) {
	  حالت vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpi_وقت_Val;
	    // fallthrough
	  حالت vpi_وقت_Val:
	    vp->value.time = &وقت_قدر;
	    vp->value.time->type = vpi_Sim_وقت;
	    vpip_وقت_مقصد_timestruct(vp->value.time, simtime);
	    توڑو;

	  حالت vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    اگر (is_int_func)  vp->value.real = دوہرا (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    ورنہ vp->value.real = vpip_وقت_مقصد_scaled_real(جدول_وقت_simtime(),
						           rfp->scope);
	    توڑو;

	  حالت vpiBinStrVal:
	    x = simtime;
	    عدد_بٹ = 8 * حجم(vvp_time64_t);

	    rbuf[عدد_بٹ] = 0;
	    برائے (غیرعلامتی i = 1; i <= عدد_بٹ; i++) {
	      rbuf[عدد_بٹ-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_سائز, "%" وقت_FMT_U, simtime);
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_سائز, "%" وقت_FMT_O, simtime);
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_سائز, "%" وقت_FMT_X, simtime);
	    vp->value.str = rbuf;
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (صحیح)vp->format);
	    أكد(0);
      }
}

ساکن خالی timevar_get_ivalue(vpiHandle ref, s_vpi_قدر*vp)
{
      timevar_get_قدر(ref, vp, سچ, جھوٹ);
}

ساکن خالی timevar_get_svalue(vpiHandle ref, s_vpi_قدر*vp)
{
      timevar_get_قدر(ref, vp, سچ, سچ);
}

ساکن خالی timevar_get_rvalue(vpiHandle ref, s_vpi_قدر*vp)
{
      timevar_get_قدر(ref, vp, جھوٹ, جھوٹ);
}

__vpiScopedTime::__vpiScopedTime()
{ }

حرف* __vpiScopedTime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    واپس simple_سیٹ_rbuf_str("$time");
	  طےشدہ:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiScopedTime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_ivalue(یہ, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

صحیح __vpiScopedSTime::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpi_سائز:
	    واپس 32;

	  طےشدہ:
	    واپس __vpiSystemTime::vpi_get(code);
      }
}


حرف* __vpiScopedSTime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    واپس simple_سیٹ_rbuf_str("$stime");
	  طےشدہ:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiScopedSTime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_svalue(یہ, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

صحیح __vpiSystemTime::get_type_رمز(خالی) ثابت
{ واپس vpiSysFuncCall; }

صحیح __vpiSystemTime::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpi_سائز:
	    واپس 64;

          حالت vpiSigned:
	    واپس 0;

	  حالت vpiFuncType:
	    واپس vpi_وقت_Func;

          حالت vpiAutomatic:
	    واپس 0;

	  طےشدہ:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    واپس 0;
      }
}


حرف* __vpiSystemTime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    واپس simple_سیٹ_rbuf_str("$simtime");
	  طےشدہ:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiSystemTime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_ivalue(یہ, val); }

vpiHandle __vpiSystemTime::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    واپس scope;
	  طےشدہ:
	    واپس 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

صحیح __vpiScopedRealtime::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpi_سائز:
	    واپس 1;

          حالت vpiSigned:
	    واپس 0;

	  حالت vpiFuncType:
	    واپس vpiRealFunc;

          حالت vpiAutomatic:
	    واپس 0;

	  طےشدہ:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    واپس 0;
      }
}


حرف* __vpiScopedRealtime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    واپس simple_سیٹ_rbuf_str("$realtime");
	  طےشدہ:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiScopedRealtime::vpi_get_قدر(p_vpi_قدر val)
{ timevar_get_rvalue(یہ, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_وقت(__vpiScope*scope, منطقی is_stime)
{
      اگر (scope) {
	    اگر (is_stime) {
		  scope->scoped_stime.scope = scope;
		  واپس &scope->scoped_stime;
	    } ورنہ {
		  scope->scoped_وقت.scope = scope;
		  واپس &scope->scoped_وقت;
	    }
      } ورنہ {
	    واپس &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      واپس &scope->scoped_realtime;
}

صحیح vpip_get_وقت_precision(خالی)
{
      واپس vpi_وقت_precision;
}

خالی vpip_سیٹ_وقت_precision(صحیح pre)
{
      vpi_وقت_precision = pre;
}
