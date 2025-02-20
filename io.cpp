/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/

#include <vector>
#include <iomanip>
#include <sstream>
#include "element.h"
#include "reinforcement.h"
#include "getinteractiveEXH.h"
#include "parserEXH.h"
#include "fileioEXH.h"
#include "circsolid.h"
#include "tsection.h"
#include "printtableEXH.h"
#include "xstype.h"

void CElement::Banner (std::ostream& OF)
// ---------------------------------------------------------------------------
// Function: Prints program banner
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    OF << '\n';
    OF << "\t\t--------------------------------------------" << '\n';
    OF << "\t\t   2D XS Designer -- Reinforced Concrete    " << '\n';
    OF << "\t\t          CEE 593: Applied Project          " << '\n';
    OF << "\t\t            By: Aaron Fairchild             " << '\n';
    OF << "\t\t--------------------------------------------" << '\n';
}

void CElement::PrepareIO (int argc, char *argv[])
// ---------------------------------------------------------------------------
// Function: Obtains file names and opens input/output files
// Input:    Command line arguments
// Output:   None
// ---------------------------------------------------------------------------
{
    if (argc == 1)
    {
        // open the input file
        m_FIO.OpenInputFileByName ("Complete input file name: ", m_FileInput,
                                   std::ios::in);

        // open the output file
        m_FIO.OpenOutputFileByName ("Complete output file name: ", m_FileOutput,
                                    std::ios::out);
    }
    else if (argc == 3) // XSDesigner input_file output_file
    {
        m_FileInput.open (argv[1], std::ios::in);
        if (!m_FileInput)
            ErrorHandler (CGlobalErrorHandler::ERRORCODE::CANNOTOPENIFILE);
        m_FileOutput.open (argv[2], std::ios::out);
        if (!m_FileOutput)
            ErrorHandler (CGlobalErrorHandler::ERRORCODE::CANNOTOPENOFILE);
        std::cout << "\n";
        std::cout << argv[1] << " opened as input file.\n";
        std::cout << argv[2] << " opened as output file.\n";
    }
	else
    {
        ErrorHandler (CLocalErrorHandler::ERRORCODE::INVALIDCOMMANDLINE);
    }

    // print banner
    Banner (m_FileOutput);
}

void CElement::ReadProblemSize ()
// ---------------------------------------------------------------------------
// Function: Reads the number of reinforcement groups (in and out of plane)
// Input:    None
// Output:   None
// TODO: Point to where the invalid input is, and what it is.
// ---------------------------------------------------------------------------
{
    bool bEOF = false;
    try
    {
        // Read the Problem Description
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*heading")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Unit Mode
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*unit" && m_strVTokens[1] != "mode")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Member Type
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*column" && m_strVTokens[1] != "or" && m_strVTokens[2] != "beam")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);


        // Concrete Material Properties
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*concrete" && m_strVTokens[1] != "material" && m_strVTokens[2] != "properties")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Reinforcement Material Properties
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*reinforcement" && m_strVTokens[1] != "material" && m_strVTokens[2] != "properties")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Element Geometry
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*geometry")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Cross-Sectional Reinforcement Data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*xs" && m_strVTokens[1] != "reinforcements")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        for (;;)
        {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
            if (m_strVTokens[0] == "*transverse" && m_strVTokens[1] == "reinforcements")
                break;
            ++m_nXSR;
        }

        // Out of Plane Reinforcement Data
        for (;;)
        {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
            if (m_strVTokens[0] == "*transverse" && m_strVTokens[1] == "spacings")
                break;
            ++m_nTR;
        }

        // Transverse Reinforcement Spacings
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Max Values
        if (m_strVTokens[0] != "*max" && m_strVTokens[1] != "values") {
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        }
        if (m_strVTokens[0] != "*max" && m_strVTokens[1] != "values")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
    }
    // Trap all input file errors here
    catch (CLocalErrorHandler::ERRORCODE &err)
    {
        m_LEH.ErrorHandler (err);
    }

    catch (CGlobalErrorHandler::ERRORCODE &err)
    {
        ErrorHandler (CLocalErrorHandler::ERRORCODE::INVALIDINPUT);
    }

    catch (...)
    {
        std::cout << "Sorry, could not catch the error whatever it is.\n";
    }

    // check data for validity
    if (m_nDebugLevel < 0 || m_nDebugLevel > 1) 
        IOErrorHandler (ERRORCODE::DEBUGCODE);
}

