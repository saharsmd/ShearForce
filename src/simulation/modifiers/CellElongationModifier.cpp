/*

Copyright (c) 2005-2019, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "CellElongationModifier.hpp"

template<unsigned DIM>
CellElongationModifier<DIM>::CellElongationModifier()
  : AbstractCellBasedSimulationModifier<DIM>()
{
}

template<unsigned DIM>
CellElongationModifier<DIM>::~CellElongationModifier()
{
}

template<unsigned DIM>
void CellElongationModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
// void CellElongationModifier<DIM>::UpdateAtEndOfTimeStep(VertexBasedCellPopulation<DIM>& rCellPopulation)
{
  UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
void CellElongationModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
// void CellElongationModifier<DIM>::SetupSolve(VertexBasedCellPopulation<DIM>& rCellPopulation, std::string outputDirectory)
{
  /*
   * Update CellData in SetupSolve(), to assure that it has been
   * fully initialised before we enter the main time loop.
   */
  UpdateCellData(rCellPopulation);
}


template<unsigned DIM>
void CellElongationModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
// void CellElongationModifier<DIM>::UpdateCellData(VertexBasedCellPopulation<DIM>& rCellPopulation)
{
  // rCellPopulation.Update();

    // // Iterate over the cell population and update the tension in
    // // CellData. Nothing gets used by the ODE solver but this allows
    // // us to visualize the variable "tension" in ParaView.
    // for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
    //      cell_iter != rCellPopulation.End();
    //      ++cell_iter)
    // // for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
    // //      cell_iter != rCellPopulation.End();
    // //      ++cell_iter)
    // {
    //   VertexElement<DIM,DIM>* elem = rCellPopulation.GetElementCorrespondingToCell(cell_iter);
    //   unsigned elem_index = elem->GetIndex();
    //   // unsigned elem_index = rCellPopulation.GetElementCorrespondingToCell(cell_iter);
    //   c_vector<double,DIM> short_axis = rCellPopulation.rGetMesh().GetShortAxisOfElement(elem_index);
    //   // Nematic
    //   double orientation = atan2(short_axis[1], short_axis[0]) % M_PI;
    //   cell_iter->GetCellData()->SetItem("orientation", orientation);
    //   double elongation_factor = rCellPopulation->rGetMesh().GetElongationShapeFactorOfElement(elem_index);
    //   cell_iter->GetCellData()->SetItem("elongation", elongation_factor);
    // }
}

template<unsigned DIM>
void CellElongationModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
// template class CellElongationModifier<1>;
template class CellElongationModifier<2>;
// template class CellElongationModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(CellElongationModifier)
