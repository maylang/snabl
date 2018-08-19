#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include <unordered_set>
#include <vector>

#include "snabl/ptrs.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Box;
	class Lib;

	class AType {
	public:
		Lib &lib;
		const size_t tag;
		const Sym id;

		static void derive(const ATypePtr &child, const ATypePtr &parent);
		bool isa(const ATypePtr &parent) const;
		virtual void dump(const Box &value, std::ostream &out);
	protected:
		AType(Lib &lib, const Sym &id);
	private:
		std::vector<ATypePtr> _parent_types;
		std::unordered_set<ATypePtr> _child_types;
	};

	template <typename ValueT>
	class Type: public AType {
	public:
		Type(Lib &lib, const Sym &id);
	};

	template <typename ValueT>
	Type<ValueT>::Type(Lib &lib, const Sym &id): AType(lib, id) { }
}

#endif