void CElement::ReadModel ()
// ---------------------------------------------------------------------------
// Function: Reads the rest of the frame data from input file
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    CVector<float> fVC(3);   // Stores float input
    bool bEOF = false;       // End of file flag

    try
    {
        // Rewind the file to read the input file again
        m_FIO.Rewind (m_FileInput);
        m_nLineNumber = 0;

        // Header line
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // Unit Mode
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // Member Type
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] == "column") {
            CElement::SetType(ElementType::COLUMN);
        }
        else if (m_strVTokens[0] == "beam") {
            CElement::SetType(ElementType::BEAM);
        }
        else {
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        }


        // Read concrete material data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_nTokens != 3)
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetFloatValue(m_strVTokens[0], fVC(1));
        m_Parse.GetFloatValue(m_strVTokens[1], fVC(2));
        m_Parse.GetFloatValue(m_strVTokens[2], fVC(3));
        if (fVC(1) <= 0.0f || fVC(2) <= 0.0f || fVC(3) <= 0.0f)
            IOErrorHandler(ERRORCODE::MATPROPERTY);

        m_ConcMatData(1).SetCompStr(fVC(1));
        m_ConcMatData(1).SetDensity(fVC(2));
        m_ConcMatData(1).SetPoisson(fVC(3));

        // Read reinforcement material data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_nTokens != 3)
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetFloatValue(m_strVTokens[0], fVC(1));
        m_Parse.GetFloatValue(m_strVTokens[1], fVC(2));
        m_Parse.GetFloatValue(m_strVTokens[2], fVC(3));
        if (fVC(1) <= 0.0f || fVC(2) <= 0.0f || fVC(3) <= 0.0f)
            IOErrorHandler(ERRORCODE::MATPROPERTY);

        m_ConcMatData(1).SetYieldStr(fVC(1));
        m_ConcMatData(1).SetYM(fVC(2));
        m_ConcMatData(1).SetDensity(fVC(3));

        // read cross-sectional geometry data
        std::string strTag;
        CVector<float> fVXSDims(MAXEPDIM);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        if (m_strVTokens[0] == "rectangular")
        {
            if (m_nTokens != 3)
                IOErrorHandler(ERRORCODE::INVALIDINPUT);
            m_Parse.GetFloatValue(m_strVTokens[1], fVXSDims(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(2));
            if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f)
                IOErrorHandler(ERRORCODE::XSDIMENSION);
            m_EPData(1) = new CRectSolid(fVXSDims);
        }
        else if (m_strVTokens[0] == "t")
        {
            if (m_nTokens != 5)
                IOErrorHandler(ERRORCODE::INVALIDINPUT);
            m_Parse.GetFloatValue(m_strVTokens[1], fVXSDims(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(2));
            m_Parse.GetFloatValue(m_strVTokens[3], fVXSDims(3));
            m_Parse.GetFloatValue(m_strVTokens[4], fVXSDims(4));
            if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f ||
                fVXSDims(3) <= 0.0f || fVXSDims(4) <= 0.0f)
                IOErrorHandler(ERRORCODE::XSDIMENSION);
            m_EPData(1) = new CTSection(fVXSDims);
        }
        else if (m_strVTokens[0] == "circular")
        {
            if (m_nTokens != 2)
                IOErrorHandler(ERRORCODE::INVALIDINPUT);
            m_Parse.GetFloatValue(m_strVTokens[1], fVXSDims(1));
            if (fVXSDims(1) <= 0.0f)
                IOErrorHandler(ERRORCODE::XSDIMENSION);
            m_EPData(1) = new CCircSolid(fVXSDims);
        }
        else
            IOErrorHandler(ERRORCODE::XSTYPE);

        // Read XS Reinforcements data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        /*
        if (m_strVTokens[0] != "*XS REINFORCEMENTS")
            IOErrorHandler(ERRORCODE::XSREINFO);
            */

        float fBarDia, fX, fY;
        CVector<float> fVReData(3*m_nXSR);
        
        for (int i = 1; i <= m_nXSR; i++) {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
            m_Parse.GetFloatValue(m_strVTokens[0], fBarDia);
            m_Parse.GetFloatValue(m_strVTokens[1], fX);
            m_Parse.GetFloatValue(m_strVTokens[2], fY);
            fVReData(3 * i - 2) = fBarDia;
            fVReData(3 * i - 1) = fX;
            fVReData(3 * i) = fY;
        }
        m_ReData(1).SetSize(3*m_nXSR);
        m_ReData(1).SetXSReData(fVReData);

        // Read Transverse Reinforcements data

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        for (int i = 1; i <= m_nTR; i++) {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            m_nVTransSegments(i) = (m_nTokens - 1) / 2 - 1;
            CVector<float> fVTransReData(m_nTokens);
            fVTransReData.Set(0.0f);

            m_Parse.GetFloatValue(m_strVTokens[0], fBarDia);
            fVTransReData(1) = fBarDia;

            for (int j = 1; j <= m_nVTransSegments(i); j++) {
                m_Parse.GetFloatValue(m_strVTokens[2*j-1], fVTransReData(2*j));
                m_Parse.GetFloatValue(m_strVTokens[2*j], fVTransReData(2*j+1));
            }
            m_TransReData(i).SetSize(m_nTokens);
            m_TransReData(i).SetXSReData(fVTransReData);
        }

        // read transverse spacings (NOT COMPLETE)
        if (m_strVTokens[0] != "*max" && m_strVTokens[1] != "values") {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
        }

        // read max values
        if (m_strVTokens[0] != "*max" && m_strVTokens[1] != "values") {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
        }

        float fMaxAxial, fAxialLoc, fMaxShear, fShearLoc, fMaxMoment, fMomentLoc;
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        m_Parse.GetFloatValue(m_strVTokens[0], fMaxAxial);
        m_Parse.GetFloatValue(m_strVTokens[1], fAxialLoc);
        m_Parse.GetFloatValue(m_strVTokens[2], fMaxShear);
        m_Parse.GetFloatValue(m_strVTokens[3], fShearLoc);
        m_Parse.GetFloatValue(m_strVTokens[4], fMaxMoment);
        m_Parse.GetFloatValue(m_strVTokens[5], fMomentLoc);

        m_ElementData(1).SetMaxValues(fMaxAxial, fAxialLoc, fMaxShear, fShearLoc, fMaxMoment, fMomentLoc);

    }
    // trap all input file errors here
    catch (CLocalErrorHandler::ERRORCODE &err)
    {
        m_LEH.ErrorHandler (err);
    }

    catch (CGlobalErrorHandler::ERRORCODE &err)
    {
        ErrorHandler (CLocalErrorHandler::ERRORCODE::INVALIDINPUT);
    }

    catch (...)
    {
        std::cout << "Sorry, could not catch the error whatever it is.\n";
    }
}

