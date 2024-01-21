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
#include "xstype.h"

class CCircHollow: public CXSType
{
    static const int m_numCircDimensions = 2;
    public:
        CCircHollow (const CVector<float>& fV);
        CCircHollow (const CCircHollow&);
        ~CCircHollow ();

        // helper functions
        virtual void ComputeProperties ();

    private:
};
