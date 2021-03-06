#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/atype.hpp"
#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"

namespace snabl {
	struct Box {
		Box(const ATypePtr &type): _type(type) { }
		
		template <typename ValT>
		Box(const TypePtr<ValT> &type, const ValT &val): _type(type), _val(val) { }

		template <typename ValT>
		const ValT &as() const {
			assert(sizeof(ValT) == _type->size);
			return any_cast<const ValT &>(_val);
		}

		template <typename ValT>
		ValT &as() {
			assert(sizeof(ValT) == _type->size);
			return any_cast<ValT &>(_val);
		}

		const ATypePtr &type() const { return _type; }
		bool isa(const ATypePtr &rhs) const;
		
		bool equid(const Box &rhs) const {
			if (rhs.type() != _type) { return false; }
			return _type->equid(*this, rhs);
		}
		
		bool eqval(const Box &rhs) const {
			if (rhs.type() != _type) { return false; }
			return _type->eqval(*this, rhs);
		}
		
		Cmp cmp(const Box &rhs) const {
			auto rt(rhs.type());
			if (rt != _type) { return snabl::cmp(_type->tag, rt->tag); }
			return _type->cmp(*this, rhs);
		}

		bool has_val() const { return _val.has_value(); }
		bool as_bool() const { return _type->as_bool(*this); }

		void call(Pos pos, bool now) const { _type->call(*this, pos, now); }
		IterPtr iter() const { return _type->iter(*this); }
		void dump(ostream &out) const { _type->dump(*this, out); }
		void print(ostream &out) const { _type->print(*this, out); }
		void write(ostream &out) const { _type->write(*this, out); }
	private:
		ATypePtr _type;
		any _val;
	};

	inline ostream &operator <<(ostream &out, const Box &x) {
		x.print(out);
		return out;
	}
}

#endif
