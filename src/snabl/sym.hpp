#ifndef SNABL_SYM_HPP
#define SNABL_SYM_HPP

#include "snabl/std.hpp"

namespace snabl {
	class Sym;
	
	class SymImp {
	public:
		const string name;
		const size_t hash;
		
		SymImp(const string &name);
	};

	class Sym {
	public:
		friend hash<Sym>;
		friend bool operator ==(Sym x, Sym y);
		friend bool operator !=(Sym x, Sym y);
		friend bool operator <(Sym x, Sym y);
		
		Sym(const SymImp *imp);
		const string &name() const;
	private:
		const SymImp *_imp;
	};

	bool operator ==(Sym x, Sym y);
	bool operator !=(Sym x, Sym y);
	bool operator <(Sym x, Sym y);
	
	string fmt_arg(const Sym &x);
}

namespace std {
	template<>
	struct hash<snabl::Sym> {
		size_t operator ()(const snabl::Sym &x) const { return x._imp->hash; }
	};
}

#endif
