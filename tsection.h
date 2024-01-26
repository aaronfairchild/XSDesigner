/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/
#pragma once
#include "..\libraryEXH\arraycontainersEXH.h"
#include "xstype.h"

class CTSection: public CXSType
{
    static const int m_numISDimensions = 4;
    public:
        CTSection (const CVector<float>& fV);
        ~CTSection ();

        // helper functions
        virtual void ComputeProperties ();

    private:
};
