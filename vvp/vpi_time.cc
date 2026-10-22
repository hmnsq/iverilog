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
স্থির পূর্ণসংখ্যা vpi_time_precision = 0;

স্থির কাঠামো __vpiSystemTime global_simtime;

শূন্য vpip_time_to_timestruct(কাঠামো t_vpi_time*ts, vvp_time64_t ti)
{
      ts->নিচের  = ti & 0xFFFFFFFF;
      ts->উপরের = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ধ্রুবক কাঠামো t_vpi_time*ts)
{
      vvp_time64_t ti = ts->উপরের;
      ti <<= 32;
      ti += ts->নিচের & 0xffffffff;
      ফেরত ti;
}

দ্বিগুণ vpip_time_to_scaled_real(vvp_time64_t ti, ধ্রুবক __vpiScope*scope)
{
      দ্বিগুণ val;
      পূর্ণসংখ্যা scale = 0;
      যদি (scope) scale = vpi_time_precision - scope->time_units;

      যদি (scale >= 0) val = (দ্বিগুণ)ti * pow(10.0, scale);
      অন্যথা val = (দ্বিগুণ)ti / pow(10.0, -scale);

      ফেরত val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(দ্বিগুণ val, ধ্রুবক __vpiScope*scope)
{
      পূর্ণসংখ্যা shift = 0;
      যদি (scope) shift = scope->time_units - scope->time_precision;
      প্রতিজ্ঞা(shift >= 0);

      প্রতিজ্ঞা(val >= 0);

	// Scale to the local precision and then round away from zero.
      দ্বিগুণ scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      যদি (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    প্রতিজ্ঞা(shift >= 0);
	    জন্য (পূর্ণসংখ্যা lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      ফেরত delay;
}

স্থির শূন্য timevar_get_মান(vpiHandle ref, s_vpi_মান*vp, যৌক্তিক is_int_func,
                              যৌক্তিক is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      স্থির কাঠামো t_vpi_time time_মান;

      কাঠামো __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long অঙ্ক_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      পূর্ণসংখ্যা units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      স্থির ধ্রুবক size_t RBUF_USE_আকার = 128;
      অক্ষর*rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_USE_আকার, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ভাজক = 1;
      যখন (units > vpi_time_precision) {
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

      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  ক্ষেত্র vpiTimeVal:
	    vp->value.time = &time_মান;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    বিরতি;

	  ক্ষেত্র vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    যদি (is_int_func)  vp->value.real = দ্বিগুণ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    অন্যথা vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    বিরতি;

	  ক্ষেত্র vpiBinStrVal:
	    x = simtime;
	    অঙ্ক_bits = 8 * আকার(vvp_time64_t);

	    rbuf[অঙ্ক_bits] = 0;
	    জন্য (unsigned i = 1; i <= অঙ্ক_bits; i++) {
	      rbuf[অঙ্ক_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_আকার, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_আকার, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_আকার, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);
      }
}

স্থির শূন্য timevar_get_ivalue(vpiHandle ref, s_vpi_মান*vp)
{
      timevar_get_মান(ref, vp, সত্য, মিথ্যা);
}

স্থির শূন্য timevar_get_svalue(vpiHandle ref, s_vpi_মান*vp)
{
      timevar_get_মান(ref, vp, সত্য, সত্য);
}

স্থির শূন্য timevar_get_rvalue(vpiHandle ref, s_vpi_মান*vp)
{
      timevar_get_মান(ref, vp, মিথ্যা, মিথ্যা);
}

__vpiScopedTime::__vpiScopedTime()
{ }

অক্ষর* __vpiScopedTime::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiName:
	    ফেরত simple_set_rbuf_str("$time");
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiScopedTime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_ivalue(এই, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

পূর্ণসংখ্যা __vpiScopedSTime::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpi_আকার:
	    ফেরত 32;

	  পূর্বনির্ধারিত:
	    ফেরত __vpiSystemTime::vpi_get(code);
      }
}


অক্ষর* __vpiScopedSTime::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiName:
	    ফেরত simple_set_rbuf_str("$stime");
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiScopedSTime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_svalue(এই, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

পূর্ণসংখ্যা __vpiSystemTime::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiSysFuncCall; }

পূর্ণসংখ্যা __vpiSystemTime::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpi_আকার:
	    ফেরত 64;

          ক্ষেত্র vpiSigned:
	    ফেরত 0;

	  ক্ষেত্র vpiFuncType:
	    ফেরত vpiTimeFunc;

          ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


অক্ষর* __vpiSystemTime::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiName:
	    ফেরত simple_set_rbuf_str("$simtime");
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiSystemTime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_ivalue(এই, val); }

vpiHandle __vpiSystemTime::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiScope:
	    ফেরত scope;
	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

পূর্ণসংখ্যা __vpiScopedRealtime::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpi_আকার:
	    ফেরত 1;

          ক্ষেত্র vpiSigned:
	    ফেরত 0;

	  ক্ষেত্র vpiFuncType:
	    ফেরত vpiRealFunc;

          ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


অক্ষর* __vpiScopedRealtime::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiName:
	    ফেরত simple_set_rbuf_str("$realtime");
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Code: %d\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiScopedRealtime::vpi_get_মান(p_vpi_মান val)
{ timevar_get_rvalue(এই, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, যৌক্তিক is_stime)
{
      যদি (scope) {
	    যদি (is_stime) {
		  scope->scoped_stime.scope = scope;
		  ফেরত &scope->scoped_stime;
	    } অন্যথা {
		  scope->scoped_time.scope = scope;
		  ফেরত &scope->scoped_time;
	    }
      } অন্যথা {
	    ফেরত &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      ফেরত &scope->scoped_realtime;
}

পূর্ণসংখ্যা vpip_get_time_precision(শূন্য)
{
      ফেরত vpi_time_precision;
}

শূন্য vpip_set_time_precision(পূর্ণসংখ্যা pre)
{
      vpi_time_precision = pre;
}
