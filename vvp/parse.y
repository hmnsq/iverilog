
%{
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

# include  "parse_misc.h"
# include  "compile.h"
# include  "delay.h"
# include  <list>
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"
# include  "version_base.h"

using namespace std;

/*
 * These are bits in the lexor.
 */
ਬਾਹਰੀ FILE*yyin;

ਸਦਿਸ਼ <ਅਚਲ ਅੱਖਰ*> file_ਨਾਂ;

/*
 * Local variables.
 */

/*
 * When parsing a modpath list, this is the processed destination that
 * the source items will attach themselves to.
 */
ਸਥਿਰ ਬਣਤਰ __vpiModPath*modpath_dst = 0;
%}

%ਸੰਘ {
      ਅੱਖਰ*ਪਾਠ;
      ਅੱਖਰ **ਸਾਰਣੀ;
      uint64_t numb;
      bool flag;

      comp_operands_t opa;

      ਬਣਤਰ symb_s  symb;
      ਬਣਤਰ symbv_s symbv;

      ਬਣਤਰ numbv_s numbv;

      ਬਣਤਰ enum_name_s enum_name;
      std::list<ਬਣਤਰ enum_name_s>*enum_namev;

      ਬਣਤਰ symb_s vect;

      ਬਣਤਰ argv_s argv;
      vpiHandle vpi;

      vvp_delay_t*cdelay;

      ਪੂਰਨ      vpi_enum;
};

%token K_A K_APV
%token K_ARITH_ABS K_ARITH_DIV K_ARITH_DIV_R K_ARITH_DIV_S K_ARITH_MOD
%token K_ARITH_MOD_R K_ARITH_MOD_S
%token K_ARITH_MULT K_ARITH_MULT_R K_ARITH_SUB K_ARITH_SUB_R
%token K_ARITH_ਜੋੜ K_ARITH_ਜੋੜ_R K_ARITH_POW K_ARITH_POW_R K_ARITH_POW_S
%token K_ARRAY K_ARRAY_2U K_ARRAY_2S K_ARRAY_I K_ARRAY_OBJ K_ARRAY_R K_ARRAY_S K_ARRAY_STR K_ARRAY_ਪੋਰਟ
%token K_CAST_INT K_CAST_REAL K_CAST_REAL_S K_CAST_2
%token K_CLASS
%token K_CMP_EEQ K_CMP_EQ K_CMP_EQX K_CMP_EQZ K_CMP_WEQ K_CMP_WNE
%token K_CMP_EQ_R K_CMP_NEE K_CMP_NE K_CMP_NE_R
%token K_CMP_GE K_CMP_GE_R K_CMP_GE_S K_CMP_GT K_CMP_GT_R K_CMP_GT_S
%token K_CONCAT K_CONCAT8 K_DEBUG K_DELAY K_DFF_N K_DFF_N_ACLR
%token K_DFF_N_ASET K_DFF_P K_DFF_P_ACLR K_DFF_P_ASET
%token K_ENUM2 K_ENUM2_S K_ENUM4 K_ENUM4_S K_EVENT K_EVENT_OR
%token K_EXPORT K_EXTEND_S K_FUNCTOR K_IMPORT K_ISLAND K_LATCH K_MODPATH
%token K_NET K_NET_S K_NET_R K_NET_2S K_NET_2U
%token K_NET8 K_NET8_2S K_NET8_2U K_NET8_S
%token K_PARAM_STR K_PARAM_L K_PARAM_REAL K_PART K_PART_PV
%token K_PART_V K_PART_V_S K_ਪੋਰਟ K_ਪੋਰਟ_INFO K_PQ K_PV K_REDUCE_AND K_REDUCE_OR K_REDUCE_XOR
%token K_REDUCE_NAND K_REDUCE_NOR K_REDUCE_XNOR K_REPEAT
%token K_RESOLV K_RTRAN K_RTRANIF0 K_RTRANIF1
%token K_SCOPE K_SFUNC K_SFUNC_E K_SHIFTL K_SHIFTR K_SHIFTRS
%token K_SUBSTITUTE
%token K_THREAD K_TIMESCALE K_TRAN K_TRANIF0 K_TRANIF1 K_TRANVP
%token K_UFUNC_REAL K_UFUNC_VEC4 K_UFUNC_E K_UDP K_UDP_C K_UDP_S
%token K_VAR K_VAR_COBJECT K_VAR_DARRAY
%token K_VAR_ਕਤਾਰ
%token K_VAR_S K_VAR_STR K_VAR_I K_VAR_R K_VAR_2S K_VAR_2U
%token K_vpi_call K_vpi_call_w K_vpi_call_i
%token K_vpi_func K_vpi_func_r K_vpi_func_s
%token K_ivl_ਸੰਸਕਰਣ K_ivl_delay_selection
%token K_vpi_module K_vpi_ਸਮਾਂ_precision K_file_ਨਾਂ K_file_ਸਤਰ
%token K_ਪੋਰਟ_ਨਿਵੇਸ਼ K_ਪੋਰਟ_OUTPUT K_ਪੋਰਟ_INOUT K_ਪੋਰਟ_MIXED K_ਪੋਰਟ_NODIR

%token <ਪਾਠ> T_INSTR
%token <ਪਾਠ> T_LABEL
%token <numb> T_ਸੰਖਿਆ
%token <ਪਾਠ> T_STRING
%token <ਪਾਠ> T_ਚਿੰਨ੍ਹ
%token <vect> T_ਸਦਿਸ਼

%ਕਿਸਮ <flag>  local_flag
%ਕਿਸਮ <vpi_enum> ਪੋਰਟ_type
%ਕਿਸਮ <numb>  signed_t_ਸੰਖਿਆ
%ਕਿਸਮ <numb>  dimension dimensions dimensions_opt
%ਕਿਸਮ <symb>  ਚਿੰਨ੍ਹ ਚਿੰਨ੍ਹ_opt
%ਕਿਸਮ <symbv> symbols symbols_net
%ਕਿਸਮ <numbv> numbers
%ਕਿਸਮ <ਪਾਠ> label_opt
%ਕਿਸਮ <opa>  ਸੰਕਾਰਜ operands operands_opt
%ਕਿਸਮ <ਸਾਰਣੀ> udp_ਸਾਰਣੀ

