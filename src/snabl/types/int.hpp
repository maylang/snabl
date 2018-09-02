#ifndef SNABL_INT_HPP
#define SNABL_INT_HPP

#include <cstdint>
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	using Int = long long;
	
	class IntType: public Type<Int> {
	public:
		IntType(Lib &lib, Sym id);
		bool is_true(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
