//======================================================================
/*!
* \file
* \section sdhlibrary_cpp_dbg_h_general General file information
* \author   Dirk Osswald
* \date     2007-02-22
*
*
* \brief This file contains interface and implementation of class
*        #SDH::cDBG, a class for colorfull debug messages.
*
*
* \section sdhlibrary_cpp_dbg_h_copyright Copyright
*
* Copyright (c) 2007 SCHUNK GmbH & Co. KG
*
* <HR>
* \internal
*
* \subsection sdhlibrary_cpp_dbg_h_details SVN related, detailed file specific information:
* $LastChangedBy: Osswald2 $
* $LastChangedDate: 2008-10-16 18:59:36 +0200 (Do, 16 Okt 2008) $
* \par SVN file revision:
* $Id: dbg.h 3722 2008-10-16 16:59:36Z Osswald2 $
*
* \subsection sdhlibrary_cpp_dbg_h_changelog Changelog of this file:
* \include dbg.h.log
*/
//======================================================================

#ifndef DBG_h_
#define DBG_h_

#include <cob_sdh/sdhlibrary_settings.h>

#if SDH_USE_VCC
# pragma warning(disable : 4996)
#endif

//----------------------------------------------------------------------
// System Includes - include with <>
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdarg.h>

//----------------------------------------------------------------------
// Project Includes - include with ""
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Defines, enums, unions, structs
//----------------------------------------------------------------------

NAMESPACE_SDH_START


//----------------------------------------------------------------------
// Global variables (declarations)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// External functions and classes (declarations)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Function prototypes (function declarations)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------


/*!
* \brief A class to print colored debug messages.
*
* - The printing can be switched on or off so the debug code can remain in the
* code. (default is off)
* - The messages can be colorized (default is red).
* - The output can be redirected. (default is sys.stderr)
* - Debug messages can be printed in a functional way or in C++ stream like way
*
* If the environment variable "SDH_NO_COLOR" is defined then the messages are
* printed without coloring (usefull for logging or if your terminal does not
* support colors.
*
* Example:
* \code
*   #include "sdh/dbg.h"
*   d = cDBG( true );
*   g = cDBG( true, "green" );
*
*   d.PDM( "This message is printed in default color red" );
*   g << "and this one in a nice green ";
*
*   g << "of course you can debug print any objects that have a string representation: " << 08 << 15 << true;
*
*   g << "Messages can be turned of and on, e.g. selected by command line options";
*   g.SetFlag(false);
*   g << "This messages is not printed";
* \endcode
*/
class cDBG
{
protected:
    char*         debug_color;
    char*         normal_color;
    std::ostream *output;
    bool          debug_flag;
public:

    /*!
    * constructor: construct a cDBG object
    *
    * \param flag - the initial state of the flag, if true then messages sent
    *               to the object are printed. Default is false. Can be changed with SetFlag()
    * \param color - the name of the color to use, default is "red". Can be changed with SetColor()
    * \param fd    - the ostream to use for output, default is stderr. Can be changed with SetOutput()
    */
    cDBG( bool flag=false, char* color="red", std::ostream *fd=&std::cerr )
    {
        debug_flag     = flag;
        SetColor( color );
        output         = fd;
    }
    //---------------------

    ~cDBG()
    {
        output->flush();
    }

    /*!
    * Set debug_flag of this cDBG object to flag. After setting
    * the flag to true debug messages are printed, else not.
    */
    void SetFlag( bool flag )
    {
        debug_flag  = flag;
    }


    /*!
    * Get debug_flag of this cDBG object.
    */
    bool GetFlag( void ) const
    {
        return debug_flag;
    }

    /*!
    * Set debug_color of this cDBG object to color.
    * color is a string like "red", see util.py for valid names.
    *
    * \attention
    * The string is \b NOT copied, just a pointer is stored
    */
    void SetColor( char* color )
    {
        debug_color = GetColor( color );
        if ( !strcmp( debug_color, "" ) )
            // no debug color hence no normal color needed
            normal_color = debug_color;
        else
            // the code to set color back to normal
            normal_color = GetColor( "normal" );
    }
    //---------------------

    /*!
    * Set output of this cDBG object to fd, which must be a file like object like sys.stderr
    */
    void SetOutput( std::ostream *fd )
    {
        output = fd;
    }
    //---------------------