void CElement::CreateOutput ()
// ---------------------------------------------------------------------------
// Function: creates the output file
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // print analysis date and time
    m_FileOutput << '\n';
    m_FileOutput << "Starting out at : " << m_strDateTime << '\n';

    // problem description
    m_FileOutput << "-------------------" << '\n';
    m_FileOutput << "MATERIAL PROPERTIES" << '\n';
    m_FileOutput << "-------------------" << '\n';
    m_FileOutput << "Group    Young's Modulus                CTE" << '\n';
    m_FileOutput << "-----    ---------------    ---------------" << '\n';
    
    m_FileOutput << '\n';
    m_FileOutput << "=================" << '\n';
    m_FileOutput << "== FEA RESULTS ==" << '\n';
    m_FileOutput << "=================" << '\n';

    // Print elapsed clock time
    m_FileOutput << '\n';
    m_FileOutput << "      Ending at : " << m_strEndDateTime;
    m_FileOutput << "      Elapsed clock time : " << Timer.DiffTime() << " s\n";
}

void CElement::IOErrorHandler (ERRORCODE ECode) const
// ---------------------------------------------------------------------------
// Function: displays error messages related to input data
// Input:    error code
// Output:   none
// ---------------------------------------------------------------------------
{
    std::cerr << '\n';

    if (ECode == ERRORCODE::NUMNODES) // invalid number of nodes
        std::cerr << "Number of nodes must be >= 2.";
    else if (ECode == ERRORCODE::NUMELEMENTS) // invalid number of elements
        std::cerr << "Number of elements must be >= 1.";
    else if (ECode == ERRORCODE::DEBUGCODE) // invalid debug level
        std::cerr << "Debug level must be 0 or 1.";
    else if (ECode == ERRORCODE::NODENUMBER) // invalid node number
        std::cerr << "Invalid node number.";
    else if (ECode == ERRORCODE::ELEMENTNUMBER) // invalid element number
        std::cerr << "Invalid element number.";
    else if (ECode == ERRORCODE::XSAREA) // invalid x/s area
        std::cerr << "Area must be positive.";
    else if (ECode == ERRORCODE::YOUNGSMODULUS) // invalid E
        std::cerr << "Modulus of elasticity must be positive.";
    else if (ECode == ERRORCODE::INVALIDINPUT) // invalid input
        std::cerr << "Invalid input.";
    else if (ECode == ERRORCODE::INVALIDLASTLINE) // invalid input
        std::cerr << "Input file needs *end as last line.";
    else if (ECode == ERRORCODE::NODALFIXITY) // invalid fixity code
        std::cerr << "Nodal fixity code must be 'free' or 'specified'.";
    else if (ECode == ERRORCODE::NUMMATGROUPS) // invalid number of material groups
        std::cerr << "Number of material groups must be >= 1.";
    else if (ECode == ERRORCODE::NUMEPROPGROUPS) // invalid number of property groups
        std::cerr << "Number of element property groups must be >= 1.";
    else if (ECode == ERRORCODE::EPROPNUMBER) // invalid element property group
        std::cerr << "Invalid element property group number.";
    else if (ECode == ERRORCODE::XSDIMENSION) // invalid x/s dimension
        std::cerr << "Invalid cross-section dimension.";
    else if (ECode == ERRORCODE::MATGROUPNUMBER) // invalid material group
        std::cerr << "Invalid material property group number.";
    else if (ECode == ERRORCODE::MATPROPERTY) // invalid material property
        std::cerr << "Invalid material property.";
    else if (ECode == ERRORCODE::ELOADTYPE) // invalid element load type
        std::cerr << "Invalid element load type.";
    else if (ECode == ERRORCODE::XSTYPE) // invalid x/s type
        std::cerr << "Invalid cross-section type.";
    else
        std::cerr << "Unknown error ...?";

    std::cerr << '\n' << "Error in input file line : " << m_nLineNumber;
    std::cerr << std::endl;

    exit (1);
}

void CElement::TerminateProgram ()
// ---------------------------------------------------------------------------
// Function: terminates the program steps by closing the input/output files
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // close the input and output files
    m_FileInput.close ();
    m_FileOutput.close ();

    std::cout << "\nExecution completed successfully." << std::endl;
}
