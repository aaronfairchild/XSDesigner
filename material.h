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

class CMaterial
{
    public:
        CMaterial ();   // ctor
        ~CMaterial ();  // dtor

        // accessor functions
        float GetYM () const;
        float GetCTE () const;

        // modifier functions
        void SetYM (const float);
        void SetCTE (const float);

    private:
        float m_fYM;   // young's modulus
        float m_fCTE;  // coef of thermal expansion
};
