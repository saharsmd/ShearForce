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

#ifndef TARGETAREAANDNEMATICPERIMETERFORCE_HPP_
#define TARGETAREAANDNEMATICPERIMETERFORCE_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include "Exception.hpp"

#include "AbstractForce.hpp"
#include "VertexBasedCellPopulation.hpp"

#include <iostream>

// Version 2 uses existing functionality
// GetElongationShapeFactorOfElement() and GetShortAxisOfElement()
// rather than my own implementation following Killeen 2022 which
// requires the Eigen package.

/**
 * A force class for use in vertex-based simulations, based on an
 * energy in which each cell has elastic terms around a preferred area
 * and perimeter.
 *
 * For each cell E = KA(A-A0)^2 + KP(P-P0)^2
 *
 * See e.g. Bi et al NatPhys (2015) "A density-independent rigidity
 * transition in biological tissues" and Equation 1 in Bi et al PRX
 * (2016) "Motility-Driven Glass and Jamming Transitions in Biological
 * Tissues"
 *
 * Line tensions are scaled by alingment with cell orientation. For edge i:
 *
 * gamma_i(1 - mLambda*(sqrt(eig_major/eig_minor)-1)*cos(2(phi_cell - phi_i)
 *
 * where eig_major and eig_minor are the major and minor eigenvalues
 * of the shape tensor of the cell, phi_eig is the angle of
 * orientation of the cell obtained from the major eigenvector of the
 * shape tensor, and phi_i is the orientation of the edge i. (See e.g. Killeen et al (2022)
 * doi:10.1103/PhysRevLett.128.078001
 *
 * The parameter mLambda determines the strength of the active
 * response to cell elongation. For positive mLambda line tensions
 * oppose elongation; for negative mLambda line tensions act to
 * encourage cell elongatation.
 */
template<unsigned DIM>
class TargetAreaAndNematicPerimeterForce  : public AbstractForce<DIM>
{
friend class TestForces;

private:

    friend class boost::serialization::access;
    /**
     * Boost Serialization method for archiving/checkpointing.
     * Archives the object and its member variables.
     *
     * @param archive  The boost archive.
     * @param version  The current version of this class.
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractForce<DIM> >(*this);
        archive & mKA;
        archive & mKP;
        archive & mP0;
	archive & mLambda;
    }

protected:

    /**
     * Cell deformation energy parameter. Has units of kg s^-2 (cell size at equilibrium rest length)^-1.
     */
    double mKA;

    /**
     * Cell membrane energy parameter. Has units of kg s^-2 (cell size at equilibrium rest length).
     */
    double mKP;

    /**
    * Preferred perimeter. Has units of cell size at equilibrium rest length.
    */
    double mP0;

    // The preferred area A0 is a variable inside the ERK-area ODE system and is stored in CellData

    /**
    * The strength of active response to cell elongation.
    */
    double mLambda;

public:

    /**
     * Constructor.
     */
    TargetAreaAndNematicPerimeterForce();

    /**
     * Destructor.
     */
    virtual ~TargetAreaAndNematicPerimeterForce();

    /**
     * Overridden AddForceContribution() method.
     *
     * Calculates the force on each node in the vertex-based cell population based on the
     * deviations away from preferred areas and perimeters.
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation);

    /**
     * @return mKA
     */
    double GetKA();

    /**
     * @return mKP
     */
    double GetKP();

    /**
     * @return mP0
     */
    double GetP0();

    /**
     * @return mLambda
     */
    double GetLambda();

     /**
     * Set mKA.
     *
     * @param KA the new value of mKA
     */
    void SetKA(double KA);

    /**
     * Set mKP.
     *
     * @param KP the new value of mKP
     */
    void SetKP(double KP);

    /**
     * Set mP0.
     *
     * @param P0 the new value of mP0
     */
    void SetP0(double P0);

    /**
     * Set mLambda.
     *
     * @param Lambda the new value of mLambda
     */
    void SetLambda(double Lambda);

    /**
     * Overridden OutputForceParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputForceParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(TargetAreaAndNematicPerimeterForce)

#endif /*TARGETAREAANDNEMATICPERIMETERFORCE_HPP_*/
