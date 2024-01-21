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
#include <iostream>
#include "..\libraryEXH\arraycontainersEXH.h"
#include "constants.h"

class CNode
{
    public:
        enum class Fixity {FREE, SPECIFIED};
        friend std::ostream &operator<< (std::ostream&, 
                                         const CNode::Fixity&);
        CNode ();             // ctor
        CNode (const CNode&); // copy ctor
        ~CNode ();            // dtor

        // accessor functions
        void GetCoords (CVector<float>& fVC) const;
        void GetFixity (CVector<Fixity>& VFC, CVector<float>& fVDisp) const;

        // modifier functions
        void SetCoords (const CVector<float>& fVC);
        void SetFixity (const CVector<Fixity>& VFC, CVector<float>& fVDisp);

    private:
        CVector<float>	m_fVCoor;	// coordinates
        CVector<Fixity>	m_VFC;		// fixity conditions
        CVector<float>	m_fVDisp;	// known displacements
};