%ਕਿਸਮ <argv> ਦਲੀਲ_opt ਦਲੀਲ_list
%ਕਿਸਮ <vpi>  ਦਲੀਲ ਚਿੰਨ੍ਹ_access
%ਕਿਸਮ <cdelay> delay

%ਕਿਸਮ <enum_name> enum_type_name
%ਕਿਸਮ <enum_namev> enum_type_ਨਾਂ

%%

ਸਰੋਤ_file : ਸਿਰਲੇਖ_ਸਤਰਾਂ_opt ਪ੍ਰੋਗਰਾਮ footer_ਸਤਰਾਂ;

ਸਿਰਲੇਖ_ਸਤਰਾਂ_opt : ਸਿਰਲੇਖ_ਸਤਰਾਂ | ;


ਸਿਰਲੇਖ_ਸਤਰਾਂ
	: ਸਿਰਲੇਖ_ਸਤਰ
	| ਸਿਰਲੇਖ_ਸਤਰਾਂ ਸਿਰਲੇਖ_ਸਤਰ
	;

ਸਿਰਲੇਖ_ਸਤਰ
	: K_ivl_ਸੰਸਕਰਣ T_STRING ';'
		{ verify_ਸੰਸਕਰਣ($2, ਨਿਰਾ); }
	| K_ivl_ਸੰਸਕਰਣ T_STRING T_STRING ';'
		{ verify_ਸੰਸਕਰਣ($2, $3); }
	| K_ivl_delay_selection T_STRING ';'
		{ ਸੈੱਟ_delay_selection($2); }
	| K_vpi_module T_STRING ';'
		{ compile_ਲੱਦੋ_vpi_module($2); }
	| K_vpi_ਸਮਾਂ_precision '+' T_ਸੰਖਿਆ ';'
		{ compile_vpi_ਸਮਾਂ_precision($3); }
	| K_vpi_ਸਮਾਂ_precision '-' T_ਸੰਖਿਆ ';'
		{ compile_vpi_ਸਮਾਂ_precision(-$3); }
	;

footer_ਸਤਰਾਂ
	: K_file_ਨਾਂ T_ਸੰਖਿਆ ';' { file_ਨਾਂ.reserve($2); }
	  name_ਲੜੀਆਂ
	;

name_ਲੜੀਆਂ
	: T_STRING ';'
		{ file_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ($1); }
	| name_ਲੜੀਆਂ T_STRING ';'
		{ file_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ($2); }
	;

  /* A program is simply a list of statements. No other structure. */
ਪ੍ਰੋਗਰਾਮ
	: statement
	| ਪ੍ਰੋਗਰਾਮ statement
	;


  /* A statement can be any of the following. In all cases, the
     statement is terminated by a semi-colon. In general, a statement
     has a label, an opcode of some source, and operands. The
     structure of the operands depends on the opcode. */

