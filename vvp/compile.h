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
বাহ্য std::vector<ধ্রুব বর্ণ*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

বাহ্য শূন্য compile_init(শূন্য);

বাহ্য শূন্য compile_cleanup(শূন্য);

বাহ্য bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
বাহ্য std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
বাহ্য শূন্য inputs_connect(vvp_net_t*fdx, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য প্রবেশ_connect(vvp_net_t*fdx, unsigned port, বর্ণ*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
বাহ্য শূন্য wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, সংরচনা symb_s*argv);

বাহ্য vvp_net_t* vvp_net_lookup(ধ্রুব বর্ণ*label);
বাহ্য vpiHandle vvp_lookup_handle(ধ্রুব বর্ণ*label);

/*
 *  Add a functor to the symbol table
 */
বাহ্য শূন্য define_functor_প্রতীক(ধ্রুব বর্ণ*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
বাহ্য unsigned compile_errors;

বাহ্য ধ্রুব বর্ণ* module_path;
বাহ্য শূন্য compile_load_vpi_module(বর্ণ*name);

বাহ্য শূন্য compile_vpi_time_precision(long pre);

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
বাহ্য শূন্য compile_functor(বর্ণ*label, বর্ণ*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, সংরচনা symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
বাহ্য শূন্য compile_resolver(বর্ণ*label, বর্ণ*type,
			     unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_concat(বর্ণ*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_concat8(বর্ণ*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_substitute(বর্ণ*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, সংরচনা symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
বাহ্য শূন্য compile_compiletf(সংরচনা __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
বাহ্য শূন্য compile_delay(বর্ণ*label, unsigned width,
                          vvp_delay_t*del, সংরচনা symb_s প্রবেশ);
বাহ্য শূন্য compile_delay(বর্ণ*label, unsigned width,
                          unsigned argc, সংরচনা symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
বাহ্য শূন্য compile_part_select(বর্ণ*label, বর্ণ*src,
				unsigned base, unsigned wid);
বাহ্য শূন্য compile_part_select_pv(বর্ণ*label, বর্ণ*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
বাহ্য শূন্য compile_part_select_var(বর্ণ*label, বর্ণ*src,
                                    বর্ণ*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
বাহ্য শূন্য compile_arith_pow(বর্ণ*label, long width, bool signed_flag,
			      unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_abs(বর্ণ*label,
			      unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_cast_int(বর্ণ*label, long width,
			           unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_cast_real(বর্ণ*label, bool signed_flag,
			            unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_cast_vec2(বর্ণ*label, long width,
				    unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_div(বর্ণ*label, long width, bool signed_flag,
			      unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_mod(বর্ণ*label, long width, bool signed_flag,
			      unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_mult(বর্ণ*label, long width,
			       unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_sum(বর্ণ*label, long width,
			      unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_sub(বর্ণ*label, long width,
			      unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_eeq(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_nee(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_eq(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_eqx(বর্ণ*label, long width,
			    unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_eqz(বর্ণ*label, long width,
			    unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_ne(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_ge(বর্ণ*label, long width, bool signed_flag,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_gt(বর্ণ*label, long width, bool signed_flag,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_weq(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_wne(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_arith_mult_r(বর্ণ*label, unsigned argc,
                                 সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_pow_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_div_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_mod_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_sum_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_arith_sub_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_eq_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_ne_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_ge_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_cmp_gt_r(বর্ণ*label, unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_dff(বর্ণ*label, unsigned width, bool negedge,
			সংরচনা symb_s arg_d,
			সংরচনা symb_s arg_c,
			সংরচনা symb_s arg_e);

বাহ্য শূন্য compile_dff_aclr(বর্ণ*label, unsigned width, bool negedge,
			     সংরচনা symb_s arg_d,
			     সংরচনা symb_s arg_c,
			     সংরচনা symb_s arg_e,
			     সংরচনা symb_s arg_a);
বাহ্য শূন্য compile_dff_aset(বর্ণ*label, unsigned width, bool negedge,
			     সংরচনা symb_s arg_d,
			     সংরচনা symb_s arg_c,
			     সংরচনা symb_s arg_e,
			     সংরচনা symb_s arg_a,
			     বর্ণ*asc_মান);

বাহ্য শূন্য compile_latch(বর্ণ*label, unsigned width,
			  সংরচনা symb_s arg_d,
			  সংরচনা symb_s arg_e);

বাহ্য শূন্য compile_enum2_type(বর্ণ*label, long width, bool signed_flag,
			      std::list<সংরচনা enum_name_s>*names);
বাহ্য শূন্য compile_enum4_type(বর্ণ*label, long width, bool signed_flag,
			      std::list<সংরচনা enum_name_s>*names);

সংরচনা __vpiModPath;
বাহ্য __vpiModPath* compile_modpath(বর্ণ*label,
                                     unsigned width,
				     সংরচনা symb_s drv,
				     সংরচনা symb_s dest);
বাহ্য শূন্য compile_modpath_src(__vpiModPath*dst,
				বর্ণ edge_c,
				ধ্রুব সংরচনা symb_s&src,
				সংরচনা numbv_s&vals,
				ধ্রুব সংরচনা symb_s&condit_src,
				ধ্রুব সংরচনা symb_s&path_term_in);
বাহ্য শূন্য compile_modpath_src(__vpiModPath*dst,
				বর্ণ edge_c,
				ধ্রুব সংরচনা symb_s&src,
				সংরচনা numbv_s&vals,
				পূর্ণাঙ্ক condit_src, /* match with '0' */
				ধ্রুব সংরচনা symb_s&path_term_in,
				bool ifnone);

বাহ্য শূন্য compile_reduce_and(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg);
বাহ্য শূন্য compile_reduce_or(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg);
বাহ্য শূন্য compile_reduce_xor(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg);
বাহ্য শূন্য compile_reduce_nand(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg);
বাহ্য শূন্য compile_reduce_nor(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg);
বাহ্য শূন্য compile_reduce_xnor(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg);

বাহ্য শূন্য compile_extend_signed(বর্ণ*label, long width, সংরচনা symb_s arg);

বাহ্য শূন্য compile_sfunc(বর্ণ*label, বর্ণ*name, বর্ণ*format_string,
			  long file_idx, long lineno,
			  unsigned argc, সংরচনা symb_s*argv,
                          বর্ণ*trigger_label);

বাহ্য শূন্য compile_repeat(বর্ণ*label, long width, long repeat,
			   সংরচনা symb_s arg);

বাহ্য শূন্য compile_shiftl(বর্ণ*label, long width,
			   unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_shiftr(বর্ণ*label, long width, bool signed_flag,
			   unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_timescale(long units, long precision);

বাহ্য শূন্য compile_vpi_প্রতীক(ধ্রুব বর্ণ*label, vpiHandle obj);
বাহ্য শূন্য compile_vpi_lookup(vpiHandle *objref, বর্ণ*label);

বাহ্য শূন্য compile_param_string(বর্ণ*label, ধ্রুব বর্ণ*name, বর্ণ*মান,
                                 bool local_flag,
                                 long file_idx, long lineno);
বাহ্য শূন্য compile_param_logic(বর্ণ*label, ধ্রুব বর্ণ*name, বর্ণ*মান,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
বাহ্য শূন্য compile_param_real(বর্ণ*label, ধ্রুব বর্ণ*name, বর্ণ*মান,
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

    সার্বজনিক:
      explicit resolv_list_s(বর্ণ*lab) : label_(lab) {
	    next = রিক্ত;
      }
      আভাস ~resolv_list_s();

      resolv_list_s(ধ্রুব resolv_list_s&) = delete;
      resolv_list_s& operator=(ধ্রুব resolv_list_s&) = delete;

      আভাস bool resolve(bool mes = false) = 0;

    রক্ষিত:
      ধ্রুব বর্ণ*label() ধ্রুব { প্রত্যাগম label_; }

    গুপ্ত:
      friend শূন্য resolv_submit(class resolv_list_s*cur);
      friend শূন্য compile_cleanup(শূন্য);

      বর্ণ*label_;
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
বাহ্য শূন্য functor_ref_lookup(vvp_net_t**ref, বর্ণ*lab);

বাহ্য vpiHandle vpip_make_prop_queue_ref(বর্ণ* class_label,
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
বাহ্য শূন্য code_label_lookup(সংরচনা vvp_code_s *code, বর্ণ *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

বাহ্য শূন্য compile_udp_def(পূর্ণাঙ্ক sequ, বর্ণ*label, বর্ণ *name,
			    unsigned nin, unsigned init, বর্ণ **table);

বাহ্য শূন্য compile_udp_functor(বর্ণ*label, বর্ণ*type,
				unsigned argc, সংরচনা symb_s*argv);

বাহ্য বর্ণ **compile_udp_table(বর্ণ **table, বর্ণ *row);

/*
 * Memory Instances, Ports, and Initialization
 */

বাহ্য শূন্য compile_var_array(বর্ণ*label, বর্ণ*name,
			      পূর্ণাঙ্ক last, পূর্ণাঙ্ক first,
			      পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, বর্ণ signed_flag);
বাহ্য শূন্য compile_var2_array(বর্ণ*label, বর্ণ*name,
			      পূর্ণাঙ্ক last, পূর্ণাঙ্ক first,
			      পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, bool signed_flag);
বাহ্য শূন্য compile_real_array(বর্ণ*label, বর্ণ*name,
			       পূর্ণাঙ্ক last, পূর্ণাঙ্ক first);
বাহ্য শূন্য compile_string_array(বর্ণ*label, বর্ণ*name,
				 পূর্ণাঙ্ক last, পূর্ণাঙ্ক first);
বাহ্য শূন্য compile_object_array(বর্ণ*label, বর্ণ*name,
				 পূর্ণাঙ্ক last, পূর্ণাঙ্ক first);
বাহ্য শূন্য compile_net_array(বর্ণ*label, বর্ণ*name,
			      পূর্ণাঙ্ক last, পূর্ণাঙ্ক first);
বাহ্য শূন্য compile_array_alias(বর্ণ*label, বর্ণ*name, বর্ণ*src);

  /* Index is a net. */
বাহ্য শূন্য compile_array_port(বর্ণ*label, বর্ণ*name, বর্ণ*addr);
  /* Index is a constant address */
বাহ্য শূন্য compile_array_port(বর্ণ*label, বর্ণ*name, long addr);

বাহ্য শূন্য compile_array_cleanup(শূন্য);

/*
 * Compile the .ufunc statement.
 */
বাহ্য শূন্য compile_ufunc_real(বর্ণ*label, বর্ণ*code, unsigned wid,
			  unsigned argc, সংরচনা symb_s*argv,
			  unsigned portc, সংরচনা symb_s*portv,
			  বর্ণ*scope_label, বর্ণ*trigger_label);
বাহ্য শূন্য compile_ufunc_vec4(বর্ণ*label, বর্ণ*code, unsigned wid,
			  unsigned argc, সংরচনা symb_s*argv,
			  unsigned portc, সংরচনা symb_s*portv,
			  বর্ণ*scope_label, বর্ণ*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
বাহ্য শূন্য compile_event(বর্ণ*label, বর্ণ*type,
			  unsigned argc, সংরচনা symb_s*argv);
বাহ্য শূন্য compile_named_event(বর্ণ*label, বর্ণ*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
গণনা ltype_e { L_NUMB, L_SYMB, L_STRING };

সংরচনা comp_operands_s {
      unsigned argc;
      সংরচনা {
	    গণনা ltype_e ltype;
	    সঙ্ঘ {
		  unsigned long numb;
		  সংরচনা symb_s symb;
		  ধ্রুব বর্ণ   *text;
	    };
      } argv[OPERAND_MAX];
};

প্রকারনির্দেশ সংরচনা comp_operands_s*comp_operands_t;

বাহ্য শূন্য compile_code(বর্ণ*label, বর্ণ*mnem, comp_operands_t opa);

বাহ্য শূন্য compile_file_সারি(বর্ণ*label, long file_idx, long lineno,
                              বর্ণ*description);

বাহ্য শূন্য compile_vpi_call(বর্ণ*label, বর্ণ*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_স্তূপ,
			     unsigned real_স্তূপ,
			     unsigned string_স্তূপ);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
বাহ্য শূন্য compile_vpi_func_call(বর্ণ*label, বর্ণ*name,
				  পূর্ণাঙ্ক val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_স্তূপ,
				  unsigned real_স্তূপ,
				  unsigned string_স্তূপ);
বাহ্য শূন্য print_vpi_call_errors();

বাহ্য শূন্য compile_codelabel(বর্ণ*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
বাহ্য শূন্য compile_scope_decl(বর্ণ*typ, বর্ণ*lab, বর্ণ*nam, বর্ণ*tnam,
                               বর্ণ*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
বাহ্য শূন্য compile_scope_recall(বর্ণ*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
বাহ্য শূন্য compile_thread(বর্ণ*start_sym, বর্ণ*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
বাহ্য শূন্য compile_variable(বর্ণ*label, বর্ণ*name,
			     পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, পূর্ণাঙ্ক vpi_type_code,
			     bool signed_flag, bool local_flag);

বাহ্য শূন্য compile_var_real(বর্ণ*label, বর্ণ*name, bool local_flag);
বাহ্য শূন্য compile_var_string(বর্ণ*label, বর্ণ*name);
বাহ্য শূন্য compile_var_darray(বর্ণ*label, বর্ণ*name, unsigned size);
বাহ্য শূন্য compile_var_cobject(বর্ণ*label, বর্ণ*name);
বাহ্য শূন্য compile_var_queue(বর্ণ*label, বর্ণ*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

বাহ্য শূন্য compile_port_info( unsigned index, পূর্ণাঙ্ক vpi_port_type, unsigned width, ধ্রুব বর্ণ *name, বর্ণ* buffer );


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
বাহ্য শূন্য compile_net(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			পূর্ণাঙ্ক vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_net_real(বর্ণ*label, বর্ণ*name,
			     পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, bool local_flag,
			     unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_netw(বর্ণ*label, বর্ণ*array_label, unsigned long array_addr,
			 পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, পূর্ণাঙ্ক vpi_type_code, bool signed_flag,
			 unsigned argc, সংরচনা symb_s*argv);

বাহ্য শূন্য compile_netw_real(বর্ণ*label, বর্ণ*array_প্রতীক,
			      unsigned long array_addr,
			      পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			      unsigned argc, সংরচনা symb_s*argv);


বাহ্য শূন্য compile_island(বর্ণ*label, বর্ণ*type);
বাহ্য শূন্য compile_island_port(বর্ণ*label, বর্ণ*island, বর্ণ*src);
বাহ্য শূন্য compile_island_import(বর্ণ*label, বর্ণ*island, বর্ণ*src);
বাহ্য শূন্য compile_island_export(বর্ণ*label, বর্ণ*island);
বাহ্য শূন্য compile_island_cleanup(শূন্য);

বাহ্য শূন্য compile_island_tran(বর্ণ*label);
বাহ্য শূন্য compile_island_tranif(পূর্ণাঙ্ক sense, বর্ণ*island,
				  বর্ণ*ba, বর্ণ*bb, বর্ণ*src, bool resistive);
বাহ্য শূন্য compile_island_tranvp(বর্ণ*island, বর্ণ*ba, বর্ণ*bb,
				  unsigned width, unsigned part, unsigned off);

বাহ্য শূন্য delete_udp_symbols(শূন্য);

বাহ্য শূন্য compile_class_start(বর্ণ*lab, বর্ণ*nam, unsigned nprop);
বাহ্য শূন্য compile_class_property(unsigned idx, বর্ণ*nam, বর্ণ*typ, uint64_t array_size);
বাহ্য শূন্য compile_class_done(শূন্য);

#endif /* IVL_compile_H */
