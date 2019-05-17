#ifndef ERROR_H
#define ERROR_H

#include <QString>

namespace VTFStudio
{
    namespace Error
    {
        //----------------------------------------------------------------------------------------------------
        /// Displays a QMessageBox with a generic error message with a code to report.
        /// @param[in] cause             Some text that identifies where in execution the error occured that can be reported.
        //----------------------------------------------------------------------------------------------------
        void Display(QString cause);
    }
}

#endif // ERROR_H
