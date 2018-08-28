#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>
#include <unordered_map>

#include "snabl/fimp.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Lib;

	class Func {
	public:
		Lib &lib;
		const Sym id;
		const size_t nargs, nrets;

		template <typename... ImpT>
		static FimpPtr add_fimp(const FuncPtr &func,
														const Fimp::Args &args, const Fimp::Rets &rets,
														ImpT &&... imp);
		
		Func(Lib &lib, Sym id, size_t nargs, size_t nrets);
		FimpPtr get_fimp() const;
		FimpPtr get_best_fimp(const Stack &stack) const;
		void clear();
	private:
		unordered_map<Sym, FimpPtr> _fimps;
	};

	template <typename... ImpT>
	FimpPtr Func::add_fimp(const FuncPtr &func,
												 const Fimp::Args &args, const Fimp::Rets &rets,
												 ImpT &&... imp) {
		auto id(Fimp::get_id(func, args));
		auto found = func->_fimps.find(id);
		if (found != func->_fimps.end()) { func->_fimps.erase(found); }

		auto f(make_shared<Fimp>(func,
																	args, rets,
																	forward<ImpT>(imp)...));
		func->_fimps.emplace(id, f);
		return f;
	}
}

#endif
