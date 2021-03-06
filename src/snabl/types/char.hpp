#ifndef SNABL_TYPE_CHAR_HPP
#define SNABL_TYPE_CHAR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"
#include "snabl/types.hpp"

namespace snabl {	
	class CharType: public Type<Char> {
	public:
		CharType(Lib &lib, Sym id);
		bool as_bool(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
