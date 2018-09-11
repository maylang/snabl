#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/pos.hpp"
#include "snabl/type.hpp"

namespace snabl {
	bool Box::isa(const ATypePtr &rhs) const {
		auto lhs((_type == _type->lib.env.meta_type) ? as<ATypePtr>() : _type);
		return lhs->isa(rhs);
	}

	bool Box::equid(const Box &rhs) const {
		if (rhs.type() != _type) { return false; }
		return _type->equid(*this, rhs);
	}

	bool Box::eqval(const Box &rhs) const {
		if (rhs.type() != _type) { return false; }
		return _type->eqval(*this, rhs);
	}

	Cmp Box::cmp(const Box &rhs) const {
		auto rt(rhs.type());
		if (rt != _type) { return snabl::cmp(_type->tag, rt->tag); }
		return _type->cmp(*this, rhs);
	}

	bool Box::is_true() const { return _type->is_true(*this); }

	void Box::call(Pos pos, bool now) const { _type->call(*this, pos, now); }

	void Box::dump(ostream &out) const { _type->dump(*this, out); }

	void Box::print(ostream &out) const { _type->print(*this, out); }

	void Box::write(ostream &out) const { _type->write(*this, out); }
}
