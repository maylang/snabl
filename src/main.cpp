#include <cassert>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

using namespace snabl;

int main() {
	Env env;

	env.home.add_macro(env.get_sym("drop"),
										 [](Forms::const_iterator &in,
												const Forms::const_iterator &end,
												AFuncPtr &func, AFimpPtr &fimp,
												Bin &out) {
											 auto &form(*in++);
											 out.emplace_back(ops::Drop::type, form.pos);			
										 });
	
	env.home.add_fimp<2, 1>(env.get_sym("+"),
		{env.int_type, env.int_type},
		{env.int_type},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Int
				y(call.scope->pop_stack().as<Int>()),
				x(call.scope->pop_stack().as<Int>());
			
			env.push_stack(env.int_type, x+y);
		});

	env.home.add_fimp<2, 1>(env.get_sym("*"),
		{env.int_type, env.int_type},
		{env.int_type},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Int
				y(call.scope->pop_stack().as<Int>()),
				x(call.scope->pop_stack().as<Int>());
			
			env.push_stack(env.int_type, x*y);
		});
	
	auto s(env.begin(env.scope()));

	env.run("42");
	assert(s->pop_stack().as<Int>() == Int(42));

	env.run("1 + 3");
	assert(s->pop_stack().as<Int>() == Int(4));

	env.run("1 +, 3 * 2");
	assert(s->pop_stack().as<Int>() == Int(7));

	env.run("1 3 5 drop +");
	assert(s->pop_stack().as<Int>() == Int(4));
	
	//"func: fib(Int)(Int) (let: n) if_else: ($n < 2) 1, (fib, $n --) + (fib, $n - 2)";
	//env.bin.compile(add_int);
	
	auto foo(env.get_sym("foo"));
	env.main->set_var(foo, env.int_type, Int(42));
	assert(s->get_var(foo)->as<Int>() == Int(42));
	env.end();

	return 0;
}
