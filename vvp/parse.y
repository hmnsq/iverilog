
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
خارجی FILE*yyin;

سمتیہ <ثابت حرف*> file_نام;

/*
 * Local variables.
 */

/*
 * When parsing a modpath list, this is the processed destination that
 * the source items will attach themselves to.
 */
ساکن ساخت __vpiModPath*modpath_dst = 0;
%}

%اتحاد {
      حرف*متن;
      حرف **جدول;
      uint64_t numb;
      bool flag;

      comp_operands_t opa;

      ساخت symb_s  symb;
      ساخت symbv_s symbv;

      ساخت numbv_s numbv;

      ساخت enum_name_s enum_name;
      std::list<ساخت enum_name_s>*enum_namev;

      ساخت symb_s vect;

      ساخت argv_s argv;
      vpiHandle vpi;

      vvp_delay_t*cdelay;

      صحیح      vpi_enum;
};

%token K_A K_APV
%token K_ARITH_ABS K_ARITH_DIV K_ARITH_DIV_R K_ARITH_DIV_S K_ARITH_MOD
%token K_ARITH_MOD_R K_ARITH_MOD_S
%token K_ARITH_MULT K_ARITH_MULT_R K_ARITH_SUB K_ARITH_SUB_R
%token K_ARITH_مجموع K_ARITH_مجموع_R K_ARITH_POW K_ARITH_POW_R K_ARITH_POW_S
%token K_ARRAY K_ARRAY_2U K_ARRAY_2S K_ARRAY_I K_ARRAY_OBJ K_ARRAY_R K_ARRAY_S K_ARRAY_STR K_ARRAY_پورٹ
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
%token K_PART_V K_PART_V_S K_پورٹ K_پورٹ_INFO K_PQ K_PV K_REDUCE_AND K_REDUCE_OR K_REDUCE_XOR
%token K_REDUCE_NAND K_REDUCE_NOR K_REDUCE_XNOR K_REPEAT
%token K_RESOLV K_RTRAN K_RTRANIF0 K_RTRANIF1
%token K_SCOPE K_SFUNC K_SFUNC_E K_SHIFTL K_SHIFTR K_SHIFTRS
%token K_SUBSTITUTE
%token K_THREAD K_TIMESCALE K_TRAN K_TRANIF0 K_TRANIF1 K_TRANVP
%token K_UFUNC_REAL K_UFUNC_VEC4 K_UFUNC_E K_UDP K_UDP_C K_UDP_S
%token K_VAR K_VAR_COBJECT K_VAR_DARRAY
%token K_VAR_قطار
%token K_VAR_S K_VAR_STR K_VAR_I K_VAR_R K_VAR_2S K_VAR_2U
%token K_vpi_call K_vpi_call_w K_vpi_call_i
%token K_vpi_func K_vpi_func_r K_vpi_func_s
%token K_ivl_نسخہ K_ivl_delay_selection
%token K_vpi_module K_vpi_وقت_precision K_file_نام K_file_سطر
%token K_پورٹ_داخلہ K_پورٹ_OUTPUT K_پورٹ_INOUT K_پورٹ_MIXED K_پورٹ_NODIR

%token <متن> T_INSTR
%token <متن> T_LABEL
%token <numb> T_نمبر
%token <متن> T_STRING
%token <متن> T_علامت
%token <vect> T_سمتیہ

%قسم <flag>  local_flag
%قسم <vpi_enum> پورٹ_type
%قسم <numb>  signed_t_نمبر
%قسم <numb>  dimension dimensions dimensions_opt
%قسم <symb>  علامت علامت_opt
%قسم <symbv> symbols symbols_net
%قسم <numbv> numbers
%قسم <متن> label_opt
%قسم <opa>  عملیہ operands operands_opt
%قسم <جدول> udp_جدول

%قسم <argv> دلیل_opt دلیل_list
%قسم <vpi>  دلیل علامت_access
%قسم <cdelay> delay

%قسم <enum_name> enum_type_name
%قسم <enum_namev> enum_type_نام

%%

ماخذ_file : سرخی_سطریں_opt پروگرام footer_سطریں;

سرخی_سطریں_opt : سرخی_سطریں | ;


سرخی_سطریں
	: سرخی_سطر
	| سرخی_سطریں سرخی_سطر
	;

سرخی_سطر
	: K_ivl_نسخہ T_STRING ';'
		{ verify_نسخہ($2, تہی); }
	| K_ivl_نسخہ T_STRING T_STRING ';'
		{ verify_نسخہ($2, $3); }
	| K_ivl_delay_selection T_STRING ';'
		{ سیٹ_delay_selection($2); }
	| K_vpi_module T_STRING ';'
		{ compile_بھرو_vpi_module($2); }
	| K_vpi_وقت_precision '+' T_نمبر ';'
		{ compile_vpi_وقت_precision($3); }
	| K_vpi_وقت_precision '-' T_نمبر ';'
		{ compile_vpi_وقت_precision(-$3); }
	;

footer_سطریں
	: K_file_نام T_نمبر ';' { file_نام.reserve($2); }
	  name_لڑیاں
	;

