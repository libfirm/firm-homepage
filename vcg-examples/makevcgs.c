#include <libfirm/firm.h>
#include <irdump_t.h>
#include <panic.h>
#include <util.h>

void begin_construction(bool params_ret)
{
	ir_type *type_int    = get_type_for_mode(mode_Is);
	ir_type *method_type;
	if (params_ret) {
		method_type = new_type_method(2, 1);
		set_method_res_type(method_type, 0, type_int);
		set_method_param_type(method_type, 0, type_int);
		set_method_param_type(method_type, 1, type_int);
	} else {
		method_type = new_type_method(0, 0);
	}

	ident     *id     = id_unique("foobar%u");
	ir_entity *entity = new_entity(get_glob_type(), id, method_type);
	ir_graph  *irg    = new_ir_graph(entity, 1);
	set_current_ir_graph(irg);
	set_optimize(0);
}

FILE *begin_vcg(const char *filename)
{
	FILE *out = fopen(filename, "w");
	if (out == NULL)
		panic("couldn't create output");
	dump_vcg_header(out, "", NULL, NULL);
	fputs("create_reference_nodes: no\n", out);
	ir_graph *irg = get_current_ir_graph();
	inc_irg_visited(irg);
	return out;
}

void dump_func(const char *filename)
{
	FILE *out = fopen(filename, "w");
	if (out == NULL)
		panic("couldn't create output");

	dump_ir_graph_file(out, get_current_ir_graph());

	fclose(out);
}

void end_vcg(FILE *out)
{
	dump_vcg_footer(out);
	fclose(out);
}

void dump_node_with_preds(FILE *out, ir_node *node)
{
	if (irn_visited_else_mark(node))
		return;
	dump_node(out, node);
	dump_ir_data_edges(out, node);
	for (int i = 0, arity = get_irn_arity(node); i < arity; ++i) {
		ir_node *in = get_irn_n(node, i);
		dump_node_with_preds(out, in);
	}
}

void dump_placeholder_for(FILE *F, ir_node *node)
{
	fputs("node: {title: ", F);
	print_nodeid(F, node);
	fputs(" label: \"\" color: transparent bordercolor: transparent }\n", F);

	mark_irn_visited(node);
}

/* 5 + 3 */
void simpleadd(void)
{
	begin_construction(false);
	ir_node *five  = new_Const_long(mode_Is, 5);
	ir_node *three = new_Const_long(mode_Is, 3);
	ir_node *add   = new_Add(five, three, mode_Is);

	FILE *out = begin_vcg("simpleadd.vcg");
	dump_node_with_preds(out, add);
	end_vcg(out);
}

/* x*x - 2 */
void load_mul(void)
{
	begin_construction(false);

	ir_node *mem      = get_store();
	ir_type *type_int = get_type_for_mode(mode_Is);
	ir_entity *x
		= new_entity(get_glob_type(), new_id_from_str("x"), type_int);
	ir_node *addr  = new_Address(x);
	ir_node *load  = new_Load(mem, addr, mode_Is, cons_none);
	ir_node *loadM = new_Proj(load, mode_M, pn_Load_M);
	ir_node *res   = new_Proj(load, mode_Is, pn_Load_res);
	ir_node *mul   = new_Mul(res, res, mode_Is);
	ir_node *two   = new_Const_long(mode_Is, 2);
	ir_node *sub   = new_Sub(mul, two, mode_Is);

	FILE *out = begin_vcg("load_mul.vcg");
	dump_placeholder_for(out, mem);
	dump_node_with_preds(out, sub);
	dump_placeholder_for(out, loadM);
	dump_ir_data_edges(out, loadM);
	end_vcg(out);
}

/* print_int(7); print_int(4); */
void two_calls(void)
{
	begin_construction(false);

	ir_type *type_int = get_type_for_mode(mode_Is);
	ir_type *pitype   = new_type_method(1, 0);
	set_method_param_type(pitype, 0, type_int);
	ir_entity *print_int
		= new_entity(get_glob_type(), new_id_from_str("print_int"), pitype);

	ir_node *mem   = get_store();
	ir_node *addr  = new_Address(print_int);
	ir_node *c7    = new_Const_long(mode_Is, 7);
	ir_node *c4    = new_Const_long(mode_Is, 4);
	ir_node *in1[] = { c7 };
	ir_node *call1 = new_Call(mem, addr, ARRAY_SIZE(in1), in1, pitype);
	ir_node *mem1  = new_Proj(call1, mode_M, pn_Call_M);
	ir_node *in2[] = { c4 };
	ir_node *call2 = new_Call(mem1, addr, ARRAY_SIZE(in2), in2, pitype);
	ir_node *mem2  = new_Proj(call2, mode_M, pn_Call_M);

	FILE *out = begin_vcg("twocalls.vcg");
	dump_placeholder_for(out, mem);
	dump_node_with_preds(out, mem2);
	end_vcg(out);
}

