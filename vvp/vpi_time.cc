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
স্থির পূর্ণাঙ্ক vpi_time_precision = 0;

স্থির সংরচনা __vpiSystemTime global_simtime;

শূন্য vpip_time_to_timestruct(সংরচনা t_vpi_time*ts, vvp_time64_t ti)
{
      ts->নিচেৰ  = ti & 0xFFFFFFFF;
      ts->উপৰেৰ = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ধ্রুৱ সংরচনা t_vpi_time*ts)
{
      vvp_time64_t ti = ts->উপৰেৰ;
      ti <<= 32;
      ti += ts->নিচেৰ & 0xffffffff;
      ঘূৰাই ti;
}

দ্ৱিগুণ vpip_time_to_scaled_real(vvp_time64_t ti, ধ্রুৱ __vpiScope*scope)
{
      দ্ৱিগুণ val;
      পূর্ণাঙ্ক scale = 0;
      যদি (scope) scale = vpi_time_precision - scope->time_units;

      যদি (scale >= 0) val = (দ্ৱিগুণ)ti * pow(10.0, scale);
      অন্যথা val = (দ্ৱিগুণ)ti / pow(10.0, -scale);

      ঘূৰাই val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(দ্ৱিগুণ val, ধ্রুৱ __vpiScope*scope)
{
      পূর্ণাঙ্ক shift = 0;
      যদি (scope) shift = scope->time_units - scope->time_precision;
      প্রতিজ্ঞা(shift >= 0);

      প্রতিজ্ঞা(val >= 0);

	// Scale to the local precision and then round away from zero.
      দ্ৱিগুণ scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      যদি (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    প্রতিজ্ঞা(shift >= 0);
	    বাবে (পূর্ণাঙ্ক lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      ঘূৰাই delay;
}

স্থির শূন্য timevar_get_মান(vpiHandle ref, s_vpi_মান*vp, তার্কিক is_int_func,
                              তার্কিক is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      স্থির সংরচনা t_vpi_time time_মান;

      সংরচনা __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long অঙ্ক_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      পূর্ণাঙ্ক units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      স্থির ধ্রুৱ size_t RBUF_USE_SIZE = 128;
      ৱর্ণ*rbuf = static_cast<ৱর্ণ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ভাজক = 1;
      যেতিয়ালৈ (units > vpi_time_precision) {
	    ভাজক *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ভাজক;
      simtime /= ভাজক;

      যদি ((ভাজক >= 10) && (simtime_fraction >= (ভাজক/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      যদি (is_stime) simtime &= 0xffffffff;

      ৱিকল্প (vp->format) {
	  পক্ষ vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  পক্ষ vpiTimeVal:
	    vp->value.time = &time_মান;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    ভঙ্গ;

	  পক্ষ vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    যদি (is_int_func)  vp->value.real = দ্ৱিগুণ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    অন্যথা vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal:
	    x = simtime;
	    অঙ্ক_bits = 8 * পরিমাণ(vvp_time64_t);

	    rbuf[অঙ্ক_bits] = 0;
	    বাবে (unsigned i = 1; i <= অঙ্ক_bits; i++) {
	      rbuf[অঙ্ক_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  স্ৱভাৱ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);
      }
}

স্থির শূন্য timevar_get_ivalue(vpiHandle ref, s_vpi_মান*vp)
{
      timevar_get_মান(ref, vp, সঁচা, মিছা);
}

স্থির শূন্য timevar_get_svalue(vpiHandle ref, s_vpi_মান*vp)
{
      timevar_get_মান(ref, vp, সঁচা, সঁচা);
}

স্থির শূন্য timevar_get_rvalue(vpiHandle ref, s_vpi_মান*vp)
{
      timevar_get_মান(ref, vp, মিছা, মিছা);
}

__vpiScopedTime::__vpiScopedTime()
{ }

ৱর্ণ* __vpiScopedTime::vpi_get_str(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiName:
	    ঘূৰাই simple_set_rbuf_str("$time");
	  স্ৱভাৱ:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


শূন্য __vpiScopedTime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_ivalue(এতত্, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

পূর্ণাঙ্ক __vpiScopedSTime::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
          পক্ষ vpiSize:
	    ঘূৰাই 32;

	  স্ৱভাৱ:
	    ঘূৰাই __vpiSystemTime::vpi_get(code);
      }
}


ৱর্ণ* __vpiScopedSTime::vpi_get_str(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiName:
	    ঘূৰাই simple_set_rbuf_str("$stime");
	  স্ৱভাৱ:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


শূন্য __vpiScopedSTime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_svalue(এতত্, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

পূর্ণাঙ্ক __vpiSystemTime::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই vpiSysFuncCall; }

পূর্ণাঙ্ক __vpiSystemTime::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
          পক্ষ vpiSize:
	    ঘূৰাই 64;

          পক্ষ vpiSigned:
	    ঘূৰাই 0;

	  পক্ষ vpiFuncType:
	    ঘূৰাই vpiTimeFunc;

          পক্ষ vpiAutomatic:
	    ঘূৰাই 0;

	  স্ৱভাৱ:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


ৱর্ণ* __vpiSystemTime::vpi_get_str(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiName:
	    ঘূৰাই simple_set_rbuf_str("$simtime");
	  স্ৱভাৱ:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


শূন্য __vpiSystemTime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_ivalue(এতত্, val); }

vpiHandle __vpiSystemTime::vpi_handle(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiScope:
	    ঘূৰাই scope;
	  স্ৱভাৱ:
	    ঘূৰাই 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

পূর্ণাঙ্ক __vpiScopedRealtime::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
          পক্ষ vpiSize:
	    ঘূৰাই 1;

          পক্ষ vpiSigned:
	    ঘূৰাই 0;

	  পক্ষ vpiFuncType:
	    ঘূৰাই vpiRealFunc;

          পক্ষ vpiAutomatic:
	    ঘূৰাই 0;

	  স্ৱভাৱ:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


ৱর্ণ* __vpiScopedRealtime::vpi_get_str(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiName:
	    ঘূৰাই simple_set_rbuf_str("$realtime");
	  স্ৱভাৱ:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


শূন্য __vpiScopedRealtime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_rvalue(এতত্, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, তার্কিক is_stime)
{
      যদি (scope) {
	    যদি (is_stime) {
		  scope->scoped_stime.scope = scope;
		  ঘূৰাই &scope->scoped_stime;
	    } অন্যথা {
		  scope->scoped_time.scope = scope;
		  ঘূৰাই &scope->scoped_time;
	    }
      } অন্যথা {
	    ঘূৰাই &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      ঘূৰাই &scope->scoped_realtime;
}

পূর্ণাঙ্ক vpip_get_time_precision(শূন্য)
{
      ঘূৰাই vpi_time_precision;
}

শূন্য vpip_set_time_precision(পূর্ণাঙ্ক pre)
{
      vpi_time_precision = pre;
}
