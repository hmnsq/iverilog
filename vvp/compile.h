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
বাহ্যিক std::vector<ধ্রুবক অক্ষর*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

বাহ্যিক শূন্য compile_init(শূন্য);

বাহ্যিক শূন্য compile_cleanup(শূন্য);

বাহ্যিক bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
বাহ্যিক std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
বাহ্যিক শূন্য inputs_connect(vvp_net_t*fdx, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য প্রবেশ_connect(vvp_net_t*fdx, unsigned port, অক্ষর*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
বাহ্যিক শূন্য wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক vvp_net_t* vvp_net_lookup(ধ্রুবক অক্ষর*label);
বাহ্যিক vpiHandle vvp_lookup_handle(ধ্রুবক অক্ষর*label);

/*
 *  Add a functor to the symbol table
 */
বাহ্যিক শূন্য define_functor_প্রতীক(ধ্রুবক অক্ষর*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
বাহ্যিক unsigned compile_errors;

বাহ্যিক ধ্রুবক অক্ষর* module_path;
বাহ্যিক শূন্য compile_load_vpi_module(অক্ষর*name);

বাহ্যিক শূন্য compile_vpi_time_precision(long pre);

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
বাহ্যিক শূন্য compile_functor(অক্ষর*label, অক্ষর*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, কাঠামো symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
বাহ্যিক শূন্য compile_resolver(অক্ষর*label, অক্ষর*type,
			     unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_concat(অক্ষর*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_concat8(অক্ষর*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_substitute(অক্ষর*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, কাঠামো symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
বাহ্যিক শূন্য compile_compiletf(কাঠামো __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
বাহ্যিক শূন্য compile_delay(অক্ষর*label, unsigned width,
                          vvp_delay_t*del, কাঠামো symb_s প্রবেশ);
বাহ্যিক শূন্য compile_delay(অক্ষর*label, unsigned width,
                          unsigned argc, কাঠামো symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
বাহ্যিক শূন্য compile_part_select(অক্ষর*label, অক্ষর*src,
				unsigned base, unsigned wid);
বাহ্যিক শূন্য compile_part_select_pv(অক্ষর*label, অক্ষর*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
বাহ্যিক শূন্য compile_part_select_var(অক্ষর*label, অক্ষর*src,
                                    অক্ষর*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
বাহ্যিক শূন্য compile_arith_pow(অক্ষর*label, long width, bool signed_flag,
			      unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_abs(অক্ষর*label,
			      unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_cast_int(অক্ষর*label, long width,
			           unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_cast_real(অক্ষর*label, bool signed_flag,
			            unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_cast_vec2(অক্ষর*label, long width,
				    unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_div(অক্ষর*label, long width, bool signed_flag,
			      unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_mod(অক্ষর*label, long width, bool signed_flag,
			      unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_mult(অক্ষর*label, long width,
			       unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_যোগ(অক্ষর*label, long width,
			      unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_sub(অক্ষর*label, long width,
			      unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_eeq(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_nee(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_eq(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_eqx(অক্ষর*label, long width,
			    unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_eqz(অক্ষর*label, long width,
			    unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_ne(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_ge(অক্ষর*label, long width, bool signed_flag,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_gt(অক্ষর*label, long width, bool signed_flag,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_weq(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_wne(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_arith_mult_r(অক্ষর*label, unsigned argc,
                                 কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_pow_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_div_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_mod_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_যোগ_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_arith_sub_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_eq_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_ne_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_ge_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_cmp_gt_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_dff(অক্ষর*label, unsigned width, bool negedge,
			কাঠামো symb_s arg_d,
			কাঠামো symb_s arg_c,
			কাঠামো symb_s arg_e);

বাহ্যিক শূন্য compile_dff_aclr(অক্ষর*label, unsigned width, bool negedge,
			     কাঠামো symb_s arg_d,
			     কাঠামো symb_s arg_c,
			     কাঠামো symb_s arg_e,
			     কাঠামো symb_s arg_a);
বাহ্যিক শূন্য compile_dff_aset(অক্ষর*label, unsigned width, bool negedge,
			     কাঠামো symb_s arg_d,
			     কাঠামো symb_s arg_c,
			     কাঠামো symb_s arg_e,
			     কাঠামো symb_s arg_a,
			     অক্ষর*asc_মান);

বাহ্যিক শূন্য compile_latch(অক্ষর*label, unsigned width,
			  কাঠামো symb_s arg_d,
			  কাঠামো symb_s arg_e);

বাহ্যিক শূন্য compile_enum2_type(অক্ষর*label, long width, bool signed_flag,
			      std::list<কাঠামো enum_name_s>*names);
বাহ্যিক শূন্য compile_enum4_type(অক্ষর*label, long width, bool signed_flag,
			      std::list<কাঠামো enum_name_s>*names);

কাঠামো __vpiModPath;
বাহ্যিক __vpiModPath* compile_modpath(অক্ষর*label,
                                     unsigned width,
				     কাঠামো symb_s drv,
				     কাঠামো symb_s dest);
বাহ্যিক শূন্য compile_modpath_src(__vpiModPath*dst,
				অক্ষর edge_c,
				ধ্রুবক কাঠামো symb_s&src,
				কাঠামো numbv_s&vals,
				ধ্রুবক কাঠামো symb_s&condit_src,
				ধ্রুবক কাঠামো symb_s&path_term_in);
বাহ্যিক শূন্য compile_modpath_src(__vpiModPath*dst,
				অক্ষর edge_c,
				ধ্রুবক কাঠামো symb_s&src,
				কাঠামো numbv_s&vals,
				পূর্ণসংখ্যা condit_src, /* match with '0' */
				ধ্রুবক কাঠামো symb_s&path_term_in,
				bool ifnone);

বাহ্যিক শূন্য compile_reduce_and(অক্ষর*label, ধ্রুবক কাঠামো symb_s&arg);
বাহ্যিক শূন্য compile_reduce_or(অক্ষর*label, ধ্রুবক কাঠামো symb_s&arg);
বাহ্যিক শূন্য compile_reduce_xor(অক্ষর*label, ধ্রুবক কাঠামো symb_s&arg);
বাহ্যিক শূন্য compile_reduce_nand(অক্ষর*label, ধ্রুবক কাঠামো symb_s&arg);
বাহ্যিক শূন্য compile_reduce_nor(অক্ষর*label, ধ্রুবক কাঠামো symb_s&arg);
বাহ্যিক শূন্য compile_reduce_xnor(অক্ষর*label, ধ্রুবক কাঠামো symb_s&arg);

বাহ্যিক শূন্য compile_extend_signed(অক্ষর*label, long width, কাঠামো symb_s arg);

বাহ্যিক শূন্য compile_sfunc(অক্ষর*label, অক্ষর*name, অক্ষর*format_string,
			  long file_idx, long lineno,
			  unsigned argc, কাঠামো symb_s*argv,
                          অক্ষর*trigger_label);

বাহ্যিক শূন্য compile_repeat(অক্ষর*label, long width, long repeat,
			   কাঠামো symb_s arg);

বাহ্যিক শূন্য compile_shiftl(অক্ষর*label, long width,
			   unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_shiftr(অক্ষর*label, long width, bool signed_flag,
			   unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_timescale(long units, long precision);

বাহ্যিক শূন্য compile_vpi_প্রতীক(ধ্রুবক অক্ষর*label, vpiHandle obj);
বাহ্যিক শূন্য compile_vpi_lookup(vpiHandle *objref, অক্ষর*label);

বাহ্যিক শূন্য compile_param_string(অক্ষর*label, ধ্রুবক অক্ষর*name, অক্ষর*মান,
                                 bool local_flag,
                                 long file_idx, long lineno);
বাহ্যিক শূন্য compile_param_logic(অক্ষর*label, ধ্রুবক অক্ষর*name, অক্ষর*মান,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
বাহ্যিক শূন্য compile_param_real(অক্ষর*label, ধ্রুবক অক্ষর*name, অক্ষর*মান,
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

    সর্বজনীন:
      explicit resolv_list_s(অক্ষর*lab) : label_(lab) {
	    পরবর্তী_2 = ফাঁকা;
      }
      আভাসী ~resolv_list_s();

      resolv_list_s(ধ্রুবক resolv_list_s&) = delete;
      resolv_list_s& operator=(ধ্রুবক resolv_list_s&) = delete;

      আভাসী bool resolve(bool mes = false) = 0;

    সুরক্ষিত:
      ধ্রুবক অক্ষর*label() ধ্রুবক { ফেরত label_; }

    ব্যক্তিগত:
      friend শূন্য resolv_submit(class resolv_list_s*cur);
      friend শূন্য compile_cleanup(শূন্য);

      অক্ষর*label_;
      class resolv_list_s*পরবর্তী_2;
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
বাহ্যিক শূন্য functor_ref_lookup(vvp_net_t**ref, অক্ষর*lab);

বাহ্যিক vpiHandle vpip_make_prop_queue_ref(অক্ষর* class_label,
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
বাহ্যিক শূন্য code_label_lookup(কাঠামো vvp_code_s *code, অক্ষর *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

বাহ্যিক শূন্য compile_udp_def(পূর্ণসংখ্যা sequ, অক্ষর*label, অক্ষর *name,
			    unsigned nin, unsigned init, অক্ষর **table);

বাহ্যিক শূন্য compile_udp_functor(অক্ষর*label, অক্ষর*type,
				unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক অক্ষর **compile_udp_table(অক্ষর **table, অক্ষর *row);

/*
 * Memory Instances, Ports, and Initialization
 */

বাহ্যিক শূন্য compile_var_array(অক্ষর*label, অক্ষর*name,
			      পূর্ণসংখ্যা last, পূর্ণসংখ্যা first,
			      পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, অক্ষর signed_flag);
বাহ্যিক শূন্য compile_var2_array(অক্ষর*label, অক্ষর*name,
			      পূর্ণসংখ্যা last, পূর্ণসংখ্যা first,
			      পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, bool signed_flag);
বাহ্যিক শূন্য compile_real_array(অক্ষর*label, অক্ষর*name,
			       পূর্ণসংখ্যা last, পূর্ণসংখ্যা first);
বাহ্যিক শূন্য compile_string_array(অক্ষর*label, অক্ষর*name,
				 পূর্ণসংখ্যা last, পূর্ণসংখ্যা first);
বাহ্যিক শূন্য compile_object_array(অক্ষর*label, অক্ষর*name,
				 পূর্ণসংখ্যা last, পূর্ণসংখ্যা first);
বাহ্যিক শূন্য compile_net_array(অক্ষর*label, অক্ষর*name,
			      পূর্ণসংখ্যা last, পূর্ণসংখ্যা first);
বাহ্যিক শূন্য compile_array_alias(অক্ষর*label, অক্ষর*name, অক্ষর*src);

  /* Index is a net. */
বাহ্যিক শূন্য compile_array_port(অক্ষর*label, অক্ষর*name, অক্ষর*addr);
  /* Index is a constant address */
বাহ্যিক শূন্য compile_array_port(অক্ষর*label, অক্ষর*name, long addr);

বাহ্যিক শূন্য compile_array_cleanup(শূন্য);

/*
 * Compile the .ufunc statement.
 */
বাহ্যিক শূন্য compile_ufunc_real(অক্ষর*label, অক্ষর*code, unsigned wid,
			  unsigned argc, কাঠামো symb_s*argv,
			  unsigned portc, কাঠামো symb_s*portv,
			  অক্ষর*scope_label, অক্ষর*trigger_label);
বাহ্যিক শূন্য compile_ufunc_vec4(অক্ষর*label, অক্ষর*code, unsigned wid,
			  unsigned argc, কাঠামো symb_s*argv,
			  unsigned portc, কাঠামো symb_s*portv,
			  অক্ষর*scope_label, অক্ষর*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
বাহ্যিক শূন্য compile_event(অক্ষর*label, অক্ষর*type,
			  unsigned argc, কাঠামো symb_s*argv);
বাহ্যিক শূন্য compile_named_event(অক্ষর*label, অক্ষর*type, bool local_flag=false);


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

কাঠামো comp_operands_s {
      unsigned argc;
      কাঠামো {
	    গণনা ltype_e ltype;
	    সংঘ {
		  unsigned long numb;
		  কাঠামো symb_s symb;
		  ধ্রুবক অক্ষর   *text;
	    };
      } argv[OPERAND_MAX];
};

প্রকারলেখ কাঠামো comp_operands_s*comp_operands_t;

বাহ্যিক শূন্য compile_code(অক্ষর*label, অক্ষর*mnem, comp_operands_t opa);

বাহ্যিক শূন্য compile_file_সারি(অক্ষর*label, long file_idx, long lineno,
                              অক্ষর*description);

বাহ্যিক শূন্য compile_vpi_call(অক্ষর*label, অক্ষর*name,
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
বাহ্যিক শূন্য compile_vpi_func_call(অক্ষর*label, অক্ষর*name,
				  পূর্ণসংখ্যা val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_স্তূপ,
				  unsigned real_স্তূপ,
				  unsigned string_স্তূপ);
বাহ্যিক শূন্য print_vpi_call_errors();

বাহ্যিক শূন্য compile_codelabel(অক্ষর*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
বাহ্যিক শূন্য compile_scope_decl(অক্ষর*typ, অক্ষর*lab, অক্ষর*nam, অক্ষর*tnam,
                               অক্ষর*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
বাহ্যিক শূন্য compile_scope_recall(অক্ষর*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
বাহ্যিক শূন্য compile_thread(অক্ষর*start_sym, অক্ষর*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
বাহ্যিক শূন্য compile_variable(অক্ষর*label, অক্ষর*name,
			     পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, পূর্ণসংখ্যা vpi_type_code,
			     bool signed_flag, bool local_flag);

বাহ্যিক শূন্য compile_var_real(অক্ষর*label, অক্ষর*name, bool local_flag);
বাহ্যিক শূন্য compile_var_string(অক্ষর*label, অক্ষর*name);
বাহ্যিক শূন্য compile_var_darray(অক্ষর*label, অক্ষর*name, unsigned আকার_2);
বাহ্যিক শূন্য compile_var_cobject(অক্ষর*label, অক্ষর*name);
বাহ্যিক শূন্য compile_var_queue(অক্ষর*label, অক্ষর*name, unsigned আকার_2);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

বাহ্যিক শূন্য compile_port_info( unsigned সূচক, পূর্ণসংখ্যা vpi_port_type, unsigned width, ধ্রুবক অক্ষর *name, অক্ষর* বাফার );


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
বাহ্যিক শূন্য compile_net(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
			পূর্ণসংখ্যা vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_net_real(অক্ষর*label, অক্ষর*name,
			     পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, bool local_flag,
			     unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_netw(অক্ষর*label, অক্ষর*array_label, unsigned long array_addr,
			 পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, পূর্ণসংখ্যা vpi_type_code, bool signed_flag,
			 unsigned argc, কাঠামো symb_s*argv);

বাহ্যিক শূন্য compile_netw_real(অক্ষর*label, অক্ষর*array_প্রতীক,
			      unsigned long array_addr,
			      পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
			      unsigned argc, কাঠামো symb_s*argv);


বাহ্যিক শূন্য compile_island(অক্ষর*label, অক্ষর*type);
বাহ্যিক শূন্য compile_island_port(অক্ষর*label, অক্ষর*island, অক্ষর*src);
বাহ্যিক শূন্য compile_island_import(অক্ষর*label, অক্ষর*island, অক্ষর*src);
বাহ্যিক শূন্য compile_island_export(অক্ষর*label, অক্ষর*island);
বাহ্যিক শূন্য compile_island_cleanup(শূন্য);

বাহ্যিক শূন্য compile_island_tran(অক্ষর*label);
বাহ্যিক শূন্য compile_island_tranif(পূর্ণসংখ্যা sense, অক্ষর*island,
				  অক্ষর*ba, অক্ষর*bb, অক্ষর*src, bool resistive);
বাহ্যিক শূন্য compile_island_tranvp(অক্ষর*island, অক্ষর*ba, অক্ষর*bb,
				  unsigned width, unsigned part, unsigned off);

বাহ্যিক শূন্য delete_udp_symbols(শূন্য);

বাহ্যিক শূন্য compile_class_start(অক্ষর*lab, অক্ষর*nam, unsigned nprop);
বাহ্যিক শূন্য compile_class_property(unsigned idx, অক্ষর*nam, অক্ষর*typ, uint64_t array_আকার_2);
বাহ্যিক শূন্য compile_class_done(শূন্য);

#endif /* IVL_compile_H */
