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
#include "TargetAreaAndNematicPerimeterForce.hpp"

template<unsigned DIM>
TargetAreaAndNematicPerimeterForce<DIM>::TargetAreaAndNematicPerimeterForce()
   : AbstractForce<DIM>(),
     mKA(0.0),
     mKP(0.0),
     mP0(1.0),
     mLambda(0.0)    // Strength of coupling between cell elongation and line tension
{
}

template<unsigned DIM>
TargetAreaAndNematicPerimeterForce<DIM>::~TargetAreaAndNematicPerimeterForce()
{
}

template<unsigned DIM>
void TargetAreaAndNematicPerimeterForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation)
{
    // Throw an exception message if not using a VertexBasedCellPopulation
    if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation) == nullptr)
    {
        EXCEPTION("TargetAreaAndNematicPerimeterForce is to be used with a VertexBasedCellPopulation only");
    }

    // Define some helper variables
    VertexBasedCellPopulation<DIM>* p_cell_population = static_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation);
    unsigned num_nodes = p_cell_population->GetNumNodes();
    unsigned num_elements = p_cell_population->GetNumElements();

    // Begin by computing the area and perimeter of each element in
    // the mesh, to avoid having to do this multiple times
    std::vector<double> element_areas(num_elements);
    std::vector<double> element_perimeters(num_elements);
    std::vector<double> target_areas(num_elements);
    for (typename VertexMesh<DIM,DIM>::VertexElementIterator elem_iter = p_cell_population->rGetMesh().GetElementIteratorBegin();
         elem_iter != p_cell_population->rGetMesh().GetElementIteratorEnd();
         ++elem_iter)
    {
        unsigned elem_index = elem_iter->GetIndex();
        element_areas[elem_index] = p_cell_population->rGetMesh().GetVolumeOfElement(elem_index);
        element_perimeters[elem_index] = p_cell_population->rGetMesh().GetSurfaceAreaOfElement(elem_index);

	//TODO: Currently a bit of a hack to save perimeters to
	//CellData here. Should be performed inside
	//CellTensionModifier instead but that involves changing the
	//AbstractCellPopulation to a VertexBasedCellPopulation
	//therein.
	p_cell_population->GetCellUsingLocationIndex(elem_index)->GetCellData()->SetItem("perimeter", element_perimeters[elem_index]);

        try
        {
            // If we haven't specified a modifier, there won't be any
            // target areas in the CellData array and CellData will
            // throw an exception that it doesn't have "target area"
            // entries. We add this piece of code to give a more
            // understandable message. There is a slight chance that
            // the exception is thrown although the error is not about
            // the target areas.
            target_areas[elem_index] = p_cell_population->GetCellUsingLocationIndex(elem_index)->GetCellData()->GetItem("Target Area");
        }
        catch (Exception&)
        {
            EXCEPTION("In order to use TargetAreaAndNematicPerimeterForce you need to assign each cell a 'Traget Area', e.g. by adding a ErkPropulsionModifierNoAlignment to the simulation");
        }


    }

    // std::vector<c_vector<double, DIM>> short_axis(num_elements);    // Angle of major axis
    std::vector<double> orientation(num_elements);    // Angle of short axis
    std::vector<double> elongation_factor(num_elements);    // Ratio of eigenvalues for major and minor axes

    for (typename VertexMesh<DIM,DIM>::VertexElementIterator elem_iter = p_cell_population->rGetMesh().GetElementIteratorBegin();
         elem_iter != p_cell_population->rGetMesh().GetElementIteratorEnd();
         ++elem_iter)
    {
      unsigned elem_index = elem_iter->GetIndex();
      elongation_factor[elem_index] = p_cell_population->rGetMesh().GetElongationShapeFactorOfElement(elem_index);
      c_vector<double, DIM> s_ax = p_cell_population->rGetMesh().GetShortAxisOfElement(elem_index);
      orientation[elem_index] = atan2(s_ax[1], s_ax[0]);
      // short_axis[elem_index] = p_cell_population->rGetMesh().GetShortAxisOfElement(elem_index);
    }

    // TODO: Redo the above by iterating over cells and then finding
    // the element so that we can add orientation and
    // elongation_factor to CellData here rather than in
    // the CellTensionModifier
    // for (typename VertexBasedCellPopulation<DIM>::Iterator cell_iter = p_cell_population->Begin();
    //      cell_iter != p_cell_population->End();
    //      ++cell_iter)
    //   {
    // 	VertexElement<DIM,DIM>* elem = p_cell_population->GetElementCorrespondingToCell(cell_iter);
    // 	unsigned elem_index = elem->GetIndex();
    // 	// unsigned elem_index = p_cell_population->GetElementCorrespondingToCell(cell_iter)->GetIndex();
    // 	elongation_factor[elem_index] = p_cell_population->rGetMesh().GetElongationShapeFactorOfElement(elem_index);
    // 	c_vector<double,DIM> s_ax = p_cell_population->rGetMesh().GetShortAxisOfElement(elem_index);
    // 	orientation[elem_index] = atan2(s_ax[1], s_ax[0]);

    // 	cell_iter->GetCellData()->SetItem("elongation", elongation_factor[elem_index]);
    // 	cell_iter->GetCellData()->SetItem("orientation", orientation[elem_index]);
    //   }



    // Iterate over vertices in the cell population
    for (unsigned node_index=0; node_index<num_nodes; node_index++)
    {
        /*
         * The force on this Node is given by the gradient of the
         * total free energy of the CellPopulation, evaluated at the
         * position of the vertex. This free energy is the sum of the
         * free energies of all CellPtrs in the cell population. The
         * free energy of each CellPtr is comprised of two parts - an
         * elastic area energy around a target area and an elastic
         * perimeter energy around a target perimeter.
         *
         * Note that since the movement of this Node only affects the free energy
         * of the CellPtrs containing it, we can just consider the contributions
         * to the free energy gradient from each of these CellPtrs.
         */
        c_vector<double, DIM> area_contribution = zero_vector<double>(DIM);
        c_vector<double, DIM> perimeter_contribution = zero_vector<double>(DIM);

	// Extra line tension in direction of elongation
	c_vector<double, DIM> nematic_contribution = zero_vector<double>(DIM);

        // Find the indices of the elements owned by this node
        std::set<unsigned> containing_elem_indices = p_cell_population->GetNode(node_index)->rGetContainingElementIndices();

        // Iterate over these elements
        for (std::set<unsigned>::iterator iter = containing_elem_indices.begin();
             iter != containing_elem_indices.end();
             ++iter)
        {
            // Get this element, its index and its number of nodes
            VertexElement<DIM, DIM>* p_element = p_cell_population->GetElement(*iter);
            unsigned elem_index = p_element->GetIndex();
            unsigned num_nodes_elem = p_element->GetNumNodes();

            // Find the local index of this node in this element
            unsigned local_index = p_element->GetNodeLocalIndex(node_index);

            // Add the force contribution from this cell's perferred area term
            c_vector<double, DIM> element_area_gradient = p_cell_population->rGetMesh().GetAreaGradientOfElementAtNode(p_element, local_index);
            area_contribution -= 2*GetKA()*(element_areas[elem_index] - target_areas[elem_index])*element_area_gradient;

            // Local index of the previous node connected to this node
            // (previous in index order) within this element
            unsigned previous_node_local_index = (num_nodes_elem+local_index-1)%num_nodes_elem;
	    // Local index of the next node (next in index order)
	    unsigned next_node_local_index = (local_index+1)%num_nodes_elem;

            // Compute the gradient of each these edges, computed at the present node
            c_vector<double, DIM> previous_edge_gradient = -p_cell_population->rGetMesh().GetNextEdgeGradientOfElementAtNode(p_element, previous_node_local_index);
            c_vector<double, DIM> next_edge_gradient = p_cell_population->rGetMesh().GetNextEdgeGradientOfElementAtNode(p_element, local_index);

	    // Get a vector for the short axis of the element
	    // Get the elongation factor of the element

            // Add the force contribution from this cell's perferred perimeter term
            c_vector<double, DIM> element_perimeter_gradient = previous_edge_gradient + next_edge_gradient;
	    perimeter_contribution -= 2*GetKP()*(element_perimeters[elem_index] - GetP0())*element_perimeter_gradient;

	    // Get vectors along the two edges connecting this node
	    // within this element
	    c_vector<double, DIM> previous_vec = p_cell_population->rGetMesh().GetVectorFromAtoB(p_element->GetNodeLocation(local_index), p_element->GetNodeLocation(previous_node_local_index));
	    c_vector<double, DIM> next_vec = p_cell_population->rGetMesh().GetVectorFromAtoB(p_element->GetNodeLocation(local_index), p_element->GetNodeLocation(next_node_local_index));

	    // The angle of orientation of the edges connected to the
	    // vertex
	    double previous_edge_orientation = atan2(previous_vec[1], previous_vec[0]);
	    double next_edge_orientation = atan2(next_vec[1], next_vec[0]);

	    // Line tension contribution from alignment of edges with
	    // cell elongation. If Lambda is positive, edges aligned
	    // with the minor axis have positive line tensions and
	    // tent to extend whereas edges anti-aligned with the
	    // minor axis (i.e. aligned with the major axis) have
	    // negative line tensions and tend to contract. The
	    // elongation factor characterizes the degree of
	    // elongation of the element. It is the squareroot of the
	    // ratio of the eigenvalues associated with the major and
	    // minor axes calculated from the shape matrix of the
	    // element (see Killeen et al 2022). A square or regular
	    // hexagon has elongation factor 1, whereas a 2x1
	    // rectangle has elongation factor 2 so that there is only
	    // non-zero nematic contribution for regular polygons
	    nematic_contribution += GetLambda()*(elongation_factor[elem_index]-1)*cos(2*(orientation[elem_index] - previous_edge_orientation))*previous_edge_gradient;
	    nematic_contribution += GetLambda()*(elongation_factor[elem_index]-1)*cos(2*(orientation[elem_index] - next_edge_orientation))*next_edge_gradient;
	}
        c_vector<double, DIM> force_on_node = area_contribution + perimeter_contribution + nematic_contribution;
        p_cell_population->GetNode(node_index)->AddAppliedForceContribution(force_on_node);
    }
}

