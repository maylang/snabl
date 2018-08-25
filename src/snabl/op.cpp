#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	AOpType::AOpType(const std::string &id):
		id(id), label_offs(next_label_offs++) { }

	namespace ops {
		const OpType<Begin> Begin::type("Begin");
		const OpType<Drop> Drop::type("Drop");
		const OpType<End> End::type("End");
		const OpType<Funcall> Funcall::type("Funcall");
		const OpType<GetVar> GetVar::type("GetVar");
		const OpType<Push> Push::type("Push");
		const OpType<PutVar> PutVar::type("PutVar");

		Begin::Begin() { }

		Drop::Drop() { }
		
		End::End() { }

		Funcall::Funcall(const AFuncPtr &func): func(func) { }
		
		Funcall::Funcall(const AFimpPtr &fimp): func(fimp->afunc()), fimp(fimp) { }

		GetVar::GetVar(const Sym &id): id(id) { }

		Push::Push(const Box &value): value(value) { }

		PutVar::PutVar(const Sym &id): id(id) { }
	}
}
