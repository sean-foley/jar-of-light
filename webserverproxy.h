#ifndef _JAROFLIGHT_WEBSERVERPROXY_H_
#define _JAROFLIGHT_WEBSERVERPROXY_H_

/*======================================================================
FILE:
webserverproxy.h

CREATOR:
Sean Foley

DESCRIPTION:
Proxy class to hide the details of using a web server on your device.

PUBLIC CLASSES AND FUNCTIONS:
WebserverProxy

Copyright (C) 2017 Sean Foley  All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted.  Enjoy.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

======================================================================*/

//======================================================================
// INCLUDES AND PUBLIC DATA DECLARATIONS
//======================================================================

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------

#include <ESP8266WebServer.h>

#include "ledanimator.h"

//----------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Global Constant Declarations
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Global Data Declarations
//----------------------------------------------------------------------

// None.

//======================================================================
// WARNINGS!!!
//======================================================================

// None.

//======================================================================
// FUNCTION DECLARATIONS
//======================================================================

// None.

//=====================================================================
// EXCEPTION CLASS DEFINITIONS
//=====================================================================

// None.

//======================================================================
// CLASS DEFINITIONS
//======================================================================

/*======================================================================
CLASS:
WebserverProxy

DESCRIPTION:
Proxy class to hide the complexity of using a webserver.  This class
sets up our REST-like endpoints.

HOW TO USE:
1. Construct with the required parameters
2. Call Begin() to start everything
3. Periodically call Process() to allow the web server handlers to
do their thing.

======================================================================*/
class WebserverProxy
{
    public:

    //=================================================================
    // TYPE DECLARATIONS AND CONSTANTS    
    //=================================================================

    // None.

    //=================================================================
    // CLIENT INTERFACE
    //=================================================================

    WebserverProxy( std::shared_ptr<LedAnimator> obj, int port = 80 );

    void Begin();

    void Process();

    protected:

    //=================================================================
    // SUBCLASS INTERFACE   
    //=================================================================

    //
    // all of our web callback handlers
    //
    void handleRoot();
    void handleNotFound();
    void handleAllOn();
    void handleAllOff();
    void handleColorWheel();
    void handlePulse();
    void handleStrobe();
    void handleFlicker();
    void handleDemo();

    // Sets the HTTP response headers to 
    // tell the client to not cache the response
    void setNoCacheHeaders();

    private:

    //=================================================================
    // CUSTOMIZATION INTERFACE    
    //=================================================================

    // None.

    //=================================================================
    // IMPLEMENTATION INTERFACE    
    //=================================================================

    // No copying - purposely note implemented to generate
    // a link error in case someone somehow invokes a copy
    WebserverProxy( const WebserverProxy &rhs );

    void init();

    //=================================================================
    // DATA MEMBERS    
    //=================================================================

    ESP8266WebServer _server;

    std::shared_ptr<LedAnimator> _ledAnimator;
};

//======================================================================
// INLINE FUNCTION DEFINITIONS
//======================================================================

// None.


/*======================================================================
// DOCUMENTATION
========================================================================

None.

======================================================================*/

#endif	// #ifendif _JAROFLIGHT_WEBSERVERPROXY_H_
