#ifndef IVL_compile_H
#define IVL_compile_H
/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  <cstdio>
# include  <fstream>
# include  <list>
# include  <vector>
# include  "parse_misc.h"
# include  "sv_vpi_user.h"
# include  "vvp_net.h"

/*
 * The file names are kept in this vector. Entry 0 is "N/A" and 1 is
 * for interactive commands.
 */
ബാഹ്യ std::vector<സ്ഥിരം അക്ഷരം*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

ബാഹ്യ ശൂന്യം compile_init(ശൂന്യം);

ബാഹ്യ ശൂന്യം compile_cleanup(ശൂന്യം);

ബാഹ്യ bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
ബാഹ്യ std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
ബാഹ്യ ശൂന്യം inputs_connect(vvp_net_t*fdx, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം നിവേശം_connect(vvp_net_t*fdx, unsigned port, അക്ഷരം*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
ബാഹ്യ ശൂന്യം wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ vvp_net_t* vvp_net_lookup(സ്ഥിരം അക്ഷരം*label);
ബാഹ്യ vpiHandle vvp_lookup_handle(സ്ഥിരം അക്ഷരം*label);

/*
 *  Add a functor to the symbol table
 */
ബാഹ്യ ശൂന്യം define_functor_ചിഹ്നം(സ്ഥിരം അക്ഷരം*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
ബാഹ്യ unsigned compile_errors;

ബാഹ്യ സ്ഥിരം അക്ഷരം* module_path;
ബാഹ്യ ശൂന്യം compile_load_vpi_module(അക്ഷരം*name);

ബാഹ്യ ശൂന്യം compile_vpi_time_precision(long pre);

/*
 * This function is called by the parser to compile a functor
 * statement. The strings passed in are allocated by the lexor, but
 * this function will free them. (Or save them permanently.) This
 * includes the argv array and the strings it references.
 *
 * The argc and argv are a list of char* that are the port parameters
 * of the functor. The compile should match those port parameters up
 * to existing functors to manage the linking.
 */
ബാഹ്യ ശൂന്യം compile_functor(അക്ഷരം*label, അക്ഷരം*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, ഘടന symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
ബാഹ്യ ശൂന്യം compile_resolver(അക്ഷരം*label, അക്ഷരം*type,
			     unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_concat(അക്ഷരം*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_concat8(അക്ഷരം*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_substitute(അക്ഷരം*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, ഘടന symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
ബാഹ്യ ശൂന്യം compile_compiletf(ഘടന __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
ബാഹ്യ ശൂന്യം compile_delay(അക്ഷരം*label, unsigned width,
                          vvp_delay_t*del, ഘടന symb_s നിവേശം);
ബാഹ്യ ശൂന്യം compile_delay(അക്ഷരം*label, unsigned width,
                          unsigned argc, ഘടന symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
ബാഹ്യ ശൂന്യം compile_part_select(അക്ഷരം*label, അക്ഷരം*src,
				unsigned base, unsigned wid);
ബാഹ്യ ശൂന്യം compile_part_select_pv(അക്ഷരം*label, അക്ഷരം*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
ബാഹ്യ ശൂന്യം compile_part_select_var(അക്ഷരം*label, അക്ഷരം*src,
                                    അക്ഷരം*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
ബാഹ്യ ശൂന്യം compile_arith_pow(അക്ഷരം*label, long width, bool signed_flag,
			      unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_abs(അക്ഷരം*label,
			      unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_cast_int(അക്ഷരം*label, long width,
			           unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_cast_real(അക്ഷരം*label, bool signed_flag,
			            unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_cast_vec2(അക്ഷരം*label, long width,
				    unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_div(അക്ഷരം*label, long width, bool signed_flag,
			      unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_mod(അക്ഷരം*label, long width, bool signed_flag,
			      unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_mult(അക്ഷരം*label, long width,
			       unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_sum(അക്ഷരം*label, long width,
			      unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_sub(അക്ഷരം*label, long width,
			      unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_eeq(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_nee(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_eq(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_eqx(അക്ഷരം*label, long width,
			    unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_eqz(അക്ഷരം*label, long width,
			    unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_ne(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_ge(അക്ഷരം*label, long width, bool signed_flag,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_gt(അക്ഷരം*label, long width, bool signed_flag,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_weq(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_wne(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_arith_mult_r(അക്ഷരം*label, unsigned argc,
                                 ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_pow_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_div_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_mod_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_sum_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_arith_sub_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_eq_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_ne_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_ge_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_cmp_gt_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_dff(അക്ഷരം*label, unsigned width, bool negedge,
			ഘടന symb_s arg_d,
			ഘടന symb_s arg_c,
			ഘടന symb_s arg_e);

ബാഹ്യ ശൂന്യം compile_dff_aclr(അക്ഷരം*label, unsigned width, bool negedge,
			     ഘടന symb_s arg_d,
			     ഘടന symb_s arg_c,
			     ഘടന symb_s arg_e,
			     ഘടന symb_s arg_a);
ബാഹ്യ ശൂന്യം compile_dff_aset(അക്ഷരം*label, unsigned width, bool negedge,
			     ഘടന symb_s arg_d,
			     ഘടന symb_s arg_c,
			     ഘടന symb_s arg_e,
			     ഘടന symb_s arg_a,
			     അക്ഷരം*asc_മൂല്യം);

ബാഹ്യ ശൂന്യം compile_latch(അക്ഷരം*label, unsigned width,
			  ഘടന symb_s arg_d,
			  ഘടന symb_s arg_e);

ബാഹ്യ ശൂന്യം compile_enum2_type(അക്ഷരം*label, long width, bool signed_flag,
			      std::list<ഘടന enum_name_s>*names);
ബാഹ്യ ശൂന്യം compile_enum4_type(അക്ഷരം*label, long width, bool signed_flag,
			      std::list<ഘടന enum_name_s>*names);

ഘടന __vpiModPath;
ബാഹ്യ __vpiModPath* compile_modpath(അക്ഷരം*label,
                                     unsigned width,
				     ഘടന symb_s drv,
				     ഘടന symb_s dest);
ബാഹ്യ ശൂന്യം compile_modpath_src(__vpiModPath*dst,
				അക്ഷരം edge_c,
				സ്ഥിരം ഘടന symb_s&src,
				ഘടന numbv_s&vals,
				സ്ഥിരം ഘടന symb_s&condit_src,
				സ്ഥിരം ഘടന symb_s&path_term_in);
ബാഹ്യ ശൂന്യം compile_modpath_src(__vpiModPath*dst,
				അക്ഷരം edge_c,
				സ്ഥിരം ഘടന symb_s&src,
				ഘടന numbv_s&vals,
				പൂർണ്ണസംഖ്യ condit_src, /* match with '0' */
				സ്ഥിരം ഘടന symb_s&path_term_in,
				bool ifnone);

ബാഹ്യ ശൂന്യം compile_reduce_and(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg);
ബാഹ്യ ശൂന്യം compile_reduce_or(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg);
ബാഹ്യ ശൂന്യം compile_reduce_xor(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg);
ബാഹ്യ ശൂന്യം compile_reduce_nand(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg);
ബാഹ്യ ശൂന്യം compile_reduce_nor(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg);
ബാഹ്യ ശൂന്യം compile_reduce_xnor(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg);

ബാഹ്യ ശൂന്യം compile_extend_signed(അക്ഷരം*label, long width, ഘടന symb_s arg);

ബാഹ്യ ശൂന്യം compile_sfunc(അക്ഷരം*label, അക്ഷരം*name, അക്ഷരം*format_string,
			  long file_idx, long lineno,
			  unsigned argc, ഘടന symb_s*argv,
                          അക്ഷരം*trigger_label);

ബാഹ്യ ശൂന്യം compile_repeat(അക്ഷരം*label, long width, long repeat,
			   ഘടന symb_s arg);

ബാഹ്യ ശൂന്യം compile_shiftl(അക്ഷരം*label, long width,
			   unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_shiftr(അക്ഷരം*label, long width, bool signed_flag,
			   unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_timescale(long units, long precision);

ബാഹ്യ ശൂന്യം compile_vpi_ചിഹ്നം(സ്ഥിരം അക്ഷരം*label, vpiHandle obj);
ബാഹ്യ ശൂന്യം compile_vpi_lookup(vpiHandle *objref, അക്ഷരം*label);

ബാഹ്യ ശൂന്യം compile_param_string(അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name, അക്ഷരം*മൂല്യം,
                                 bool local_flag,
                                 long file_idx, long lineno);
ബാഹ്യ ശൂന്യം compile_param_logic(അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name, അക്ഷരം*മൂല്യം,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
ബാഹ്യ ശൂന്യം compile_param_real(അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name, അക്ഷരം*മൂല്യം,
                               bool local_flag,
                               long file_idx, long lineno);

/*
 * The resolv_list_s is the base class for a symbol resolve
 * action. Some function creates an instance of a resolv_list_s object
 * that contains the data pertinent to that resolution request, and
 * executes it with the resolv_submit function. If the operation can
 * complete, then the resolv_submit deletes the object. Otherwise, it
 * pushes it onto the resolv_list for later processing.
 *
 * Derived classes implement the resolve function to perform the
 * actual binding or resolution that the instance requires. If the
 * function succeeds, the resolve method returns true and the object
 * can be deleted any time.
 *
 * The mes parameter of the resolve method tells the resolver that
 * this call is its last chance. If it cannot complete the operation,
 * it must print an error message and return false.
 */
class resolv_list_s {

    പൊതു:
      explicit resolv_list_s(അക്ഷരം*lab) : label_(lab) {
	    next = ശൂന്യ;
      }
      മായാ ~resolv_list_s();

      resolv_list_s(സ്ഥിരം resolv_list_s&) = delete;
      resolv_list_s& operator=(സ്ഥിരം resolv_list_s&) = delete;

      മായാ bool resolve(bool mes = false) = 0;

    സംരക്ഷിത:
      സ്ഥിരം അക്ഷരം*label() സ്ഥിരം { തിരികെ label_; }

    സ്വകാര്യ:
      friend ശൂന്യം resolv_submit(class resolv_list_s*cur);
      friend ശൂന്യം compile_cleanup(ശൂന്യം);

      അക്ഷരം*label_;
      class resolv_list_s*next;
};

/*
 * This function schedules a lookup of an indexed label. The ref
 * points to the vvp_net_t that receives the result. The result may
 * be assigned later, if the symbol is defined later in the source
 * file, so the memory that ref points to must persist.
 *
 * The text for the label will be deleted by this function, or the
 * cleanup that completes the binding.
 */
ബാഹ്യ ശൂന്യം functor_ref_lookup(vvp_net_t**ref, അക്ഷരം*lab);

ബാഹ്യ vpiHandle vpip_make_prop_queue_ref(അക്ഷരം* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
ബാഹ്യ ശൂന്യം code_label_lookup(ഘടന vvp_code_s *code, അക്ഷരം *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

ബാഹ്യ ശൂന്യം compile_udp_def(പൂർണ്ണസംഖ്യ sequ, അക്ഷരം*label, അക്ഷരം *name,
			    unsigned nin, unsigned init, അക്ഷരം **table);

ബാഹ്യ ശൂന്യം compile_udp_functor(അക്ഷരം*label, അക്ഷരം*type,
				unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ അക്ഷരം **compile_udp_table(അക്ഷരം **table, അക്ഷരം *row);

/*
 * Memory Instances, Ports, and Initialization
 */

ബാഹ്യ ശൂന്യം compile_var_array(അക്ഷരം*label, അക്ഷരം*name,
			      പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first,
			      പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, അക്ഷരം signed_flag);
ബാഹ്യ ശൂന്യം compile_var2_array(അക്ഷരം*label, അക്ഷരം*name,
			      പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first,
			      പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, bool signed_flag);
ബാഹ്യ ശൂന്യം compile_real_array(അക്ഷരം*label, അക്ഷരം*name,
			       പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first);
ബാഹ്യ ശൂന്യം compile_string_array(അക്ഷരം*label, അക്ഷരം*name,
				 പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first);
ബാഹ്യ ശൂന്യം compile_object_array(അക്ഷരം*label, അക്ഷരം*name,
				 പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first);
ബാഹ്യ ശൂന്യം compile_net_array(അക്ഷരം*label, അക്ഷരം*name,
			      പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first);
ബാഹ്യ ശൂന്യം compile_array_alias(അക്ഷരം*label, അക്ഷരം*name, അക്ഷരം*src);

  /* Index is a net. */
ബാഹ്യ ശൂന്യം compile_array_port(അക്ഷരം*label, അക്ഷരം*name, അക്ഷരം*addr);
  /* Index is a constant address */
ബാഹ്യ ശൂന്യം compile_array_port(അക്ഷരം*label, അക്ഷരം*name, long addr);

ബാഹ്യ ശൂന്യം compile_array_cleanup(ശൂന്യം);

/*
 * Compile the .ufunc statement.
 */
ബാഹ്യ ശൂന്യം compile_ufunc_real(അക്ഷരം*label, അക്ഷരം*code, unsigned wid,
			  unsigned argc, ഘടന symb_s*argv,
			  unsigned portc, ഘടന symb_s*portv,
			  അക്ഷരം*scope_label, അക്ഷരം*trigger_label);
ബാഹ്യ ശൂന്യം compile_ufunc_vec4(അക്ഷരം*label, അക്ഷരം*code, unsigned wid,
			  unsigned argc, ഘടന symb_s*argv,
			  unsigned portc, ഘടന symb_s*portv,
			  അക്ഷരം*scope_label, അക്ഷരം*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
ബാഹ്യ ശൂന്യം compile_event(അക്ഷരം*label, അക്ഷരം*type,
			  unsigned argc, ഘടന symb_s*argv);
ബാഹ്യ ശൂന്യം compile_named_event(അക്ഷരം*label, അക്ഷരം*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
എണ്ണൽ ltype_e { L_NUMB, L_SYMB, L_STRING };

ഘടന comp_operands_s {
      unsigned argc;
      ഘടന {
	    എണ്ണൽ ltype_e ltype;
	    സംഘം {
		  unsigned long numb;
		  ഘടന symb_s symb;
		  സ്ഥിരം അക്ഷരം   *text;
	    };
      } argv[OPERAND_MAX];
};

തരനിർവചനം ഘടന comp_operands_s*comp_operands_t;

ബാഹ്യ ശൂന്യം compile_code(അക്ഷരം*label, അക്ഷരം*mnem, comp_operands_t opa);

ബാഹ്യ ശൂന്യം compile_file_വരി(അക്ഷരം*label, long file_idx, long lineno,
                              അക്ഷരം*description);

ബാഹ്യ ശൂന്യം compile_vpi_call(അക്ഷരം*label, അക്ഷരം*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_അട്ടി,
			     unsigned real_അട്ടി,
			     unsigned string_അട്ടി);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
ബാഹ്യ ശൂന്യം compile_vpi_func_call(അക്ഷരം*label, അക്ഷരം*name,
				  പൂർണ്ണസംഖ്യ val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_അട്ടി,
				  unsigned real_അട്ടി,
				  unsigned string_അട്ടി);
ബാഹ്യ ശൂന്യം print_vpi_call_errors();

ബാഹ്യ ശൂന്യം compile_codelabel(അക്ഷരം*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
ബാഹ്യ ശൂന്യം compile_scope_decl(അക്ഷരം*typ, അക്ഷരം*lab, അക്ഷരം*nam, അക്ഷരം*tnam,
                               അക്ഷരം*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
ബാഹ്യ ശൂന്യം compile_scope_recall(അക്ഷരം*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
ബാഹ്യ ശൂന്യം compile_thread(അക്ഷരം*start_sym, അക്ഷരം*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
ബാഹ്യ ശൂന്യം compile_variable(അക്ഷരം*label, അക്ഷരം*name,
			     പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, പൂർണ്ണസംഖ്യ vpi_type_code,
			     bool signed_flag, bool local_flag);

ബാഹ്യ ശൂന്യം compile_var_real(അക്ഷരം*label, അക്ഷരം*name, bool local_flag);
ബാഹ്യ ശൂന്യം compile_var_string(അക്ഷരം*label, അക്ഷരം*name);
ബാഹ്യ ശൂന്യം compile_var_darray(അക്ഷരം*label, അക്ഷരം*name, unsigned size);
ബാഹ്യ ശൂന്യം compile_var_cobject(അക്ഷരം*label, അക്ഷരം*name);
ബാഹ്യ ശൂന്യം compile_var_queue(അക്ഷരം*label, അക്ഷരം*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

ബാഹ്യ ശൂന്യം compile_port_info( unsigned index, പൂർണ്ണസംഖ്യ vpi_port_type, unsigned width, സ്ഥിരം അക്ഷരം *name, അക്ഷരം* buffer );


/*
 * The compile_net function is called to create a .net vector with a
 * given name.
 *
 * The vpi_type_code argument of compile_net() is one of the vpi
 * object codes for the equivalent variable types. The supported codes
 * are:
 *   vpiLogic  -- 4-value logic
 *   vpiIntVar -- 2-value logic
 *  -vpiLogic  -- 8-value (i.e. strength aware) logic
 */
ബാഹ്യ ശൂന്യം compile_net(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			പൂർണ്ണസംഖ്യ vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_net_real(അക്ഷരം*label, അക്ഷരം*name,
			     പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, bool local_flag,
			     unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_netw(അക്ഷരം*label, അക്ഷരം*array_label, unsigned long array_addr,
			 പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, പൂർണ്ണസംഖ്യ vpi_type_code, bool signed_flag,
			 unsigned argc, ഘടന symb_s*argv);

ബാഹ്യ ശൂന്യം compile_netw_real(അക്ഷരം*label, അക്ഷരം*array_ചിഹ്നം,
			      unsigned long array_addr,
			      പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			      unsigned argc, ഘടന symb_s*argv);


ബാഹ്യ ശൂന്യം compile_island(അക്ഷരം*label, അക്ഷരം*type);
ബാഹ്യ ശൂന്യം compile_island_port(അക്ഷരം*label, അക്ഷരം*island, അക്ഷരം*src);
ബാഹ്യ ശൂന്യം compile_island_import(അക്ഷരം*label, അക്ഷരം*island, അക്ഷരം*src);
ബാഹ്യ ശൂന്യം compile_island_export(അക്ഷരം*label, അക്ഷരം*island);
ബാഹ്യ ശൂന്യം compile_island_cleanup(ശൂന്യം);

ബാഹ്യ ശൂന്യം compile_island_tran(അക്ഷരം*label);
ബാഹ്യ ശൂന്യം compile_island_tranif(പൂർണ്ണസംഖ്യ sense, അക്ഷരം*island,
				  അക്ഷരം*ba, അക്ഷരം*bb, അക്ഷരം*src, bool resistive);
ബാഹ്യ ശൂന്യം compile_island_tranvp(അക്ഷരം*island, അക്ഷരം*ba, അക്ഷരം*bb,
				  unsigned width, unsigned part, unsigned off);

ബാഹ്യ ശൂന്യം delete_udp_symbols(ശൂന്യം);

ബാഹ്യ ശൂന്യം compile_class_start(അക്ഷരം*lab, അക്ഷരം*nam, unsigned nprop);
ബാഹ്യ ശൂന്യം compile_class_property(unsigned idx, അക്ഷരം*nam, അക്ഷരം*typ, uint64_t array_size);
ബാഹ്യ ശൂന്യം compile_class_done(ശൂന്യം);

#endif /* IVL_compile_H */
