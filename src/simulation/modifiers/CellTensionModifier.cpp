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

#include "CellTensionModifier.hpp"
// #include "ErkPropulsionSrnModelVelocityAlignment.hpp"


template<unsigned DIM>
CellTensionModifier<DIM>::CellTensionModifier()
  : AbstractCellBasedSimulationModifier<DIM>(),
    mKA(1.0),
    mKP(1.0),
    mP0(3.8)
{
}

template<unsigned DIM>
CellTensionModifier<DIM>::~CellTensionModifier()
{
}

template<unsigned DIM>
void CellTensionModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
// void CellTensionModifier<DIM>::UpdateAtEndOfTimeStep(VertexBasedCellPopulation<DIM>& rCellPopulation)
{
  UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
void CellTensionModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
// void CellTensionModifier<DIM>::SetupSolve(VertexBasedCellPopulation<DIM>& rCellPopulation, std::string outputDirectory)
{
  /*
   * Update CellData in SetupSolve(), to assure that it has been
   * fully initialised before we enter the main time loop.
   */
  UpdateCellData(rCellPopulation);
}

template<unsigned DIM>
double CellTensionModifier<DIM>::GetKA()
{
  return mKA;
}

template<unsigned DIM>
double CellTensionModifier<DIM>::GetKP()
{
  return mKP;
}

template<unsigned DIM>
double CellTensionModifier<DIM>::GetP0()
{
  return mP0;
}

template<unsigned DIM>
void CellTensionModifier<DIM>::SetKA(double KA)
{
  mKA = KA;
}

template<unsigned DIM>
void CellTensionModifier<DIM>::SetKP(double KP)
{
  mKP = KP;
}

template<unsigned DIM>
void CellTensionModifier<DIM>::SetP0(double P0)
{
  mP0 = P0;
}

template<unsigned DIM>
void CellTensionModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
// void CellTensionModifier<DIM>::UpdateCellData(VertexBasedCellPopulation<DIM>& rCellPopulation)
{
    rCellPopulation.Update();

    // Iterate over the cell population and update the tension in
    // CellData. Nothing gets used by the ODE solver but this allows
    // us to visualize the variable "tension" in ParaView.
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
         cell_iter != rCellPopulation.End();
         ++cell_iter)
    // for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = rCellPopulation.Begin();
    //      cell_iter != rCellPopulation.End();
    //      ++cell_iter)
    {
      // // Get the current values of ERK and theta for this cell
      // ErkPropulsionSrnModelVelocityAlignment* p_model = static_cast<ErkPropulsionSrnModelVelocityAlignment*>(cell_iter->GetSrnModel());

      // Get the target area for this cell from the solver and update
      // in CellData
      // double A0 = p_model->GetTargetArea();
      double A0 = cell_iter->GetCellData()->GetItem("Target Area");

      // Get the area for this cell from the solver and update in
      // CellData
      // double A = rCellPopulation.GetVolumeOfCell(*cell_iter);
      double A = cell_iter->GetCellData()->GetItem("volume");
      double area_contribution = mKA*pow(A-A0, 2);

      // TODO: Get the perimeter of the cell
      // unsigned elem_index = rCellPopulation.GetElementCorrespondingToCell(*cell_iter)->GetIndex();
      // double P = rCellPopulation.rGetMesh().GetSurfaceAreaOfElement(elem_index);
      double P = cell_iter->GetCellData()->GetItem("perimeter");
      double perimeter_contribution = mKP*pow(P-mP0, 2);

      double tension = area_contribution + perimeter_contribution;
      cell_iter->GetCellData()->SetItem("tension", tension);
    }
}

template<unsigned DIM>
void CellTensionModifier<DIM>::OutputSimulationModifierParameters(out_stream& rParamsFile)
{
    // No parameters to output, so just call method on direct parent class
    AbstractCellBasedSimulationModifier<DIM>::OutputSimulationModifierParameters(rParamsFile);
}

// Explicit instantiation
// template class CellTensionModifier<1>;
template class CellTensionModifier<2>;
// template class CellTensionModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(CellTensionModifier)