statement

  /* Functor statements define functors. The functor must have a
     label and a type name, and may have operands. The functor may
     also have a delay specification and output strengths. */

	: T_LABEL K_FUNCTOR T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' symbols ';'
		{ compile_functor($1, $3, $4, 6, 6, $6.cnt, $6.vect); }

	| T_LABEL K_FUNCTOR T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ
	          '[' T_ਸੰਖਿਆ T_ਸੰਖਿਆ ']' ',' symbols ';'
		{ ਬਿਨਾਨਿਸ਼ਾਨੀ str0 = $6;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ str1 = $7;
		  compile_functor($1, $3, $4, str0, str1,
				  $10.cnt, $10.vect);
		}


  /* UDP statements define or instantiate UDPs.  Definitions take a
     label (UDP type id) a name (string), the number of inputs, and
     for sequential UDPs the initial value. */

	| T_LABEL K_UDP_S T_STRING ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ',' udp_ਸਾਰਣੀ ';'
		{ compile_udp_def(1, $1, $3, $5, $7, $9); }

	| T_LABEL K_UDP_C T_STRING ',' T_ਸੰਖਿਆ ',' udp_ਸਾਰਣੀ ';'
		{ compile_udp_def(0, $1, $3, $5, 0, $7); }

	| T_LABEL K_UDP T_ਚਿੰਨ੍ਹ ',' symbols ';'
		{ compile_udp_functor($1, $3, $5.cnt, $5.vect); }


  /* Memory.  Definition, port, initialization */

        | T_LABEL K_ARRAY T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_var_array($1, $3, $5, $6, $8, $9, 0); }

        | T_LABEL K_ARRAY_2U T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_var2_array($1, $3, $5, $6, $8, $9, false); }
        | T_LABEL K_ARRAY_2S T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_var2_array($1, $3, $5, $6, $8, $9, true); }

        | T_LABEL K_ARRAY_I T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_var_array($1, $3, $5, $6, $8, $9, 2); }

        | T_LABEL K_ARRAY_R T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_real_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY_S T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_var_array($1, $3, $5, $6, $8, $9, 1); }

        | T_LABEL K_ARRAY_STR T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_string_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY_OBJ T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_object_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
                { compile_net_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY_ਪੋਰਟ T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
		{ compile_array_ਪੋਰਟ($1, $3, $5); }

        | T_LABEL K_ARRAY_ਪੋਰਟ T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ';'
		{ compile_array_ਪੋਰਟ($1, $3, $5); }

        | T_LABEL K_ARRAY T_STRING ',' T_ਚਿੰਨ੍ਹ ';'
                { compile_array_alias($1, $3, $5); }

 /* The .ufunc functors are for implementing user defined functions, or
     other thread code that is automatically invoked if any of the
     bits in the symbols list change. */

  | T_LABEL K_UFUNC_REAL T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_ufunc_real($1, $3, $5, 0, 0, 0, 0, $7, strdup("E_0x0")); }
  | T_LABEL K_UFUNC_REAL T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' symbols '(' symbols ')' T_ਚਿੰਨ੍ਹ ';'
      { compile_ufunc_real($1, $3, $5, $7.cnt, $7.vect, $9.cnt, $9.vect, $11, 0); }

  | T_LABEL K_UFUNC_VEC4 T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_ufunc_vec4($1, $3, $5, 0, 0, 0, 0, $7, strdup("E_0x0")); }
  | T_LABEL K_UFUNC_VEC4 T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' symbols '(' symbols ')' T_ਚਿੰਨ੍ਹ ';'
      { compile_ufunc_vec4($1, $3, $5, $7.cnt, $7.vect, $9.cnt, $9.vect, $11, 0); }

  | T_LABEL K_UFUNC_E T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਚਿੰਨ੍ਹ ',' symbols '(' symbols ')' T_ਚਿੰਨ੍ਹ ';'
      { compile_ufunc_vec4($1, $3, $5, $9.cnt, $9.vect, $11.cnt, $11.vect, $13, $7); }

  /* Resolver statements are very much like functors. They are
     compiled to functors of a different mode. */

	| T_LABEL K_RESOLV T_ਚਿੰਨ੍ਹ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_resolver($1, $3, obj.cnt, obj.vect);
		}

  /* Part select statements take a single netlist input, and numbers
     that define the part to be selected out of the input. */

	| T_LABEL K_PART T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ';'
		{ compile_part_select($1, $3, $5, $7); }

	| T_LABEL K_PART_PV T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ';'
		{ compile_part_select_pv($1, $3, $5, $7, $9); }

	| T_LABEL K_PART_V T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ';'
		{ compile_part_select_var($1, $3, $5, $7, false); }
	| T_LABEL K_PART_V_S T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ';'
		{ compile_part_select_var($1, $3, $5, $7, true); }

  /* Concatenations */

  | T_LABEL K_CONCAT '[' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ ']' ','
    symbols ';'
      { compile_concat($1, $4, $5, $6, $7, $10.cnt, $10.vect); }

  | T_LABEL K_CONCAT8 '[' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ ']' ','
    symbols ';'
      { compile_concat8($1, $4, $5, $6, $7, $10.cnt, $10.vect); }

  /* Substitutions (similar to concatenations) */
  | T_LABEL K_SUBSTITUTE T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ T_ਸੰਖਿਆ ',' symbols ';'
      { compile_substitute($1, $3, $5, $6, $8.cnt, $8.vect); }

  /* The ABS statement is a special arithmetic node that takes 1
     input. Re-use the symbols rule. */

        | T_LABEL K_ARITH_ABS symbols ';'
		{ ਬਣਤਰ symbv_s obj = $3;
		  compile_arith_abs($1, obj.cnt, obj.vect);
		}

  | T_LABEL K_CAST_INT T_ਸੰਖਿਆ ',' symbols ';'
      { ਬਣਤਰ symbv_s obj = $5;
	compile_arith_cast_int($1, $3, obj.cnt, obj.vect);
      }

  | T_LABEL K_CAST_REAL symbols ';'
      { ਬਣਤਰ symbv_s obj = $3;
	compile_arith_cast_real($1, false, obj.cnt, obj.vect);
      }

  | T_LABEL K_CAST_REAL_S symbols ';'
      { ਬਣਤਰ symbv_s obj = $3;
	compile_arith_cast_real($1, true, obj.cnt, obj.vect);
      }

  | T_LABEL K_CAST_2 T_ਸੰਖਿਆ ',' symbols ';'
      { ਬਣਤਰ symbv_s obj = $5;
	compile_arith_cast_vec2($1, $3, obj.cnt, obj.vect);
      }

  /* Arithmetic statements generate functor arrays of a given width
     that take like size input vectors. */

	| T_LABEL K_ARITH_DIV T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_div($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_DIV_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_div_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_DIV_S T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_div($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MOD T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_mod($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MOD_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_mod_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MOD_S T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_mod($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MULT T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_mult($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MULT_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_mult_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_POW T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_pow($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_POW_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_pow_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_POW_S T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_pow($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_SUB T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_sub($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_SUB_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_sub_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_ਜੋੜ T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_ਜੋੜ($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_ਜੋੜ_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_arith_ਜੋੜ_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EEQ T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_eeq($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_NEE T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_nee($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQ T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_eq($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQX T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_eqx($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQZ T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_eqz($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQ_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_eq_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_NE T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_ne($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_NE_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_ne_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GE T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_ge($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GE_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_ge_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GE_S T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_ge($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GT T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_gt($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GT_R T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_gt_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GT_S T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_gt($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_WEQ T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_weq($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_WNE T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_cmp_wne($1, $3, obj.cnt, obj.vect);
		}

  /* Delay nodes take a set of numbers or a set of inputs. The delay
     node takes two form, one with an array of constants and a single
     input, and another with an array of inputs. */

 | T_LABEL K_DELAY T_ਸੰਖਿਆ delay ਚਿੰਨ੍ਹ ';'
    { compile_delay($1, $3, $4, $5); }
 | T_LABEL K_DELAY T_ਸੰਖਿਆ symbols ';'
    { ਬਣਤਰ symbv_s obj = $4;
      compile_delay($1, $3, obj.cnt, obj.vect, false);
    }
 | T_LABEL K_DELAY T_ਸੰਖਿਆ symbols ',' T_ਸੰਖਿਆ ';'
    { ਬਣਤਰ symbv_s obj = $4;
      ਜੇ ($6 != 0) ਪ੍ਰਤਿਜ੍ਞਾ(0);
      compile_delay($1, $3, obj.cnt, obj.vect, true);
    }

 | T_LABEL K_MODPATH T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ਚਿੰਨ੍ਹ ','
    { modpath_dst = compile_modpath($1, $3, $4, $5); }
   modpath_src_list ';'
    { modpath_dst = 0; }

  /* DFF nodes have an output and take up to 4 inputs. */

  | T_LABEL K_DFF_N T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_dff($1, $3, true, $4, $6, $8); }

  | T_LABEL K_DFF_P T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_dff($1, $3, false, $4, $6, $8); }

  | T_LABEL K_DFF_N_ACLR T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_dff_aclr($1, $3, true, $4, $6, $8, $10); }

  | T_LABEL K_DFF_P_ACLR T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_dff_aclr($1, $3, false, $4, $6, $8, $10); }

  | T_LABEL K_DFF_N_ASET T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_dff_aset($1, $3, true, $4, $6, $8, $10, 0); }

  | T_LABEL K_DFF_P_ASET T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_dff_aset($1, $3, false, $4, $6, $8, $10, 0); }

  | T_LABEL K_DFF_N_ASET T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_dff_aset($1, $3, true, $4, $6, $8, $10, $12); }

  | T_LABEL K_DFF_P_ASET T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_dff_aset($1, $3, false, $4, $6, $8, $10, $12); }

  /* LATCH nodes have an output and take 2 inputs. */

  | T_LABEL K_LATCH T_ਸੰਖਿਆ ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ ';'
      { compile_latch($1, $3, $4, $6); }

  /* The various reduction operator nodes take a single input. */

        | T_LABEL K_REDUCE_AND ਚਿੰਨ੍ਹ ';'
                { compile_reduce_and($1, $3); }

        | T_LABEL K_REDUCE_OR ਚਿੰਨ੍ਹ ';'
                { compile_reduce_or($1, $3); }

        | T_LABEL K_REDUCE_XOR ਚਿੰਨ੍ਹ ';'
                { compile_reduce_xor($1, $3); }

        | T_LABEL K_REDUCE_NAND ਚਿੰਨ੍ਹ ';'
                { compile_reduce_nand($1, $3); }

        | T_LABEL K_REDUCE_NOR ਚਿੰਨ੍ਹ ';'
                { compile_reduce_nor($1, $3); }

        | T_LABEL K_REDUCE_XNOR ਚਿੰਨ੍ਹ ';'
                { compile_reduce_xnor($1, $3); }

        | T_LABEL K_REPEAT T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ',' ਚਿੰਨ੍ਹ ';'
                { compile_repeat($1, $3, $5, $7); }

  /* The extend nodes take a width and a symbol. */

        | T_LABEL K_EXTEND_S T_ਸੰਖਿਆ ',' ਚਿੰਨ੍ਹ ';'
                { compile_extend_signed($1, $3, $5); }

  /* System function call */
        | T_LABEL K_SFUNC T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING ','
          T_STRING ',' symbols ';'
                { compile_sfunc($1, $5, $7, $3, $4, $9.cnt, $9.vect, 0); }

        | T_LABEL K_SFUNC_E T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING ',' T_ਚਿੰਨ੍ਹ ','
          T_STRING ',' symbols ';'
                { compile_sfunc($1, $5, $9, $3, $4, $11.cnt, $11.vect, $7); }

  /* System function call - no arguments */
        | T_LABEL K_SFUNC T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING ','
          T_STRING ';'
                { compile_sfunc($1, $5, $7, $3, $4, 0, 0, 0); }

        | T_LABEL K_SFUNC_E T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING ',' T_ਚਿੰਨ੍ਹ ','
          T_STRING ';'
                { compile_sfunc($1, $5, $9, $3, $4, 0, 0, $7); }

  /* Shift nodes. */

	| T_LABEL K_SHIFTL T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		  compile_shiftl($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_SHIFTR T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		      compile_shiftr($1, $3, false, obj.cnt, obj.vect);
		}


	| T_LABEL K_SHIFTRS T_ਸੰਖਿਆ ',' symbols ';'
		{ ਬਣਤਰ symbv_s obj = $5;
		      compile_shiftr($1, $3, true, obj.cnt, obj.vect);
		}


  /* Event statements take a label, a type (the first T_SYMBOL) and a
     list of inputs. If the type is instead a string, then we have a
     named event instead. */

	| T_LABEL K_EVENT T_ਚਿੰਨ੍ਹ ',' symbols ';'
                { compile_event($1, $3, $5.cnt, $5.vect); }

	| T_LABEL K_EVENT K_DEBUG T_ਚਿੰਨ੍ਹ ',' symbols ';'
                { compile_event($1, $4, $6.cnt, $6.vect); }

	| T_LABEL K_EVENT T_STRING ';'
		{ compile_named_event($1, $3); }

	| T_LABEL K_EVENT_OR symbols ';'
                { compile_event($1, 0, $3.cnt, $3.vect); }


  /* Instructions may have a label, and have zero or more
     operands. The meaning of and restrictions on the operands depends
     on the specific instruction. */

	| label_opt T_INSTR operands_opt ';'
		{ compile_ਕੋਡ($1, $2, $3); }

	| T_LABEL ';'
		{ compile_codelabel($1); }

  /* %file_line statements are instructions that have unusual operand
     requirements so are handled by their own rules. */
	| label_opt K_file_ਸਤਰ T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING ';'
		{ compile_file_ਸਤਰ($1, $3, $4, $5); }

	| label_opt K_file_ਸਤਰ T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ ';'
		{ ਪ੍ਰਤਿਜ੍ਞਾ($5 == 0);
		  compile_file_ਸਤਰ($1, $3, $4, 0); }

  /* %vpi_call statements are instructions that have unusual operand
     requirements so are handled by their own rules. The %vpi_func
     statement is a variant of %vpi_call that includes a thread vector
     after the name, and is used for function calls. */

  /* This version does not allow a function to be called as a task. */
  | label_opt K_vpi_call T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING
    ਦਲੀਲ_opt '{' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ '}' ';'
      { compile_vpi_call($1, $5, true, false, $3, $4,
			 $6.argc, $6.argv, $8, $9, $10); }

  /* This version allows a function to be called as a task, but prints a
   * warning message. */
  | label_opt K_vpi_call_w T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING
    ਦਲੀਲ_opt '{' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ '}' ';'
      { compile_vpi_call($1, $5, false, true, $3, $4,
			 $6.argc, $6.argv, $8, $9, $10); }

  /* This version allows a function to be called as a task and does not
   * print a message. */
  | label_opt K_vpi_call_i T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING
    ਦਲੀਲ_opt '{' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ '}' ';'
      { compile_vpi_call($1, $5, false, false, $3, $4,
			 $6.argc, $6.argv, $8, $9, $10); }

  | label_opt K_vpi_func T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING T_ਸੰਖਿਆ
    ਦਲੀਲ_opt  '{' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ '}' ';'
      { compile_vpi_func_call($1, $5, -vpiVectorVal, $6, $3, $4,
			      $7.argc, $7.argv, $9, $10, $11); }

  | label_opt K_vpi_func_r T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING
    ਦਲੀਲ_opt '{' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ '}' ';'
      { compile_vpi_func_call($1, $5, -vpiRealVal, 0, $3, $4,
			      $6.argc, $6.argv, $8, $9, $10); }

  | label_opt K_vpi_func_s T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_STRING
    ਦਲੀਲ_opt '{' T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ '}' ';'
      { compile_vpi_func_call($1, $5, -vpiStringVal, 0, $3, $4,
			      $6.argc, $6.argv, $8, $9, $10); }

  /* Scope statements come in two forms. There are the scope
     declaration and the scope recall. The declarations create the
     scope, with their association with a parent. The label of the
     scope declaration is associated with the new scope.

     The symbol is module, function task, fork or begin. It is the
     general class of the scope.

     The strings are the instance name and type name of the
     module. For example, if it is instance U of module foo, the
     instance name is "U" and the type name is "foo".

     The final symbol is the label of the parent scope. If there is no
     parent scope, then this is a root scope. */

	| T_LABEL K_SCOPE T_ਚਿੰਨ੍ਹ ',' T_STRING T_STRING T_ਸੰਖਿਆ T_ਸੰਖਿਆ ';'
		{ compile_scope_decl($1, $3, $5, $6, 0, $7, $8, $7, $8, 0); }

	| T_LABEL K_SCOPE T_ਚਿੰਨ੍ਹ ',' T_STRING T_STRING T_ਸੰਖਿਆ T_ਸੰਖਿਆ ','
	  T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ ',' T_ਚਿੰਨ੍ਹ ';'
		{ compile_scope_decl($1, $3, $5, $6, $14, $7, $8, $10, $11, $12); }

  /* Scope recall has no label of its own, but refers by label to a
     declared scope. */

	|         K_SCOPE T_ਚਿੰਨ੍ਹ ';'
		{ compile_scope_recall($2); }


  /* Port information for scopes... currently this is just meta-data for VPI queries */

	| K_ਪੋਰਟ_INFO T_ਸੰਖਿਆ ਪੋਰਟ_type T_ਸੰਖਿਆ T_STRING T_ਚਿੰਨ੍ਹ ';'
		{ compile_ਪੋਰਟ_info( $2 /* port_index */, $3, $4 /*ਚੌੜਾਈ*/,
		                     $5 /*&name */, $6 /*ਬਫਰ*/ ); }

	| K_ਪੋਰਟ_INFO T_ਸੰਖਿਆ ਪੋਰਟ_type T_ਸੰਖਿਆ T_STRING ';'
		{ compile_ਪੋਰਟ_info( $2 /* port_index */, $3, $4 /*ਚੌੜਾਈ*/,
		                     $5 /*&name */, nullptr /*ਬਫਰ*/ ); }

	|         K_TIMESCALE T_ਸੰਖਿਆ T_ਸੰਖਿਆ';'
		{ compile_timescale($2, $3); }
	|         K_TIMESCALE '-' T_ਸੰਖਿਆ T_ਸੰਖਿਆ';'
		{ compile_timescale(-$3, $4); }
	|         K_TIMESCALE T_ਸੰਖਿਆ '-' T_ਸੰਖਿਆ';'
		{ compile_timescale($2, -$4); }
	|         K_TIMESCALE '-' T_ਸੰਖਿਆ '-' T_ਸੰਖਿਆ';'
		{ compile_timescale(-$3, -$5); }

  /* Thread statements declare a thread with its starting address. The
     starting address must already be defined. The .thread statement
     may also take an optional flag word. */

	|         K_THREAD T_ਚਿੰਨ੍ਹ ';'
		{ compile_thread($2, 0); }

	|         K_THREAD T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
		{ compile_thread($2, $4); }

  /* Var statements declare a bit of a variable. This also implicitly
     creates a functor with the same name that acts as the output of
     the variable in the netlist. */

  | T_LABEL K_VAR local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
      { compile_variable($1, $4, $6, $7, vpiLogicVar, false, $3); }

  | T_LABEL K_VAR_S local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
      { compile_variable($1, $4, $6, $7, vpiLogicVar, true, $3); }

  | T_LABEL K_VAR_I local_flag T_STRING ',' T_ਸੰਖਿਆ T_ਸੰਖਿਆ ';'
      { compile_variable($1, $4, $6, $7, vpiIntegerVar, true, $3); }

  | T_LABEL K_VAR_2S local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
      { compile_variable($1, $4, $6, $7, vpiIntVar, true, $3); }

  | T_LABEL K_VAR_2U local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
      { compile_variable($1, $4, $6, $7, vpiIntVar, false, $3); }

  | T_LABEL K_VAR_R local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ';'
      { compile_var_real($1, $4, $3); }

  | T_LABEL K_VAR_STR T_STRING ';'
      { compile_var_string($1, $3); }

  | T_LABEL K_VAR_DARRAY T_STRING ',' T_ਸੰਖਿਆ ';'
      { compile_var_darray($1, $3, $5); }

  | T_LABEL K_VAR_ਕਤਾਰ T_STRING  ',' T_ਸੰਖਿਆ';'
      { compile_var_ਕਤਾਰ($1, $3, $5); }

  | T_LABEL K_VAR_COBJECT T_STRING ';'
      { compile_var_cobject($1, $3); }

  /* Net statements are similar to .var statements, except that they
     declare nets, and they have an input list. */

  | T_LABEL K_NET local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiLogicVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_S local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiLogicVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2U local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiIntVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2S local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiIntVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8 local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_2U local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_S local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_2S local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_R local_flag T_STRING ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ
    ',' symbols_net ';'
      { compile_net_real($1, $4, $6, $7, $3, $9.cnt, $9.vect); }

  /* Arrayed versions of net directives. */

  | T_LABEL K_NET T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiLogicVar, false, $9.cnt, $9.vect); }

  | T_LABEL K_NET_S T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiLogicVar, true, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2U T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiIntVar, false, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2S T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiIntVar, true, $9.cnt, $9.vect); }

  | T_LABEL K_NET8 T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, -vpiLogicVar, false, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_S T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, -vpiLogicVar, true, $9.cnt, $9.vect); }

  | T_LABEL K_NET_R T_ਚਿੰਨ੍ਹ T_ਸੰਖਿਆ ',' signed_t_ਸੰਖਿਆ signed_t_ਸੰਖਿਆ ','
    symbols_net ';'
      { compile_netw_real($1, $3, $4, $6, $7, $9.cnt, $9.vect); }

  /* Parameter statements come in a few simple forms. The most basic
     is the string parameter. */

	| T_LABEL K_PARAM_STR T_STRING T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ',' T_STRING ';'
		{ compile_param_string($1, $3, $8, $4, $5, $6); }

	| T_LABEL K_PARAM_L T_STRING T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ',' T_ਚਿੰਨ੍ਹ ';'
		{ compile_param_logic($1, $3, $8, false, $4, $5, $6); }

	| T_LABEL K_PARAM_L T_STRING T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ',' '+' T_ਚਿੰਨ੍ਹ ';'
		{ compile_param_logic($1, $3, $9, true, $4, $5, $6 ); }

	| T_LABEL K_PARAM_REAL T_STRING T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ',' T_ਚਿੰਨ੍ਹ ';'
		{ compile_param_real($1, $3, $8, $4, $5, $6); }

  /* Islands */

  | T_LABEL K_ISLAND T_ਚਿੰਨ੍ਹ ';'
      { compile_island($1, $3); }

  | T_LABEL K_ਪੋਰਟ T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_island_ਪੋਰਟ($1, $3, $5); }

  | T_LABEL K_IMPORT T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_island_import($1, $3, $5); }

  | T_LABEL K_EXPORT T_ਚਿੰਨ੍ਹ ';'
      { compile_island_export($1, $3); }

  | K_RTRAN T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranif(0, $2, $4, $5, 0, 1); }

  | K_RTRANIF0 T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranif(0, $2, $4, $5, $7, 1); }

  | K_RTRANIF1 T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranif(1, $2, $4, $5, $7, 1); }

  | K_TRAN T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranif(0, $2, $4, $5, 0, 0); }

  | K_TRANIF0 T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranif(0, $2, $4, $5, $7, 0); }

  | K_TRANIF1 T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranif(1, $2, $4, $5, $7, 0); }

  | K_TRANVP T_ਸੰਖਿਆ T_ਸੰਖਿਆ T_ਸੰਖਿਆ ',' T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ T_ਚਿੰਨ੍ਹ ';'
      { compile_island_tranvp($6, $8, $9, $2, $3, $4); }

  /* Other statements */

  | T_LABEL K_CLASS T_STRING '[' T_ਸੰਖਿਆ ']'
      { compile_class_ਸ਼ੁਰੂ($1, $3, $5); }
    class_properties_opt ';'
      { compile_class_ਪੂਰਾ(); }

  | enum_type
      { ; }

  /* Oh and by the way, empty statements are OK as well. */

  | ';'
  ;

