#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
#include "snabl/run.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/task.hpp"
#include "snabl/types.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/char.hpp"
#include "snabl/types/error.hpp"
#include "snabl/types/float.hpp"
#include "snabl/types/int.hpp"
#include "snabl/types/iter.hpp"
#include "snabl/types/lambda.hpp"
#include "snabl/types/meta.hpp"
#include "snabl/types/nil.hpp"
#include "snabl/types/stack.hpp"
#include "snabl/types/str.hpp"
#include "snabl/types/sym.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
	template <typename ValT>
	class Type;
	
	class Env {
	public:
	private:
		list<SymImp> _syms;
		unordered_map<string, Sym> _sym_table;
		size_t _type_tag;
		TaskPtr _task;
		ScopePtr _scope;
		Stack _stack;
	public:
		set<char> separators;

		TraitPtr root_type, maybe_type, no_type, num_type, seq_type, sink_type, 
			source_type;
		
		TypePtr<ATypePtr> meta_type;
		TypePtr<bool> bool_type;
		TypePtr<Char> char_type;
		TypePtr<ErrorPtr> error_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;
		TypePtr<IterPtr> iter_type;
		TypePtr<LambdaPtr> lambda_type;
		TypePtr<Nil> nil_type;
		TypePtr<StackPtr> stack_type;
		TypePtr<StrPtr> str_type;
		TypePtr<Sym> sym_type;
		TypePtr<Time> time_type;
		
		libs::Home home_lib;
		const TaskPtr main_task;
		const ScopePtr &root_scope;
		
		Env():
			_type_tag(1),
			separators({
					' ', '\t', '\n', ',', ';', '?', '.', '|',
						'<', '>', '(', ')', '{', '}', '[', ']'
						}),
			home_lib(*this),
			main_task(spawn()),
			root_scope(begin_scope()),
			_lib(&home_lib),
			_stack_offs(0),
			_try(nullptr) {
			add_special_char('t', 8);
			add_special_char('n', 10);
			add_special_char('r', 13);
			add_special_char('e', 27);
			add_special_char('s', 32);
		}

		Env(const Env &) = delete;
		const Env &operator=(const Env &) = delete;

		size_t next_type_tag() { return _type_tag++; }

		void add_special_char(char in, Char out) {
			_special_chars.emplace(in, out);
			_char_specials.emplace(out, in);
		}

		optional<Char> find_special_char(char in) {
			auto found(_special_chars.find(in));
			return (found == _special_chars.end()) ? nullopt : make_optional(found->second);
		}

		optional<char> find_char_special(Char in) {
			auto found(_char_specials.find(in));
			return (found == _char_specials.end()) ? nullopt : make_optional(found->second);
		}
		
		Sym sym(const string &name) {
			const auto found(_sym_table.find(name));
			if (found != _sym_table.end()) { return found->second; }
			_syms.emplace_back(name);
			const auto imp(&_syms.back());
			return _sym_table.emplace(name, Sym(imp)).first->second;
		}

		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args) {
			Ops &ops(_task->_ops);
		  Op *prev(ops.empty() ? nullptr : &ops.back());
			ops.emplace_back(*this, type, args...);
			auto &op(ops.back());
			if (prev) { prev->next = &op.imp; }
			return op;
		}

		void emit(Pos pos, FuncPtr &func, FimpPtr &fimp);

		void compile(string_view in);
		void compile(istream &in);
		void compile(const Forms &forms);
		void compile(const Form &form);
		void compile(const Form &form, FuncPtr &func, FimpPtr &fimp);
		void compile(Forms::const_iterator begin, Forms::const_iterator end);
		void compile(Forms::const_iterator begin, Forms::const_iterator end,
								 FuncPtr &func, FimpPtr &fimp);
		
		void run(string_view in);
		void run(istream &in);
		void run();

		Lib &lib() const { return *_lib; }
		const Ops &ops() const { return _task->_ops; }
		PC pc() const { return _task->_pc; }
		
		TaskPtr spawn() {
			const bool is_main(!_task);
			_task = make_shared<Task>(_task);

			if (!is_main) {
				copy(main_task->_ops.begin(), main_task->_ops.end(),
						 back_inserter(_task->_ops));
			}
																		 
			return _task;
		}
		
		const ScopePtr &begin_scope(const ScopePtr &parent=nullptr) {
			_scope = make_shared<Scope>(_scope, parent);
			return _scope;
		}

		const ScopePtr &scope() const { return _scope; }

		void end_scope() {
			auto prev(_scope->prev);
			_scope->prev = nullptr;
			_scope = prev;
		}

		void jump(PC pc) { _task->_pc = pc; }
		void push(const Box &val) { _stack.push_back(val); }

		template <typename ValT, typename... ArgsT>
		void push(const TypePtr<ValT> &type, ArgsT &&...args) {
			_stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
		}

		Box &peek() {
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to peek"); }
			return _stack.back();
		}

		Box pop() {
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to pop"); }
			Box v(_stack.back());
			_stack.pop_back();
			return v;
		}

		const Stack &stack() { return _stack; }

		void split(size_t offs=0) {
			_stack_offs = _stack.size()-offs;
			_splits.push_back(_stack_offs);
		}

		void unsplit() {
			_splits.pop_back();
			_stack_offs = _splits.empty() ? 0 : _splits.back();
		}

		const TargetPtr &target() const { return _target; }
		
		template <typename... ArgsT>
		void note(Pos pos, const string &msg, ArgsT &&... args) {
			cerr << fmt("Note in row %0, col %1: ", {pos.row, pos.col})
					 << fmt(msg, {args...}) << endl;
		}

		template <typename... ArgsT>
		void warn(Pos pos, const string &msg, ArgsT &&... args) {
			cerr << fmt("Warning in row %0, col %1: ", {pos.row, pos.col})
					 << fmt(msg, {args...}) << endl;
		}

	private:
		map<char, Char> _special_chars;
		map<Char, char> _char_specials;
		vector<size_t> _splits;
		
		Lib *_lib;
		size_t _stack_offs;
		TargetPtr _target;
		ops::Try *_try;
		
		friend RuntimeError;
		friend State;
		friend Target;

		friend ops::DDrop::Type;
		friend ops::Drop::Type;
		friend ops::Dup::Type;
		friend ops::Eqval::Type;
		friend ops::Fimp::Type;
		friend ops::Funcall::Type;
		friend ops::Isa::Type;
		friend ops::Let::Type;
		friend ops::Recall::Type;
		friend ops::Return::Type;
		friend ops::Rot::Type;
		friend ops::RSwap::Type;
		friend ops::SDrop::Type;
		friend ops::Stack::Type;
		friend ops::Swap::Type;
		friend ops::Try::Type;
		friend ops::TryEnd::Type;
	};

	inline bool Box::isa(const ATypePtr &rhs) const {
		auto &lhs((_type == _type->lib.env.meta_type) ? as<ATypePtr>() : _type);
		return lhs->isa(rhs);
	}

	template <typename ValT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id,
																 const TypePtr<ValT> &type,
																 ArgsT &&... args) {
		return add_macro(id, [type, args...](Forms::const_iterator &in,
																					Forms::const_iterator end,
																					FuncPtr &func, FimpPtr &fimp,
																					Env &env) {
											 env.emit(ops::Push::type, (in++)->pos, type, args...);			
										 });
	}

	template <typename ImpT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id, const OpType<ImpT> &type, ArgsT &&... args) {
		return add_macro(id, [&type, args...](Forms::const_iterator &in,
																					 Forms::const_iterator end,
																					 FuncPtr &func, FimpPtr &fimp,
																					 Env &env) {
											 env.emit(type, (in++)->pos, args...);			
										 });
	}
}

#endif
