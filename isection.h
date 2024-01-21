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

class CISection: public CXSType
{
    static const int m_numISDimensions = 4;
    public:
        CISection (const CVector<float>& fV);
        ~CISection ();

        // helper functions
        virtual void ComputeProperties ();

    private:
};
