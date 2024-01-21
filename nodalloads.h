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
#include "..\libraryEXH\arraycontainersEXH.h"
#include "constants.h"

class CNodalLoads
{
    public:
        CNodalLoads ();                   // default ctor 
        CNodalLoads (const CNodalLoads&); //copy ctor
        ~CNodalLoads ();

        // accessor functions
        void GetValues (CVector<float>&);

        // modifier functions
        void SetValues (const CVector<float>&);

    private:
        CVector<float> m_fVLoads;	// nodal loads
};
