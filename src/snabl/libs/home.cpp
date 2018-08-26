#include "snabl/env.hpp"
#include "snabl/libs/home.hpp"

namespace snabl {
	namespace libs {
		Home::Home(Env &env): Lib(env, env.get_sym("home")) {
			env.maybe_type = add_type<Trait>(env.get_sym("Maybe"));
			env.a_type = add_type<Trait>(env.get_sym("A"), {env.maybe_type});
			env.no_type = add_type<Trait>(env.get_sym("_"));
			env.num_type = add_type<Trait>(env.get_sym("Num"), {env.a_type});
			env.bool_type = add_type<BoolType>(env.get_sym("Bool"), {env.a_type});
			env.float_type = add_type<FloatType>(env.get_sym("Float"), {env.num_type});
			env.int_type = add_type<IntType>(env.get_sym("Int"), {env.num_type});
	
			add_macro(env.get_sym("t"),
								[](Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) {
									out.emplace_back(ops::Push::type,
																	 (in++)->pos,
																	 Box(out.env.bool_type, true));			
								});

			add_macro(env.get_sym("f"),
								[](Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) {
									out.emplace_back(ops::Push::type,
																	 (in++)->pos,
																	 Box(out.env.bool_type, false));			
								});

			add_macro(env.get_sym("drop"),
								[](Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) {
									auto &form(*in++);
									out.emplace_back(ops::Drop::type, form.pos);			
								});

			add_macro(env.get_sym("let:"),
								[](Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) {
									auto &form(*in++);
									auto &p(*in++);

									if (&p.type != &forms::Id::type) {
										throw SyntaxError(p.pos, "Invalid let: place");
									}
												 
									(*in).type.compile(in, end, func, fimp, out);
									out.emplace_back(ops::PutVar::type,
																	 form.pos,
																	 p.as<forms::Id>().sym);
								});

			add_macro(env.get_sym("if:"),
								[](Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) {
									auto &form(*in++);
									out.compile(in++, in+1);
									auto &else_skip(out.emplace_back(ops::Else::type,
																									 form.pos,
																									 0));
									size_t start_pc(out.ops().size());								
									out.compile(in++, in+1);
									auto &if_skip(out.emplace_back(ops::Skip::type, form.pos, 0));
									else_skip.as<ops::Else>().nops = out.ops().size()-start_pc;
									start_pc = out.ops().size();
									out.compile(in++, in+1);
									if_skip.as<ops::Skip>().nops = out.ops().size()-start_pc;
								});	
	
			add_fimp<2, 1>(env.get_sym("="),
				{Box(env.maybe_type), Box(env.maybe_type)},
				{Box(env.bool_type)},
				[](Call &call) {
					Env &env(call.scope->env);
			
					Box
						y(call.scope->pop_stack()),
						x(call.scope->pop_stack());
			
					env.push_stack(env.bool_type, x.is_eqval(y));
				});

			add_fimp<2, 1>(env.get_sym("=="),
				{Box(env.maybe_type), Box(env.maybe_type)},
				{Box(env.bool_type)},
				[](Call &call) {
					Env &env(call.scope->env);
			
					Box
						y(call.scope->pop_stack()),
						x(call.scope->pop_stack());
			
					env.push_stack(env.bool_type, x.is_equid(y));
				});

			add_fimp<2, 1>(env.get_sym("<"),
				{Box(env.a_type), Box(env.a_type)},
				{Box(env.bool_type)},
				[](Call &call) {
					Env &env(call.scope->env);
			
					Box
						y(call.scope->pop_stack()),
						x(call.scope->pop_stack());

					env.push_stack(env.bool_type, x.cmp(y) == Cmp::LT);
				});
	
			add_fimp<1, 1>(env.get_sym("int"),
				{Box(env.float_type)},
				{Box(env.int_type)},
				[](Call &call) {
					Env &env(call.scope->env);
					const Float v(call.scope->pop_stack().as<Float>());
					env.push_stack(env.int_type, Int(v));
				});

			add_fimp<1, 1>(env.get_sym("float"),
				{Box(env.int_type)},
				{Box(env.float_type)},
				[](Call &call) {
					Env &env(call.scope->env);
					const Int v(call.scope->pop_stack().as<Int>());
					env.push_stack(env.float_type, Float(v));
				});

			add_fimp<2, 1>(env.get_sym("+"),
				{Box(env.int_type), Box(env.int_type)},
				{Box(env.int_type)},
				[](Call &call) {
					Env &env(call.scope->env);
			
					Int
						y(call.scope->pop_stack().as<Int>()),
						x(call.scope->pop_stack().as<Int>());
			
					env.push_stack(env.int_type, x+y);
				});

			add_fimp<2, 1>(env.get_sym("*"),
				{Box(env.int_type), Box(env.int_type)},
				{Box(env.int_type)},
				[](Call &call) {
					Env &env(call.scope->env);
			
					Int
						y(call.scope->pop_stack().as<Int>()),
						x(call.scope->pop_stack().as<Int>());
			
					env.push_stack(env.int_type, x*y);
				});
		}
	}
}