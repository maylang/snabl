#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;

	class Scope {
	public:
		static const Int MaxRegs = 8;
		
		ScopePtr prev;
		const ScopePtr source;
		
		Scope(const ScopePtr &prev, const ScopePtr &source):
			prev(prev), source(source) { }

		Scope(const Scope &) = delete;
		const Scope &operator=(const Scope &) = delete;
		
		const Box *get(Sym id) const {
			const auto found(_vars.find(id));
			if (found != _vars.end()) { return &found->second; }
			return source ? source->get(id) : nullptr;
		}

		void let(Sym id, const Box &val) {
			const auto ok(_vars.emplace(make_pair(id, val)));
			if (!ok.second) { throw Error("Duplicate var: " + id.name()); }
		}

		void clear_vars() { _vars.clear(); }
	private:
		array<any, MaxRegs> _regs;
		map<Sym, Box> _vars;

		friend Env;
	};
}

#endif
