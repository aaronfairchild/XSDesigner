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
#include <string>
#include "..\libraryEXH\arraycontainersEXH.h"

class CXSType
{
    public:
        enum class EPType {CIRCS, RECTS, TSECTION };

        CXSType ();
        CXSType (int);
        virtual ~CXSType ();

        // helper function
        void DisplayProperties ();

        // accessor functions
        void GetProperties(float&, float&, float&, float&, float&, float&, float&);
        void GetDimensions (CVector<float>&) const;
        void GetType(EPType& Type) const;
        virtual void ComputeProperties () = 0;

    private:
        void Initialize ();

    protected:
        std::string m_strID;           // identification tag
        EPType m_Type;	               // type of cross section
        float m_fArea;                 // x/s area
        float m_fIyy;                  // MOI y-axis
        float m_fIzz;                  // MOI z-axis
        float m_fSzz;                  // section modulus z-axis
        float m_fSyy;                  // secition modulus y-axis
        float m_fSFzz;                 // shear factor z-axis
        float m_fSFyy;                 // shear factor y-axis
        int   m_numDimensions;         // number of dimensions
        CVector<float> m_fVDimensions; // the dimensions
};
