/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2025

TODO:
- Ensure all is deallocated
- Moment capacity, rect.solid
**********************************************/

#include "element.h"
#include "clockEXH.h"

int main (int argc, char *argv[])
{
    // CArrayBase class is used to track memory allocation, deallocation
    // and route the error messages from the CVector and CMatrix classes
    CArrayBase AB;
    {
        CElement TheElement; // the one and only element!
        try
        {
	        // show program banner
            TheElement.Banner (std::cout);

	        // Prepare for I/O
	        TheElement.PrepareIO (argc, argv);
	
            // start the timer --------------------------------------------------------
            CClock Timer;
            std::string strDateTime;
            Timer.GetDateTime (strDateTime);
            std::cout << "\nStarting out at : " << strDateTime << "\n";
            std::cout << "\nHello!";

            // read the data and analyze
            TheElement.Analyze ();

            // end the timer --------------------------------------------------------
            // get the current date and time
            Timer.GetDateTime (strDateTime);
            std::cout << "\n      Ending at : " << strDateTime << "\n";
            // compute the elapsed time -----------------------------------------------
            std::cout << "Elapsed wall clock time: " << Timer.DiffTime ()
                      << " seconds\n";
        }
        // --------------------
        // trap all errors here
        // --------------------
        // errors trapped by this program
        catch (CLocalErrorHandler::ERRORCODE &err)
        {
            TheElement.DisplayErrorMessage (err);
        }

        // errors trapped by the library functions
        catch (CGlobalErrorHandler::ERRORCODE &err)
        {
            CGlobalErrorHandler::ErrorHandler (err);
        }

        // errors trapped in the CVector/CMatrix classes
        catch (CArrayBase::ERRORVM &err)
        {
            CGlobalErrorHandler::ErrorHandler (err);
        }

        // errors trapped by C++ 
        catch (std::exception &err)
        {
            CGlobalErrorHandler::ErrorHandler (err);
        }

        // forgotten to handle a trapped error?
        catch (...)
        {
            std::cout << "Sorry, could not catch the error whatever it is.\n";
        }

        // Close input and output files
        TheElement.TerminateProgram ();
    }

    AB.ShowStatistics (std::cout);

	return 0;
}