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
बाह्य std::vector<ध्रुव वर्ण*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

बाह्य शून्य compile_init(शून्य);

बाह्य शून्य compile_cleanup(शून्य);

बाह्य bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
बाह्य std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
बाह्य शून्य inputs_connect(vvp_net_t*fdx, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य आदान_connect(vvp_net_t*fdx, unsigned port, वर्ण*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
बाह्य शून्य wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, संरचना symb_s*argv);

बाह्य vvp_net_t* vvp_net_lookup(ध्रुव वर्ण*label);
बाह्य vpiHandle vvp_lookup_handle(ध्रुव वर्ण*label);

/*
 *  Add a functor to the symbol table
 */
बाह्य शून्य define_functor_चिन्ह(ध्रुव वर्ण*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
बाह्य unsigned compile_errors;

बाह्य ध्रुव वर्ण* module_path;
बाह्य शून्य compile_load_vpi_module(वर्ण*name);

बाह्य शून्य compile_vpi_time_precision(long pre);

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
बाह्य शून्य compile_functor(वर्ण*label, वर्ण*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, संरचना symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
बाह्य शून्य compile_resolver(वर्ण*label, वर्ण*type,
			     unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_concat(वर्ण*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_concat8(वर्ण*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_substitute(वर्ण*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, संरचना symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
बाह्य शून्य compile_compiletf(संरचना __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
बाह्य शून्य compile_delay(वर्ण*label, unsigned width,
                          vvp_delay_t*del, संरचना symb_s आदान);
बाह्य शून्य compile_delay(वर्ण*label, unsigned width,
                          unsigned argc, संरचना symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
बाह्य शून्य compile_part_select(वर्ण*label, वर्ण*src,
				unsigned base, unsigned wid);
बाह्य शून्य compile_part_select_pv(वर्ण*label, वर्ण*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
बाह्य शून्य compile_part_select_var(वर्ण*label, वर्ण*src,
                                    वर्ण*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
बाह्य शून्य compile_arith_pow(वर्ण*label, long width, bool signed_flag,
			      unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_abs(वर्ण*label,
			      unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_cast_int(वर्ण*label, long width,
			           unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_cast_real(वर्ण*label, bool signed_flag,
			            unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_cast_vec2(वर्ण*label, long width,
				    unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_div(वर्ण*label, long width, bool signed_flag,
			      unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_mod(वर्ण*label, long width, bool signed_flag,
			      unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_mult(वर्ण*label, long width,
			       unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sum(वर्ण*label, long width,
			      unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sub(वर्ण*label, long width,
			      unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eeq(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_nee(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eq(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eqx(वर्ण*label, long width,
			    unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eqz(वर्ण*label, long width,
			    unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ne(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ge(वर्ण*label, long width, bool signed_flag,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_gt(वर्ण*label, long width, bool signed_flag,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_weq(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_wne(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_arith_mult_r(वर्ण*label, unsigned argc,
                                 संरचना symb_s*argv);
बाह्य शून्य compile_arith_pow_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_div_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_mod_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sum_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sub_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eq_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ne_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ge_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_gt_r(वर्ण*label, unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_dff(वर्ण*label, unsigned width, bool negedge,
			संरचना symb_s arg_d,
			संरचना symb_s arg_c,
			संरचना symb_s arg_e);

बाह्य शून्य compile_dff_aclr(वर्ण*label, unsigned width, bool negedge,
			     संरचना symb_s arg_d,
			     संरचना symb_s arg_c,
			     संरचना symb_s arg_e,
			     संरचना symb_s arg_a);
बाह्य शून्य compile_dff_aset(वर्ण*label, unsigned width, bool negedge,
			     संरचना symb_s arg_d,
			     संरचना symb_s arg_c,
			     संरचना symb_s arg_e,
			     संरचना symb_s arg_a,
			     वर्ण*asc_मूल्य);

बाह्य शून्य compile_latch(वर्ण*label, unsigned width,
			  संरचना symb_s arg_d,
			  संरचना symb_s arg_e);

बाह्य शून्य compile_enum2_type(वर्ण*label, long width, bool signed_flag,
			      std::list<संरचना enum_name_s>*names);
बाह्य शून्य compile_enum4_type(वर्ण*label, long width, bool signed_flag,
			      std::list<संरचना enum_name_s>*names);

संरचना __vpiModPath;
बाह्य __vpiModPath* compile_modpath(वर्ण*label,
                                     unsigned width,
				     संरचना symb_s drv,
				     संरचना symb_s dest);
बाह्य शून्य compile_modpath_src(__vpiModPath*dst,
				वर्ण edge_c,
				ध्रुव संरचना symb_s&src,
				संरचना numbv_s&vals,
				ध्रुव संरचना symb_s&condit_src,
				ध्रुव संरचना symb_s&path_term_in);
बाह्य शून्य compile_modpath_src(__vpiModPath*dst,
				वर्ण edge_c,
				ध्रुव संरचना symb_s&src,
				संरचना numbv_s&vals,
				पूर्णाङ्क condit_src, /* match with '0' */
				ध्रुव संरचना symb_s&path_term_in,
				bool ifnone);

बाह्य शून्य compile_reduce_and(वर्ण*label, ध्रुव संरचना symb_s&arg);
बाह्य शून्य compile_reduce_or(वर्ण*label, ध्रुव संरचना symb_s&arg);
बाह्य शून्य compile_reduce_xor(वर्ण*label, ध्रुव संरचना symb_s&arg);
बाह्य शून्य compile_reduce_nand(वर्ण*label, ध्रुव संरचना symb_s&arg);
बाह्य शून्य compile_reduce_nor(वर्ण*label, ध्रुव संरचना symb_s&arg);
बाह्य शून्य compile_reduce_xnor(वर्ण*label, ध्रुव संरचना symb_s&arg);

बाह्य शून्य compile_extend_signed(वर्ण*label, long width, संरचना symb_s arg);

बाह्य शून्य compile_sfunc(वर्ण*label, वर्ण*name, वर्ण*format_string,
			  long file_idx, long lineno,
			  unsigned argc, संरचना symb_s*argv,
                          वर्ण*trigger_label);

बाह्य शून्य compile_repeat(वर्ण*label, long width, long repeat,
			   संरचना symb_s arg);

बाह्य शून्य compile_shiftl(वर्ण*label, long width,
			   unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_shiftr(वर्ण*label, long width, bool signed_flag,
			   unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_timescale(long units, long precision);

बाह्य शून्य compile_vpi_चिन्ह(ध्रुव वर्ण*label, vpiHandle obj);
बाह्य शून्य compile_vpi_lookup(vpiHandle *objref, वर्ण*label);

बाह्य शून्य compile_param_string(वर्ण*label, ध्रुव वर्ण*name, वर्ण*मूल्य,
                                 bool local_flag,
                                 long file_idx, long lineno);
बाह्य शून्य compile_param_logic(वर्ण*label, ध्रुव वर्ण*name, वर्ण*मूल्य,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
बाह्य शून्य compile_param_real(वर्ण*label, ध्रुव वर्ण*name, वर्ण*मूल्य,
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

    सार्वजनिक:
      explicit resolv_list_s(वर्ण*lab) : label_(lab) {
	    next = रिक्त;
      }
      आभास ~resolv_list_s();

      resolv_list_s(ध्रुव resolv_list_s&) = delete;
      resolv_list_s& operator=(ध्रुव resolv_list_s&) = delete;

      आभास bool resolve(bool mes = false) = 0;

    रक्षित:
      ध्रुव वर्ण*label() ध्रुव { प्रत्यागम label_; }

    गुप्त:
      friend शून्य resolv_submit(class resolv_list_s*cur);
      friend शून्य compile_cleanup(शून्य);

      वर्ण*label_;
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
बाह्य शून्य functor_ref_lookup(vvp_net_t**ref, वर्ण*lab);

बाह्य vpiHandle vpip_make_prop_queue_ref(वर्ण* class_label,
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
बाह्य शून्य code_label_lookup(संरचना vvp_code_s *code, वर्ण *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

बाह्य शून्य compile_udp_def(पूर्णाङ्क sequ, वर्ण*label, वर्ण *name,
			    unsigned nin, unsigned init, वर्ण **table);

बाह्य शून्य compile_udp_functor(वर्ण*label, वर्ण*type,
				unsigned argc, संरचना symb_s*argv);

बाह्य वर्ण **compile_udp_table(वर्ण **table, वर्ण *row);

/*
 * Memory Instances, Ports, and Initialization
 */

बाह्य शून्य compile_var_array(वर्ण*label, वर्ण*name,
			      पूर्णाङ्क last, पूर्णाङ्क first,
			      पूर्णाङ्क msb, पूर्णाङ्क lsb, वर्ण signed_flag);
बाह्य शून्य compile_var2_array(वर्ण*label, वर्ण*name,
			      पूर्णाङ्क last, पूर्णाङ्क first,
			      पूर्णाङ्क msb, पूर्णाङ्क lsb, bool signed_flag);
बाह्य शून्य compile_real_array(वर्ण*label, वर्ण*name,
			       पूर्णाङ्क last, पूर्णाङ्क first);
बाह्य शून्य compile_string_array(वर्ण*label, वर्ण*name,
				 पूर्णाङ्क last, पूर्णाङ्क first);
बाह्य शून्य compile_object_array(वर्ण*label, वर्ण*name,
				 पूर्णाङ्क last, पूर्णाङ्क first);
बाह्य शून्य compile_net_array(वर्ण*label, वर्ण*name,
			      पूर्णाङ्क last, पूर्णाङ्क first);
बाह्य शून्य compile_array_alias(वर्ण*label, वर्ण*name, वर्ण*src);

  /* Index is a net. */
बाह्य शून्य compile_array_port(वर्ण*label, वर्ण*name, वर्ण*addr);
  /* Index is a constant address */
बाह्य शून्य compile_array_port(वर्ण*label, वर्ण*name, long addr);

बाह्य शून्य compile_array_cleanup(शून्य);

/*
 * Compile the .ufunc statement.
 */
बाह्य शून्य compile_ufunc_real(वर्ण*label, वर्ण*code, unsigned wid,
			  unsigned argc, संरचना symb_s*argv,
			  unsigned portc, संरचना symb_s*portv,
			  वर्ण*scope_label, वर्ण*trigger_label);
बाह्य शून्य compile_ufunc_vec4(वर्ण*label, वर्ण*code, unsigned wid,
			  unsigned argc, संरचना symb_s*argv,
			  unsigned portc, संरचना symb_s*portv,
			  वर्ण*scope_label, वर्ण*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
बाह्य शून्य compile_event(वर्ण*label, वर्ण*type,
			  unsigned argc, संरचना symb_s*argv);
बाह्य शून्य compile_named_event(वर्ण*label, वर्ण*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
गणना ltype_e { L_NUMB, L_SYMB, L_STRING };

संरचना comp_operands_s {
      unsigned argc;
      संरचना {
	    गणना ltype_e ltype;
	    सङ्घ {
		  unsigned long numb;
		  संरचना symb_s symb;
		  ध्रुव वर्ण   *text;
	    };
      } argv[OPERAND_MAX];
};

प्रकारनिर्देश संरचना comp_operands_s*comp_operands_t;

बाह्य शून्य compile_code(वर्ण*label, वर्ण*mnem, comp_operands_t opa);

बाह्य शून्य compile_file_ओळ(वर्ण*label, long file_idx, long lineno,
                              वर्ण*description);

बाह्य शून्य compile_vpi_call(वर्ण*label, वर्ण*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_चळत,
			     unsigned real_चळत,
			     unsigned string_चळत);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
बाह्य शून्य compile_vpi_func_call(वर्ण*label, वर्ण*name,
				  पूर्णाङ्क val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_चळत,
				  unsigned real_चळत,
				  unsigned string_चळत);
बाह्य शून्य print_vpi_call_errors();

बाह्य शून्य compile_codelabel(वर्ण*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
बाह्य शून्य compile_scope_decl(वर्ण*typ, वर्ण*lab, वर्ण*nam, वर्ण*tnam,
                               वर्ण*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
बाह्य शून्य compile_scope_recall(वर्ण*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
बाह्य शून्य compile_thread(वर्ण*start_sym, वर्ण*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
बाह्य शून्य compile_variable(वर्ण*label, वर्ण*name,
			     पूर्णाङ्क msb, पूर्णाङ्क lsb, पूर्णाङ्क vpi_type_code,
			     bool signed_flag, bool local_flag);

बाह्य शून्य compile_var_real(वर्ण*label, वर्ण*name, bool local_flag);
बाह्य शून्य compile_var_string(वर्ण*label, वर्ण*name);
बाह्य शून्य compile_var_darray(वर्ण*label, वर्ण*name, unsigned size);
बाह्य शून्य compile_var_cobject(वर्ण*label, वर्ण*name);
बाह्य शून्य compile_var_queue(वर्ण*label, वर्ण*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

बाह्य शून्य compile_port_info( unsigned index, पूर्णाङ्क vpi_port_type, unsigned width, ध्रुव वर्ण *name, वर्ण* buffer );


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
बाह्य शून्य compile_net(वर्ण*label, वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			पूर्णाङ्क vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_net_real(वर्ण*label, वर्ण*name,
			     पूर्णाङ्क msb, पूर्णाङ्क lsb, bool local_flag,
			     unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_netw(वर्ण*label, वर्ण*array_label, unsigned long array_addr,
			 पूर्णाङ्क msb, पूर्णाङ्क lsb, पूर्णाङ्क vpi_type_code, bool signed_flag,
			 unsigned argc, संरचना symb_s*argv);

बाह्य शून्य compile_netw_real(वर्ण*label, वर्ण*array_चिन्ह,
			      unsigned long array_addr,
			      पूर्णाङ्क msb, पूर्णाङ्क lsb,
			      unsigned argc, संरचना symb_s*argv);


बाह्य शून्य compile_island(वर्ण*label, वर्ण*type);
बाह्य शून्य compile_island_port(वर्ण*label, वर्ण*island, वर्ण*src);
बाह्य शून्य compile_island_import(वर्ण*label, वर्ण*island, वर्ण*src);
बाह्य शून्य compile_island_export(वर्ण*label, वर्ण*island);
बाह्य शून्य compile_island_cleanup(शून्य);

बाह्य शून्य compile_island_tran(वर्ण*label);
बाह्य शून्य compile_island_tranif(पूर्णाङ्क sense, वर्ण*island,
				  वर्ण*ba, वर्ण*bb, वर्ण*src, bool resistive);
बाह्य शून्य compile_island_tranvp(वर्ण*island, वर्ण*ba, वर्ण*bb,
				  unsigned width, unsigned part, unsigned off);

बाह्य शून्य delete_udp_symbols(शून्य);

बाह्य शून्य compile_class_start(वर्ण*lab, वर्ण*nam, unsigned nprop);
बाह्य शून्य compile_class_property(unsigned idx, वर्ण*nam, वर्ण*typ, uint64_t array_size);
बाह्य शून्य compile_class_done(शून्य);

#endif /* IVL_compile_H */
