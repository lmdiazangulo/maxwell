#pragma once

#include "mfem.hpp"
#include "BilinearIntegrators.h"
#include "Types.h"
#include "Model.h"
#include "Sources.h"

#include <array>

namespace maxwell {

class FiniteElementEvolutionNoCond : public TimeDependentOperator {
public:

	struct Options {
		FluxType fluxType = FluxType::Upwind;
	};

	static const std::size_t numberOfFieldComponents = 2;
	static const std::size_t numberOfMaxDimensions = 3;

	FiniteElementEvolutionNoCond(FiniteElementSpace* fes, Options options, Model& model, Sources& sources);
	virtual void Mult(const Vector& x, Vector& y) const;
	virtual ~FiniteElementEvolutionNoCond() = default;

private:
	struct FluxCoefficient {
		double alpha;
		double beta;
	};

	typedef std::unique_ptr<BilinearForm> Operator;

	FiniteElementSpace* fes_;
	Options opts_;
	Model model_;
	Sources sources_;

	std::array<std::array<Operator, 3>, 2> MS_;
	std::array<std::array<std::array<Operator, 3>, 2>, 2> MF_;
	std::array<std::array<std::array<Operator, 3>, 2>, 2> MP_;

	Vector buildNVector(const Direction& d) const;
	Vector buildPieceWiseArgVector(const FieldType& f) const;
	
	Operator buildDerivativeOperator(const Direction&) const;
	Operator buildInverseMassMatrix(const FieldType&) const;
	Operator buildFluxOperator(const FieldType&, const Direction& d) const;
	Operator buildPenaltyOperator(const FieldType& f, const Direction& d) const;

	Operator buildByMult(const BilinearForm*, const BilinearForm*) const;

	FluxCoefficient interiorFluxCoefficient() const;
	FluxCoefficient interiorPenaltyFluxCoefficient() const;
	FluxCoefficient boundaryFluxCoefficient(const FieldType&, const BdrCond& bdrC) const;
	FluxCoefficient boundaryPenaltyFluxCoefficient(const FieldType&, const BdrCond& bdrC) const;
};

}