class_properties_opt
  : class_properties
  |
  ;

class_properties
  : class_properties class_property
  | class_property
  ;

class_property
  : T_ਸੰਖਿਆ ':' T_STRING ',' T_STRING dimensions_opt
      { compile_class_property($1, $3, $5, $6); }
  ;

/*
 * The syntax for dimensions allows the code generator to give the
 * accurate dimensions for for the property, but for now we are only
 * interested in the total number of elements. So reduce the ranges
 * to a simple number, and scale the number.
 */
dimensions_opt
  : dimensions
      { $$ = $1; }
  |   { $$ = 0; }
  ;

dimensions
  : dimensions dimension
      { $$ = $1 * $2; }
  | dimension
      { $$ = $1; }
  ;

dimension
  : '[' T_ਸੰਖਿਆ ':' T_ਸੰਖਿਆ ']'
      { $$ = ($2 > $4? $2 - $4 : $4 - $2) + 1; }
  ;

  /* Enumeration types */
enum_type
  : T_LABEL K_ENUM2 '(' T_ਸੰਖਿਆ ')' enum_type_ਨਾਂ ';'
      { compile_enum2_type($1, $4, false, $6); }
  | T_LABEL K_ENUM2_S '(' T_ਸੰਖਿਆ ')' enum_type_ਨਾਂ ';'
      { compile_enum2_type($1, $4, true, $6); }
  | T_LABEL K_ENUM4 '(' T_ਸੰਖਿਆ ')' enum_type_ਨਾਂ ';'
      { compile_enum4_type($1, $4, false, $6); }
  | T_LABEL K_ENUM4_S '(' T_ਸੰਖਿਆ ')' enum_type_ਨਾਂ ';'
      { compile_enum4_type($1, $4, true, $6); }
  ;

