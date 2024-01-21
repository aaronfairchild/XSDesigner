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

class CRectSolid: public CXSType
{
    static const int m_numRectDimensions = 2;
    public:
        CRectSolid (const CVector<float>& fV);
        CRectSolid (const CRectSolid&);
        ~CRectSolid ();

        // helper functions
        virtual void ComputeProperties ();

    private:
};
