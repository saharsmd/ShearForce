#include "ShearForce.hpp"
#include <math.h>
#include <cmath>
template<unsigned DIM>
ShearForce<DIM>::ShearForce()
  : AbstractForce<DIM>(),
    // Self propulsion force magnitude. Each vertex has contributions
    // with this magnitude (but different directions) from the three
    // cells which it connects.
    mF1(0.0)
{
}

template<unsigned DIM>
ShearForce<DIM>::~ShearForce()
{
}

template<unsigned DIM>
void ShearForce<DIM>::SetF1(double newValue)
{
  mF1 = newValue;
}

template<unsigned DIM>
double ShearForce<DIM>::GetF1()
{
  return mF1;
}

template<unsigned DIM>
void ShearForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation)
{
  // Throw an exception message if not using a VertexBasedCellPopulation
  if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation) == nullptr)
    {
      EXCEPTION("ShearForce is to be used with a VertexBasedCellPopulation only");
    }

  // Define some helper variables
  VertexBasedCellPopulation<DIM>* p_cell_population = static_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation);
  unsigned num_nodes = p_cell_population->GetNumNodes();
  unsigned width = p_cell_population->GetWidth(1);

  
  
  // Iterate over vertices in the cell population
  for (unsigned node_index=0; node_index<num_nodes; node_index++)
    {
      c_vector<double,DIM> force = zero_vector<double>(DIM);

      c_vector<double,DIM>location_node = p_cell_population->GetNode(node_index)->rGetLocation()

      
      added_force = location_node[1]
      
      force[0] = GetF1()*added_force
     
      p_cell_population->GetNode(node_index)->AddAppliedForceContribution(force);
    }
}

template<unsigned DIM>
void ShearForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<F1>" << mF1 << "</F1>\n";

    // Call direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

// Explicit instantiation
template class ShearForce<2>;
template class ShearForce<3>;    // Might work in three dimensions but only adds force in x-y plane

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ShearForce)
