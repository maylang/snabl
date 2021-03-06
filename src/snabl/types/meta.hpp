#ifndef SNABL_TYPE_META_HPP
#define SNABL_TYPE_META_HPP

#include "snabl/type.hpp"
#include "snabl/ptrs.hpp"

namespace snabl {
	class MetaType: public Type<ATypePtr> {
	public:
		MetaType(Lib &lib, Sym id);
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