template<unsigned DIM>
double TargetAreaAndNematicPerimeterForce<DIM>::GetKA()
{
    return mKA;
}

template<unsigned DIM>
double TargetAreaAndNematicPerimeterForce<DIM>::GetKP()
{
    return mKP;
}

template<unsigned DIM>
double TargetAreaAndNematicPerimeterForce<DIM>::GetP0()
{
    return mP0;
}

template<unsigned DIM>
double TargetAreaAndNematicPerimeterForce<DIM>::GetLambda()
{
    return mLambda;
}

template<unsigned DIM>
void TargetAreaAndNematicPerimeterForce<DIM>::SetKA(double KA)
{
    mKA = KA;
}

template<unsigned DIM>
void TargetAreaAndNematicPerimeterForce<DIM>::SetKP(double KP)
{
    mKP = KP;
}

template<unsigned DIM>
void TargetAreaAndNematicPerimeterForce<DIM>::SetP0(double P0)
{
    mP0 = P0;
}

template<unsigned DIM>
void TargetAreaAndNematicPerimeterForce<DIM>::SetLambda(double Lambda)
{
    mLambda = Lambda;
}

template<unsigned DIM>
void TargetAreaAndNematicPerimeterForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<KA>" << mKA << "</KA>\n";
    *rParamsFile << "\t\t\t<KP>" << mKP << "</KP>\n";
    *rParamsFile << "\t\t\t<P0>" << mP0 << "</P0>\n";

    // Call method on direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

// Explicit instantiation
template class TargetAreaAndNematicPerimeterForce<1>;
template class TargetAreaAndNematicPerimeterForce<2>;
template class TargetAreaAndNematicPerimeterForce<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(TargetAreaAndNematicPerimeterForce)
