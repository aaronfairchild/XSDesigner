/*********************************************
Planar Frame Analysis Program
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis
*********************************************/
#pragma once
#include <fstream>
#include <iostream>
#include <functional>
#include <string>
#include "constants.h"
#include "..\libraryEXH\arraycontainersEXH.h"
#include "..\libraryEXH\parserEXH.h"
#include "..\libraryEXH\GlobalErrorHandler.h"
#include "..\libraryEXH\fileioEXH.h"
#include "node.h"
#include "element.h"
#include "material.h"
#include "xstype.h"
#include "rectsolid.h"
#include "isection.h"
#include "nodalloads.h"
#include "elementloads.h"
#include "nodalresponse.h"
#include "elementresponse.h"
#include "LocalErrorHandler.h"
#include "MatToolBox.h"
#include "clockEXH.h"

class CFrame
{
    public:
        CFrame ();    // ctor
        ~CFrame ();   // dtor
        enum class ERRORCODE {NUMNODES, NUMELEMENTS, DEBUGCODE,
                              NODENUMBER, ELEMENTNUMBER, XSAREA,
                              YOUNGSMODULUS, UNSTABLE, INVALIDINPUT,
                              INVALIDLASTLINE, NODALFIXITY, NUMMATGROUPS,
                              NUMEPROPGROUPS, EPROPNUMBER, XSDIMENSION,
                              MATGROUPNUMBER, MATPROPERTY, ELOADTYPE,
                              XSTYPE, INVALIDCOMMANDLINE,
		                      CANNOTOPENIFILE, CANNOTOPENOFILE};

        // helper functions
        void Banner (std::ostream& OF);
        void PrepareIO (int argc, char *argv[]);
        void Analyze ();
        void TerminateProgram ();
        void DisplayErrorMessage (CLocalErrorHandler::ERRORCODE);

    private:
        int m_nNodes;		   // number of nodes
        int m_nElements;       // number of elements
        int m_nEPGroups;       // number of x/s properties
        int m_nMatGroups;      // number of material groups
        int m_nElementLoads;   // number of element loads

        int m_nDOF;			   // total degrees-of-freedom
        int m_nDebugLevel;	   // debugging level

        double m_dAbsErr;        // absolute error
        double m_dRelAbsErr;     // relative absolute error


        CVector<CNode>				m_NodalData;               // nodal data
        CVector<CElement>			m_ElementData;             // element data
        CVector<CMaterial>			m_MaterialData;            // material data
        CVector<CXSType*>			m_EPData;                  // element property data
        CVector<CNodalLoads>		m_NodalLoadData;           // nodal load data
        CVector<CElementLoads>		m_ElementLoadData;         // element load data
        CVector<CNodalResponse>		m_NodalResponseData;       // nodal response data
        CVector<CElementResponse>	m_ElementResponseData;     // element response data

        CMatrix<double> m_SSM;	 // structural stiffness matrix
        CVector<double> m_SND;	 // structural nodal displacements
        CVector<double> m_SNF;	 // structural nodal forces
        CVector<double> m_dR;    // Residual Vector
        CMatrix<double> m_ELL;	 // element loads (local coor system)
        CMatrix<double> m_ELG;	 // element loads (global coor system)

        CMatToolBox<int> m_MTBI;     // integer toolbox
        CMatToolBox<float> m_MTB;   // single precision toolbox
        CMatToolBox<double> m_MTBDP; // double precision toolbox

        CClock Timer;                 // timer

        // input and output
        std::ifstream m_FileInput;	   // file input
        int m_nLineNumber;	           // current line number in input file
        CParser m_Parse;               // parser for free format read
        int     m_nV;                  // integer value that is read in
        float   m_fV;                  // float value that is read in
        std::string m_strDelimiters;   // delimiters used in input file
        std::string m_strComment;      // characters to signify comment line
        std::vector<std::string> m_strVTokens; // vector to store tokens read
        std::string m_strDateTime;     // date and time at start of analysis
        std::string m_strEndDateTime;     // date and time at end of analysis
        int     m_nTokens;             // number of tokens read
        std::ofstream m_FileOutput;	   // file output
        CFileIO m_FIO;                 // for file operations
        CLocalErrorHandler m_LEH;      // for handling errors detected by program

        // FEA-related functions
        void ReadProblemSize ();
        void ReadFrameModel ();
        void ConstructK ();
        void ConstructF ();
        void ImposeBC ();
        void Solve ();
        void Response ();
        void CreateOutput ();
        void SuppressDOF (const int, const float);
        void Residual();

        // modifier functions
        void SetSize ();

        // error handlers
        void ErrorHandler (CLocalErrorHandler::ERRORCODE);        // gateway to local error handler
        void ErrorHandler (CGlobalErrorHandler::ERRORCODE) const; // gateway to global error handler
        void IOErrorHandler (ERRORCODE nCode) const;
};
