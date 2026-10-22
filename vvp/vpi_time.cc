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
സ്ഥിതം പൂർണ്ണസംഖ്യ vpi_time_precision = 0;

സ്ഥിതം ഘടന __vpiSystemTime global_simtime;

ശൂന്യം vpip_time_to_timestruct(ഘടന t_vpi_time*ts, vvp_time64_t ti)
{
      ts->താഴെയുള്ള  = ti & 0xFFFFFFFF;
      ts->മുകളിലെ = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(സ്ഥിരം ഘടന t_vpi_time*ts)
{
      vvp_time64_t ti = ts->മുകളിലെ;
      ti <<= 32;
      ti += ts->താഴെയുള്ള & 0xffffffff;
      തിരികെ ti;
}

ഇരട്ട vpip_time_to_scaled_real(vvp_time64_t ti, സ്ഥിരം __vpiScope*scope)
{
      ഇരട്ട val;
      പൂർണ്ണസംഖ്യ scale = 0;
      എങ്കിൽ (scope) scale = vpi_time_precision - scope->time_units;

      എങ്കിൽ (scale >= 0) val = (ഇരട്ട)ti * pow(10.0, scale);
      അല്ലെങ്കിൽ val = (ഇരട്ട)ti / pow(10.0, -scale);

      തിരികെ val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(ഇരട്ട val, സ്ഥിരം __vpiScope*scope)
{
      പൂർണ്ണസംഖ്യ shift = 0;
      എങ്കിൽ (scope) shift = scope->time_units - scope->time_precision;
      പ്രതിജ്ഞാ(shift >= 0);

      പ്രതിജ്ഞാ(val >= 0);

	// Scale to the local precision and then round away from zero.
      ഇരട്ട scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      എങ്കിൽ (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    പ്രതിജ്ഞാ(shift >= 0);
	    വേണ്ടി (പൂർണ്ണസംഖ്യ lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      തിരികെ delay;
}

സ്ഥിതം ശൂന്യം timevar_get_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp, തർക്കം is_int_func,
                              തർക്കം is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      സ്ഥിതം ഘടന t_vpi_time time_മൂല്യം;

      ഘടന __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long അക്കം_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      പൂർണ്ണസംഖ്യ units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      സ്ഥിതം സ്ഥിരം size_t RBUF_USE_SIZE = 128;
      അക്ഷരം*rbuf = static_cast<അക്ഷരം *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ഹാരകം = 1;
      ആകുമ്പോൾ (units > vpi_time_precision) {
	    ഹാരകം *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ഹാരകം;
      simtime /= ഹാരകം;

      എങ്കിൽ ((ഹാരകം >= 10) && (simtime_fraction >= (ഹാരകം/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      എങ്കിൽ (is_stime) simtime &= 0xffffffff;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  അവസ്ഥ vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  അവസ്ഥ vpiTimeVal:
	    vp->value.time = &time_മൂല്യം;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    നിർത്തുക;

	  അവസ്ഥ vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    എങ്കിൽ (is_int_func)  vp->value.real = ഇരട്ട (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    അല്ലെങ്കിൽ vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    നിർത്തുക;

	  അവസ്ഥ vpiBinStrVal:
	    x = simtime;
	    അക്കം_bits = 8 * വലുപ്പം(vvp_time64_t);

	    rbuf[അക്കം_bits] = 0;
	    വേണ്ടി (unsigned i = 1; i <= അക്കം_bits; i++) {
	      rbuf[അക്കം_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);
      }
}

സ്ഥിതം ശൂന്യം timevar_get_ivalue(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      timevar_get_മൂല്യം(ref, vp, സത്യം, അസത്യം);
}

സ്ഥിതം ശൂന്യം timevar_get_svalue(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      timevar_get_മൂല്യം(ref, vp, സത്യം, സത്യം);
}

സ്ഥിതം ശൂന്യം timevar_get_rvalue(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      timevar_get_മൂല്യം(ref, vp, അസത്യം, അസത്യം);
}

__vpiScopedTime::__vpiScopedTime()
{ }

അക്ഷരം* __vpiScopedTime::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiName:
	    തിരികെ simple_set_rbuf_str("$time");
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Code: %d\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiScopedTime::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ timevar_get_ivalue(ഇത്, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

പൂർണ്ണസംഖ്യ __vpiScopedSTime::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiSize:
	    തിരികെ 32;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ __vpiSystemTime::vpi_get(code);
      }
}


അക്ഷരം* __vpiScopedSTime::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiName:
	    തിരികെ simple_set_rbuf_str("$stime");
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Code: %d\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiScopedSTime::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ timevar_get_svalue(ഇത്, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

പൂർണ്ണസംഖ്യ __vpiSystemTime::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiSysFuncCall; }

പൂർണ്ണസംഖ്യ __vpiSystemTime::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiSize:
	    തിരികെ 64;

          അവസ്ഥ vpiSigned:
	    തിരികെ 0;

	  അവസ്ഥ vpiFuncType:
	    തിരികെ vpiTimeFunc;

          അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Code: %d\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


അക്ഷരം* __vpiSystemTime::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiName:
	    തിരികെ simple_set_rbuf_str("$simtime");
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Code: %d\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiSystemTime::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ timevar_get_ivalue(ഇത്, val); }

vpiHandle __vpiSystemTime::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiScope:
	    തിരികെ scope;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

പൂർണ്ണസംഖ്യ __vpiScopedRealtime::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiSize:
	    തിരികെ 1;

          അവസ്ഥ vpiSigned:
	    തിരികെ 0;

	  അവസ്ഥ vpiFuncType:
	    തിരികെ vpiRealFunc;

          അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Code: %d\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


അക്ഷരം* __vpiScopedRealtime::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiName:
	    തിരികെ simple_set_rbuf_str("$realtime");
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Code: %d\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiScopedRealtime::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ timevar_get_rvalue(ഇത്, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, തർക്കം is_stime)
{
      എങ്കിൽ (scope) {
	    എങ്കിൽ (is_stime) {
		  scope->scoped_stime.scope = scope;
		  തിരികെ &scope->scoped_stime;
	    } അല്ലെങ്കിൽ {
		  scope->scoped_time.scope = scope;
		  തിരികെ &scope->scoped_time;
	    }
      } അല്ലെങ്കിൽ {
	    തിരികെ &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      തിരികെ &scope->scoped_realtime;
}

പൂർണ്ണസംഖ്യ vpip_get_time_precision(ശൂന്യം)
{
      തിരികെ vpi_time_precision;
}

ശൂന്യം vpip_set_time_precision(പൂർണ്ണസംഖ്യ pre)
{
      vpi_time_precision = pre;
}
