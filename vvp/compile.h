#నిర్వచితం_కాకపోతే IVL_compile_H
#నిర్వచించు IVL_compile_H
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
బాహ్య std::సదిశ<స్థిరరాశి అక్షరం*> file_పేర్లు;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

బాహ్య శూన్య compile_init(శూన్య);

బాహ్య శూన్య compile_cleanup(శూన్య);

బాహ్య bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
బాహ్య std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
బాహ్య శూన్య inputs_connect(vvp_net_t*fdx, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య ఆదానం_connect(vvp_net_t*fdx, గుర్తులేని పోర్టు, అక్షరం*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
బాహ్య శూన్య wide_inputs_connect(vvp_wide_fun_core*core,
				గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య vvp_net_t* vvp_net_lookup(స్థిరరాశి అక్షరం*label);
బాహ్య vpiHandle vvp_lookup_handle(స్థిరరాశి అక్షరం*label);

/*
 *  Add a functor to the symbol table
 */
బాహ్య శూన్య define_functor_చిహ్నం(స్థిరరాశి అక్షరం*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
బాహ్య గుర్తులేని compile_errors;

బాహ్య స్థిరరాశి అక్షరం* module_మార్గం;
బాహ్య శూన్య compile_ఎక్కించు_vpi_module(అక్షరం*name);

బాహ్య శూన్య compile_vpi_సమయం_precision(దీర్ఘ pre);

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
బాహ్య శూన్య compile_functor(అక్షరం*label, అక్షరం*type, గుర్తులేని వెడల్పు,
			    గుర్తులేని ostr0, గుర్తులేని ostr1,
			    గుర్తులేని argc, నిర్మాణం symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
బాహ్య శూన్య compile_resolver(అక్షరం*label, అక్షరం*type,
			     గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_concat(అక్షరం*label, గుర్తులేని w0, గుర్తులేని w1,
			   గుర్తులేని w2, గుర్తులేని w3,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_concat8(అక్షరం*label, గుర్తులేని w0, గుర్తులేని w1,
			    గుర్తులేని w2, గుర్తులేని w3,
			    గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_substitute(అక్షరం*label, గుర్తులేని వెడల్పు,
			       గుర్తులేని soff, గుర్తులేని swidth,
			       గుర్తులేని argc, నిర్మాణం symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
బాహ్య శూన్య compile_compiletf(నిర్మాణం __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
బాహ్య శూన్య compile_delay(అక్షరం*label, గుర్తులేని వెడల్పు,
                          vvp_delay_t*del, నిర్మాణం symb_s ఆదానం);
బాహ్య శూన్య compile_delay(అక్షరం*label, గుర్తులేని వెడల్పు,
                          గుర్తులేని argc, నిర్మాణం symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
బాహ్య శూన్య compile_part_select(అక్షరం*label, అక్షరం*src,
				గుర్తులేని పునాది, గుర్తులేని wid);
బాహ్య శూన్య compile_part_select_pv(అక్షరం*label, అక్షరం*src,
				   గుర్తులేని పునాది, గుర్తులేని wid,
				   గుర్తులేని vec_wid);
బాహ్య శూన్య compile_part_select_var(అక్షరం*label, అక్షరం*src,
                                    అక్షరం*var, గుర్తులేని wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
బాహ్య శూన్య compile_arith_pow(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_abs(అక్షరం*label,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_cast_int(అక్షరం*label, దీర్ఘ వెడల్పు,
			           గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_cast_real(అక్షరం*label, bool signed_flag,
			            గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_cast_vec2(అక్షరం*label, దీర్ఘ వెడల్పు,
				    గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_div(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_mod(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_mult(అక్షరం*label, దీర్ఘ వెడల్పు,
			       గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_కూడిక(అక్షరం*label, దీర్ఘ వెడల్పు,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_sub(అక్షరం*label, దీర్ఘ వెడల్పు,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_eeq(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_nee(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_eq(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_eqx(అక్షరం*label, దీర్ఘ వెడల్పు,
			    గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_eqz(అక్షరం*label, దీర్ఘ వెడల్పు,
			    గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_ne(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_ge(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_gt(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_weq(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_wne(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_arith_mult_r(అక్షరం*label, గుర్తులేని argc,
                                 నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_pow_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_div_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_mod_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_కూడిక_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_arith_sub_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_eq_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_ne_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_ge_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_cmp_gt_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_dff(అక్షరం*label, గుర్తులేని వెడల్పు, bool negedge,
			నిర్మాణం symb_s arg_d,
			నిర్మాణం symb_s arg_c,
			నిర్మాణం symb_s arg_e);

బాహ్య శూన్య compile_dff_aclr(అక్షరం*label, గుర్తులేని వెడల్పు, bool negedge,
			     నిర్మాణం symb_s arg_d,
			     నిర్మాణం symb_s arg_c,
			     నిర్మాణం symb_s arg_e,
			     నిర్మాణం symb_s arg_a);
బాహ్య శూన్య compile_dff_aset(అక్షరం*label, గుర్తులేని వెడల్పు, bool negedge,
			     నిర్మాణం symb_s arg_d,
			     నిర్మాణం symb_s arg_c,
			     నిర్మాణం symb_s arg_e,
			     నిర్మాణం symb_s arg_a,
			     అక్షరం*asc_విలువ);

బాహ్య శూన్య compile_latch(అక్షరం*label, గుర్తులేని వెడల్పు,
			  నిర్మాణం symb_s arg_d,
			  నిర్మాణం symb_s arg_e);

బాహ్య శూన్య compile_enum2_type(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			      std::list<నిర్మాణం enum_name_s>*పేర్లు);
బాహ్య శూన్య compile_enum4_type(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			      std::list<నిర్మాణం enum_name_s>*పేర్లు);

నిర్మాణం __vpiModPath;
బాహ్య __vpiModPath* compile_modpath(అక్షరం*label,
                                     గుర్తులేని వెడల్పు,
				     నిర్మాణం symb_s drv,
				     నిర్మాణం symb_s dest);
బాహ్య శూన్య compile_modpath_src(__vpiModPath*dst,
				అక్షరం అంచు_c,
				స్థిరరాశి నిర్మాణం symb_s&src,
				నిర్మాణం numbv_s&vals,
				స్థిరరాశి నిర్మాణం symb_s&condit_src,
				స్థిరరాశి నిర్మాణం symb_s&మార్గం_పదం_లోపల);
బాహ్య శూన్య compile_modpath_src(__vpiModPath*dst,
				అక్షరం అంచు_c,
				స్థిరరాశి నిర్మాణం symb_s&src,
				నిర్మాణం numbv_s&vals,
				పూర్ణాంకం condit_src, /* match with '0' */
				స్థిరరాశి నిర్మాణం symb_s&మార్గం_పదం_లోపల,
				bool ifnone);

బాహ్య శూన్య compile_reduce_and(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg);
బాహ్య శూన్య compile_reduce_or(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg);
బాహ్య శూన్య compile_reduce_xor(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg);
బాహ్య శూన్య compile_reduce_nand(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg);
బాహ్య శూన్య compile_reduce_nor(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg);
బాహ్య శూన్య compile_reduce_xnor(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg);

బాహ్య శూన్య compile_extend_signed(అక్షరం*label, దీర్ఘ వెడల్పు, నిర్మాణం symb_s arg);

బాహ్య శూన్య compile_sfunc(అక్షరం*label, అక్షరం*name, అక్షరం*format_string,
			  దీర్ఘ file_idx, దీర్ఘ lineno,
			  గుర్తులేని argc, నిర్మాణం symb_s*argv,
                          అక్షరం*trigger_label);

బాహ్య శూన్య compile_repeat(అక్షరం*label, దీర్ఘ వెడల్పు, దీర్ఘ repeat,
			   నిర్మాణం symb_s arg);

బాహ్య శూన్య compile_shiftl(అక్షరం*label, దీర్ఘ వెడల్పు,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_shiftr(అక్షరం*label, దీర్ఘ వెడల్పు, bool signed_flag,
			   గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_timescale(దీర్ఘ units, దీర్ఘ precision);

బాహ్య శూన్య compile_vpi_చిహ్నం(స్థిరరాశి అక్షరం*label, vpiHandle obj);
బాహ్య శూన్య compile_vpi_lookup(vpiHandle *objref, అక్షరం*label);

బాహ్య శూన్య compile_param_string(అక్షరం*label, స్థిరరాశి అక్షరం*name, అక్షరం*విలువ,
                                 bool local_flag,
                                 దీర్ఘ file_idx, దీర్ఘ lineno);
బాహ్య శూన్య compile_param_logic(అక్షరం*label, స్థిరరాశి అక్షరం*name, అక్షరం*విలువ,
				bool signed_flag, bool local_flag,
                                దీర్ఘ file_idx, దీర్ఘ lineno);
బాహ్య శూన్య compile_param_real(అక్షరం*label, స్థిరరాశి అక్షరం*name, అక్షరం*విలువ,
                               bool local_flag,
                               దీర్ఘ file_idx, దీర్ఘ lineno);

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

    బహిరంగ:
      explicit resolv_list_s(అక్షరం*lab) : label_(lab) {
	    తదుపరి_2 = ఖాళీ;
      }
      మాయా ~resolv_list_s();

      resolv_list_s(స్థిరరాశి resolv_list_s&) = delete;
      resolv_list_s& క్రియాకారకం=(స్థిరరాశి resolv_list_s&) = delete;

      మాయా bool resolve(bool mes = false) = 0;

    రక్షిత:
      స్థిరరాశి అక్షరం*label() స్థిరరాశి { తిరిగి label_; }

    అంతరంగ:
      మిత్ర శూన్య resolv_submit(class resolv_list_s*cur);
      మిత్ర శూన్య compile_cleanup(శూన్య);

      అక్షరం*label_;
      class resolv_list_s*తదుపరి_2;
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
బాహ్య శూన్య functor_ref_lookup(vvp_net_t**ref, అక్షరం*lab);

బాహ్య vpiHandle vpip_make_prop_వరుస_ref(అక్షరం* class_label,
                                          గుర్తులేని prop_idx,
                                          గుర్తులేని is_వరుస_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
బాహ్య శూన్య సంకేతం_label_lookup(నిర్మాణం vvp_సంకేతం_s *సంకేతం_2, అక్షరం *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

బాహ్య శూన్య compile_udp_def(పూర్ణాంకం sequ, అక్షరం*label, అక్షరం *name,
			    గుర్తులేని nin, గుర్తులేని init, అక్షరం **పట్టిక);

బాహ్య శూన్య compile_udp_functor(అక్షరం*label, అక్షరం*type,
				గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య అక్షరం **compile_udp_పట్టిక(అక్షరం **పట్టిక, అక్షరం *అడ్డువరుస);

/*
 * Memory Instances, Ports, and Initialization
 */

బాహ్య శూన్య compile_var_array(అక్షరం*label, అక్షరం*name,
			      పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి,
			      పూర్ణాంకం msb, పూర్ణాంకం lsb, అక్షరం signed_flag);
బాహ్య శూన్య compile_var2_array(అక్షరం*label, అక్షరం*name,
			      పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి,
			      పూర్ణాంకం msb, పూర్ణాంకం lsb, bool signed_flag);
బాహ్య శూన్య compile_real_array(అక్షరం*label, అక్షరం*name,
			       పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి);
బాహ్య శూన్య compile_string_array(అక్షరం*label, అక్షరం*name,
				 పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి);
బాహ్య శూన్య compile_object_array(అక్షరం*label, అక్షరం*name,
				 పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి);
బాహ్య శూన్య compile_net_array(అక్షరం*label, అక్షరం*name,
			      పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి);
బాహ్య శూన్య compile_array_alias(అక్షరం*label, అక్షరం*name, అక్షరం*src);

  /* Index is a net. */
బాహ్య శూన్య compile_array_పోర్టు(అక్షరం*label, అక్షరం*name, అక్షరం*addr);
  /* Index is a constant address */
బాహ్య శూన్య compile_array_పోర్టు(అక్షరం*label, అక్షరం*name, దీర్ఘ addr);

బాహ్య శూన్య compile_array_cleanup(శూన్య);

/*
 * Compile the .ufunc statement.
 */
బాహ్య శూన్య compile_ufunc_real(అక్షరం*label, అక్షరం*సంకేతం_2, గుర్తులేని wid,
			  గుర్తులేని argc, నిర్మాణం symb_s*argv,
			  గుర్తులేని portc, నిర్మాణం symb_s*portv,
			  అక్షరం*scope_label, అక్షరం*trigger_label);
బాహ్య శూన్య compile_ufunc_vec4(అక్షరం*label, అక్షరం*సంకేతం_2, గుర్తులేని wid,
			  గుర్తులేని argc, నిర్మాణం symb_s*argv,
			  గుర్తులేని portc, నిర్మాణం symb_s*portv,
			  అక్షరం*scope_label, అక్షరం*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
బాహ్య శూన్య compile_event(అక్షరం*label, అక్షరం*type,
			  గుర్తులేని argc, నిర్మాణం symb_s*argv);
బాహ్య శూన్య compile_named_event(అక్షరం*label, అక్షరం*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#నిర్వచించు నిర్వాహ్యం_MAX 3
గణన ltype_e { L_NUMB, L_SYMB, L_STRING };

నిర్మాణం comp_operands_s {
      గుర్తులేని argc;
      నిర్మాణం {
	    గణన ltype_e ltype;
	    సంఘం {
		  గుర్తులేని దీర్ఘ numb;
		  నిర్మాణం symb_s symb;
		  స్థిరరాశి అక్షరం   *పాఠ్యం;
	    };
      } argv[నిర్వాహ్యం_MAX];
};

రకనిర్వచనం నిర్మాణం comp_operands_s*comp_operands_t;

బాహ్య శూన్య compile_సంకేతం(అక్షరం*label, అక్షరం*mnem, comp_operands_t opa);

బాహ్య శూన్య compile_file_పంక్తి(అక్షరం*label, దీర్ఘ file_idx, దీర్ఘ lineno,
                              అక్షరం*description);

బాహ్య శూన్య compile_vpi_call(అక్షరం*label, అక్షరం*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     దీర్ఘ file_idx, దీర్ఘ lineno,
			     గుర్తులేని argc, vpiHandle*argv,
			     గుర్తులేని vec4_దొంతర,
			     గుర్తులేని real_దొంతర,
			     గుర్తులేని string_దొంతర);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
బాహ్య శూన్య compile_vpi_func_call(అక్షరం*label, అక్షరం*name,
				  పూర్ణాంకం val_type, గుర్తులేని val_wid,
				  దీర్ఘ file_idx, దీర్ఘ lineno,
				  గుర్తులేని argc, vpiHandle*argv,
				  గుర్తులేని vec4_దొంతర,
				  గుర్తులేని real_దొంతర,
				  గుర్తులేని string_దొంతర);
బాహ్య శూన్య print_vpi_call_errors();

బాహ్య శూన్య compile_codelabel(అక్షరం*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
బాహ్య శూన్య compile_scope_decl(అక్షరం*typ, అక్షరం*lab, అక్షరం*nam, అక్షరం*tnam,
                               అక్షరం*par, దీర్ఘ file_idx, దీర్ఘ lineno,
                               దీర్ఘ def_file_idx, దీర్ఘ def_lineno,
                               దీర్ఘ is_cell);
బాహ్య శూన్య compile_scope_recall(అక్షరం*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
బాహ్య శూన్య compile_thread(అక్షరం*ప్రారంభం_sym, అక్షరం*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
బాహ్య శూన్య compile_variable(అక్షరం*label, అక్షరం*name,
			     పూర్ణాంకం msb, పూర్ణాంకం lsb, పూర్ణాంకం vpi_type_సంకేతం,
			     bool signed_flag, bool local_flag);

బాహ్య శూన్య compile_var_real(అక్షరం*label, అక్షరం*name, bool local_flag);
బాహ్య శూన్య compile_var_string(అక్షరం*label, అక్షరం*name);
బాహ్య శూన్య compile_var_darray(అక్షరం*label, అక్షరం*name, గుర్తులేని కొలత);
బాహ్య శూన్య compile_var_cobject(అక్షరం*label, అక్షరం*name);
బాహ్య శూన్య compile_var_వరుస(అక్షరం*label, అక్షరం*name, గుర్తులేని కొలత);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

బాహ్య శూన్య compile_పోర్టు_info( గుర్తులేని సూచీ, పూర్ణాంకం vpi_పోర్టు_type, గుర్తులేని వెడల్పు, స్థిరరాశి అక్షరం *name, అక్షరం* బఫరు );


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
బాహ్య శూన్య compile_net(అక్షరం*label, అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			పూర్ణాంకం vpi_type_సంకేతం, bool signed_flag, bool local_flag,
			గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_net_real(అక్షరం*label, అక్షరం*name,
			     పూర్ణాంకం msb, పూర్ణాంకం lsb, bool local_flag,
			     గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_netw(అక్షరం*label, అక్షరం*array_label, గుర్తులేని దీర్ఘ array_addr,
			 పూర్ణాంకం msb, పూర్ణాంకం lsb, పూర్ణాంకం vpi_type_సంకేతం, bool signed_flag,
			 గుర్తులేని argc, నిర్మాణం symb_s*argv);

బాహ్య శూన్య compile_netw_real(అక్షరం*label, అక్షరం*array_చిహ్నం,
			      గుర్తులేని దీర్ఘ array_addr,
			      పూర్ణాంకం msb, పూర్ణాంకం lsb,
			      గుర్తులేని argc, నిర్మాణం symb_s*argv);


బాహ్య శూన్య compile_island(అక్షరం*label, అక్షరం*type);
బాహ్య శూన్య compile_island_పోర్టు(అక్షరం*label, అక్షరం*island, అక్షరం*src);
బాహ్య శూన్య compile_island_import(అక్షరం*label, అక్షరం*island, అక్షరం*src);
బాహ్య శూన్య compile_island_export(అక్షరం*label, అక్షరం*island);
బాహ్య శూన్య compile_island_cleanup(శూన్య);

బాహ్య శూన్య compile_island_tran(అక్షరం*label);
బాహ్య శూన్య compile_island_tranif(పూర్ణాంకం sense, అక్షరం*island,
				  అక్షరం*ba, అక్షరం*bb, అక్షరం*src, bool resistive);
బాహ్య శూన్య compile_island_tranvp(అక్షరం*island, అక్షరం*ba, అక్షరం*bb,
				  గుర్తులేని వెడల్పు, గుర్తులేని part, గుర్తులేని off);

బాహ్య శూన్య delete_udp_symbols(శూన్య);

బాహ్య శూన్య compile_class_ప్రారంభం(అక్షరం*lab, అక్షరం*nam, గుర్తులేని nprop);
బాహ్య శూన్య compile_class_property(గుర్తులేని idx, అక్షరం*nam, అక్షరం*typ, uint64_t array_కొలత_2);
బాహ్య శూన్య compile_class_పూర్తి(శూన్య);

#అయితే_ముగింపు /* IVL_compile_H */
