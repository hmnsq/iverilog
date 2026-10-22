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
બાહ્ય std::vector<અચલ અક્ષર*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

બાહ્ય શૂન્ય compile_init(શૂન્ય);

બાહ્ય શૂન્ય compile_cleanup(શૂન્ય);

બાહ્ય bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
બાહ્ય std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
બાહ્ય શૂન્ય inputs_connect(vvp_net_t*fdx, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય આગત_connect(vvp_net_t*fdx, unsigned port, અક્ષર*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
બાહ્ય શૂન્ય wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, સંરચના symb_s*argv);

બાહ્ય vvp_net_t* vvp_net_lookup(અચલ અક્ષર*label);
બાહ્ય vpiHandle vvp_lookup_handle(અચલ અક્ષર*label);

/*
 *  Add a functor to the symbol table
 */
બાહ્ય શૂન્ય define_functor_પ્રતીક(અચલ અક્ષર*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
બાહ્ય unsigned compile_errors;

બાહ્ય અચલ અક્ષર* module_path;
બાહ્ય શૂન્ય compile_load_vpi_module(અક્ષર*name);

બાહ્ય શૂન્ય compile_vpi_time_precision(long pre);

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
બાહ્ય શૂન્ય compile_functor(અક્ષર*label, અક્ષર*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, સંરચના symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
બાહ્ય શૂન્ય compile_resolver(અક્ષર*label, અક્ષર*type,
			     unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_concat(અક્ષર*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_concat8(અક્ષર*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_substitute(અક્ષર*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, સંરચના symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
બાહ્ય શૂન્ય compile_compiletf(સંરચના __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
બાહ્ય શૂન્ય compile_delay(અક્ષર*label, unsigned width,
                          vvp_delay_t*del, સંરચના symb_s આગત);
બાહ્ય શૂન્ય compile_delay(અક્ષર*label, unsigned width,
                          unsigned argc, સંરચના symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
બાહ્ય શૂન્ય compile_part_select(અક્ષર*label, અક્ષર*src,
				unsigned base, unsigned wid);
બાહ્ય શૂન્ય compile_part_select_pv(અક્ષર*label, અક્ષર*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
બાહ્ય શૂન્ય compile_part_select_var(અક્ષર*label, અક્ષર*src,
                                    અક્ષર*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
બાહ્ય શૂન્ય compile_arith_pow(અક્ષર*label, long width, bool signed_flag,
			      unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_abs(અક્ષર*label,
			      unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_cast_int(અક્ષર*label, long width,
			           unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_cast_real(અક્ષર*label, bool signed_flag,
			            unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_cast_vec2(અક્ષર*label, long width,
				    unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_div(અક્ષર*label, long width, bool signed_flag,
			      unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_mod(અક્ષર*label, long width, bool signed_flag,
			      unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_mult(અક્ષર*label, long width,
			       unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_sum(અક્ષર*label, long width,
			      unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_sub(અક્ષર*label, long width,
			      unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_eeq(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_nee(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_eq(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_eqx(અક્ષર*label, long width,
			    unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_eqz(અક્ષર*label, long width,
			    unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_ne(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_ge(અક્ષર*label, long width, bool signed_flag,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_gt(અક્ષર*label, long width, bool signed_flag,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_weq(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_wne(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_arith_mult_r(અક્ષર*label, unsigned argc,
                                 સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_pow_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_div_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_mod_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_sum_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_arith_sub_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_eq_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_ne_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_ge_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_cmp_gt_r(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_dff(અક્ષર*label, unsigned width, bool negedge,
			સંરચના symb_s arg_d,
			સંરચના symb_s arg_c,
			સંરચના symb_s arg_e);

બાહ્ય શૂન્ય compile_dff_aclr(અક્ષર*label, unsigned width, bool negedge,
			     સંરચના symb_s arg_d,
			     સંરચના symb_s arg_c,
			     સંરચના symb_s arg_e,
			     સંરચના symb_s arg_a);
બાહ્ય શૂન્ય compile_dff_aset(અક્ષર*label, unsigned width, bool negedge,
			     સંરચના symb_s arg_d,
			     સંરચના symb_s arg_c,
			     સંરચના symb_s arg_e,
			     સંરચના symb_s arg_a,
			     અક્ષર*asc_કિંમત);

બાહ્ય શૂન્ય compile_latch(અક્ષર*label, unsigned width,
			  સંરચના symb_s arg_d,
			  સંરચના symb_s arg_e);

બાહ્ય શૂન્ય compile_enum2_type(અક્ષર*label, long width, bool signed_flag,
			      std::list<સંરચના enum_name_s>*names);
બાહ્ય શૂન્ય compile_enum4_type(અક્ષર*label, long width, bool signed_flag,
			      std::list<સંરચના enum_name_s>*names);

સંરચના __vpiModPath;
બાહ્ય __vpiModPath* compile_modpath(અક્ષર*label,
                                     unsigned width,
				     સંરચના symb_s drv,
				     સંરચના symb_s dest);
બાહ્ય શૂન્ય compile_modpath_src(__vpiModPath*dst,
				અક્ષર edge_c,
				અચલ સંરચના symb_s&src,
				સંરચના numbv_s&vals,
				અચલ સંરચના symb_s&condit_src,
				અચલ સંરચના symb_s&path_term_in);
બાહ્ય શૂન્ય compile_modpath_src(__vpiModPath*dst,
				અક્ષર edge_c,
				અચલ સંરચના symb_s&src,
				સંરચના numbv_s&vals,
				પૂર્ણાંક condit_src, /* match with '0' */
				અચલ સંરચના symb_s&path_term_in,
				bool ifnone);

બાહ્ય શૂન્ય compile_reduce_and(અક્ષર*label, અચલ સંરચના symb_s&arg);
બાહ્ય શૂન્ય compile_reduce_or(અક્ષર*label, અચલ સંરચના symb_s&arg);
બાહ્ય શૂન્ય compile_reduce_xor(અક્ષર*label, અચલ સંરચના symb_s&arg);
બાહ્ય શૂન્ય compile_reduce_nand(અક્ષર*label, અચલ સંરચના symb_s&arg);
બાહ્ય શૂન્ય compile_reduce_nor(અક્ષર*label, અચલ સંરચના symb_s&arg);
બાહ્ય શૂન્ય compile_reduce_xnor(અક્ષર*label, અચલ સંરચના symb_s&arg);

બાહ્ય શૂન્ય compile_extend_signed(અક્ષર*label, long width, સંરચના symb_s arg);

બાહ્ય શૂન્ય compile_sfunc(અક્ષર*label, અક્ષર*name, અક્ષર*format_string,
			  long file_idx, long lineno,
			  unsigned argc, સંરચના symb_s*argv,
                          અક્ષર*trigger_label);

બાહ્ય શૂન્ય compile_repeat(અક્ષર*label, long width, long repeat,
			   સંરચના symb_s arg);

બાહ્ય શૂન્ય compile_shiftl(અક્ષર*label, long width,
			   unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_shiftr(અક્ષર*label, long width, bool signed_flag,
			   unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_timescale(long units, long precision);

બાહ્ય શૂન્ય compile_vpi_પ્રતીક(અચલ અક્ષર*label, vpiHandle obj);
બાહ્ય શૂન્ય compile_vpi_lookup(vpiHandle *objref, અક્ષર*label);

બાહ્ય શૂન્ય compile_param_string(અક્ષર*label, અચલ અક્ષર*name, અક્ષર*કિંમત,
                                 bool local_flag,
                                 long file_idx, long lineno);
બાહ્ય શૂન્ય compile_param_logic(અક્ષર*label, અચલ અક્ષર*name, અક્ષર*કિંમત,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
બાહ્ય શૂન્ય compile_param_real(અક્ષર*label, અચલ અક્ષર*name, અક્ષર*કિંમત,
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

    જાહેર:
      explicit resolv_list_s(અક્ષર*lab) : label_(lab) {
	    next = ખાલી;
      }
      આભાસી ~resolv_list_s();

      resolv_list_s(અચલ resolv_list_s&) = delete;
      resolv_list_s& operator=(અચલ resolv_list_s&) = delete;

      આભાસી bool resolve(bool mes = false) = 0;

    સુરક્ષિત:
      અચલ અક્ષર*label() અચલ { પાછા label_; }

    ખાનગી:
      friend શૂન્ય resolv_submit(class resolv_list_s*cur);
      friend શૂન્ય compile_cleanup(શૂન્ય);

      અક્ષર*label_;
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
બાહ્ય શૂન્ય functor_ref_lookup(vvp_net_t**ref, અક્ષર*lab);

બાહ્ય vpiHandle vpip_make_prop_queue_ref(અક્ષર* class_label,
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
બાહ્ય શૂન્ય code_label_lookup(સંરચના vvp_code_s *code, અક્ષર *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

બાહ્ય શૂન્ય compile_udp_def(પૂર્ણાંક sequ, અક્ષર*label, અક્ષર *name,
			    unsigned nin, unsigned init, અક્ષર **table);

બાહ્ય શૂન્ય compile_udp_functor(અક્ષર*label, અક્ષર*type,
				unsigned argc, સંરચના symb_s*argv);

બાહ્ય અક્ષર **compile_udp_table(અક્ષર **table, અક્ષર *row);

/*
 * Memory Instances, Ports, and Initialization
 */

બાહ્ય શૂન્ય compile_var_array(અક્ષર*label, અક્ષર*name,
			      પૂર્ણાંક last, પૂર્ણાંક first,
			      પૂર્ણાંક msb, પૂર્ણાંક lsb, અક્ષર signed_flag);
બાહ્ય શૂન્ય compile_var2_array(અક્ષર*label, અક્ષર*name,
			      પૂર્ણાંક last, પૂર્ણાંક first,
			      પૂર્ણાંક msb, પૂર્ણાંક lsb, bool signed_flag);
બાહ્ય શૂન્ય compile_real_array(અક્ષર*label, અક્ષર*name,
			       પૂર્ણાંક last, પૂર્ણાંક first);
બાહ્ય શૂન્ય compile_string_array(અક્ષર*label, અક્ષર*name,
				 પૂર્ણાંક last, પૂર્ણાંક first);
બાહ્ય શૂન્ય compile_object_array(અક્ષર*label, અક્ષર*name,
				 પૂર્ણાંક last, પૂર્ણાંક first);
બાહ્ય શૂન્ય compile_net_array(અક્ષર*label, અક્ષર*name,
			      પૂર્ણાંક last, પૂર્ણાંક first);
બાહ્ય શૂન્ય compile_array_alias(અક્ષર*label, અક્ષર*name, અક્ષર*src);

  /* Index is a net. */
બાહ્ય શૂન્ય compile_array_port(અક્ષર*label, અક્ષર*name, અક્ષર*addr);
  /* Index is a constant address */
બાહ્ય શૂન્ય compile_array_port(અક્ષર*label, અક્ષર*name, long addr);

બાહ્ય શૂન્ય compile_array_cleanup(શૂન્ય);

/*
 * Compile the .ufunc statement.
 */
બાહ્ય શૂન્ય compile_ufunc_real(અક્ષર*label, અક્ષર*code, unsigned wid,
			  unsigned argc, સંરચના symb_s*argv,
			  unsigned portc, સંરચના symb_s*portv,
			  અક્ષર*scope_label, અક્ષર*trigger_label);
બાહ્ય શૂન્ય compile_ufunc_vec4(અક્ષર*label, અક્ષર*code, unsigned wid,
			  unsigned argc, સંરચના symb_s*argv,
			  unsigned portc, સંરચના symb_s*portv,
			  અક્ષર*scope_label, અક્ષર*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
બાહ્ય શૂન્ય compile_event(અક્ષર*label, અક્ષર*type,
			  unsigned argc, સંરચના symb_s*argv);
બાહ્ય શૂન્ય compile_named_event(અક્ષર*label, અક્ષર*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
ગણના ltype_e { L_NUMB, L_SYMB, L_STRING };

સંરચના comp_operands_s {
      unsigned argc;
      સંરચના {
	    ગણના ltype_e ltype;
	    સંઘ {
		  unsigned long numb;
		  સંરચના symb_s symb;
		  અચલ અક્ષર   *text;
	    };
      } argv[OPERAND_MAX];
};

પ્રકારવ્યાખ્યા સંરચના comp_operands_s*comp_operands_t;

બાહ્ય શૂન્ય compile_code(અક્ષર*label, અક્ષર*mnem, comp_operands_t opa);

બાહ્ય શૂન્ય compile_file_લીટી(અક્ષર*label, long file_idx, long lineno,
                              અક્ષર*description);

બાહ્ય શૂન્ય compile_vpi_call(અક્ષર*label, અક્ષર*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_ઢગલો,
			     unsigned real_ઢગલો,
			     unsigned string_ઢગલો);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
બાહ્ય શૂન્ય compile_vpi_func_call(અક્ષર*label, અક્ષર*name,
				  પૂર્ણાંક val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_ઢગલો,
				  unsigned real_ઢગલો,
				  unsigned string_ઢગલો);
બાહ્ય શૂન્ય print_vpi_call_errors();

બાહ્ય શૂન્ય compile_codelabel(અક્ષર*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
બાહ્ય શૂન્ય compile_scope_decl(અક્ષર*typ, અક્ષર*lab, અક્ષર*nam, અક્ષર*tnam,
                               અક્ષર*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
બાહ્ય શૂન્ય compile_scope_recall(અક્ષર*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
બાહ્ય શૂન્ય compile_thread(અક્ષર*start_sym, અક્ષર*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
બાહ્ય શૂન્ય compile_variable(અક્ષર*label, અક્ષર*name,
			     પૂર્ણાંક msb, પૂર્ણાંક lsb, પૂર્ણાંક vpi_type_code,
			     bool signed_flag, bool local_flag);

બાહ્ય શૂન્ય compile_var_real(અક્ષર*label, અક્ષર*name, bool local_flag);
બાહ્ય શૂન્ય compile_var_string(અક્ષર*label, અક્ષર*name);
બાહ્ય શૂન્ય compile_var_darray(અક્ષર*label, અક્ષર*name, unsigned size);
બાહ્ય શૂન્ય compile_var_cobject(અક્ષર*label, અક્ષર*name);
બાહ્ય શૂન્ય compile_var_queue(અક્ષર*label, અક્ષર*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

બાહ્ય શૂન્ય compile_port_info( unsigned index, પૂર્ણાંક vpi_port_type, unsigned width, અચલ અક્ષર *name, અક્ષર* buffer );


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
બાહ્ય શૂન્ય compile_net(અક્ષર*label, અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
			પૂર્ણાંક vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_net_real(અક્ષર*label, અક્ષર*name,
			     પૂર્ણાંક msb, પૂર્ણાંક lsb, bool local_flag,
			     unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_netw(અક્ષર*label, અક્ષર*array_label, unsigned long array_addr,
			 પૂર્ણાંક msb, પૂર્ણાંક lsb, પૂર્ણાંક vpi_type_code, bool signed_flag,
			 unsigned argc, સંરચના symb_s*argv);

બાહ્ય શૂન્ય compile_netw_real(અક્ષર*label, અક્ષર*array_પ્રતીક,
			      unsigned long array_addr,
			      પૂર્ણાંક msb, પૂર્ણાંક lsb,
			      unsigned argc, સંરચના symb_s*argv);


બાહ્ય શૂન્ય compile_island(અક્ષર*label, અક્ષર*type);
બાહ્ય શૂન્ય compile_island_port(અક્ષર*label, અક્ષર*island, અક્ષર*src);
બાહ્ય શૂન્ય compile_island_import(અક્ષર*label, અક્ષર*island, અક્ષર*src);
બાહ્ય શૂન્ય compile_island_export(અક્ષર*label, અક્ષર*island);
બાહ્ય શૂન્ય compile_island_cleanup(શૂન્ય);

બાહ્ય શૂન્ય compile_island_tran(અક્ષર*label);
બાહ્ય શૂન્ય compile_island_tranif(પૂર્ણાંક sense, અક્ષર*island,
				  અક્ષર*ba, અક્ષર*bb, અક્ષર*src, bool resistive);
બાહ્ય શૂન્ય compile_island_tranvp(અક્ષર*island, અક્ષર*ba, અક્ષર*bb,
				  unsigned width, unsigned part, unsigned off);

બાહ્ય શૂન્ય delete_udp_symbols(શૂન્ય);

બાહ્ય શૂન્ય compile_class_start(અક્ષર*lab, અક્ષર*nam, unsigned nprop);
બાહ્ય શૂન્ય compile_class_property(unsigned idx, અક્ષર*nam, અક્ષર*typ, uint64_t array_size);
બાહ્ય શૂન્ય compile_class_done(શૂન્ય);

#endif /* IVL_compile_H */
