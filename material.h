/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/
#pragma once

class CMaterial
{
    public:
        CMaterial ();   // ctor
        ~CMaterial ();  // dtor

        // accessor functions
        float GetYM () const;
        float GetCompStr () const;
        float GetYieldStr () const;
        float GetDensity () const;
        float GetPoisson () const;

        // modifier functions
        void SetYM (const float);
        void SetCompStr (const float);
        void SetYieldStr (const float);
        void SetDensity (const float);
        void SetPoisson (const float);



    private:
        float m_fYM;       // young's modulus
        float m_fCompStr;  // compressive strength
        float m_fYieldStr; // yield strength
        float m_fDensity;  // density
        float m_fPoisson;  // poisson's ratio
};
