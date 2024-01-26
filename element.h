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
#include <fstream>
#include <iostream>
#include <functional>
#include <string>
#include "constants.h"
#include "..\libraryEXH\arraycontainersEXH.h"
#include "..\libraryEXH\parserEXH.h"
#include "..\libraryEXH\GlobalErrorHandler.h"
#include "..\libraryEXH\fileioEXH.h"
#include "element.h"
#include "material.h"
#include "reinforcement.h"
#include "xstype.h"
#include "rectsolid.h"
#include "nodalloads.h"
#include "elementloads.h"
#include "LocalErrorHandler.h"
#include "MatToolBox.h"
#include "clockEXH.h"

class CElement
{
    public:
        CElement ();                  // default ctor
        ~CElement ();                 // dtor
        enum class ERRORCODE {
            NUMNODES, NUMELEMENTS, DEBUGCODE,
            NODENUMBER, ELEMENTNUMBER, XSAREA,
            YOUNGSMODULUS, UNSTABLE, INVALIDINPUT,
            INVALIDLASTLINE, NODALFIXITY, NUMMATGROUPS,
            NUMEPROPGROUPS, EPROPNUMBER, XSDIMENSION,
            MATGROUPNUMBER, MATPROPERTY, ELOADTYPE,
            XSTYPE, INVALIDCOMMANDLINE,
            CANNOTOPENIFILE, CANNOTOPENOFILE
        };
        enum class ElementType {
            COLUMN, BEAM
        };

        // helper functions
        void Banner(std::ostream& OF);
        void PrepareIO(int argc, char* argv[]);
        void Analyze();
        void TerminateProgram();
        void DisplayErrorMessage(CLocalErrorHandler::ERRORCODE);

        // accessor functions
        int  GetMatPropertyGroup () const;
        CXSType* GetEPropertyGroup () const;
        void GetLength(float& fLength) const;
        ElementType GetType() const; // returns element type
        
        // modifier functions
        void SetEPropertyGroup (CXSType*);
        void SetMatPropertyGroup (const int);
        void SetLength(const float fLength);
        void SetType(ElementType Type); // sets element type
        

    private:
        int m_nXSR;            // # of XS reinforcements
        int m_nTR;             // # of transverse reinforcements
        int m_nElementLoads;   // # of element loads

        int m_nDOF;			   // total degrees-of-freedom
        int m_nDebugLevel;	   // debugging level

        int m_nMPGroup;      // material property group
        float m_fLength;     // length of the element
        CXSType* m_pEPGroup; // element property group
        ElementType m_Type;  // element type (beam or column)

        CVector<CElement>			m_ElementData;             // element data
        CVector<CMaterial>			m_ConcMatData;            // concrete material data
        CVector<CMaterial>          m_ReMatData;              // reinforcement material data
        CVector<CXSType*>			m_EPData;                  // element property data
        CVector<CReinforcement>     m_ReData;                  // reinforcement data
        CVector<CNodalLoads>		m_NodalLoadData;           // nodal load data
        CVector<CElementLoads>		m_ElementLoadData;         // element load data

        CMatrix<double> m_ELL;	 // element loads (local coor system)


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


        void ReadProblemSize();
        void ReadFrameModel();
        void CreateOutput();

        // modifier functions
        void SetSize();

        // error handlers
        void ErrorHandler(CLocalErrorHandler::ERRORCODE);        // gateway to local error handler
        void ErrorHandler(CGlobalErrorHandler::ERRORCODE) const; // gateway to global error handler
        void IOErrorHandler(ERRORCODE nCode) const;
};
