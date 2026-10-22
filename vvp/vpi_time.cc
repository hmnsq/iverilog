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
ਸਥਿਰ ਪੂਰਨ vpi_ਸਮਾਂ_precision = 0;

ਸਥਿਰ ਬਣਤਰ __vpiSystemTime global_simtime;

ਖਾਲੀ vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(ਬਣਤਰ t_vpi_ਸਮਾਂ*ts, vvp_time64_t ti)
{
      ts->ਨੀਵਾਂ  = ti & 0xFFFFFFFF;
      ts->ਉੱਚਾ = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_ਮੰਜ਼ਿਲ_ਸਮਾਂ(ਅਚਲ ਬਣਤਰ t_vpi_ਸਮਾਂ*ts)
{
      vvp_time64_t ti = ts->ਉੱਚਾ;
      ti <<= 32;
      ti += ts->ਨੀਵਾਂ & 0xffffffff;
      ਵਾਪਸ ti;
}

ਦੁੱਗਣਾ vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_scaled_real(vvp_time64_t ti, ਅਚਲ __vpiScope*scope)
{
      ਦੁੱਗਣਾ val;
      ਪੂਰਨ scale = 0;
      ਜੇ (scope) scale = vpi_ਸਮਾਂ_precision - scope->ਸਮਾਂ_units;

      ਜੇ (scale >= 0) val = (ਦੁੱਗਣਾ)ti * ਘਾਤ(10.0, scale);
      ਨਹੀਂਤਾਂ val = (ਦੁੱਗਣਾ)ti / ਘਾਤ(10.0, -scale);

      ਵਾਪਸ val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_ਮੰਜ਼ਿਲ_time64(ਦੁੱਗਣਾ val, ਅਚਲ __vpiScope*scope)
{
      ਪੂਰਨ ਖਿਸਕਾਓ = 0;
      ਜੇ (scope) ਖਿਸਕਾਓ = scope->ਸਮਾਂ_units - scope->ਸਮਾਂ_precision;
      ਪ੍ਰਤਿਜ੍ਞਾ(ਖਿਸਕਾਓ >= 0);

      ਪ੍ਰਤਿਜ੍ਞਾ(val >= 0);

	// Scale to the local precision and then round away from zero.
      ਦੁੱਗਣਾ scale = ਘਾਤ(10.0L, ਖਿਸਕਾਓ);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      ਜੇ (scope) {
	    ਖਿਸਕਾਓ = scope->ਸਮਾਂ_precision - vpi_ਸਮਾਂ_precision;
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਖਿਸਕਾਓ >= 0);
	    ਲਈ (ਪੂਰਨ lp = 0; lp <  ਖਿਸਕਾਓ; lp += 1) delay *= 10;
      }

      ਵਾਪਸ delay;
}

ਸਥਿਰ ਖਾਲੀ timevar_get_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp, ਤਾਰਕਿਕ is_int_func,
                              ਤਾਰਕਿਕ is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      ਸਥਿਰ ਬਣਤਰ t_vpi_ਸਮਾਂ ਸਮਾਂ_ਮੁੱਲ;

      ਬਣਤਰ __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕ_ਬਿੱਟ;
      vvp_time64_t x, simtime = ਸਮਾਂ_ਸਾਰਣੀ_simtime();
      ਪੂਰਨ units = rfp->scope? rfp->scope->ਸਮਾਂ_units : vpi_ਸਮਾਂ_precision;

      ਸਥਿਰ ਅਚਲ size_t RBUF_USE_ਮਾਪ = 128;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_USE_ਮਾਪ, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ਭਾਜਕ = 1;
      ਜਦੋਂ (units > vpi_ਸਮਾਂ_precision) {
	    ਭਾਜਕ *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ਭਾਜਕ;
      simtime /= ਭਾਜਕ;

      ਜੇ ((ਭਾਜਕ >= 10) && (simtime_fraction >= (ਭਾਜਕ/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      ਜੇ (is_stime) simtime &= 0xffffffff;

      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpi_ਸਮਾਂ_Val;
	    // fallthrough
	  ਹਾਲਤ vpi_ਸਮਾਂ_Val:
	    vp->value.time = &ਸਮਾਂ_ਮੁੱਲ;
	    vp->value.time->type = vpi_Sim_ਸਮਾਂ;
	    vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(vp->value.time, simtime);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    ਜੇ (is_int_func)  vp->value.real = ਦੁੱਗਣਾ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    ਨਹੀਂਤਾਂ vp->value.real = vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_scaled_real(ਸਮਾਂ_ਸਾਰਣੀ_simtime(),
						           rfp->scope);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal:
	    x = simtime;
	    ਅੰਕ_ਬਿੱਟ = 8 * ਆਕਾਰ(vvp_time64_t);

	    rbuf[ਅੰਕ_ਬਿੱਟ] = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 1; i <= ਅੰਕ_ਬਿੱਟ; i++) {
	      rbuf[ਅੰਕ_ਬਿੱਟ-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "%" ਸਮਾਂ_FMT_U, simtime);
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "%" ਸਮਾਂ_FMT_O, simtime);
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "%" ਸਮਾਂ_FMT_X, simtime);
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਸਥਿਰ ਖਾਲੀ timevar_get_ivalue(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      timevar_get_ਮੁੱਲ(ref, vp, ਸੱਚ, ਝੂਠ);
}

ਸਥਿਰ ਖਾਲੀ timevar_get_svalue(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      timevar_get_ਮੁੱਲ(ref, vp, ਸੱਚ, ਸੱਚ);
}

ਸਥਿਰ ਖਾਲੀ timevar_get_rvalue(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      timevar_get_ਮੁੱਲ(ref, vp, ਝੂਠ, ਝੂਠ);
}

__vpiScopedTime::__vpiScopedTime()
{ }

ਅੱਖਰ* __vpiScopedTime::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiName:
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("$time");
	  ਮੂਲ:
	    fprintf(stderr, "Code: %d\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiScopedTime::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ timevar_get_ivalue(ਇਹ, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

ਪੂਰਨ __vpiScopedSTime::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 32;

	  ਮੂਲ:
	    ਵਾਪਸ __vpiSystemTime::vpi_get(code);
      }
}


ਅੱਖਰ* __vpiScopedSTime::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiName:
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("$stime");
	  ਮੂਲ:
	    fprintf(stderr, "Code: %d\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiScopedSTime::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ timevar_get_svalue(ਇਹ, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

ਪੂਰਨ __vpiSystemTime::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiSysFuncCall; }

ਪੂਰਨ __vpiSystemTime::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 64;

          ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 0;

	  ਹਾਲਤ vpiFuncType:
	    ਵਾਪਸ vpi_ਸਮਾਂ_Func;

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

	  ਮੂਲ:
	    fprintf(stderr, "Code: %d\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਅੱਖਰ* __vpiSystemTime::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiName:
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("$simtime");
	  ਮੂਲ:
	    fprintf(stderr, "Code: %d\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiSystemTime::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ timevar_get_ivalue(ਇਹ, val); }

vpiHandle __vpiSystemTime::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ scope;
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

ਪੂਰਨ __vpiScopedRealtime::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 1;

          ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 0;

	  ਹਾਲਤ vpiFuncType:
	    ਵਾਪਸ vpiRealFunc;

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

	  ਮੂਲ:
	    fprintf(stderr, "Code: %d\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਅੱਖਰ* __vpiScopedRealtime::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiName:
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("$realtime");
	  ਮੂਲ:
	    fprintf(stderr, "Code: %d\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiScopedRealtime::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ timevar_get_rvalue(ਇਹ, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_ਸਮਾਂ(__vpiScope*scope, ਤਾਰਕਿਕ is_stime)
{
      ਜੇ (scope) {
	    ਜੇ (is_stime) {
		  scope->scoped_stime.scope = scope;
		  ਵਾਪਸ &scope->scoped_stime;
	    } ਨਹੀਂਤਾਂ {
		  scope->scoped_ਸਮਾਂ.scope = scope;
		  ਵਾਪਸ &scope->scoped_ਸਮਾਂ;
	    }
      } ਨਹੀਂਤਾਂ {
	    ਵਾਪਸ &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      ਵਾਪਸ &scope->scoped_realtime;
}

ਪੂਰਨ vpip_get_ਸਮਾਂ_precision(ਖਾਲੀ)
{
      ਵਾਪਸ vpi_ਸਮਾਂ_precision;
}

ਖਾਲੀ vpip_ਸੈੱਟ_ਸਮਾਂ_precision(ਪੂਰਨ pre)
{
      vpi_ਸਮਾਂ_precision = pre;
}
