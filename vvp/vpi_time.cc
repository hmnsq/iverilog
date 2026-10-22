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
अटल पूर्णांक vpi_time_precision = 0;

अटल काष्ठा __vpiSystemTime global_simtime;

व्योम vpip_time_to_timestruct(काष्ठा t_vpi_time*ts, vvp_time64_t ti)
{
      ts->निचला  = ti & 0xFFFFFFFF;
      ts->ऊपरी = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(स्थिर काष्ठा t_vpi_time*ts)
{
      vvp_time64_t ti = ts->ऊपरी;
      ti <<= 32;
      ti += ts->निचला & 0xffffffff;
      वापस ti;
}

द्विगुन vpip_time_to_scaled_real(vvp_time64_t ti, स्थिर __vpiScope*scope)
{
      द्विगुन val;
      पूर्णांक scale = 0;
      अगर (scope) scale = vpi_time_precision - scope->time_units;

      अगर (scale >= 0) val = (द्विगुन)ti * घात(10.0, scale);
      अन्यथा val = (द्विगुन)ti / घात(10.0, -scale);

      वापस val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(द्विगुन val, स्थिर __vpiScope*scope)
{
      पूर्णांक shift = 0;
      अगर (scope) shift = scope->time_units - scope->time_precision;
      निश्चित(shift >= 0);

      निश्चित(val >= 0);

	// Scale to the local precision and then round away from zero.
      द्विगुन scale = घात(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      अगर (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    निश्चित(shift >= 0);
	    क्रम (पूर्णांक lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      वापस delay;
}

अटल व्योम timevar_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, bool is_int_func,
                              bool is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      अटल काष्ठा t_vpi_time time_मूल्य;

      काष्ठा __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      अचिन्हित दीर्घ अंक_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      पूर्णांक units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      अटल स्थिर माप_प्रकार RBUF_USE_आकार = 128;
      अक्षर*rbuf = static_cast<अक्षर *>(need_परिणाम_buf(RBUF_USE_आकार, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t भाजक = 1;
      जबतक (units > vpi_time_precision) {
	    भाजक *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % भाजक;
      simtime /= भाजक;

      अगर ((भाजक >= 10) && (simtime_fraction >= (भाजक/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      अगर (is_stime) simtime &= 0xffffffff;

      चयन (vp->format) {
	  हाल vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  हाल vpiTimeVal:
	    vp->value.समय = &time_मूल्य;
	    vp->value.समय->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.समय, simtime);
	    अवरोध;

	  हाल vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    अगर (is_int_func)  vp->value.real = द्विगुन (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    अन्यथा vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    अवरोध;

	  हाल vpiBinStrVal:
	    x = simtime;
	    अंक_bits = 8 * माप(vvp_time64_t);

	    rbuf[अंक_bits] = 0;
	    क्रम (अचिन्हित i = 1; i <= अंक_bits; i++) {
	      rbuf[अंक_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_आकार, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_आकार, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_आकार, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vpi_time: unknown format: %d\n", (पूर्णांक)vp->format);
	    निश्चित(0);
      }
}

अटल व्योम timevar_get_ivalue(vpiHandle ref, s_vpi_मूल्य*vp)
{
      timevar_get_मूल्य(ref, vp, true, false);
}

अटल व्योम timevar_get_svalue(vpiHandle ref, s_vpi_मूल्य*vp)
{
      timevar_get_मूल्य(ref, vp, true, true);
}

अटल व्योम timevar_get_rvalue(vpiHandle ref, s_vpi_मूल्य*vp)
{
      timevar_get_मूल्य(ref, vp, false, false);
}

__vpiScopedTime::__vpiScopedTime()
{ }

अक्षर* __vpiScopedTime::vpi_get_str(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiName:
	    वापस simple_set_rbuf_str("$time");
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiScopedTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_ivalue(यह, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

पूर्णांक __vpiScopedSTime::vpi_get(पूर्णांक code)
{
      चयन (code) {
          हाल vpi_आकार:
	    वापस 32;

	  शेष:
	    वापस __vpiSystemTime::vpi_get(code);
      }
}


अक्षर* __vpiScopedSTime::vpi_get_str(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiName:
	    वापस simple_set_rbuf_str("$stime");
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiScopedSTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_svalue(यह, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

पूर्णांक __vpiSystemTime::get_type_code(व्योम) स्थिर
{ वापस vpiSysFuncCall; }

पूर्णांक __vpiSystemTime::vpi_get(पूर्णांक code)
{
      चयन (code) {
          हाल vpi_आकार:
	    वापस 64;

          हाल vpiSigned:
	    वापस 0;

	  हाल vpiFuncType:
	    वापस vpiTimeFunc;

          हाल vpiAutomatic:
	    वापस 0;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


अक्षर* __vpiSystemTime::vpi_get_str(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiName:
	    वापस simple_set_rbuf_str("$simtime");
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiSystemTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_ivalue(यह, val); }

vpiHandle __vpiSystemTime::vpi_handle(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiScope:
	    वापस scope;
	  शेष:
	    वापस 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

पूर्णांक __vpiScopedRealtime::vpi_get(पूर्णांक code)
{
      चयन (code) {
          हाल vpi_आकार:
	    वापस 1;

          हाल vpiSigned:
	    वापस 0;

	  हाल vpiFuncType:
	    वापस vpiRealFunc;

          हाल vpiAutomatic:
	    वापस 0;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


अक्षर* __vpiScopedRealtime::vpi_get_str(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiName:
	    वापस simple_set_rbuf_str("$realtime");
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiScopedRealtime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_rvalue(यह, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime)
{
      अगर (scope) {
	    अगर (is_stime) {
		  scope->scoped_stime.scope = scope;
		  वापस &scope->scoped_stime;
	    } अन्यथा {
		  scope->scoped_time.scope = scope;
		  वापस &scope->scoped_time;
	    }
      } अन्यथा {
	    वापस &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      वापस &scope->scoped_realtime;
}

पूर्णांक vpip_get_time_precision(व्योम)
{
      वापस vpi_time_precision;
}

व्योम vpip_set_time_precision(पूर्णांक pre)
{
      vpi_time_precision = pre;
}
