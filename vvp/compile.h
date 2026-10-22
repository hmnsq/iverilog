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
வெளி std::vector<மாறிலி எழுத்து*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

வெளி வெறுமை compile_init(வெறுமை);

வெளி வெறுமை compile_cleanup(வெறுமை);

வெளி bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
வெளி std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
வெளி வெறுமை inputs_connect(vvp_net_t*fdx, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை உள்ளீடு_connect(vvp_net_t*fdx, unsigned port, எழுத்து*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
வெளி வெறுமை wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி vvp_net_t* vvp_net_lookup(மாறிலி எழுத்து*label);
வெளி vpiHandle vvp_lookup_handle(மாறிலி எழுத்து*label);

/*
 *  Add a functor to the symbol table
 */
வெளி வெறுமை define_functor_குறியீடு(மாறிலி எழுத்து*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
வெளி unsigned compile_errors;

வெளி மாறிலி எழுத்து* module_path;
வெளி வெறுமை compile_load_vpi_module(எழுத்து*name);

வெளி வெறுமை compile_vpi_time_precision(long pre);

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
வெளி வெறுமை compile_functor(எழுத்து*label, எழுத்து*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, கட்டமைப்பு symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
வெளி வெறுமை compile_resolver(எழுத்து*label, எழுத்து*type,
			     unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_concat(எழுத்து*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_concat8(எழுத்து*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_substitute(எழுத்து*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, கட்டமைப்பு symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
வெளி வெறுமை compile_compiletf(கட்டமைப்பு __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
வெளி வெறுமை compile_delay(எழுத்து*label, unsigned width,
                          vvp_delay_t*del, கட்டமைப்பு symb_s உள்ளீடு);
வெளி வெறுமை compile_delay(எழுத்து*label, unsigned width,
                          unsigned argc, கட்டமைப்பு symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
வெளி வெறுமை compile_part_select(எழுத்து*label, எழுத்து*src,
				unsigned base, unsigned wid);
வெளி வெறுமை compile_part_select_pv(எழுத்து*label, எழுத்து*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
வெளி வெறுமை compile_part_select_var(எழுத்து*label, எழுத்து*src,
                                    எழுத்து*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
வெளி வெறுமை compile_arith_pow(எழுத்து*label, long width, bool signed_flag,
			      unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_abs(எழுத்து*label,
			      unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_cast_int(எழுத்து*label, long width,
			           unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_cast_real(எழுத்து*label, bool signed_flag,
			            unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_cast_vec2(எழுத்து*label, long width,
				    unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_div(எழுத்து*label, long width, bool signed_flag,
			      unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_mod(எழுத்து*label, long width, bool signed_flag,
			      unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_mult(எழுத்து*label, long width,
			       unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_sum(எழுத்து*label, long width,
			      unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_sub(எழுத்து*label, long width,
			      unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_eeq(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_nee(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_eq(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_eqx(எழுத்து*label, long width,
			    unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_eqz(எழுத்து*label, long width,
			    unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_ne(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_ge(எழுத்து*label, long width, bool signed_flag,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_gt(எழுத்து*label, long width, bool signed_flag,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_weq(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_wne(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_arith_mult_r(எழுத்து*label, unsigned argc,
                                 கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_pow_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_div_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_mod_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_sum_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_arith_sub_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_eq_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_ne_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_ge_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_cmp_gt_r(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_dff(எழுத்து*label, unsigned width, bool negedge,
			கட்டமைப்பு symb_s arg_d,
			கட்டமைப்பு symb_s arg_c,
			கட்டமைப்பு symb_s arg_e);

வெளி வெறுமை compile_dff_aclr(எழுத்து*label, unsigned width, bool negedge,
			     கட்டமைப்பு symb_s arg_d,
			     கட்டமைப்பு symb_s arg_c,
			     கட்டமைப்பு symb_s arg_e,
			     கட்டமைப்பு symb_s arg_a);
வெளி வெறுமை compile_dff_aset(எழுத்து*label, unsigned width, bool negedge,
			     கட்டமைப்பு symb_s arg_d,
			     கட்டமைப்பு symb_s arg_c,
			     கட்டமைப்பு symb_s arg_e,
			     கட்டமைப்பு symb_s arg_a,
			     எழுத்து*asc_மதிப்பு);

வெளி வெறுமை compile_latch(எழுத்து*label, unsigned width,
			  கட்டமைப்பு symb_s arg_d,
			  கட்டமைப்பு symb_s arg_e);

வெளி வெறுமை compile_enum2_type(எழுத்து*label, long width, bool signed_flag,
			      std::list<கட்டமைப்பு enum_name_s>*names);
வெளி வெறுமை compile_enum4_type(எழுத்து*label, long width, bool signed_flag,
			      std::list<கட்டமைப்பு enum_name_s>*names);

கட்டமைப்பு __vpiModPath;
வெளி __vpiModPath* compile_modpath(எழுத்து*label,
                                     unsigned width,
				     கட்டமைப்பு symb_s drv,
				     கட்டமைப்பு symb_s dest);
வெளி வெறுமை compile_modpath_src(__vpiModPath*dst,
				எழுத்து edge_c,
				மாறிலி கட்டமைப்பு symb_s&src,
				கட்டமைப்பு numbv_s&vals,
				மாறிலி கட்டமைப்பு symb_s&condit_src,
				மாறிலி கட்டமைப்பு symb_s&path_term_in);
வெளி வெறுமை compile_modpath_src(__vpiModPath*dst,
				எழுத்து edge_c,
				மாறிலி கட்டமைப்பு symb_s&src,
				கட்டமைப்பு numbv_s&vals,
				முழு condit_src, /* match with '0' */
				மாறிலி கட்டமைப்பு symb_s&path_term_in,
				bool ifnone);

வெளி வெறுமை compile_reduce_and(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg);
வெளி வெறுமை compile_reduce_or(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg);
வெளி வெறுமை compile_reduce_xor(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg);
வெளி வெறுமை compile_reduce_nand(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg);
வெளி வெறுமை compile_reduce_nor(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg);
வெளி வெறுமை compile_reduce_xnor(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg);

வெளி வெறுமை compile_extend_signed(எழுத்து*label, long width, கட்டமைப்பு symb_s arg);

வெளி வெறுமை compile_sfunc(எழுத்து*label, எழுத்து*name, எழுத்து*format_string,
			  long file_idx, long lineno,
			  unsigned argc, கட்டமைப்பு symb_s*argv,
                          எழுத்து*trigger_label);

வெளி வெறுமை compile_repeat(எழுத்து*label, long width, long repeat,
			   கட்டமைப்பு symb_s arg);

வெளி வெறுமை compile_shiftl(எழுத்து*label, long width,
			   unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_shiftr(எழுத்து*label, long width, bool signed_flag,
			   unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_timescale(long units, long precision);

வெளி வெறுமை compile_vpi_குறியீடு(மாறிலி எழுத்து*label, vpiHandle obj);
வெளி வெறுமை compile_vpi_lookup(vpiHandle *objref, எழுத்து*label);

வெளி வெறுமை compile_param_string(எழுத்து*label, மாறிலி எழுத்து*name, எழுத்து*மதிப்பு,
                                 bool local_flag,
                                 long file_idx, long lineno);
வெளி வெறுமை compile_param_logic(எழுத்து*label, மாறிலி எழுத்து*name, எழுத்து*மதிப்பு,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
வெளி வெறுமை compile_param_real(எழுத்து*label, மாறிலி எழுத்து*name, எழுத்து*மதிப்பு,
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

    பொது:
      explicit resolv_list_s(எழுத்து*lab) : label_(lab) {
	    next = வெற்று;
      }
      மெய்நிகர் ~resolv_list_s();

      resolv_list_s(மாறிலி resolv_list_s&) = delete;
      resolv_list_s& operator=(மாறிலி resolv_list_s&) = delete;

      மெய்நிகர் bool resolve(bool mes = false) = 0;

    பாதுகாக்கப்பட்ட:
      மாறிலி எழுத்து*label() மாறிலி { திரும்பு label_; }

    தனிப்பட்ட:
      friend வெறுமை resolv_submit(class resolv_list_s*cur);
      friend வெறுமை compile_cleanup(வெறுமை);

      எழுத்து*label_;
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
வெளி வெறுமை functor_ref_lookup(vvp_net_t**ref, எழுத்து*lab);

வெளி vpiHandle vpip_make_prop_queue_ref(எழுத்து* class_label,
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
வெளி வெறுமை code_label_lookup(கட்டமைப்பு vvp_code_s *code, எழுத்து *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

வெளி வெறுமை compile_udp_def(முழு sequ, எழுத்து*label, எழுத்து *name,
			    unsigned nin, unsigned init, எழுத்து **table);

வெளி வெறுமை compile_udp_functor(எழுத்து*label, எழுத்து*type,
				unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி எழுத்து **compile_udp_table(எழுத்து **table, எழுத்து *row);

/*
 * Memory Instances, Ports, and Initialization
 */

வெளி வெறுமை compile_var_array(எழுத்து*label, எழுத்து*name,
			      முழு last, முழு first,
			      முழு msb, முழு lsb, எழுத்து signed_flag);
வெளி வெறுமை compile_var2_array(எழுத்து*label, எழுத்து*name,
			      முழு last, முழு first,
			      முழு msb, முழு lsb, bool signed_flag);
வெளி வெறுமை compile_real_array(எழுத்து*label, எழுத்து*name,
			       முழு last, முழு first);
வெளி வெறுமை compile_string_array(எழுத்து*label, எழுத்து*name,
				 முழு last, முழு first);
வெளி வெறுமை compile_object_array(எழுத்து*label, எழுத்து*name,
				 முழு last, முழு first);
வெளி வெறுமை compile_net_array(எழுத்து*label, எழுத்து*name,
			      முழு last, முழு first);
வெளி வெறுமை compile_array_alias(எழுத்து*label, எழுத்து*name, எழுத்து*src);

  /* Index is a net. */
வெளி வெறுமை compile_array_port(எழுத்து*label, எழுத்து*name, எழுத்து*addr);
  /* Index is a constant address */
வெளி வெறுமை compile_array_port(எழுத்து*label, எழுத்து*name, long addr);

வெளி வெறுமை compile_array_cleanup(வெறுமை);

/*
 * Compile the .ufunc statement.
 */
வெளி வெறுமை compile_ufunc_real(எழுத்து*label, எழுத்து*code, unsigned wid,
			  unsigned argc, கட்டமைப்பு symb_s*argv,
			  unsigned portc, கட்டமைப்பு symb_s*portv,
			  எழுத்து*scope_label, எழுத்து*trigger_label);
வெளி வெறுமை compile_ufunc_vec4(எழுத்து*label, எழுத்து*code, unsigned wid,
			  unsigned argc, கட்டமைப்பு symb_s*argv,
			  unsigned portc, கட்டமைப்பு symb_s*portv,
			  எழுத்து*scope_label, எழுத்து*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
வெளி வெறுமை compile_event(எழுத்து*label, எழுத்து*type,
			  unsigned argc, கட்டமைப்பு symb_s*argv);
வெளி வெறுமை compile_named_event(எழுத்து*label, எழுத்து*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
எண்ணிக்கை ltype_e { L_NUMB, L_SYMB, L_STRING };

கட்டமைப்பு comp_operands_s {
      unsigned argc;
      கட்டமைப்பு {
	    எண்ணிக்கை ltype_e ltype;
	    ஒன்றிப்பு {
		  unsigned long numb;
		  கட்டமைப்பு symb_s symb;
		  மாறிலி எழுத்து   *text;
	    };
      } argv[OPERAND_MAX];
};

வகைவரை கட்டமைப்பு comp_operands_s*comp_operands_t;

வெளி வெறுமை compile_code(எழுத்து*label, எழுத்து*mnem, comp_operands_t opa);

வெளி வெறுமை compile_file_வரி(எழுத்து*label, long file_idx, long lineno,
                              எழுத்து*description);

வெளி வெறுமை compile_vpi_call(எழுத்து*label, எழுத்து*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_அடுக்கு,
			     unsigned real_அடுக்கு,
			     unsigned string_அடுக்கு);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
வெளி வெறுமை compile_vpi_func_call(எழுத்து*label, எழுத்து*name,
				  முழு val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_அடுக்கு,
				  unsigned real_அடுக்கு,
				  unsigned string_அடுக்கு);
வெளி வெறுமை print_vpi_call_errors();

வெளி வெறுமை compile_codelabel(எழுத்து*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
வெளி வெறுமை compile_scope_decl(எழுத்து*typ, எழுத்து*lab, எழுத்து*nam, எழுத்து*tnam,
                               எழுத்து*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
வெளி வெறுமை compile_scope_recall(எழுத்து*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
வெளி வெறுமை compile_thread(எழுத்து*start_sym, எழுத்து*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
வெளி வெறுமை compile_variable(எழுத்து*label, எழுத்து*name,
			     முழு msb, முழு lsb, முழு vpi_type_code,
			     bool signed_flag, bool local_flag);

வெளி வெறுமை compile_var_real(எழுத்து*label, எழுத்து*name, bool local_flag);
வெளி வெறுமை compile_var_string(எழுத்து*label, எழுத்து*name);
வெளி வெறுமை compile_var_darray(எழுத்து*label, எழுத்து*name, unsigned size);
வெளி வெறுமை compile_var_cobject(எழுத்து*label, எழுத்து*name);
வெளி வெறுமை compile_var_queue(எழுத்து*label, எழுத்து*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

வெளி வெறுமை compile_port_info( unsigned index, முழு vpi_port_type, unsigned width, மாறிலி எழுத்து *name, எழுத்து* buffer );


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
வெளி வெறுமை compile_net(எழுத்து*label, எழுத்து*name, முழு msb, முழு lsb,
			முழு vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_net_real(எழுத்து*label, எழுத்து*name,
			     முழு msb, முழு lsb, bool local_flag,
			     unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_netw(எழுத்து*label, எழுத்து*array_label, unsigned long array_addr,
			 முழு msb, முழு lsb, முழு vpi_type_code, bool signed_flag,
			 unsigned argc, கட்டமைப்பு symb_s*argv);

வெளி வெறுமை compile_netw_real(எழுத்து*label, எழுத்து*array_குறியீடு,
			      unsigned long array_addr,
			      முழு msb, முழு lsb,
			      unsigned argc, கட்டமைப்பு symb_s*argv);


வெளி வெறுமை compile_island(எழுத்து*label, எழுத்து*type);
வெளி வெறுமை compile_island_port(எழுத்து*label, எழுத்து*island, எழுத்து*src);
வெளி வெறுமை compile_island_import(எழுத்து*label, எழுத்து*island, எழுத்து*src);
வெளி வெறுமை compile_island_export(எழுத்து*label, எழுத்து*island);
வெளி வெறுமை compile_island_cleanup(வெறுமை);

வெளி வெறுமை compile_island_tran(எழுத்து*label);
வெளி வெறுமை compile_island_tranif(முழு sense, எழுத்து*island,
				  எழுத்து*ba, எழுத்து*bb, எழுத்து*src, bool resistive);
வெளி வெறுமை compile_island_tranvp(எழுத்து*island, எழுத்து*ba, எழுத்து*bb,
				  unsigned width, unsigned part, unsigned off);

வெளி வெறுமை delete_udp_symbols(வெறுமை);

வெளி வெறுமை compile_class_start(எழுத்து*lab, எழுத்து*nam, unsigned nprop);
வெளி வெறுமை compile_class_property(unsigned idx, எழுத்து*nam, எழுத்து*typ, uint64_t array_size);
வெளி வெறுமை compile_class_done(வெறுமை);

#endif /* IVL_compile_H */
