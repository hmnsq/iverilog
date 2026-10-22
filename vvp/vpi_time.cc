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
நிலையான முழு vpi_time_precision = 0;

நிலையான கட்டமைப்பு __vpiSystemTime global_simtime;

வெறுமை vpip_time_to_timestruct(கட்டமைப்பு t_vpi_time*ts, vvp_time64_t ti)
{
      ts->கீழ்  = ti & 0xFFFFFFFF;
      ts->உயர்வு = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(மாறிலி கட்டமைப்பு t_vpi_time*ts)
{
      vvp_time64_t ti = ts->உயர்வு;
      ti <<= 32;
      ti += ts->கீழ் & 0xffffffff;
      திரும்பு ti;
}

இரட்டை vpip_time_to_scaled_real(vvp_time64_t ti, மாறிலி __vpiScope*scope)
{
      இரட்டை val;
      முழு scale = 0;
      எனில் (scope) scale = vpi_time_precision - scope->time_units;

      எனில் (scale >= 0) val = (இரட்டை)ti * pow(10.0, scale);
      இல்லையெனில் val = (இரட்டை)ti / pow(10.0, -scale);

      திரும்பு val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(இரட்டை val, மாறிலி __vpiScope*scope)
{
      முழு shift = 0;
      எனில் (scope) shift = scope->time_units - scope->time_precision;
      ப்ரதிஜ்ஞா(shift >= 0);

      ப்ரதிஜ்ஞா(val >= 0);

	// Scale to the local precision and then round away from zero.
      இரட்டை scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      எனில் (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    ப்ரதிஜ்ஞா(shift >= 0);
	    க்கு (முழு lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      திரும்பு delay;
}

நிலையான வெறுமை timevar_get_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு*vp, தருக்க is_int_func,
                              தருக்க is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      நிலையான கட்டமைப்பு t_vpi_time time_மதிப்பு;

      கட்டமைப்பு __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long இலக்கம்_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      முழு units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      நிலையான மாறிலி size_t RBUF_USE_SIZE = 128;
      எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t வகுஎண் = 1;
      போது (units > vpi_time_precision) {
	    வகுஎண் *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % வகுஎண்;
      simtime /= வகுஎண்;

      எனில் ((வகுஎண் >= 10) && (simtime_fraction >= (வகுஎண்/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      எனில் (is_stime) simtime &= 0xffffffff;

      தேர்வு (vp->format) {
	  நிலை vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  நிலை vpiTimeVal:
	    vp->value.time = &time_மதிப்பு;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    நிறுத்து;

	  நிலை vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    எனில் (is_int_func)  vp->value.real = இரட்டை (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    இல்லையெனில் vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    நிறுத்து;

	  நிலை vpiBinStrVal:
	    x = simtime;
	    இலக்கம்_bits = 8 * அளவு(vvp_time64_t);

	    rbuf[இலக்கம்_bits] = 0;
	    க்கு (unsigned i = 1; i <= இலக்கம்_bits; i++) {
	      rbuf[இலக்கம்_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);
      }
}

நிலையான வெறுமை timevar_get_ivalue(vpiHandle ref, s_vpi_மதிப்பு*vp)
{
      timevar_get_மதிப்பு(ref, vp, உண்மை, பொய்);
}

நிலையான வெறுமை timevar_get_svalue(vpiHandle ref, s_vpi_மதிப்பு*vp)
{
      timevar_get_மதிப்பு(ref, vp, உண்மை, உண்மை);
}

நிலையான வெறுமை timevar_get_rvalue(vpiHandle ref, s_vpi_மதிப்பு*vp)
{
      timevar_get_மதிப்பு(ref, vp, பொய், பொய்);
}

__vpiScopedTime::__vpiScopedTime()
{ }

எழுத்து* __vpiScopedTime::vpi_get_str(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiName:
	    திரும்பு simple_set_rbuf_str("$time");
	  இயல்பு:
	    fprintf(stderr, "Code: %d\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiScopedTime::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ timevar_get_ivalue(இது, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

முழு __vpiScopedSTime::vpi_get(முழு code)
{
      தேர்வு (code) {
          நிலை vpiSize:
	    திரும்பு 32;

	  இயல்பு:
	    திரும்பு __vpiSystemTime::vpi_get(code);
      }
}


எழுத்து* __vpiScopedSTime::vpi_get_str(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiName:
	    திரும்பு simple_set_rbuf_str("$stime");
	  இயல்பு:
	    fprintf(stderr, "Code: %d\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiScopedSTime::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ timevar_get_svalue(இது, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

முழு __vpiSystemTime::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiSysFuncCall; }

முழு __vpiSystemTime::vpi_get(முழு code)
{
      தேர்வு (code) {
          நிலை vpiSize:
	    திரும்பு 64;

          நிலை vpiSigned:
	    திரும்பு 0;

	  நிலை vpiFuncType:
	    திரும்பு vpiTimeFunc;

          நிலை vpiAutomatic:
	    திரும்பு 0;

	  இயல்பு:
	    fprintf(stderr, "Code: %d\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


எழுத்து* __vpiSystemTime::vpi_get_str(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiName:
	    திரும்பு simple_set_rbuf_str("$simtime");
	  இயல்பு:
	    fprintf(stderr, "Code: %d\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiSystemTime::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ timevar_get_ivalue(இது, val); }

vpiHandle __vpiSystemTime::vpi_handle(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiScope:
	    திரும்பு scope;
	  இயல்பு:
	    திரும்பு 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

முழு __vpiScopedRealtime::vpi_get(முழு code)
{
      தேர்வு (code) {
          நிலை vpiSize:
	    திரும்பு 1;

          நிலை vpiSigned:
	    திரும்பு 0;

	  நிலை vpiFuncType:
	    திரும்பு vpiRealFunc;

          நிலை vpiAutomatic:
	    திரும்பு 0;

	  இயல்பு:
	    fprintf(stderr, "Code: %d\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


எழுத்து* __vpiScopedRealtime::vpi_get_str(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiName:
	    திரும்பு simple_set_rbuf_str("$realtime");
	  இயல்பு:
	    fprintf(stderr, "Code: %d\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiScopedRealtime::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ timevar_get_rvalue(இது, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, தருக்க is_stime)
{
      எனில் (scope) {
	    எனில் (is_stime) {
		  scope->scoped_stime.scope = scope;
		  திரும்பு &scope->scoped_stime;
	    } இல்லையெனில் {
		  scope->scoped_time.scope = scope;
		  திரும்பு &scope->scoped_time;
	    }
      } இல்லையெனில் {
	    திரும்பு &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      திரும்பு &scope->scoped_realtime;
}

முழு vpip_get_time_precision(வெறுமை)
{
      திரும்பு vpi_time_precision;
}

வெறுமை vpip_set_time_precision(முழு pre)
{
      vpi_time_precision = pre;
}