/* x.z */
void member(void)
{
	begin_construction(false);

	ir_type   *strct    = new_type_struct(new_id_from_str("S"));
	ir_type   *type_int = get_type_for_mode(mode_Is);
	ir_entity *z        = new_entity(strct, new_id_from_str("z"), type_int);
	ir_entity *x        = new_entity(get_glob_type(), new_id_from_str("x"), strct);

	ir_node *mem    = get_store();
	ir_node *addr   = new_Address(x);
	ir_node *member = new_Member(addr, z);
	ir_node *load   = new_Load(mem, member, mode_Is, cons_none);
	ir_node *pmem   = new_Proj(load, mode_M, pn_Load_M);
	ir_node *res    = new_Proj(load, mode_Is, pn_Load_res);

	FILE *out = begin_vcg("member.vcg");
	dump_placeholder_for(out, mem);
	dump_placeholder_for(out, pmem);
	dump_ir_data_edges(out, pmem);
	dump_node_with_preds(out, res);
	end_vcg(out);
}

/* Jump */
void jump(void)
{
	begin_construction(false);

	ir_node *block1 = get_cur_block();
	ir_node *jmp    = new_Jmp();
	ir_node *block2 = new_immBlock();
	add_immBlock_pred(block2, jmp);
	mature_immBlock(block2);

	FILE *out = begin_vcg("jump.vcg");
	dump_begin_block_subgraph(out, block1);
	dump_node_with_preds(out, jmp);
	dump_end_block_subgraph(out, block1);
	dump_begin_block_subgraph(out, block2);
	dump_end_block_subgraph(out, block2);
	dump_block_edges(out, block2);
	end_vcg(out);
}

/* if (1 == 0) { return 4; } else { return 7; } */
void condjmp(void)
{
	begin_construction(true);
	ir_node *block0 = get_cur_block();
	ir_node *c1     = new_Const_long(mode_Is, 1);
	ir_node *c0     = new_Const_long(mode_Is, 0);
	ir_node *cmp    = new_Cmp(c1, c0, ir_relation_equal);
	ir_node *cond   = new_Cond(cmp);
	ir_node *ptrue  = new_Proj(cond, mode_X, pn_Cond_true);
	ir_node *pfalse = new_Proj(cond, mode_X, pn_Cond_false);

	ir_node *btrue  = new_immBlock();
	add_immBlock_pred(btrue, ptrue);
	mature_immBlock(btrue);
	set_cur_block(btrue);

	ir_node *mem1  = new_Dummy(mode_M);
	ir_node *c4    = new_Const_long(mode_Is, 4);
	ir_node *in1[] = { c4 };
	ir_node *ret1  = new_Return(mem1, ARRAY_SIZE(in1), in1);

	ir_node *bfalse = new_immBlock();
	add_immBlock_pred(bfalse, pfalse);
	mature_immBlock(bfalse);
	set_cur_block(bfalse);

	ir_node *mem2  = new_Dummy(mode_M);
	ir_node *c7    = new_Const_long(mode_Is, 7);
	ir_node *in2[] = { c7 };
	ir_node *ret2  = new_Return(mem2, ARRAY_SIZE(in2), in2);

	FILE *out = begin_vcg("condjump.vcg");
	dump_begin_block_subgraph(out, block0);
	dump_node_with_preds(out, ptrue);
	dump_node_with_preds(out, pfalse);
	dump_end_block_subgraph(out, block0);

	dump_begin_block_subgraph(out, btrue);
	dump_placeholder_for(out, mem1);
	dump_node_with_preds(out, ret1);
	dump_end_block_subgraph(out, btrue);
	dump_block_edges(out, btrue);

	dump_begin_block_subgraph(out, bfalse);
	dump_placeholder_for(out, mem2);
	dump_node_with_preds(out, ret2);
	dump_end_block_subgraph(out, bfalse);
	dump_block_edges(out, bfalse);
	end_vcg(out);
}

