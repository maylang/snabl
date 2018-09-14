#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/scope.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/target.hpp"

namespace snabl {
	struct Op;

	struct AOpType {
		const string id;
		const size_t label_offs;
		AOpType(const string &id): id(id), label_offs(next_label_offs++) { }
		AOpType(const AOpType &) = delete;
		const AOpType &operator=(const AOpType &) = delete;

		virtual void dump(const Op &op, ostream &out) const { }
	private:
		static size_t next_label_offs;
	};

	template <typename ImpT>
	struct OpType: public AOpType {
		OpType(const string &id): AOpType(id) { }
		void dump(const Op &op, ostream &out) const override;
		virtual void dump(const ImpT &op, ostream &out) const { }
	};
	
	using Ops = deque<Op>;

	namespace ops {
		struct Call {				
			static const OpType<Call> type;
		};

		struct DDrop {
			static const OpType<DDrop> type;
		};

		struct Drop {
			static const OpType<Drop> type;
		};

		struct Dup {
			static const OpType<Dup> type;
		};

		struct Else {
			static const OpType<Else> type;
			optional<size_t> nops;
		};
		
		struct Eqval {
			struct Type: public OpType<Eqval> {
				Type(const string &id): OpType<Eqval>(id) { }
				void dump(const Eqval &op, ostream &out) const override;
			};

			static const Type type;
			const optional<const Box> rhs;
			Eqval(const optional<const Box> rhs=nullopt): rhs(rhs) { }
		};

		struct Fimp {
			struct Type: public OpType<Fimp> {
				Type(const string &id): OpType<Fimp>(id) { }
				void dump(const Fimp &op, ostream &out) const override;
			};

			static const Type type;
			const FimpPtr ptr;
			
			Fimp(const FimpPtr &ptr): ptr(ptr) { }
		};

		struct FimpEnd {
			static const OpType<FimpEnd> type;
			const bool end_scope;
			FimpEnd(bool end_scope): end_scope(end_scope) { }
		};

		struct Funcall {
			struct Type: public OpType<Funcall> {
				Type(const string &id): OpType<Funcall>(id) { }
				void dump(const Funcall &op, ostream &out) const override;
			};
			
			static const Type type;
			const FuncPtr func;
			const FimpPtr fimp;
			
			FimpPtr prev_fimp;
			Funcall(const FuncPtr &func);
			Funcall(const FimpPtr &fimp);
		};
		
		struct Get {
			static const OpType<Get> type;
			const Sym id;
			Get(Sym id): id(id) { }
		};

		struct Isa {
			struct Type: public OpType<Isa> {
				Type(const string &id): OpType<Isa>(id) { }
				void dump(const Isa &op, ostream &out) const override;
			};

			static const Type type;
			const ATypePtr rhs;
			Isa(const ATypePtr &rhs): rhs(rhs) { }
		};

		struct Lambda {
			static const OpType<Lambda> type;
			optional<size_t> start_pc, nops;
			Target::Opts opts;
			Lambda(): opts(Target::Opts::None) { }
		};

		struct LambdaEnd {
			static const OpType<LambdaEnd> type;
		};

		struct Let {
			static const OpType<Let> type;
			const Sym id;
			Let(Sym id): id(id) { }
		};

		struct Nop {
			static const OpType<Nop> type;
		};

		struct Push {
			struct Type: public OpType<Push> {
				Type(const string &id): OpType<Push>(id) { }
				void dump(const Push &op, ostream &out) const override;
			};
				
			static const Type type;			
			const Box val;
			Push(const Box &val): val(val) { }
			template <typename ValT, typename... ArgsT>
			Push(const TypePtr<ValT> &type, ArgsT &&...args):
				val(type, forward<ArgsT>(args)...) { }
		};

		struct Recall {
			static const OpType<Recall> type;
		};

		struct Rot {
			static const OpType<Rot> type;
		};

		struct RSwap {
			static const OpType<RSwap> type;
		};

		struct SDrop {
			static const OpType<SDrop> type;
		};

		struct Skip {
			struct Type: public OpType<Skip> {
				Type(const string &id): OpType<Skip>(id) { }

				void dump(const Skip &op, ostream &out) const override {
					out << ' ' << *op.nops;
				}
			};

			static const Type type;
			optional<size_t> nops;

			Skip(optional<size_t> nops=nullopt): nops(nops) { }
		};

		struct Split {
			static const OpType<Split> type;
		};

		struct SplitEnd {
			static const OpType<SplitEnd> type;
		};

		struct Stack {
			static const OpType<Stack> type;
			const bool unsplit;
			Stack(bool unsplit): unsplit(unsplit) { }
		};

		struct Swap {
			static const OpType<Swap> type;
		};

		struct Try {
			static const OpType<Try> type;
			optional<size_t> handler_pc;
			optional<State> state;
		};

		struct TryEnd {
			static const OpType<TryEnd> type;
		};
	}
	
	struct Op {
		const AOpType &type;
		const Pos pos;
		
		template <typename ImpT, typename... ArgsT>
		Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args);

		virtual ~Op() { }

		template <typename ImpT>
		const ImpT &as() const;

		template <typename ImpT>
		ImpT &as();

		void dump(ostream &out) const {
			out << type.id;
			type.dump(*this, out);
			out << endl;
		}
	private:
		variant<ops::Call, ops::DDrop, ops::Drop, ops::Dup, ops::Else, ops::Eqval,
					  ops::Fimp, ops::FimpEnd, ops::Funcall, ops::Get, ops::Isa,
						ops::Lambda, ops::LambdaEnd, ops::Let, ops::Nop, ops::Push, ops::Recall,
						ops::Rot, ops::RSwap, ops::SDrop, ops::Skip, ops::Split, ops::SplitEnd,
						ops::Stack, ops::Swap, ops::Try, ops::TryEnd> _imp;
	};
	
	template <typename ImpT, typename... ArgsT>
	Op::Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args):
		type(type), pos(pos), _imp(ImpT(forward<ArgsT>(args)...)) { }

	template <typename ImpT>
	const ImpT &Op::as() const { return get<ImpT>(_imp); }

	template <typename ImpT>
	ImpT &Op::as() { return get<ImpT>(_imp); }

	template <typename ImpT>
	void OpType<ImpT>::dump(const Op &op, ostream &out) const {
		dump(op.as<ImpT>(), out);
	}
}

#endif
