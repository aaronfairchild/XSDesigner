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

class CElementLoads
{
    public:
        enum class ELType {CONCENTRATEDY, CONCENTRATEDX, CONCENTRATEDM, DISTRIBUTEDY};

        CElementLoads ();                         // default ctor
        CElementLoads (const CElementLoads& ELO); // copy ctor
        ~CElementLoads ();                        // dtor

        // accessor functions
        void GetValues (int& nE, ELType&, 
                        float& fLeft, float& fRight) const;
        void GetENF (CVector<float>& fV1, CVector<float>& fV2,
                     const float fLength) const;

        // modifier functions
        void SetValues (const int nE, const ELType Type,
                        const float fLeft, const float fRight);
    private:
        int    m_nElement;	// element number
        ELType m_Type;	    // load type: concentrated or distributed
        float  m_fValue1;	// distance from start node
                            // or load intensity at start node
        float  m_fValue2;	// load value or
                            // load intensity at end node
};		