enum_type_ਨਾਂ
  : enum_type_name
      { list<ਬਣਤਰ enum_name_s>*tmp = new list<ਬਣਤਰ enum_name_s>;
	tmp->ਧੱਕੋ_ਪਿੱਠ($1);
	$$ = tmp;
      }
  | enum_type_ਨਾਂ ',' enum_type_name
      { list<ਬਣਤਰ enum_name_s>*tmp = $1;
	tmp->ਧੱਕੋ_ਪਿੱਠ($3);
	$$ = tmp;
      }
  ;

enum_type_name
  : T_STRING T_ਸੰਖਿਆ
      { $$.ਪਾਠ = $1; $$.val2 = $2; $$.val4 = 0; }
  | T_STRING T_ਸਦਿਸ਼
      { $$.ਪਾਠ = $1; $$.val2 = 0; $$.val4 = $2.ਪਾਠ; }
  ;

local_flag
  : '*' { $$ = true; }
  |     { $$ = false; }
  ;

  /* There are a few places where the label is optional. This rule
     returns the label value if present, or 0 if not. */

label_opt
	: T_LABEL { $$ = $1; }
	|         { $$ = 0; }
	;

operands_opt
	: operands { $$ = $1; }
	|          { $$ = 0; }
	;

operands
	: operands ',' ਸੰਕਾਰਜ
		{ comp_operands_t opa = $1;
		  ਪ੍ਰਤਿਜ੍ਞਾ(opa->argc < 3);
		  ਪ੍ਰਤਿਜ੍ਞਾ($3->argc == 1);
		  opa->argv[opa->argc] = $3->argv[0];
		  opa->argc += 1;
		  ਮੁਕਤ($3);
		  $$ =  opa;
		}
	| ਸੰਕਾਰਜ
		{ $$ = $1; }
	;

