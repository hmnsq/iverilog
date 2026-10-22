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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_time_precision = 0;

ସ୍ଥିର ସଂରଚନା __vpiSystemTime global_simtime;

ଶୂନ୍ୟ vpip_time_to_timestruct(ସଂରଚନା t_vpi_time*ts, vvp_time64_t ti)
{
      ts->ତଳ  = ti & 0xFFFFFFFF;
      ts->ଉପର = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ଅଚଳ ସଂରଚନା t_vpi_time*ts)
{
      vvp_time64_t ti = ts->ଉପର;
      ti <<= 32;
      ti += ts->ତଳ & 0xffffffff;
      ଫେରନ୍ତୁ ti;
}

ଦ୍ୱିଗୁଣ vpip_time_to_scaled_real(vvp_time64_t ti, ଅଚଳ __vpiScope*scope)
{
      ଦ୍ୱିଗୁଣ val;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା scale = 0;
      ଯଦି (scope) scale = vpi_time_precision - scope->time_units;

      ଯଦି (scale >= 0) val = (ଦ୍ୱିଗୁଣ)ti * pow(10.0, scale);
      ଅନ୍ୟଥା val = (ଦ୍ୱିଗୁଣ)ti / pow(10.0, -scale);

      ଫେରନ୍ତୁ val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(ଦ୍ୱିଗୁଣ val, ଅଚଳ __vpiScope*scope)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift = 0;
      ଯଦି (scope) shift = scope->time_units - scope->time_precision;
      ପ୍ରତିଜ୍ଞା(shift >= 0);

      ପ୍ରତିଜ୍ଞା(val >= 0);

	// Scale to the local precision and then round away from zero.
      ଦ୍ୱିଗୁଣ scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      ଯଦି (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    ପ୍ରତିଜ୍ଞା(shift >= 0);
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      ଫେରନ୍ତୁ delay;
}

ସ୍ଥିର ଶୂନ୍ୟ timevar_get_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp, ତାର୍କିକ is_int_func,
                              ତାର୍କିକ is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ସ୍ଥିର ସଂରଚନା t_vpi_time time_ମୂଲ୍ୟ;

      ସଂରଚନା __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long ଅଙ୍କ_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      ସ୍ଥିର ଅଚଳ size_t RBUF_USE_SIZE = 128;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ଭାଜକ = 1;
      ଯେତେବେଳେ (units > vpi_time_precision) {
	    ଭାଜକ *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ଭାଜକ;
      simtime /= ଭାଜକ;

      ଯଦି ((ଭାଜକ >= 10) && (simtime_fraction >= (ଭାଜକ/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      ଯଦି (is_stime) simtime &= 0xffffffff;

      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  ସ୍ଥିତି vpiTimeVal:
	    vp->value.time = &time_ମୂଲ୍ୟ;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    ଯଦି (is_int_func)  vp->value.real = ଦ୍ୱିଗୁଣ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    ଅନ୍ୟଥା vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal:
	    x = simtime;
	    ଅଙ୍କ_bits = 8 * ଆକାର(vvp_time64_t);

	    rbuf[ଅଙ୍କ_bits] = 0;
	    ପାଇଁ (unsigned i = 1; i <= ଅଙ୍କ_bits; i++) {
	      rbuf[ଅଙ୍କ_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ସ୍ଥିର ଶୂନ୍ୟ timevar_get_ivalue(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      timevar_get_ମୂଲ୍ୟ(ref, vp, ସତ୍ୟ, ମିଥ୍ୟା);
}

ସ୍ଥିର ଶୂନ୍ୟ timevar_get_svalue(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      timevar_get_ମୂଲ୍ୟ(ref, vp, ସତ୍ୟ, ସତ୍ୟ);
}

ସ୍ଥିର ଶୂନ୍ୟ timevar_get_rvalue(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      timevar_get_ମୂଲ୍ୟ(ref, vp, ମିଥ୍ୟା, ମିଥ୍ୟା);
}

__vpiScopedTime::__vpiScopedTime()
{ }

ଅକ୍ଷର* __vpiScopedTime::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiName:
	    ଫେରନ୍ତୁ simple_set_rbuf_str("$time");
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Code: %d\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiScopedTime::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ timevar_get_ivalue(ଏହି, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiScopedSTime::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 32;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ __vpiSystemTime::vpi_get(code);
      }
}


ଅକ୍ଷର* __vpiScopedSTime::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiName:
	    ଫେରନ୍ତୁ simple_set_rbuf_str("$stime");
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Code: %d\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiScopedSTime::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ timevar_get_svalue(ଏହି, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiSystemTime::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiSysFuncCall; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiSystemTime::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 64;

          ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 0;

	  ସ୍ଥିତି vpiFuncType:
	    ଫେରନ୍ତୁ vpiTimeFunc;

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Code: %d\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଅକ୍ଷର* __vpiSystemTime::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiName:
	    ଫେରନ୍ତୁ simple_set_rbuf_str("$simtime");
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Code: %d\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiSystemTime::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ timevar_get_ivalue(ଏହି, val); }

vpiHandle __vpiSystemTime::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ scope;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiScopedRealtime::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 1;

          ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 0;

	  ସ୍ଥିତି vpiFuncType:
	    ଫେରନ୍ତୁ vpiRealFunc;

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Code: %d\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଅକ୍ଷର* __vpiScopedRealtime::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiName:
	    ଫେରନ୍ତୁ simple_set_rbuf_str("$realtime");
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Code: %d\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiScopedRealtime::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ timevar_get_rvalue(ଏହି, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, ତାର୍କିକ is_stime)
{
      ଯଦି (scope) {
	    ଯଦି (is_stime) {
		  scope->scoped_stime.scope = scope;
		  ଫେରନ୍ତୁ &scope->scoped_stime;
	    } ଅନ୍ୟଥା {
		  scope->scoped_time.scope = scope;
		  ଫେରନ୍ତୁ &scope->scoped_time;
	    }
      } ଅନ୍ୟଥା {
	    ଫେରନ୍ତୁ &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      ଫେରନ୍ତୁ &scope->scoped_realtime;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_get_time_precision(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ vpi_time_precision;
}

ଶୂନ୍ୟ vpip_set_time_precision(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pre)
{
      vpi_time_precision = pre;
}