/* int x; if (7 > 4) { x = 9; } else { x = 3; } return x; */
void phi(void)
{
	begin_construction(true);
	ir_node *block0 = get_cur_block();
	ir_node *c7     = new_Const_long(mode_Is, 7);
	ir_node *c4     = new_Const_long(mode_Is, 4);
	ir_node *cmp    = new_Cmp(c7, c4, ir_relation_greater);
	ir_node *cond   = new_Cond(cmp);
	ir_node *ptrue  = new_Proj(cond, mode_X, pn_Cond_true);
	ir_node *pfalse = new_Proj(cond, mode_X, pn_Cond_false);

	ir_node *btrue  = new_immBlock();
	add_immBlock_pred(btrue, ptrue);
	set_cur_block(btrue);
	ir_node *c9   = new_Const_long(mode_Is, 9);
	ir_node *jmpt = new_Jmp();
	mature_immBlock(btrue);

	ir_node *bfalse = new_immBlock();
	add_immBlock_pred(bfalse, pfalse);
	set_cur_block(bfalse);
	ir_node *c3   = new_Const_long(mode_Is, 3);
	ir_node *jmpf = new_Jmp();
	mature_immBlock(bfalse);

	ir_node *bjoin = new_immBlock();
	add_immBlock_pred(bjoin, jmpt);
	add_immBlock_pred(bjoin, jmpf);
	set_cur_block(bjoin);
	ir_node *in[]  = { c9, c3 };
	ir_node *phi   = new_Phi(ARRAY_SIZE(in), in, mode_Is);
	ir_node *mem   = new_Dummy(mode_M);
	ir_node *rin[] = { phi };
	ir_node *ret   = new_Return(mem, ARRAY_SIZE(rin), rin);
	mature_immBlock(bjoin);

	FILE *out = begin_vcg("phi.vcg");
	dump_begin_block_subgraph(out, block0);
	dump_node_with_preds(out, ptrue);
	dump_node_with_preds(out, pfalse);
	dump_end_block_subgraph(out, block0);

	dump_begin_block_subgraph(out, btrue);
	dump_node_with_preds(out, c9);
	dump_node_with_preds(out, jmpt);
	dump_end_block_subgraph(out, btrue);
	dump_block_edges(out, btrue);

	dump_begin_block_subgraph(out, bfalse);
	dump_node_with_preds(out, c3);
	dump_node_with_preds(out, jmpf);
	dump_end_block_subgraph(out, bfalse);
	dump_block_edges(out, bfalse);

	dump_begin_block_subgraph(out, bjoin);
	dump_placeholder_for(out, mem);
	dump_node_with_preds(out, ret);
	dump_end_block_subgraph(out, bjoin);
	dump_block_edges(out, bjoin);
	end_vcg(out);
}

/* void empty(void) { } */
void start_return(void)
{
	begin_construction(false);
	ir_graph *irg = get_current_ir_graph();
	ir_node  *mem = get_store();
	ir_node  *ret = new_Return(mem, 0, NULL);
	ir_node  *end_block = get_irg_end_block(irg);
	add_immBlock_pred(end_block, ret);
	irg_finalize_cons(irg);

	FILE *out = begin_vcg("start_return.vcg");
	dump_blocks_as_subgraphs(out, irg);
	end_vcg(out);
}

/* int add(int x, int y) { return x + y; } */
void params(void)
{
	begin_construction(true);
	ir_graph *irg    = get_current_ir_graph();
	ir_node  *params = get_irg_args(irg);
	ir_node  *param0 = new_Proj(params, mode_Is, 0);
	ir_node  *param1 = new_Proj(params, mode_Is, 1);
	ir_node  *add    = new_Add(param0, param1, mode_Is);
	ir_node  *in[]   = { add };
	ir_node  *mem    = get_store();
	ir_node  *ret    = new_Return(mem, ARRAY_SIZE(in), in);
	ir_node  *end_block = get_irg_end_block(irg);
	add_immBlock_pred(end_block, ret);
	irg_finalize_cons(irg);

	FILE *out = begin_vcg("params.vcg");
	dump_blocks_as_subgraphs(out, irg);
	end_vcg(out);
}

/* TODO: endless loop, compound parameter/return, alloca */

int main(void)
{
	ir_init();
	ir_set_dump_flags(ir_dump_flag_blocks_as_subgraphs);

	simpleadd();
	load_mul();
	two_calls();
	member();
	jump();
	condjmp();
	phi();
	start_return();
	params();

	return 0;
}