ਸੰਕਾਰਜ
  : ਚਿੰਨ੍ਹ
      { comp_operands_t opa = static_cast<comp_operands_t>
                                         (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, ਆਕਾਰ(ਬਣਤਰ comp_operands_s)));
	opa->argc = 1;
	opa->argv[0].ltype = L_SYMB;
	opa->argv[0].symb = $1;
	$$ = opa;
      }
  | T_ਸੰਖਿਆ
      { comp_operands_t opa = static_cast<comp_operands_t>
                                         (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, ਆਕਾਰ(ਬਣਤਰ comp_operands_s)));
	opa->argc = 1;
	opa->argv[0].ltype = L_NUMB;
	opa->argv[0].numb = $1;
	$$ = opa;
      }
  | T_STRING
      { comp_operands_t opa = static_cast<comp_operands_t>
                                         (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, ਆਕਾਰ(ਬਣਤਰ comp_operands_s)));
	opa->argc = 1;
	opa->argv[0].ltype = L_STRING;
	opa->argv[0].ਪਾਠ = $1;
	$$ = opa;
      }
  ;


  /* The argument_list is a list of vpiHandle objects that can be
     passed to a %vpi_call statement (and hence built into a
     vpiCallSysTask handle). We build up an arbitrary sized list with
     the struct argv_s type.

     Each argument of the call is represented as a vpiHandle
     object.  If the argument is a symbol, the symbol name will be
     kept, until the argument_list is complete.  Then, all symbol
     lookups will be attempted.  Postponed lookups will point into the
     resulting $$->argv.
     If it is some other supported object, the necessary
     vpiHandle object is created to support it. */