name_لڑیاں
	: T_STRING ';'
		{ file_نام.ڈالو_عقب($1); }
	| name_لڑیاں T_STRING ';'
		{ file_نام.ڈالو_عقب($2); }
	;

  /* A program is simply a list of statements. No other structure. */
پروگرام
	: statement
	| پروگرام statement
	;


  /* A statement can be any of the following. In all cases, the
     statement is terminated by a semi-colon. In general, a statement
     has a label, an opcode of some source, and operands. The
     structure of the operands depends on the opcode. */

statement

  /* Functor statements define functors. The functor must have a
     label and a type name, and may have operands. The functor may
     also have a delay specification and output strengths. */

	: T_LABEL K_FUNCTOR T_علامت T_نمبر ',' symbols ';'
		{ compile_functor($1, $3, $4, 6, 6, $6.cnt, $6.vect); }

	| T_LABEL K_FUNCTOR T_علامت T_نمبر
	          '[' T_نمبر T_نمبر ']' ',' symbols ';'
		{ غیرعلامتی str0 = $6;
		  غیرعلامتی str1 = $7;
		  compile_functor($1, $3, $4, str0, str1,
				  $10.cnt, $10.vect);
		}


  /* UDP statements define or instantiate UDPs.  Definitions take a
     label (UDP type id) a name (string), the number of inputs, and
     for sequential UDPs the initial value. */

	| T_LABEL K_UDP_S T_STRING ',' T_نمبر ',' T_نمبر ',' udp_جدول ';'
		{ compile_udp_def(1, $1, $3, $5, $7, $9); }

	| T_LABEL K_UDP_C T_STRING ',' T_نمبر ',' udp_جدول ';'
		{ compile_udp_def(0, $1, $3, $5, 0, $7); }

	| T_LABEL K_UDP T_علامت ',' symbols ';'
		{ compile_udp_functor($1, $3, $5.cnt, $5.vect); }


  /* Memory.  Definition, port, initialization */

        | T_LABEL K_ARRAY T_STRING ',' signed_t_نمبر signed_t_نمبر ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_var_array($1, $3, $5, $6, $8, $9, 0); }

        | T_LABEL K_ARRAY_2U T_STRING ',' signed_t_نمبر signed_t_نمبر ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_var2_array($1, $3, $5, $6, $8, $9, false); }
        | T_LABEL K_ARRAY_2S T_STRING ',' signed_t_نمبر signed_t_نمبر ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_var2_array($1, $3, $5, $6, $8, $9, true); }

        | T_LABEL K_ARRAY_I T_STRING ',' signed_t_نمبر signed_t_نمبر ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_var_array($1, $3, $5, $6, $8, $9, 2); }

        | T_LABEL K_ARRAY_R T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_real_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY_S T_STRING ',' signed_t_نمبر signed_t_نمبر ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_var_array($1, $3, $5, $6, $8, $9, 1); }

        | T_LABEL K_ARRAY_STR T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_string_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY_OBJ T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_object_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
                { compile_net_array($1, $3, $5, $6); }

        | T_LABEL K_ARRAY_پورٹ T_علامت ',' T_علامت ';'
		{ compile_array_پورٹ($1, $3, $5); }

        | T_LABEL K_ARRAY_پورٹ T_علامت ',' T_نمبر ';'
		{ compile_array_پورٹ($1, $3, $5); }

        | T_LABEL K_ARRAY T_STRING ',' T_علامت ';'
                { compile_array_alias($1, $3, $5); }

 /* The .ufunc functors are for implementing user defined functions, or
     other thread code that is automatically invoked if any of the
     bits in the symbols list change. */

  | T_LABEL K_UFUNC_REAL T_علامت ',' T_نمبر ',' T_علامت ';'
      { compile_ufunc_real($1, $3, $5, 0, 0, 0, 0, $7, strdup("E_0x0")); }
  | T_LABEL K_UFUNC_REAL T_علامت ',' T_نمبر ',' symbols '(' symbols ')' T_علامت ';'
      { compile_ufunc_real($1, $3, $5, $7.cnt, $7.vect, $9.cnt, $9.vect, $11, 0); }

  | T_LABEL K_UFUNC_VEC4 T_علامت ',' T_نمبر ',' T_علامت ';'
      { compile_ufunc_vec4($1, $3, $5, 0, 0, 0, 0, $7, strdup("E_0x0")); }
  | T_LABEL K_UFUNC_VEC4 T_علامت ',' T_نمبر ',' symbols '(' symbols ')' T_علامت ';'
      { compile_ufunc_vec4($1, $3, $5, $7.cnt, $7.vect, $9.cnt, $9.vect, $11, 0); }

  | T_LABEL K_UFUNC_E T_علامت ',' T_نمبر ',' T_علامت ',' symbols '(' symbols ')' T_علامت ';'
      { compile_ufunc_vec4($1, $3, $5, $9.cnt, $9.vect, $11.cnt, $11.vect, $13, $7); }

  /* Resolver statements are very much like functors. They are
     compiled to functors of a different mode. */

	| T_LABEL K_RESOLV T_علامت ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_resolver($1, $3, obj.cnt, obj.vect);
		}

  /* Part select statements take a single netlist input, and numbers
     that define the part to be selected out of the input. */

	| T_LABEL K_PART T_علامت ',' T_نمبر ',' T_نمبر ';'
		{ compile_part_select($1, $3, $5, $7); }

	| T_LABEL K_PART_PV T_علامت ',' T_نمبر ',' T_نمبر ',' T_نمبر ';'
		{ compile_part_select_pv($1, $3, $5, $7, $9); }

	| T_LABEL K_PART_V T_علامت ',' T_علامت ',' T_نمبر ';'
		{ compile_part_select_var($1, $3, $5, $7, false); }
	| T_LABEL K_PART_V_S T_علامت ',' T_علامت ',' T_نمبر ';'
		{ compile_part_select_var($1, $3, $5, $7, true); }

  /* Concatenations */

  | T_LABEL K_CONCAT '[' T_نمبر T_نمبر T_نمبر T_نمبر ']' ','
    symbols ';'
      { compile_concat($1, $4, $5, $6, $7, $10.cnt, $10.vect); }

  | T_LABEL K_CONCAT8 '[' T_نمبر T_نمبر T_نمبر T_نمبر ']' ','
    symbols ';'
      { compile_concat8($1, $4, $5, $6, $7, $10.cnt, $10.vect); }

  /* Substitutions (similar to concatenations) */
  | T_LABEL K_SUBSTITUTE T_نمبر ',' T_نمبر T_نمبر ',' symbols ';'
      { compile_substitute($1, $3, $5, $6, $8.cnt, $8.vect); }

  /* The ABS statement is a special arithmetic node that takes 1
     input. Re-use the symbols rule. */

        | T_LABEL K_ARITH_ABS symbols ';'
		{ ساخت symbv_s obj = $3;
		  compile_arith_abs($1, obj.cnt, obj.vect);
		}

  | T_LABEL K_CAST_INT T_نمبر ',' symbols ';'
      { ساخت symbv_s obj = $5;
	compile_arith_cast_int($1, $3, obj.cnt, obj.vect);
      }

  | T_LABEL K_CAST_REAL symbols ';'
      { ساخت symbv_s obj = $3;
	compile_arith_cast_real($1, false, obj.cnt, obj.vect);
      }

  | T_LABEL K_CAST_REAL_S symbols ';'
      { ساخت symbv_s obj = $3;
	compile_arith_cast_real($1, true, obj.cnt, obj.vect);
      }

  | T_LABEL K_CAST_2 T_نمبر ',' symbols ';'
      { ساخت symbv_s obj = $5;
	compile_arith_cast_vec2($1, $3, obj.cnt, obj.vect);
      }

  /* Arithmetic statements generate functor arrays of a given width
     that take like size input vectors. */

	| T_LABEL K_ARITH_DIV T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_div($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_DIV_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_div_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_DIV_S T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_div($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MOD T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_mod($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MOD_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_mod_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MOD_S T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_mod($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MULT T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_mult($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_MULT_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_mult_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_POW T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_pow($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_POW_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_pow_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_POW_S T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_pow($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_SUB T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_sub($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_SUB_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_sub_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_مجموع T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_مجموع($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_ARITH_مجموع_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_arith_مجموع_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EEQ T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_eeq($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_NEE T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_nee($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQ T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_eq($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQX T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_eqx($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQZ T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_eqz($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_EQ_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_eq_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_NE T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_ne($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_NE_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_ne_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GE T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_ge($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GE_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_ge_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GE_S T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_ge($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GT T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_gt($1, $3, false, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GT_R T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_gt_r($1, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_GT_S T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_gt($1, $3, true, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_WEQ T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_weq($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_CMP_WNE T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_cmp_wne($1, $3, obj.cnt, obj.vect);
		}

  /* Delay nodes take a set of numbers or a set of inputs. The delay
     node takes two form, one with an array of constants and a single
     input, and another with an array of inputs. */

 | T_LABEL K_DELAY T_نمبر delay علامت ';'
    { compile_delay($1, $3, $4, $5); }
 | T_LABEL K_DELAY T_نمبر symbols ';'
    { ساخت symbv_s obj = $4;
      compile_delay($1, $3, obj.cnt, obj.vect, false);
    }
 | T_LABEL K_DELAY T_نمبر symbols ',' T_نمبر ';'
    { ساخت symbv_s obj = $4;
      اگر ($6 != 0) أكد(0);
      compile_delay($1, $3, obj.cnt, obj.vect, true);
    }

 | T_LABEL K_MODPATH T_نمبر علامت علامت ','
    { modpath_dst = compile_modpath($1, $3, $4, $5); }
   modpath_src_list ';'
    { modpath_dst = 0; }

  /* DFF nodes have an output and take up to 4 inputs. */

  | T_LABEL K_DFF_N T_نمبر علامت ',' علامت ',' علامت ';'
      { compile_dff($1, $3, true, $4, $6, $8); }

  | T_LABEL K_DFF_P T_نمبر علامت ',' علامت ',' علامت ';'
      { compile_dff($1, $3, false, $4, $6, $8); }

  | T_LABEL K_DFF_N_ACLR T_نمبر علامت ',' علامت ',' علامت ',' علامت ';'
      { compile_dff_aclr($1, $3, true, $4, $6, $8, $10); }

  | T_LABEL K_DFF_P_ACLR T_نمبر علامت ',' علامت ',' علامت ',' علامت ';'
      { compile_dff_aclr($1, $3, false, $4, $6, $8, $10); }

  | T_LABEL K_DFF_N_ASET T_نمبر علامت ',' علامت ',' علامت ',' علامت ';'
      { compile_dff_aset($1, $3, true, $4, $6, $8, $10, 0); }

  | T_LABEL K_DFF_P_ASET T_نمبر علامت ',' علامت ',' علامت ',' علامت ';'
      { compile_dff_aset($1, $3, false, $4, $6, $8, $10, 0); }

  | T_LABEL K_DFF_N_ASET T_نمبر علامت ',' علامت ',' علامت ',' علامت ',' T_علامت ';'
      { compile_dff_aset($1, $3, true, $4, $6, $8, $10, $12); }

  | T_LABEL K_DFF_P_ASET T_نمبر علامت ',' علامت ',' علامت ',' علامت ',' T_علامت ';'
      { compile_dff_aset($1, $3, false, $4, $6, $8, $10, $12); }

  /* LATCH nodes have an output and take 2 inputs. */

  | T_LABEL K_LATCH T_نمبر علامت ',' علامت ';'
      { compile_latch($1, $3, $4, $6); }

  /* The various reduction operator nodes take a single input. */

        | T_LABEL K_REDUCE_AND علامت ';'
                { compile_reduce_and($1, $3); }

        | T_LABEL K_REDUCE_OR علامت ';'
                { compile_reduce_or($1, $3); }

        | T_LABEL K_REDUCE_XOR علامت ';'
                { compile_reduce_xor($1, $3); }

        | T_LABEL K_REDUCE_NAND علامت ';'
                { compile_reduce_nand($1, $3); }

        | T_LABEL K_REDUCE_NOR علامت ';'
                { compile_reduce_nor($1, $3); }

        | T_LABEL K_REDUCE_XNOR علامت ';'
                { compile_reduce_xnor($1, $3); }

        | T_LABEL K_REPEAT T_نمبر ',' T_نمبر ',' علامت ';'
                { compile_repeat($1, $3, $5, $7); }

  /* The extend nodes take a width and a symbol. */

        | T_LABEL K_EXTEND_S T_نمبر ',' علامت ';'
                { compile_extend_signed($1, $3, $5); }

  /* System function call */
        | T_LABEL K_SFUNC T_نمبر T_نمبر T_STRING ','
          T_STRING ',' symbols ';'
                { compile_sfunc($1, $5, $7, $3, $4, $9.cnt, $9.vect, 0); }

        | T_LABEL K_SFUNC_E T_نمبر T_نمبر T_STRING ',' T_علامت ','
          T_STRING ',' symbols ';'
                { compile_sfunc($1, $5, $9, $3, $4, $11.cnt, $11.vect, $7); }

  /* System function call - no arguments */
        | T_LABEL K_SFUNC T_نمبر T_نمبر T_STRING ','
          T_STRING ';'
                { compile_sfunc($1, $5, $7, $3, $4, 0, 0, 0); }

        | T_LABEL K_SFUNC_E T_نمبر T_نمبر T_STRING ',' T_علامت ','
          T_STRING ';'
                { compile_sfunc($1, $5, $9, $3, $4, 0, 0, $7); }

  /* Shift nodes. */

	| T_LABEL K_SHIFTL T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		  compile_shiftl($1, $3, obj.cnt, obj.vect);
		}

	| T_LABEL K_SHIFTR T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		      compile_shiftr($1, $3, false, obj.cnt, obj.vect);
		}


	| T_LABEL K_SHIFTRS T_نمبر ',' symbols ';'
		{ ساخت symbv_s obj = $5;
		      compile_shiftr($1, $3, true, obj.cnt, obj.vect);
		}


  /* Event statements take a label, a type (the first T_SYMBOL) and a
     list of inputs. If the type is instead a string, then we have a
     named event instead. */

	| T_LABEL K_EVENT T_علامت ',' symbols ';'
                { compile_event($1, $3, $5.cnt, $5.vect); }

	| T_LABEL K_EVENT K_DEBUG T_علامت ',' symbols ';'
                { compile_event($1, $4, $6.cnt, $6.vect); }

	| T_LABEL K_EVENT T_STRING ';'
		{ compile_named_event($1, $3); }

	| T_LABEL K_EVENT_OR symbols ';'
                { compile_event($1, 0, $3.cnt, $3.vect); }


  /* Instructions may have a label, and have zero or more
     operands. The meaning of and restrictions on the operands depends
     on the specific instruction. */

	| label_opt T_INSTR operands_opt ';'
		{ compile_رمز($1, $2, $3); }

	| T_LABEL ';'
		{ compile_codelabel($1); }

  /* %file_line statements are instructions that have unusual operand
     requirements so are handled by their own rules. */
	| label_opt K_file_سطر T_نمبر T_نمبر T_STRING ';'
		{ compile_file_سطر($1, $3, $4, $5); }

	| label_opt K_file_سطر T_نمبر T_نمبر T_نمبر ';'
		{ أكد($5 == 0);
		  compile_file_سطر($1, $3, $4, 0); }

  /* %vpi_call statements are instructions that have unusual operand
     requirements so are handled by their own rules. The %vpi_func
     statement is a variant of %vpi_call that includes a thread vector
     after the name, and is used for function calls. */

  /* This version does not allow a function to be called as a task. */
  | label_opt K_vpi_call T_نمبر T_نمبر T_STRING
    دلیل_opt '{' T_نمبر T_نمبر T_نمبر '}' ';'
      { compile_vpi_call($1, $5, true, false, $3, $4,
			 $6.argc, $6.argv, $8, $9, $10); }

  /* This version allows a function to be called as a task, but prints a
   * warning message. */
  | label_opt K_vpi_call_w T_نمبر T_نمبر T_STRING
    دلیل_opt '{' T_نمبر T_نمبر T_نمبر '}' ';'
      { compile_vpi_call($1, $5, false, true, $3, $4,
			 $6.argc, $6.argv, $8, $9, $10); }

  /* This version allows a function to be called as a task and does not
   * print a message. */
  | label_opt K_vpi_call_i T_نمبر T_نمبر T_STRING
    دلیل_opt '{' T_نمبر T_نمبر T_نمبر '}' ';'
      { compile_vpi_call($1, $5, false, false, $3, $4,
			 $6.argc, $6.argv, $8, $9, $10); }

  | label_opt K_vpi_func T_نمبر T_نمبر T_STRING T_نمبر
    دلیل_opt  '{' T_نمبر T_نمبر T_نمبر '}' ';'
      { compile_vpi_func_call($1, $5, -vpiVectorVal, $6, $3, $4,
			      $7.argc, $7.argv, $9, $10, $11); }

  | label_opt K_vpi_func_r T_نمبر T_نمبر T_STRING
    دلیل_opt '{' T_نمبر T_نمبر T_نمبر '}' ';'
      { compile_vpi_func_call($1, $5, -vpiRealVal, 0, $3, $4,
			      $6.argc, $6.argv, $8, $9, $10); }

  | label_opt K_vpi_func_s T_نمبر T_نمبر T_STRING
    دلیل_opt '{' T_نمبر T_نمبر T_نمبر '}' ';'
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

	| T_LABEL K_SCOPE T_علامت ',' T_STRING T_STRING T_نمبر T_نمبر ';'
		{ compile_scope_decl($1, $3, $5, $6, 0, $7, $8, $7, $8, 0); }

	| T_LABEL K_SCOPE T_علامت ',' T_STRING T_STRING T_نمبر T_نمبر ','
	  T_نمبر T_نمبر T_نمبر ',' T_علامت ';'
		{ compile_scope_decl($1, $3, $5, $6, $14, $7, $8, $10, $11, $12); }

  /* Scope recall has no label of its own, but refers by label to a
     declared scope. */

	|         K_SCOPE T_علامت ';'
		{ compile_scope_recall($2); }


  /* Port information for scopes... currently this is just meta-data for VPI queries */

	| K_پورٹ_INFO T_نمبر پورٹ_type T_نمبر T_STRING T_علامت ';'
		{ compile_پورٹ_info( $2 /* port_index */, $3, $4 /*چوڑائی*/,
		                     $5 /*&name */, $6 /*بفر*/ ); }

	| K_پورٹ_INFO T_نمبر پورٹ_type T_نمبر T_STRING ';'
		{ compile_پورٹ_info( $2 /* port_index */, $3, $4 /*چوڑائی*/,
		                     $5 /*&name */, nullptr /*بفر*/ ); }

	|         K_TIMESCALE T_نمبر T_نمبر';'
		{ compile_timescale($2, $3); }
	|         K_TIMESCALE '-' T_نمبر T_نمبر';'
		{ compile_timescale(-$3, $4); }
	|         K_TIMESCALE T_نمبر '-' T_نمبر';'
		{ compile_timescale($2, -$4); }
	|         K_TIMESCALE '-' T_نمبر '-' T_نمبر';'
		{ compile_timescale(-$3, -$5); }

  /* Thread statements declare a thread with its starting address. The
     starting address must already be defined. The .thread statement
     may also take an optional flag word. */

	|         K_THREAD T_علامت ';'
		{ compile_thread($2, 0); }

	|         K_THREAD T_علامت ',' T_علامت ';'
		{ compile_thread($2, $4); }

  /* Var statements declare a bit of a variable. This also implicitly
     creates a functor with the same name that acts as the output of
     the variable in the netlist. */

  | T_LABEL K_VAR local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
      { compile_variable($1, $4, $6, $7, vpiLogicVar, false, $3); }

  | T_LABEL K_VAR_S local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
      { compile_variable($1, $4, $6, $7, vpiLogicVar, true, $3); }

  | T_LABEL K_VAR_I local_flag T_STRING ',' T_نمبر T_نمبر ';'
      { compile_variable($1, $4, $6, $7, vpiIntegerVar, true, $3); }

  | T_LABEL K_VAR_2S local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
      { compile_variable($1, $4, $6, $7, vpiIntVar, true, $3); }

  | T_LABEL K_VAR_2U local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
      { compile_variable($1, $4, $6, $7, vpiIntVar, false, $3); }

  | T_LABEL K_VAR_R local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر ';'
      { compile_var_real($1, $4, $3); }

  | T_LABEL K_VAR_STR T_STRING ';'
      { compile_var_string($1, $3); }

  | T_LABEL K_VAR_DARRAY T_STRING ',' T_نمبر ';'
      { compile_var_darray($1, $3, $5); }

  | T_LABEL K_VAR_قطار T_STRING  ',' T_نمبر';'
      { compile_var_قطار($1, $3, $5); }

  | T_LABEL K_VAR_COBJECT T_STRING ';'
      { compile_var_cobject($1, $3); }

  /* Net statements are similar to .var statements, except that they
     declare nets, and they have an input list. */

  | T_LABEL K_NET local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiLogicVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_S local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiLogicVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2U local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiIntVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2S local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, vpiIntVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8 local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_2U local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, false, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_S local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_2S local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net($1, $4, $6, $7, -vpiLogicVar, true, $3, $9.cnt, $9.vect); }

  | T_LABEL K_NET_R local_flag T_STRING ',' signed_t_نمبر signed_t_نمبر
    ',' symbols_net ';'
      { compile_net_real($1, $4, $6, $7, $3, $9.cnt, $9.vect); }

  /* Arrayed versions of net directives. */

  | T_LABEL K_NET T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiLogicVar, false, $9.cnt, $9.vect); }

  | T_LABEL K_NET_S T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiLogicVar, true, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2U T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiIntVar, false, $9.cnt, $9.vect); }

  | T_LABEL K_NET_2S T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, vpiIntVar, true, $9.cnt, $9.vect); }

  | T_LABEL K_NET8 T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, -vpiLogicVar, false, $9.cnt, $9.vect); }

  | T_LABEL K_NET8_S T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw($1, $3, $4, $6, $7, -vpiLogicVar, true, $9.cnt, $9.vect); }

  | T_LABEL K_NET_R T_علامت T_نمبر ',' signed_t_نمبر signed_t_نمبر ','
    symbols_net ';'
      { compile_netw_real($1, $3, $4, $6, $7, $9.cnt, $9.vect); }

  /* Parameter statements come in a few simple forms. The most basic
     is the string parameter. */

	| T_LABEL K_PARAM_STR T_STRING T_نمبر T_نمبر T_نمبر',' T_STRING ';'
		{ compile_param_string($1, $3, $8, $4, $5, $6); }

	| T_LABEL K_PARAM_L T_STRING T_نمبر T_نمبر T_نمبر',' T_علامت ';'
		{ compile_param_logic($1, $3, $8, false, $4, $5, $6); }

	| T_LABEL K_PARAM_L T_STRING T_نمبر T_نمبر T_نمبر',' '+' T_علامت ';'
		{ compile_param_logic($1, $3, $9, true, $4, $5, $6 ); }

	| T_LABEL K_PARAM_REAL T_STRING T_نمبر T_نمبر T_نمبر',' T_علامت ';'
		{ compile_param_real($1, $3, $8, $4, $5, $6); }

  /* Islands */

  | T_LABEL K_ISLAND T_علامت ';'
      { compile_island($1, $3); }

  | T_LABEL K_پورٹ T_علامت ',' T_علامت ';'
      { compile_island_پورٹ($1, $3, $5); }

  | T_LABEL K_IMPORT T_علامت ',' T_علامت ';'
      { compile_island_import($1, $3, $5); }

  | T_LABEL K_EXPORT T_علامت ';'
      { compile_island_export($1, $3); }

  | K_RTRAN T_علامت ',' T_علامت T_علامت ';'
      { compile_island_tranif(0, $2, $4, $5, 0, 1); }

  | K_RTRANIF0 T_علامت ',' T_علامت T_علامت ',' T_علامت ';'
      { compile_island_tranif(0, $2, $4, $5, $7, 1); }

  | K_RTRANIF1 T_علامت ',' T_علامت T_علامت ',' T_علامت ';'
      { compile_island_tranif(1, $2, $4, $5, $7, 1); }

  | K_TRAN T_علامت ',' T_علامت T_علامت ';'
      { compile_island_tranif(0, $2, $4, $5, 0, 0); }

  | K_TRANIF0 T_علامت ',' T_علامت T_علامت ',' T_علامت ';'
      { compile_island_tranif(0, $2, $4, $5, $7, 0); }

  | K_TRANIF1 T_علامت ',' T_علامت T_علامت ',' T_علامت ';'
      { compile_island_tranif(1, $2, $4, $5, $7, 0); }

  | K_TRANVP T_نمبر T_نمبر T_نمبر ',' T_علامت ',' T_علامت T_علامت ';'
      { compile_island_tranvp($6, $8, $9, $2, $3, $4); }

  /* Other statements */

  | T_LABEL K_CLASS T_STRING '[' T_نمبر ']'
      { compile_class_آغاز($1, $3, $5); }
    class_properties_opt ';'
      { compile_class_مکمل(); }

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
  : T_نمبر ':' T_STRING ',' T_STRING dimensions_opt
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
  : '[' T_نمبر ':' T_نمبر ']'
      { $$ = ($2 > $4? $2 - $4 : $4 - $2) + 1; }
  ;

  /* Enumeration types */
enum_type
  : T_LABEL K_ENUM2 '(' T_نمبر ')' enum_type_نام ';'
      { compile_enum2_type($1, $4, false, $6); }
  | T_LABEL K_ENUM2_S '(' T_نمبر ')' enum_type_نام ';'
      { compile_enum2_type($1, $4, true, $6); }
  | T_LABEL K_ENUM4 '(' T_نمبر ')' enum_type_نام ';'
      { compile_enum4_type($1, $4, false, $6); }
  | T_LABEL K_ENUM4_S '(' T_نمبر ')' enum_type_نام ';'
      { compile_enum4_type($1, $4, true, $6); }
  ;

enum_type_نام
  : enum_type_name
      { list<ساخت enum_name_s>*tmp = new list<ساخت enum_name_s>;
	tmp->ڈالو_عقب($1);
	$$ = tmp;
      }
  | enum_type_نام ',' enum_type_name
      { list<ساخت enum_name_s>*tmp = $1;
	tmp->ڈالو_عقب($3);
	$$ = tmp;
      }
  ;

enum_type_name
  : T_STRING T_نمبر
      { $$.متن = $1; $$.val2 = $2; $$.val4 = 0; }
  | T_STRING T_سمتیہ
      { $$.متن = $1; $$.val2 = 0; $$.val4 = $2.متن; }
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
	: operands ',' عملیہ
		{ comp_operands_t opa = $1;
		  أكد(opa->argc < 3);
		  أكد($3->argc == 1);
		  opa->argv[opa->argc] = $3->argv[0];
		  opa->argc += 1;
		  آزاد($3);
		  $$ =  opa;
		}
	| عملیہ
		{ $$ = $1; }
	;

عملیہ
  : علامت
      { comp_operands_t opa = static_cast<comp_operands_t>
                                         (صفر_حافظہ_دو(1, حجم(ساخت comp_operands_s)));
	opa->argc = 1;
	opa->argv[0].ltype = L_SYMB;
	opa->argv[0].symb = $1;
	$$ = opa;
      }
  | T_نمبر
      { comp_operands_t opa = static_cast<comp_operands_t>
                                         (صفر_حافظہ_دو(1, حجم(ساخت comp_operands_s)));
	opa->argc = 1;
	opa->argv[0].ltype = L_NUMB;
	opa->argv[0].numb = $1;
	$$ = opa;
      }
  | T_STRING
      { comp_operands_t opa = static_cast<comp_operands_t>
                                         (صفر_حافظہ_دو(1, حجم(ساخت comp_operands_s)));
	opa->argc = 1;
	opa->argv[0].ltype = L_STRING;
	opa->argv[0].متن = $1;
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

دلیل_opt
	: ',' دلیل_list
		{
		  argv_sym_lookup(&$2);
		  $$ = $2;
		}
	| /*خالی_خانے*/
		{ ساخت argv_s tmp;
		  argv_init(&tmp);
		  $$ = tmp;
		}
	;

دلیل_list
  : دلیل
      { ساخت argv_s tmp;
	argv_init(&tmp);
	argv_جوڑو(&tmp, $1);
	$$ = tmp;
      }
  | دلیل_list ',' دلیل
      { ساخت argv_s tmp = $1;
	argv_جوڑو(&tmp, $3);
	$$ = tmp;
      }
  | T_علامت
      { ساخت argv_s tmp;
	argv_init(&tmp);
	argv_sym_جوڑو(&tmp, $1);
	$$ = tmp;
      }
  | دلیل_list ',' T_علامت
      { ساخت argv_s tmp = $1;
	argv_sym_جوڑو(&tmp, $3);
	$$ = tmp;
      }
  ;

دلیل
  : T_STRING
      { $$ = vpip_make_string_const($1); }
  | T_سمتیہ
      { $$ = vpip_make_ثنائی_const($1.idx, $1.متن);
	آزاد($1.متن);
      }
  | علامت_access
      { $$ = $1; }
  ;

علامت_access
  : K_A '<' T_علامت ',' T_نمبر '>'
      { $$ = vpip_make_vthr_A($3, $5); }
  | K_A '<' T_علامت ',' T_علامت '>'
      { $$ = vpip_make_vthr_A($3, $5); }
  | K_A '<' T_علامت ',' علامت_access '>'
      { $$ = vpip_make_vthr_A($3, $5); }
  | K_PV '<' T_علامت ',' T_نمبر ',' T_نمبر '>'
      { $$ = vpip_make_PV($3, $5, $7); }
  | K_PV '<' T_علامت ',' '-' T_نمبر ',' T_نمبر '>'
      { $$ = vpip_make_PV($3, -$6, $8); }
  | K_PV '<' T_علامت ',' T_علامت ',' T_نمبر '>'
      { $$ = vpip_make_PV($3, $5, $7); }
  | K_PV '<' T_علامت ',' علامت_access ',' T_نمبر '>'
      { $$ = vpip_make_PV($3, $5, $7); }
  | K_APV '<' T_علامت ',' T_نمبر ',' T_نمبر ',' T_نمبر '>'
      { $$ = vpip_make_vthr_APV($3, $5, $7, $9); }
  | K_PQ '<' T_علامت ',' T_نمبر ',' T_نمبر '>'
      { $$ = vpip_make_prop_قطار_ref($3, $5, $7); }
  ;

  /* functor operands can only be a list of symbols. */
symbols
	: علامت
		{ ساخت symbv_s obj;
		  symbv_init(&obj);
		  symbv_جوڑو(&obj, $1);
		  $$ = obj;
		}
	| symbols ',' علامت
		{ ساخت symbv_s obj = $1;
		  symbv_جوڑو(&obj, $3);
		  $$ = obj;
		}
	;


numbers
	: T_نمبر
		{ ساخت numbv_s obj;
		  numbv_init(&obj);
		  numbv_جوڑو(&obj, $1);
		  $$ = obj;
		}
	| numbers ',' T_نمبر
		{ ساخت numbv_s obj = $1;
		  numbv_جوڑو(&obj, $3);
		  $$ = obj;
		}
	;


symbols_net
	: علامت_opt
		{ ساخت symbv_s obj;
		  symbv_init(&obj);
		  symbv_جوڑو(&obj, $1);
		  $$ = obj;
		}
	| symbols_net ',' علامت_opt
		{ ساخت symbv_s obj = $1;
		  symbv_جوڑو(&obj, $3);
		  $$ = obj;
		}
	;

  /* In some cases, simple pointer arithmetic is allowed. In
     particular, functor vectors can be indexed with the [] syntax,
     with values from 0 up. */

علامت
	: T_علامت
		{ $$.متن = $1;
		  $$.idx = 0;
		}
	;

علامت_opt
	: علامت
		{ $$ = $1; }
	|
		{ $$.متن = 0;
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

پورٹ_type
        : K_پورٹ_داخلہ { $$ = vpi_داخلہ; }
        | K_پورٹ_OUTPUT { $$ = vpiOutput; }
        | K_پورٹ_INOUT  { $$ = vpiInout; }
        | K_پورٹ_MIXED  { $$ = vpiMixedIO; }
        | K_پورٹ_NODIR  { $$ = vpiNoDirection; }
        ;

modpath_src
  : علامت '(' numbers ')' علامت
      { compile_modpath_src(modpath_dst, 0, $1, $3, 0, $5, false); }
  | علامت '(' numbers '?' ')' علامت
      { compile_modpath_src(modpath_dst, 0, $1, $3, 0, $6, true); }
  | علامت '(' numbers '?' علامت ')' علامت
      { compile_modpath_src(modpath_dst, 0, $1, $3, $5, $7); }
  | علامت '+' '(' numbers ')' علامت
      { compile_modpath_src(modpath_dst, '+', $1, $4, 0, $6, false); }
  | علامت '+' '(' numbers '?' ')' علامت
      { compile_modpath_src(modpath_dst, '+', $1, $4, 0, $7, true); }
  | علامت '+' '(' numbers '?' علامت ')' علامت
      { compile_modpath_src(modpath_dst, '+', $1, $4, $6, $8); }
  | علامت '-' '(' numbers ')' علامت
      { compile_modpath_src(modpath_dst, '-', $1, $4, 0, $6, false); }
  | علامت '-' '(' numbers '?' ')' علامت
      { compile_modpath_src(modpath_dst, '-', $1, $4, 0, $7, true); }
  | علامت '-' '(' numbers '?' علامت ')' علامت
      { compile_modpath_src(modpath_dst, '-', $1, $4, $6, $8); }
  ;

udp_جدول
	: T_STRING
		{ $$ = compile_udp_جدول(0x0, $1); }
	| udp_جدول ',' T_STRING
		{ $$ = compile_udp_جدول($1,  $3); }
	;

signed_t_نمبر
	: T_نمبر     { $$ = $1; }
	| '-' T_نمبر { $$ = -$2; }
	;

delay
	: '(' T_نمبر ')'
		{ $$ = new vvp_delay_t($2, $2); }
	| '(' T_نمبر ',' T_نمبر ')'
		{ $$ = new vvp_delay_t($2, $4); }
	| '(' T_نمبر ',' T_نمبر ',' T_نمبر ')'
		{ $$ = new vvp_delay_t($2, $4, $6); }
	;

%%

صحیح compile_design(ثابت حرف*راستہ)
{
      yypath = راستہ;
      yyline = 1;
      yyin = fopen(راستہ, "r");
      اگر (yyin == 0) {
	    fprintf(stderr, "%s: Unable to open input file.\n", راستہ);
	    واپس -1;
      }

      صحیح rc = yyparse();
      fclose(yyin);
      واپس rc;
}
