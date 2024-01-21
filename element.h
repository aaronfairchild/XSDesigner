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
#include "xstype.h"

class CElement
{
    public:
        CElement ();                  // default ctor
        CElement(const CElement& EO); // copy ctor
        ~CElement ();                 // dtor

        // accessor functions
        void GetENodes (int& nSN, int& nEN) const;
        int  GetMatPropertyGroup () const;
        CXSType* GetEPropertyGroup () const;
        void GetLength(float& fLength) const;
        
        // modifier functions
        void SetENodes (const int nSN, const int nEN);
        void SetEPropertyGroup (CXSType*);
        void SetMatPropertyGroup (const int);
        void SetLength(const float fLength);

    private:
        int m_nSN;		     // start node
        int m_nEN;		     // end node
        int m_nMPGroup;      // material property group
        float m_fLength;     // length of the element
        CXSType* m_pEPGroup; // element property group
};