    /*!
    * Print debug messages of printf like fmt, ... in the color set
    * with SetColor, but only if debug_flag is true.
    */
    void PDM( char const* fmt, ... ) SDH__attribute__ ((format (printf, 2, 3)))
    /*
    * Remark:
    *   Since non-static C++ methods have an implicit `this' argument,
    *   the arguments of such methods should be counted from two, not
    *   one, when giving values for STRING-INDEX and FIRST-TO-CHECK
    *   parameter of the format __attribute__.)
    */
    {
        if (!debug_flag) return;

        char buffer[ 256 ];

        va_list arglist;
        va_start( arglist, fmt );
#if SDH_USE_VCC
        vsnprintf_s( buffer, 256, 256, fmt, arglist );
#else
        vsnprintf( buffer, 256, fmt, arglist );
#endif
        va_end( arglist );

        *output << debug_color << buffer << normal_color << std::flush;
    }
    //---------------------

    /*!
    * return a string (terminal escape sequence) that when printed sets the
    * color to \c c, where \c c must be in:
    * - "normal", "bold", "red", "green", "yellow", "blue", "magenta", "cyan", "white", "black" for normal color or
    * - "black_back", "red_back", "green_back", "yellow_back", "blue_back", "cyan_back", "magenta_back", "white_back" for reverse color
    *
    * If the environment variable "SDH_NO_COLOR" is set then "" is returned.
    * If the color is not found in the list of known colors then the string "" is returned.
    */
    char* GetColor( char* c )
    {
        char* sdh_no_color = getenv( "SDH_NO_COLOR" );
        if ( sdh_no_color != NULL )
            return "";

        if ( !strcmp( c, "normal" ) )       return "\x1b[0m";
        if ( !strcmp( c, "bold" ) )         return "\x1b[1m";
        if ( !strcmp( c, "red" ) )          return "\x1b[31m";
        if ( !strcmp( c, "green" ) )        return "\x1b[32m";
        if ( !strcmp( c, "yellow" ) )       return "\x1b[33m";
        if ( !strcmp( c, "blue" ) )         return "\x1b[34m";
        if ( !strcmp( c, "magenta" ) )      return "\x1b[35m";
        if ( !strcmp( c, "cyan" ) )         return "\x1b[36m";
        if ( !strcmp( c, "white" ) )        return "\x1b[37m";
        if ( !strcmp( c, "black" ) )        return "\x1b[39m";
        if ( !strcmp( c, "black_back" ) )   return "\x1b[40m";
        if ( !strcmp( c, "red_back" ) )     return "\x1b[41m";
        if ( !strcmp( c, "green_back" ) )   return "\x1b[42m";
        if ( !strcmp( c, "yellow_back" ) )  return "\x1b[43m";
        if ( !strcmp( c, "blue_back" ) )    return "\x1b[44m";
        if ( !strcmp( c, "cyan_back" ) )    return "\x1b[45m";
        if ( !strcmp( c, "magenta_back" ) ) return "\x1b[46m";
        if ( !strcmp( c, "white_back" ) )   return "\x1b[47m";

        // no coloring possible or unknown color: return ""
        return "";
    }

    /*!
    * C++ stream like printing:
    * \code
    *   d = cDBG( true );
    *   d << "bla" << "blu " << 42 << true << 0815;
    * \endcode
    */
    template <typename T>
    cDBG& operator<<( T v )
    {
        if (!debug_flag) return *this;

        *output << debug_color
        << v
        << normal_color << std::flush;
        return *this;
    }
    //---------------------

    /*!
    * Print name and value of variable \a _var in output stream \a _d. This will
    * print a "NAME='VALUE'" pair for variable _var
    *
    * \code
    *   d = cDBG( true );
    *   v = 42;
    *   s = "test";
    *   VAR( d, v );
    *   VAR( d, s );
    * \endcode
    * Will print "v='42'" and "s='test'" on stderr
    */
# define VAR( _d, _var )                        \
    (_d) << #_var << "='" << _var << "'\n"

    /*!
    * Insert name and value of the variable \a _var. This will
    * insert a "NAME='VALUE' " in an output stream
    *
    * \code
    *   d = cDBG( true );
    *   v = 42;
    *   s = "test";
    *   d << V( v ) << "but " << V(s);
    * \endcode
    * Will print "v=42" and "s=test" on stderr
    */
# define V( _var )                      \
    #_var << "='" << _var << "' "
};

NAMESPACE_SDH_END

#endif


//======================================================================
/*
  Here are some settings for the emacs/xemacs editor (and can be safely ignored):
  (e.g. to explicitely set C++ mode for *.h header files)

  Local Variables:
  mode:C++
  mode:ELSE
  End:
 */
//======================================================================
