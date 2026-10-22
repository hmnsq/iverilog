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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vpi_ಸಮಯ_precision = 0;

ಸ್ಥಿರ ರಚನೆ __vpiSystemTime global_simtime;

ಶೂನ್ಯ vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(ರಚನೆ t_vpi_ಸಮಯ*ts, vvp_time64_t ti)
{
      ts->ಕೆಳಗಿನ  = ti & 0xFFFFFFFF;
      ts->ಮೇಲಿನ = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_ಗಮ್ಯ_ಸಮಯ(ಸ್ಥಿರಾಂಕ ರಚನೆ t_vpi_ಸಮಯ*ts)
{
      vvp_time64_t ti = ts->ಮೇಲಿನ;
      ti <<= 32;
      ti += ts->ಕೆಳಗಿನ & 0xffffffff;
      ಹಿಂತಿರುಗಿ ti;
}

ದ್ವಿಗುಣ vpip_ಸಮಯ_ಗಮ್ಯ_scaled_real(vvp_time64_t ti, ಸ್ಥಿರಾಂಕ __vpiScope*scope)
{
      ದ್ವಿಗುಣ val;
      ಪೂರ್ಣಾಂಕ scale = 0;
      ಆದರೆ (scope) scale = vpi_ಸಮಯ_precision - scope->ಸಮಯ_units;

      ಆದರೆ (scale >= 0) val = (ದ್ವಿಗುಣ)ti * ಘಾತ(10.0, scale);
      ಇಲ್ಲದಿದ್ದರೆ val = (ದ್ವಿಗುಣ)ti / ಘಾತ(10.0, -scale);

      ಹಿಂತಿರುಗಿ val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_ಗಮ್ಯ_time64(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ __vpiScope*scope)
{
      ಪೂರ್ಣಾಂಕ ಸರಿಸು = 0;
      ಆದರೆ (scope) ಸರಿಸು = scope->ಸಮಯ_units - scope->ಸಮಯ_precision;
      ಪ್ರತಿಜ್ಞಾ(ಸರಿಸು >= 0);

      ಪ್ರತಿಜ್ಞಾ(val >= 0);

	// Scale to the local precision and then round away from zero.
      ದ್ವಿಗುಣ scale = ಘಾತ(10.0L, ಸರಿಸು);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      ಆದರೆ (scope) {
	    ಸರಿಸು = scope->ಸಮಯ_precision - vpi_ಸಮಯ_precision;
	    ಪ್ರತಿಜ್ಞಾ(ಸರಿಸು >= 0);
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ lp = 0; lp <  ಸರಿಸು; lp += 1) delay *= 10;
      }

      ಹಿಂತಿರುಗಿ delay;
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp, ತಾರ್ಕಿಕ is_int_func,
                              ತಾರ್ಕಿಕ is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ಸ್ಥಿರ ರಚನೆ t_vpi_ಸಮಯ ಸಮಯ_ಮೌಲ್ಯ;

      ರಚನೆ __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು;
      vvp_time64_t x, simtime = ವೇಳಾಪಟ್ಟಿ_simtime();
      ಪೂರ್ಣಾಂಕ units = rfp->scope? rfp->scope->ಸಮಯ_units : vpi_ಸಮಯ_precision;

      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_USE_ಗಾತ್ರ = 128;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_USE_ಗಾತ್ರ, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ಭಾಜಕ = 1;
      ಆಗ (units > vpi_ಸಮಯ_precision) {
	    ಭಾಜಕ *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ಭಾಜಕ;
      simtime /= ಭಾಜಕ;

      ಆದರೆ ((ಭಾಜಕ >= 10) && (simtime_fraction >= (ಭಾಜಕ/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      ಆದರೆ (is_stime) simtime &= 0xffffffff;

      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpi_ಸಮಯ_Val;
	    // fallthrough
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Val:
	    vp->value.time = &ಸಮಯ_ಮೌಲ್ಯ;
	    vp->value.time->type = vpi_Sim_ಸಮಯ;
	    vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(vp->value.time, simtime);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    ಆದರೆ (is_int_func)  vp->value.real = ದ್ವಿಗುಣ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    ಇಲ್ಲದಿದ್ದರೆ vp->value.real = vpip_ಸಮಯ_ಗಮ್ಯ_scaled_real(ವೇಳಾಪಟ್ಟಿ_simtime(),
						           rfp->scope);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal:
	    x = simtime;
	    ಅಂಕೆ_ಬಿಟ್ಟುಗಳು = 8 * ಗಾತ್ರ(vvp_time64_t);

	    rbuf[ಅಂಕೆ_ಬಿಟ್ಟುಗಳು] = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 1; i <= ಅಂಕೆ_ಬಿಟ್ಟುಗಳು; i++) {
	      rbuf[ಅಂಕೆ_ಬಿಟ್ಟುಗಳು-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%" ಸಮಯ_FMT_U, simtime);
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%" ಸಮಯ_FMT_O, simtime);
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%" ಸಮಯ_FMT_X, simtime);
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_ivalue(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      timevar_get_ಮೌಲ್ಯ(ref, vp, ನಿಜ, ಸುಳ್ಳು);
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_svalue(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      timevar_get_ಮೌಲ್ಯ(ref, vp, ನಿಜ, ನಿಜ);
}

ಸ್ಥಿರ ಶೂನ್ಯ timevar_get_rvalue(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      timevar_get_ಮೌಲ್ಯ(ref, vp, ಸುಳ್ಳು, ಸುಳ್ಳು);
}

__vpiScopedTime::__vpiScopedTime()
{ }

ಅಕ್ಷರ* __vpiScopedTime::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiName:
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("$time");
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiScopedTime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_ivalue(ಇದು, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

ಪೂರ್ಣಾಂಕ __vpiScopedSTime::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 32;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ __vpiSystemTime::vpi_get(code);
      }
}


ಅಕ್ಷರ* __vpiScopedSTime::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiName:
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("$stime");
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiScopedSTime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_svalue(ಇದು, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

ಪೂರ್ಣಾಂಕ __vpiSystemTime::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiSysFuncCall; }

ಪೂರ್ಣಾಂಕ __vpiSystemTime::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 64;

          ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 0;

	  ಸ್ಥಿತಿ vpiFuncType:
	    ಹಿಂತಿರುಗಿ vpi_ಸಮಯ_Func;

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಅಕ್ಷರ* __vpiSystemTime::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiName:
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("$simtime");
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiSystemTime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_ivalue(ಇದು, val); }

vpiHandle __vpiSystemTime::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ scope;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

ಪೂರ್ಣಾಂಕ __vpiScopedRealtime::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 1;

          ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 0;

	  ಸ್ಥಿತಿ vpiFuncType:
	    ಹಿಂತಿರುಗಿ vpiRealFunc;

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಅಕ್ಷರ* __vpiScopedRealtime::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiName:
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("$realtime");
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Code: %d\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiScopedRealtime::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ timevar_get_rvalue(ಇದು, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_ಸಮಯ(__vpiScope*scope, ತಾರ್ಕಿಕ is_stime)
{
      ಆದರೆ (scope) {
	    ಆದರೆ (is_stime) {
		  scope->scoped_stime.scope = scope;
		  ಹಿಂತಿರುಗಿ &scope->scoped_stime;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  scope->scoped_ಸಮಯ.scope = scope;
		  ಹಿಂತಿರುಗಿ &scope->scoped_ಸಮಯ;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಹಿಂತಿರುಗಿ &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      ಹಿಂತಿರುಗಿ &scope->scoped_realtime;
}

ಪೂರ್ಣಾಂಕ vpip_get_ಸಮಯ_precision(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ vpi_ಸಮಯ_precision;
}

ಶೂನ್ಯ vpip_ಗಣ_ಸಮಯ_precision(ಪೂರ್ಣಾಂಕ pre)
{
      vpi_ಸಮಯ_precision = pre;
}