ਦਲੀਲ_opt
	: ',' ਦਲੀਲ_list
		{
		  argv_sym_lookup(&$2);
		  $$ = $2;
		}
	| /*ਖਾਲੀ_ਥਾਵਾਂ*/
		{ ਬਣਤਰ argv_s tmp;
		  argv_init(&tmp);
		  $$ = tmp;
		}
	;

ਦਲੀਲ_list
  : ਦਲੀਲ
      { ਬਣਤਰ argv_s tmp;
	argv_init(&tmp);
	argv_ਜੋੜੋ(&tmp, $1);
	$$ = tmp;
      }
  | ਦਲੀਲ_list ',' ਦਲੀਲ
      { ਬਣਤਰ argv_s tmp = $1;
	argv_ਜੋੜੋ(&tmp, $3);
	$$ = tmp;
      }
  | T_ਚਿੰਨ੍ਹ
      { ਬਣਤਰ argv_s tmp;
	argv_init(&tmp);
	argv_sym_ਜੋੜੋ(&tmp, $1);
	$$ = tmp;
      }
  | ਦਲੀਲ_list ',' T_ਚਿੰਨ੍ਹ
      { ਬਣਤਰ argv_s tmp = $1;
	argv_sym_ਜੋੜੋ(&tmp, $3);
	$$ = tmp;
      }
  ;

ਦਲੀਲ
  : T_STRING
      { $$ = vpip_make_string_const($1); }
  | T_ਸਦਿਸ਼
      { $$ = vpip_make_ਦੁਆਧਾਰੀ_const($1.idx, $1.ਪਾਠ);
	ਮੁਕਤ($1.ਪਾਠ);
      }
  | ਚਿੰਨ੍ਹ_access
      { $$ = $1; }
  ;

ਚਿੰਨ੍ਹ_access
  : K_A '<' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_vthr_A($3, $5); }
  | K_A '<' T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ '>'
      { $$ = vpip_make_vthr_A($3, $5); }
  | K_A '<' T_ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ_access '>'
      { $$ = vpip_make_vthr_A($3, $5); }
  | K_PV '<' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_PV($3, $5, $7); }
  | K_PV '<' T_ਚਿੰਨ੍ਹ ',' '-' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_PV($3, -$6, $8); }
  | K_PV '<' T_ਚਿੰਨ੍ਹ ',' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_PV($3, $5, $7); }
  | K_PV '<' T_ਚਿੰਨ੍ਹ ',' ਚਿੰਨ੍ਹ_access ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_PV($3, $5, $7); }
  | K_APV '<' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_vthr_APV($3, $5, $7, $9); }
  | K_PQ '<' T_ਚਿੰਨ੍ਹ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ '>'
      { $$ = vpip_make_prop_ਕਤਾਰ_ref($3, $5, $7); }
  ;

  /* functor operands can only be a list of symbols. */
