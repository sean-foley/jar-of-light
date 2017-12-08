/*======================================================================
FILE:
webserverproxy.h

CREATOR:
Sean Foley

DESCRIPTION:
Proxy class to hide the details of using a web server on your device.

PUBLIC CLASSES AND FUNCTIONS:
WebserverProxy

INITIALIZATION AND SEQUENCING REQUIREMENTS:
The device must be on the network and have a valid 
ip address assigned before using this class/object.

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
// INCLUDES AND VARIABLE DEFINITIONS
//======================================================================

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------

#include "webserverproxy.h"

// std::bind support
#include <functional>

//----------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Global Constant Definitions
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Static Variable Definitions 
//----------------------------------------------------------------------

// None

//----------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Required Libraries
//----------------------------------------------------------------------

// None. (Where supported these should be in the form
// of C++ pragmas).

//======================================================================
// FUNCTION IMPLEMENTATIONS
//======================================================================

/*======================================================================
FUNCTION:
WebserverProxy()

DESCRIPTION:
C-tor

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
WebserverProxy::WebserverProxy( std::shared_ptr<LedAnimator> obj, int port )
    : _server( port )
{
    _ledAnimator = obj;
    init();
}

/*======================================================================
FUNCTION:
Begin()

DESCRIPTION:
Starts the webserver so it listens for requests

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::Begin()
{
    _server.begin();
}

/*======================================================================
FUNCTION:
Process()

DESCRIPTION:
Let's the web server do its thing. If you don't call this periodically
(like in a tight loop) the web server will be unresponsive to clients.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::Process()
{
    // Pump the server so it can do things
    _server.handleClient();

    // Just in case the caller is calling this in a tight loop
    yield();
}

/*======================================================================
FUNCTION:
init()

DESCRIPTION:
Sets up all the callback handlers for the webserver, including the
REST-like API endpoints

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::init()
{
    _server.on( "/", std::bind( &WebserverProxy::handleRoot, this ) );
    _server.on( "/", std::bind( &WebserverProxy::handleNotFound, this ) );

    _server.on( "/led/command/on", std::bind( &WebserverProxy::handleAllOn, this ));
    _server.on( "/led/command/off", std::bind( &WebserverProxy::handleAllOff, this ) );
    _server.on( "/led/command/wheel", std::bind( &WebserverProxy::handleColorWheel, this ) );
    _server.on( "/led/command/pulse", std::bind( &WebserverProxy::handlePulse, this ) );
    _server.on( "/led/command/strobe", std::bind( &WebserverProxy::handleStrobe, this ));
    _server.on( "/led/command/flicker", std::bind( &WebserverProxy::handleFlicker, this ) );
    _server.on( "/led/command/demo", std::bind( &WebserverProxy::handleDemo, this ) );
}

/*======================================================================
FUNCTION:
handleAlOn()

DESCRIPTION:
Callback handler that will then use the led animator to turn all the
neopixels on.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleAllOn()
{
    _ledAnimator->TurnAllOn();

    String message = "on";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleAllOff()

DESCRIPTION:
Callback handler that will turn all of the leds off

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleAllOff()
{
    _ledAnimator->TurnAllOff();

    String message = "off";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleColorWheel()

DESCRIPTION:
Callback handler that will cause the leds to cycle thru a color wheel
of animation

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleColorWheel()
{
    _ledAnimator->CycleThruColorWheel();

    String message = "wheel";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handlePulse()

DESCRIPTION:
Callback handler that will make the leds pulse

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handlePulse()
{
    _ledAnimator->AllPulse();

    String message = "pulse";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleStrobe()

DESCRIPTION:
Callback handler that will cause the leds to strobe

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleStrobe()
{
    _ledAnimator->AllStrobe();

    String message = "strobe";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleFlicker()

DESCRIPTION:
Callback handler that will cause the leds to do a flicker animation

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleFlicker()
{
    _ledAnimator->Flicker();

    String message = "flicker";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleDemo()

DESCRIPTION:
Callback handler that puts the leds into demo animation mode

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleDemo()
{
    _ledAnimator->Demo();

    String message = "demo";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleNotFound()

DESCRIPTION:
This is the 404 not found handler for any URI that we don't know 
what to do with.  Source code from the web server example.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server.uri();
    message += "\nMethod: ";
    message += ( _server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _server.args();
    message += "\n";

    for ( uint8_t i = 0; i < _server.args(); i++ )
    {
        message += " " + _server.argName( i ) + ": " + _server.arg( i ) + "\n";
    }

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 404, "text/plain", message );
}

/*======================================================================
FUNCTION:
handleRoot()

DESCRIPTION:
This handler handles root ("/") requests.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::handleRoot()
{
    String message = "Hi there from the jar-o-light!";

    setNoCacheHeaders();
    _server.sendHeader( "Content-Length", String( message.length() ) );
    _server.send( 200, "text/plain", message );
}


/*======================================================================
FUNCTION:
setNoCacheHeaders()

DESCRIPTION:
Helper to set HTTP headers to direct the client to not cache
the response.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void WebserverProxy::setNoCacheHeaders()
{
    _server.sendHeader( "Cache-Control", "no-cache, no-store, must-revalidate" );
    _server.sendHeader( "Pragma", "no-cache" );
    _server.sendHeader( "Expires", "-1" );
}

/*=====================================================================
// IMPLEMENTATION NOTES
//=====================================================================

None

=====================================================================*/


