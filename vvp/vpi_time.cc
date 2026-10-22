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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vpi_time_precision = 0;

ಸ್ಥಿರ ಸಂರಚನಾ __vpiSystemTime global_simtime;

ಶೂನ್ಯ vpip_time_to_timestruct(ಸಂರಚನಾ t_vpi_time*ts, vvp_time64_t ti)
{
      ts->ಕೆಳಗಿನ  = ti & 0xFFFFFFFF;
      ts->ಮೇಲಿನ = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ಧ್ರುವ ಸಂರಚನಾ t_vpi_time*ts)
{
      vvp_time64_t ti = ts->ಮೇಲಿನ;
      ti <<= 32;
      ti += ts->ಕೆಳಗಿನ & 0xffffffff;
      ಪ್ರತ್ಯಾಗಮ ti;
}

ದ್ವಿಗುಣ vpip_time_to_scaled_real(vvp_time64_t ti, ಧ್ರುವ __vpiScope*scope)
{
      ದ್ವಿಗುಣ val;
      ಪೂರ್ಣಾಙ್ಕ scale = 0;
      ಯದಿ (scope) scale = vpi_time_precision - scope->time_units;

      ಯದಿ (scale >= 0) val = (ದ್ವಿಗುಣ)ti * pow(10.0, scale);
      ಅನ್ಯಥಾ val = (ದ್ವಿಗುಣ)ti / pow(10.0, -scale);

      ಪ್ರತ್ಯಾಗಮ val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(ದ್ವಿಗುಣ val, ಧ್ರುವ __vpiScope*scope)
{
      ಪೂರ್ಣಾಙ್ಕ shift = 0;
      ಯದಿ (scope) shift = scope->time_units - scope->time_precision;
      ಪ್ರತಿಜ್ಞಾ(shift >= 0);

      ಪ್ರತಿಜ್ಞಾ(val >= 0);

	// Scale to the local precision and then round away from zero.
      ದ್ವಿಗುಣ scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      ಯದಿ (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    ಪ್ರತಿಜ್ಞಾ(shift >= 0);
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      ಪ್ರತ್ಯಾಗಮ delay;
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp, ತಾರ್ಕಿಕ is_int_func,
                              ತಾರ್ಕಿಕ is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ಸ್ಥಿರ ಸಂರಚನಾ t_vpi_time time_ಮೌಲ್ಯ;

      ಸಂರಚನಾ __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long ಅಂಕೆ_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      ಪೂರ್ಣಾಙ್ಕ units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_USE_SIZE = 128;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ಭಾಜಕ = 1;
      ಯಾವತ್ (units > vpi_time_precision) {
	    ಭಾಜಕ *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ಭಾಜಕ;
      simtime /= ಭಾಜಕ;

      ಯದಿ ((ಭಾಜಕ >= 10) && (simtime_fraction >= (ಭಾಜಕ/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      ಯದಿ (is_stime) simtime &= 0xffffffff;

      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  ಪಕ್ಷ vpiTimeVal:
	    vp->value.time = &time_ಮೌಲ್ಯ;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    ಯದಿ (is_int_func)  vp->value.real = ದ್ವಿಗುಣ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    ಅನ್ಯಥಾ vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal:
	    x = simtime;
	    ಅಂಕೆ_bits = 8 * ಪರಿಮಾಣ(vvp_time64_t);

	    rbuf[ಅಂಕೆ_bits] = 0;
	    ಪ್ರತಿ (unsigned i = 1; i <= ಅಂಕೆ_bits; i++) {
	      rbuf[ಅಂಕೆ_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_ivalue(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      timevar_get_ಮೌಲ್ಯ(ref, vp, ಸತ್ಯ, ಅಸತ್ಯ);
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_svalue(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      timevar_get_ಮೌಲ್ಯ(ref, vp, ಸತ್ಯ, ಸತ್ಯ);
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_rvalue(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      timevar_get_ಮೌಲ್ಯ(ref, vp, ಅಸತ್ಯ, ಅಸತ್ಯ);
}

__vpiScopedTime::__vpiScopedTime()
{ }

ವರ್ಣ* __vpiScopedTime::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiName:
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("$time");
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiScopedTime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_ivalue(ಏತತ್, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiScopedSTime::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 32;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ __vpiSystemTime::vpi_get(code);
      }
}


ವರ್ಣ* __vpiScopedSTime::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiName:
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("$stime");
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiScopedSTime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_svalue(ಏತತ್, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

ಪೂರ್ಣಾಙ್ಕ __vpiSystemTime::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiSysFuncCall; }

ಪೂರ್ಣಾಙ್ಕ __vpiSystemTime::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 64;

          ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 0;

	  ಪಕ್ಷ vpiFuncType:
	    ಪ್ರತ್ಯಾಗಮ vpiTimeFunc;

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ವರ್ಣ* __vpiSystemTime::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiName:
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("$simtime");
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiSystemTime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_ivalue(ಏತತ್, val); }

vpiHandle __vpiSystemTime::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ scope;
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiScopedRealtime::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 1;

          ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 0;

	  ಪಕ್ಷ vpiFuncType:
	    ಪ್ರತ್ಯಾಗಮ vpiRealFunc;

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ವರ್ಣ* __vpiScopedRealtime::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiName:
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("$realtime");
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiScopedRealtime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_rvalue(ಏತತ್, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, ತಾರ್ಕಿಕ is_stime)
{
      ಯದಿ (scope) {
	    ಯದಿ (is_stime) {
		  scope->scoped_stime.scope = scope;
		  ಪ್ರತ್ಯಾಗಮ &scope->scoped_stime;
	    } ಅನ್ಯಥಾ {
		  scope->scoped_time.scope = scope;
		  ಪ್ರತ್ಯಾಗಮ &scope->scoped_time;
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತ್ಯಾಗಮ &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      ಪ್ರತ್ಯಾಗಮ &scope->scoped_realtime;
}

ಪೂರ್ಣಾಙ್ಕ vpip_get_time_precision(ಶೂನ್ಯ)
{
      ಪ್ರತ್ಯಾಗಮ vpi_time_precision;
}

ಶೂನ್ಯ vpip_set_time_precision(ಪೂರ್ಣಾಙ್ಕ pre)
{
      vpi_time_precision = pre;
}
