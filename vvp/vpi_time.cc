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
ایستا صحیح vpi_time_precision = 0;

ایستا ساختار __vpiSystemTime global_simtime;

تهی vpip_time_to_timestruct(ساختار t_vpi_time*ts, vvp_time64_t ti)
{
      ts->کم  = ti & 0xFFFFFFFF;
      ts->بالا = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ثابت ساختار t_vpi_time*ts)
{
      vvp_time64_t ti = ts->بالا;
      ti <<= 32;
      ti += ts->کم & 0xffffffff;
      بازگشت ti;
}

دوگانه vpip_time_to_scaled_real(vvp_time64_t ti, ثابت __vpiScope*scope)
{
      دوگانه val;
      صحیح scale = 0;
      اگر (scope) scale = vpi_time_precision - scope->time_units;

      اگر (scale >= 0) val = (دوگانه)ti * pow(10.0, scale);
      وگرنه val = (دوگانه)ti / pow(10.0, -scale);

      بازگشت val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(دوگانه val, ثابت __vpiScope*scope)
{
      صحیح shift = 0;
      اگر (scope) shift = scope->time_units - scope->time_precision;
      أكد(shift >= 0);

      أكد(val >= 0);

	// Scale to the local precision and then round away from zero.
      دوگانه scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      اگر (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    أكد(shift >= 0);
	    برای (صحیح lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      بازگشت delay;
}

ایستا تهی timevar_get_مقدار(vpiHandle ref, s_vpi_مقدار*vp, منطقی is_int_func,
                              منطقی is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ایستا ساختار t_vpi_time time_مقدار;

      ساختار __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long رقم_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      صحیح units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      ایستا ثابت size_t RBUF_USE_SIZE = 128;
      نویسه*rbuf = static_cast<نویسه *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t مقسوم_علیه = 1;
      وقتی (units > vpi_time_precision) {
	    مقسوم_علیه *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % مقسوم_علیه;
      simtime /= مقسوم_علیه;

      اگر ((مقسوم_علیه >= 10) && (simtime_fraction >= (مقسوم_علیه/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      اگر (is_stime) simtime &= 0xffffffff;

      انتخاب (vp->format) {
	  حالت vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  حالت vpiTimeVal:
	    vp->value.time = &time_مقدار;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    بشکن;

	  حالت vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    اگر (is_int_func)  vp->value.real = دوگانه (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    وگرنه vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    بشکن;

	  حالت vpiBinStrVal:
	    x = simtime;
	    رقم_bits = 8 * اندازه(vvp_time64_t);

	    rbuf[رقم_bits] = 0;
	    برای (unsigned i = 1; i <= رقم_bits; i++) {
	      rbuf[رقم_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (صحیح)vp->format);
	    أكد(0);
      }
}

ایستا تهی timevar_get_ivalue(vpiHandle ref, s_vpi_مقدار*vp)
{
      timevar_get_مقدار(ref, vp, درست, نادرست);
}

ایستا تهی timevar_get_svalue(vpiHandle ref, s_vpi_مقدار*vp)
{
      timevar_get_مقدار(ref, vp, درست, درست);
}

ایستا تهی timevar_get_rvalue(vpiHandle ref, s_vpi_مقدار*vp)
{
      timevar_get_مقدار(ref, vp, نادرست, نادرست);
}

__vpiScopedTime::__vpiScopedTime()
{ }

نویسه* __vpiScopedTime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    بازگشت simple_set_rbuf_str("$time");
	  پیشفرض:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiScopedTime::vpi_get_مقدار(p_vpi_مقدار val)
{ timevar_get_ivalue(این, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

صحیح __vpiScopedSTime::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpiSize:
	    بازگشت 32;

	  پیشفرض:
	    بازگشت __vpiSystemTime::vpi_get(code);
      }
}


نویسه* __vpiScopedSTime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    بازگشت simple_set_rbuf_str("$stime");
	  پیشفرض:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiScopedSTime::vpi_get_مقدار(p_vpi_مقدار val)
{ timevar_get_svalue(این, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

صحیح __vpiSystemTime::get_type_code(تهی) ثابت
{ بازگشت vpiSysFuncCall; }

صحیح __vpiSystemTime::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpiSize:
	    بازگشت 64;

          حالت vpiSigned:
	    بازگشت 0;

	  حالت vpiFuncType:
	    بازگشت vpiTimeFunc;

          حالت vpiAutomatic:
	    بازگشت 0;

	  پیشفرض:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


نویسه* __vpiSystemTime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    بازگشت simple_set_rbuf_str("$simtime");
	  پیشفرض:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiSystemTime::vpi_get_مقدار(p_vpi_مقدار val)
{ timevar_get_ivalue(این, val); }

vpiHandle __vpiSystemTime::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    بازگشت scope;
	  پیشفرض:
	    بازگشت 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

صحیح __vpiScopedRealtime::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpiSize:
	    بازگشت 1;

          حالت vpiSigned:
	    بازگشت 0;

	  حالت vpiFuncType:
	    بازگشت vpiRealFunc;

          حالت vpiAutomatic:
	    بازگشت 0;

	  پیشفرض:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


نویسه* __vpiScopedRealtime::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    بازگشت simple_set_rbuf_str("$realtime");
	  پیشفرض:
	    fprintf(stderr, "Code: %d\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiScopedRealtime::vpi_get_مقدار(p_vpi_مقدار val)
{ timevar_get_rvalue(این, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, منطقی is_stime)
{
      اگر (scope) {
	    اگر (is_stime) {
		  scope->scoped_stime.scope = scope;
		  بازگشت &scope->scoped_stime;
	    } وگرنه {
		  scope->scoped_time.scope = scope;
		  بازگشت &scope->scoped_time;
	    }
      } وگرنه {
	    بازگشت &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      بازگشت &scope->scoped_realtime;
}

صحیح vpip_get_time_precision(تهی)
{
      بازگشت vpi_time_precision;
}

تهی vpip_set_time_precision(صحیح pre)
{
      vpi_time_precision = pre;
}
