/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis
*********************************************/
#pragma once
#include "arraycontainersEXH.h"
#include "constants.h"

class CElementResponse
{
    public:
        CElementResponse ();    // default ctor
        ~CElementResponse ();   // dtor

        // accessor functions
        void GetValues (CVector<float>& fVFStartNode,
                        CVector<float>& fVFEndNode) const;
        void GetStresses(float&, float&, float&) const;

        // modifier functions
        void SetValues (const CVector<float>& fVFStartNode,
                        const CVector<float>& fVFEndNode);
        void SetStresses(float& fCompStress, float& fTensStress,
                         float& fShearStress);
    private:
        CVector<float> m_fVFStartNode;
                // nodal forces at start node
        CVector<float> m_fVFEndNode;
                // nodal forces at end node
        float m_fCompStress;     // max compressive stress
        float m_fTensStress;     // max tensile stress
        float m_fShearStress;    // max shear stress
};