symbols
	: ਚਿੰਨ੍ਹ
		{ ਬਣਤਰ symbv_s obj;
		  symbv_init(&obj);
		  symbv_ਜੋੜੋ(&obj, $1);
		  $$ = obj;
		}
	| symbols ',' ਚਿੰਨ੍ਹ
		{ ਬਣਤਰ symbv_s obj = $1;
		  symbv_ਜੋੜੋ(&obj, $3);
		  $$ = obj;
		}
	;


numbers
	: T_ਸੰਖਿਆ
		{ ਬਣਤਰ numbv_s obj;
		  numbv_init(&obj);
		  numbv_ਜੋੜੋ(&obj, $1);
		  $$ = obj;
		}
	| numbers ',' T_ਸੰਖਿਆ
		{ ਬਣਤਰ numbv_s obj = $1;
		  numbv_ਜੋੜੋ(&obj, $3);
		  $$ = obj;
		}
	;


symbols_net
	: ਚਿੰਨ੍ਹ_opt
		{ ਬਣਤਰ symbv_s obj;
		  symbv_init(&obj);
		  symbv_ਜੋੜੋ(&obj, $1);
		  $$ = obj;
		}
	| symbols_net ',' ਚਿੰਨ੍ਹ_opt
		{ ਬਣਤਰ symbv_s obj = $1;
		  symbv_ਜੋੜੋ(&obj, $3);
		  $$ = obj;
		}
	;

  /* In some cases, simple pointer arithmetic is allowed. In
     particular, functor vectors can be indexed with the [] syntax,
     with values from 0 up. */

ਚਿੰਨ੍ਹ
	: T_ਚਿੰਨ੍ਹ
		{ $$.ਪਾਠ = $1;
		  $$.idx = 0;
		}
	;

ਚਿੰਨ੍ਹ_opt
	: ਚਿੰਨ੍ਹ
		{ $$ = $1; }
	|
		{ $$.ਪਾਠ = 0;
		  $$.idx = 0;
		}
	;

  /* This rule is invoked within the rule for a modpath statement. The
     beginning of that run has already created the modpath dst object
     and saved it in the modpath_dst variable. The modpath_src rule,
     then simply needs to attach the items it creates. */
modpath_src_list
        : modpath_src
        | modpath_src_list ',' modpath_src
        ;

ਪੋਰਟ_type
        : K_ਪੋਰਟ_ਨਿਵੇਸ਼ { $$ = vpi_ਨਿਵੇਸ਼; }
        | K_ਪੋਰਟ_OUTPUT { $$ = vpiOutput; }
        | K_ਪੋਰਟ_INOUT  { $$ = vpiInout; }
        | K_ਪੋਰਟ_MIXED  { $$ = vpiMixedIO; }
        | K_ਪੋਰਟ_NODIR  { $$ = vpiNoDirection; }
        ;

modpath_src
  : ਚਿੰਨ੍ਹ '(' numbers ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, 0, $1, $3, 0, $5, false); }
  | ਚਿੰਨ੍ਹ '(' numbers '?' ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, 0, $1, $3, 0, $6, true); }
  | ਚਿੰਨ੍ਹ '(' numbers '?' ਚਿੰਨ੍ਹ ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, 0, $1, $3, $5, $7); }
  | ਚਿੰਨ੍ਹ '+' '(' numbers ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, '+', $1, $4, 0, $6, false); }
  | ਚਿੰਨ੍ਹ '+' '(' numbers '?' ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, '+', $1, $4, 0, $7, true); }
  | ਚਿੰਨ੍ਹ '+' '(' numbers '?' ਚਿੰਨ੍ਹ ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, '+', $1, $4, $6, $8); }
  | ਚਿੰਨ੍ਹ '-' '(' numbers ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, '-', $1, $4, 0, $6, false); }
  | ਚਿੰਨ੍ਹ '-' '(' numbers '?' ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, '-', $1, $4, 0, $7, true); }
  | ਚਿੰਨ੍ਹ '-' '(' numbers '?' ਚਿੰਨ੍ਹ ')' ਚਿੰਨ੍ਹ
      { compile_modpath_src(modpath_dst, '-', $1, $4, $6, $8); }
  ;

udp_ਸਾਰਣੀ
	: T_STRING
		{ $$ = compile_udp_ਸਾਰਣੀ(0x0, $1); }
	| udp_ਸਾਰਣੀ ',' T_STRING
		{ $$ = compile_udp_ਸਾਰਣੀ($1,  $3); }
	;

signed_t_ਸੰਖਿਆ
	: T_ਸੰਖਿਆ     { $$ = $1; }
	| '-' T_ਸੰਖਿਆ { $$ = -$2; }
	;

delay
	: '(' T_ਸੰਖਿਆ ')'
		{ $$ = new vvp_delay_t($2, $2); }
	| '(' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ')'
		{ $$ = new vvp_delay_t($2, $4); }
	| '(' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ',' T_ਸੰਖਿਆ ')'
		{ $$ = new vvp_delay_t($2, $4, $6); }
	;

%%

ਪੂਰਨ compile_design(ਅਚਲ ਅੱਖਰ*ਰਸਤਾ)
{
      yypath = ਰਸਤਾ;
      yyline = 1;
      yyin = fopen(ਰਸਤਾ, "r");
      ਜੇ (yyin == 0) {
	    fprintf(stderr, "%s: Unable to open input file.\n", ਰਸਤਾ);
	    ਵਾਪਸ -1;
      }

      ਪੂਰਨ rc = yyparse();
      fclose(yyin);
      ਵਾਪਸ rc;
}
