#ifndef SNABL_LIB_HPP
#define SNABL_LIB_HPP

#include "snabl/def.hpp"
#include "snabl/error.hpp"
#include "snabl/func.hpp"
#include "snabl/macro.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Env;
	
	class Lib: public Def {
	public:
		Env &env;
		
		Lib(Env &env, Sym id);
		~Lib();
		
		template <typename ValT, typename... ArgsT>
		const MacroPtr &add_macro(Sym id, const TypePtr<ValT> &type, ArgsT &&... args);

		template <typename ImpT, typename... ArgsT>
		const MacroPtr &add_macro(Sym id, const OpType<ImpT> &type, ArgsT &&... args);

		const MacroPtr &add_macro(Sym id, const Macro::Imp &imp);

		template <typename TypeT, typename... ArgsT>
		shared_ptr<TypeT> add_type(Sym id,
															 initializer_list<ATypePtr> parent_types={},
															 ArgsT &&... args);

		const FuncPtr &add_func(Sym id, Int nargs);

		template <typename... ImpT>
		const FimpPtr &add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp);

		const MacroPtr *get_macro(Sym id);
		const ATypePtr *get_type(Sym id);
		const FuncPtr *get_func(Sym id);
	private:
		unordered_map<Sym, MacroPtr> _macros;
		unordered_map<Sym, ATypePtr> _types;
		unordered_map<Sym, FuncPtr> _funcs;
	};

	template <typename TypeT, typename... ArgsT>
	shared_ptr<TypeT> Lib::add_type(Sym id,
																	initializer_list<ATypePtr> parent_types,
																	ArgsT &&... args) {
		auto t(make_shared<TypeT>(*this,
															id,
															forward<ArgsT>(args)...));
		_types.emplace(t->id, t);
		for (auto &pt: parent_types) { AType::derive(t, pt); }
		return t;
	}

	template <typename... ImpT>
	const FimpPtr &Lib::add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp) {
		auto &fn(add_func(id, args.size()));
		return Func::add_fimp(fn, args, forward<ImpT>(imp)...);
	}
}

#endif
