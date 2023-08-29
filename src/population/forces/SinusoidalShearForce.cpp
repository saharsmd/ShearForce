#include "SinusoidalShearForce.hpp"
// #include <math.h>
// #include <cmath>

template<unsigned DIM>
SinusoidalShearForce<DIM>::SinusoidalShearForce()
  : AbstractForce<DIM>(),
    mF1(0.0)
{
}

template<unsigned DIM>
SinusoidalShearForce<DIM>::~SinusoidalShearForce()
{
}

template<unsigned DIM>
void SinusoidalShearForce<DIM>::SetF1(double newValue)
{
  mF1 = newValue;
}

template<unsigned DIM>
double SinusoidalShearForce<DIM>::GetF1()
{
  return mF1;
}

template<unsigned DIM>
void SinusoidalShearForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation)
{
  // Throw an exception message if not using a VertexBasedCellPopulation
  if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation) == nullptr)
    {
      EXCEPTION("SinusoidalShearForce is to be used with a VertexBasedCellPopulation only");
    }

  // Define some helper variables
  VertexBasedCellPopulation<DIM>* p_cell_population = static_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation);
  unsigned num_nodes = p_cell_population->GetNumNodes();

  // Should equal N*(3/4)**(1/4) for periodic bcs (toroidal) with unit cell area
  double width = p_cell_population->GetWidth(1);    // Height

  // Iterate over vertices in the cell population
  for (unsigned node_index=0; node_index<num_nodes; node_index++)
    {
      c_vector<double,DIM> force = zero_vector<double>(DIM);

      // DB: I needed to initialization location_node somehow to
      // prevent the following warning: "error: ‘*((void*)&
      // location_node +16)’ may be used uninitialized in this
      // function [-Werror=maybe-uninitialized] force[0] = -1 *
      // GetF1() * (width/2 - location_node[1]);"
      c_vector<double,DIM> location_node = zero_vector<double>(DIM);
      location_node = p_cell_population->GetNode(node_index)->rGetLocation();

      force[0] = GetF1() * sin(2 * M_PI * location_node[1] / width);

      p_cell_population->GetNode(node_index)->AddAppliedForceContribution(force);
    }
}

template<unsigned DIM>
void SinusoidalShearForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<F1>" << mF1 << "</F1>\n";

    // Call direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

// Explicit instantiation
template class SinusoidalShearForce<2>;
template class SinusoidalShearForce<3>;    // Might work in three dimensions but only adds force in x-y plane

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(SinusoidalShearForce)
