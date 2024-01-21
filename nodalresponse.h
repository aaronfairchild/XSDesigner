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

class CNodalResponse
{
    public:
        CNodalResponse ();   // ctor
        ~CNodalResponse ();  // dtor

        // accessor functions
        void GetValues (CVector<float>& fVDisp) const;
        void GetReactions(float& fRX, float& fRY, float& fRZ) const;

        // modifier functions
        void SetValues (const CVector<float>& fVDisp);
        void SetReactions(const float fRX, const float fRY, const float fRZ);

    private:
        CVector<float> m_fVDisp; // nodal displacements	
        float m_fRX;
        float m_fRY;
        float m_fRZ;
};
