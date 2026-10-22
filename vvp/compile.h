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
بیرونی std::vector<ثابت نویسه*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

بیرونی تهی compile_init(تهی);

بیرونی تهی compile_cleanup(تهی);

بیرونی bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
بیرونی std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
بیرونی تهی inputs_connect(vvp_net_t*fdx, unsigned argc, ساختار symb_s*argv);
بیرونی تهی درون_داد_connect(vvp_net_t*fdx, unsigned port, نویسه*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
بیرونی تهی wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, ساختار symb_s*argv);

بیرونی vvp_net_t* vvp_net_lookup(ثابت نویسه*label);
بیرونی vpiHandle vvp_lookup_handle(ثابت نویسه*label);

/*
 *  Add a functor to the symbol table
 */
بیرونی تهی define_functor_نماد(ثابت نویسه*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
بیرونی unsigned compile_errors;

بیرونی ثابت نویسه* module_path;
بیرونی تهی compile_load_vpi_module(نویسه*name);

بیرونی تهی compile_vpi_time_precision(long pre);

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
بیرونی تهی compile_functor(نویسه*label, نویسه*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, ساختار symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
بیرونی تهی compile_resolver(نویسه*label, نویسه*type,
			     unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_concat(نویسه*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_concat8(نویسه*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_substitute(نویسه*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, ساختار symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
بیرونی تهی compile_compiletf(ساختار __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
بیرونی تهی compile_delay(نویسه*label, unsigned width,
                          vvp_delay_t*del, ساختار symb_s درون_داد);
بیرونی تهی compile_delay(نویسه*label, unsigned width,
                          unsigned argc, ساختار symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
بیرونی تهی compile_part_select(نویسه*label, نویسه*src,
				unsigned base, unsigned wid);
بیرونی تهی compile_part_select_pv(نویسه*label, نویسه*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
بیرونی تهی compile_part_select_var(نویسه*label, نویسه*src,
                                    نویسه*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
بیرونی تهی compile_arith_pow(نویسه*label, long width, bool signed_flag,
			      unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_abs(نویسه*label,
			      unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_cast_int(نویسه*label, long width,
			           unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_cast_real(نویسه*label, bool signed_flag,
			            unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_cast_vec2(نویسه*label, long width,
				    unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_div(نویسه*label, long width, bool signed_flag,
			      unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_mod(نویسه*label, long width, bool signed_flag,
			      unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_mult(نویسه*label, long width,
			       unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_sum(نویسه*label, long width,
			      unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_sub(نویسه*label, long width,
			      unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_eeq(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_nee(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_eq(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_eqx(نویسه*label, long width,
			    unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_eqz(نویسه*label, long width,
			    unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_ne(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_ge(نویسه*label, long width, bool signed_flag,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_gt(نویسه*label, long width, bool signed_flag,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_weq(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_wne(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_arith_mult_r(نویسه*label, unsigned argc,
                                 ساختار symb_s*argv);
بیرونی تهی compile_arith_pow_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_div_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_mod_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_sum_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_arith_sub_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_eq_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_ne_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_ge_r(نویسه*label, unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_cmp_gt_r(نویسه*label, unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_dff(نویسه*label, unsigned width, bool negedge,
			ساختار symb_s arg_d,
			ساختار symb_s arg_c,
			ساختار symb_s arg_e);

بیرونی تهی compile_dff_aclr(نویسه*label, unsigned width, bool negedge,
			     ساختار symb_s arg_d,
			     ساختار symb_s arg_c,
			     ساختار symb_s arg_e,
			     ساختار symb_s arg_a);
بیرونی تهی compile_dff_aset(نویسه*label, unsigned width, bool negedge,
			     ساختار symb_s arg_d,
			     ساختار symb_s arg_c,
			     ساختار symb_s arg_e,
			     ساختار symb_s arg_a,
			     نویسه*asc_مقدار);

بیرونی تهی compile_latch(نویسه*label, unsigned width,
			  ساختار symb_s arg_d,
			  ساختار symb_s arg_e);

بیرونی تهی compile_enum2_type(نویسه*label, long width, bool signed_flag,
			      std::list<ساختار enum_name_s>*names);
بیرونی تهی compile_enum4_type(نویسه*label, long width, bool signed_flag,
			      std::list<ساختار enum_name_s>*names);

ساختار __vpiModPath;
بیرونی __vpiModPath* compile_modpath(نویسه*label,
                                     unsigned width,
				     ساختار symb_s drv,
				     ساختار symb_s dest);
بیرونی تهی compile_modpath_src(__vpiModPath*dst,
				نویسه edge_c,
				ثابت ساختار symb_s&src,
				ساختار numbv_s&vals,
				ثابت ساختار symb_s&condit_src,
				ثابت ساختار symb_s&path_term_in);
بیرونی تهی compile_modpath_src(__vpiModPath*dst,
				نویسه edge_c,
				ثابت ساختار symb_s&src,
				ساختار numbv_s&vals,
				صحیح condit_src, /* match with '0' */
				ثابت ساختار symb_s&path_term_in,
				bool ifnone);

بیرونی تهی compile_reduce_and(نویسه*label, ثابت ساختار symb_s&arg);
بیرونی تهی compile_reduce_or(نویسه*label, ثابت ساختار symb_s&arg);
بیرونی تهی compile_reduce_xor(نویسه*label, ثابت ساختار symb_s&arg);
بیرونی تهی compile_reduce_nand(نویسه*label, ثابت ساختار symb_s&arg);
بیرونی تهی compile_reduce_nor(نویسه*label, ثابت ساختار symb_s&arg);
بیرونی تهی compile_reduce_xnor(نویسه*label, ثابت ساختار symb_s&arg);

بیرونی تهی compile_extend_signed(نویسه*label, long width, ساختار symb_s arg);

بیرونی تهی compile_sfunc(نویسه*label, نویسه*name, نویسه*format_string,
			  long file_idx, long lineno,
			  unsigned argc, ساختار symb_s*argv,
                          نویسه*trigger_label);

بیرونی تهی compile_repeat(نویسه*label, long width, long repeat,
			   ساختار symb_s arg);

بیرونی تهی compile_shiftl(نویسه*label, long width,
			   unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_shiftr(نویسه*label, long width, bool signed_flag,
			   unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_timescale(long units, long precision);

بیرونی تهی compile_vpi_نماد(ثابت نویسه*label, vpiHandle obj);
بیرونی تهی compile_vpi_lookup(vpiHandle *objref, نویسه*label);

بیرونی تهی compile_param_string(نویسه*label, ثابت نویسه*name, نویسه*مقدار,
                                 bool local_flag,
                                 long file_idx, long lineno);
بیرونی تهی compile_param_logic(نویسه*label, ثابت نویسه*name, نویسه*مقدار,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
بیرونی تهی compile_param_real(نویسه*label, ثابت نویسه*name, نویسه*مقدار,
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

    عمومی:
      explicit resolv_list_s(نویسه*lab) : label_(lab) {
	    next = پوچ;
      }
      مجازی ~resolv_list_s();

      resolv_list_s(ثابت resolv_list_s&) = delete;
      resolv_list_s& operator=(ثابت resolv_list_s&) = delete;

      مجازی bool resolve(bool mes = false) = 0;

    محافظتشده:
      ثابت نویسه*label() ثابت { بازگشت label_; }

    خصوصی:
      friend تهی resolv_submit(class resolv_list_s*cur);
      friend تهی compile_cleanup(تهی);

      نویسه*label_;
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
بیرونی تهی functor_ref_lookup(vvp_net_t**ref, نویسه*lab);

بیرونی vpiHandle vpip_make_prop_queue_ref(نویسه* class_label,
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
بیرونی تهی code_label_lookup(ساختار vvp_code_s *code, نویسه *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

بیرونی تهی compile_udp_def(صحیح sequ, نویسه*label, نویسه *name,
			    unsigned nin, unsigned init, نویسه **table);

بیرونی تهی compile_udp_functor(نویسه*label, نویسه*type,
				unsigned argc, ساختار symb_s*argv);

بیرونی نویسه **compile_udp_table(نویسه **table, نویسه *row);

/*
 * Memory Instances, Ports, and Initialization
 */

بیرونی تهی compile_var_array(نویسه*label, نویسه*name,
			      صحیح last, صحیح first,
			      صحیح msb, صحیح lsb, نویسه signed_flag);
بیرونی تهی compile_var2_array(نویسه*label, نویسه*name,
			      صحیح last, صحیح first,
			      صحیح msb, صحیح lsb, bool signed_flag);
بیرونی تهی compile_real_array(نویسه*label, نویسه*name,
			       صحیح last, صحیح first);
بیرونی تهی compile_string_array(نویسه*label, نویسه*name,
				 صحیح last, صحیح first);
بیرونی تهی compile_object_array(نویسه*label, نویسه*name,
				 صحیح last, صحیح first);
بیرونی تهی compile_net_array(نویسه*label, نویسه*name,
			      صحیح last, صحیح first);
بیرونی تهی compile_array_alias(نویسه*label, نویسه*name, نویسه*src);

  /* Index is a net. */
بیرونی تهی compile_array_port(نویسه*label, نویسه*name, نویسه*addr);
  /* Index is a constant address */
بیرونی تهی compile_array_port(نویسه*label, نویسه*name, long addr);

بیرونی تهی compile_array_cleanup(تهی);

/*
 * Compile the .ufunc statement.
 */
بیرونی تهی compile_ufunc_real(نویسه*label, نویسه*code, unsigned wid,
			  unsigned argc, ساختار symb_s*argv,
			  unsigned portc, ساختار symb_s*portv,
			  نویسه*scope_label, نویسه*trigger_label);
بیرونی تهی compile_ufunc_vec4(نویسه*label, نویسه*code, unsigned wid,
			  unsigned argc, ساختار symb_s*argv,
			  unsigned portc, ساختار symb_s*portv,
			  نویسه*scope_label, نویسه*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
بیرونی تهی compile_event(نویسه*label, نویسه*type,
			  unsigned argc, ساختار symb_s*argv);
بیرونی تهی compile_named_event(نویسه*label, نویسه*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
شمارش ltype_e { L_NUMB, L_SYMB, L_STRING };

ساختار comp_operands_s {
      unsigned argc;
      ساختار {
	    شمارش ltype_e ltype;
	    اجتماع {
		  unsigned long numb;
		  ساختار symb_s symb;
		  ثابت نویسه   *text;
	    };
      } argv[OPERAND_MAX];
};

تعریفنوع ساختار comp_operands_s*comp_operands_t;

بیرونی تهی compile_code(نویسه*label, نویسه*mnem, comp_operands_t opa);

بیرونی تهی compile_file_سطر(نویسه*label, long file_idx, long lineno,
                              نویسه*description);

بیرونی تهی compile_vpi_call(نویسه*label, نویسه*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_پشته,
			     unsigned real_پشته,
			     unsigned string_پشته);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
بیرونی تهی compile_vpi_func_call(نویسه*label, نویسه*name,
				  صحیح val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_پشته,
				  unsigned real_پشته,
				  unsigned string_پشته);
بیرونی تهی print_vpi_call_errors();

بیرونی تهی compile_codelabel(نویسه*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
بیرونی تهی compile_scope_decl(نویسه*typ, نویسه*lab, نویسه*nam, نویسه*tnam,
                               نویسه*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
بیرونی تهی compile_scope_recall(نویسه*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
بیرونی تهی compile_thread(نویسه*start_sym, نویسه*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
بیرونی تهی compile_variable(نویسه*label, نویسه*name,
			     صحیح msb, صحیح lsb, صحیح vpi_type_code,
			     bool signed_flag, bool local_flag);

بیرونی تهی compile_var_real(نویسه*label, نویسه*name, bool local_flag);
بیرونی تهی compile_var_string(نویسه*label, نویسه*name);
بیرونی تهی compile_var_darray(نویسه*label, نویسه*name, unsigned size);
بیرونی تهی compile_var_cobject(نویسه*label, نویسه*name);
بیرونی تهی compile_var_queue(نویسه*label, نویسه*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

بیرونی تهی compile_port_info( unsigned index, صحیح vpi_port_type, unsigned width, ثابت نویسه *name, نویسه* buffer );


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
بیرونی تهی compile_net(نویسه*label, نویسه*name, صحیح msb, صحیح lsb,
			صحیح vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_net_real(نویسه*label, نویسه*name,
			     صحیح msb, صحیح lsb, bool local_flag,
			     unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_netw(نویسه*label, نویسه*array_label, unsigned long array_addr,
			 صحیح msb, صحیح lsb, صحیح vpi_type_code, bool signed_flag,
			 unsigned argc, ساختار symb_s*argv);

بیرونی تهی compile_netw_real(نویسه*label, نویسه*array_نماد,
			      unsigned long array_addr,
			      صحیح msb, صحیح lsb,
			      unsigned argc, ساختار symb_s*argv);


بیرونی تهی compile_island(نویسه*label, نویسه*type);
بیرونی تهی compile_island_port(نویسه*label, نویسه*island, نویسه*src);
بیرونی تهی compile_island_import(نویسه*label, نویسه*island, نویسه*src);
بیرونی تهی compile_island_export(نویسه*label, نویسه*island);
بیرونی تهی compile_island_cleanup(تهی);

بیرونی تهی compile_island_tran(نویسه*label);
بیرونی تهی compile_island_tranif(صحیح sense, نویسه*island,
				  نویسه*ba, نویسه*bb, نویسه*src, bool resistive);
بیرونی تهی compile_island_tranvp(نویسه*island, نویسه*ba, نویسه*bb,
				  unsigned width, unsigned part, unsigned off);

بیرونی تهی delete_udp_symbols(تهی);

بیرونی تهی compile_class_start(نویسه*lab, نویسه*nam, unsigned nprop);
بیرونی تهی compile_class_property(unsigned idx, نویسه*nam, نویسه*typ, uint64_t array_size);
بیرونی تهی compile_class_done(تهی);

#endif /* IVL_compile_H */
