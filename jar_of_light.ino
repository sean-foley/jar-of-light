/*======================================================================
FILE:
jar_of_light.ino

CREATOR:
Sean Foley

GENERAL DESCRIPTION:
This file drives the Jar-of-Light project. It is an arduino style
file with setup() containing initalization code, and loop() driving
the overall program execution.

INITIALIZATION AND SEQUENCING REQUIREMENTS:
None.

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

#define GPIO_PIXEL_DATA_PIN 15
#define NEOPIXEL_COUNT      7

#define PIN_ACTIVITY_LED    0
#define PIN_NETWORK_LED     2

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------

// Pointer stupport
#include <memory>

#include <ESP8266WiFi.h>

#include <WString.h>

// Animation for the neopixels
#include "ledanimator.h"

// For the board-level leds
#include "ledhelper.h"

// Other feature support
#include "webserverproxy.h"
#include "firmwareupdater.h"
#include "timeproxy.h"
#include "discoveryproxy.h"

//----------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------

// None.

//----------------------------------------------------------------------
// Global Constant Definitions
//----------------------------------------------------------------------

const char *WLAN_SSID = "PUT SSID HERE";
const char *WLAN_PASS = "PUT PASSWORD HERE";

const String PROJECT_NAME = "jar-of-light";

const int STATE_INITIALIZING          = 0;
const int STATE_INITIALIZED           = 1;
const int STATE_CHECK_STORED_CONFIG   = 2;
const int STATE_WIFI_AP_CONFIG_MODE   = 3;
const int STATE_WIFI_STA_DISCONNECTED = 4;
const int STATE_WIFI_STA_CONNECTED    = 5;
const int STATE_READY                 = 6;

// This will make it easier to pass around colors.
// Values are grbw respectively.
const uint32_t COLOR_GREEN  = LedAnimator::Color( 255, 0, 0, 0 );
const uint32_t COLOR_YELLOW = LedAnimator::Color( 140, 255, 0, 0 );
const uint32_t COLOR_RED    = LedAnimator::Color( 0, 255, 0, 0 );
const uint32_t COLOR_BLUE   = LedAnimator::Color( 0, 0, 255, 0 );
const uint32_t COLOR_WHITE  = LedAnimator::Color( 0, 0, 0, 255 );
const uint32_t COLOR_BLACK  = LedAnimator::Color( 0, 0, 0, 0 );
const uint32_t COLOR_ALL    = LedAnimator::Color( 255, 255, 255, 255 );

//----------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------

LedHelper activityLed( PIN_ACTIVITY_LED );
LedHelper networkLed( PIN_NETWORK_LED );

// This object is shared (aggregated) between
// this code in our main loop and the WebserverProxy
std::shared_ptr<LedAnimator> ledAnimator;

std::unique_ptr<WebserverProxy> webserverProxy;
std::unique_ptr<FirmwareUpdater> firmwareUpdater;
std::unique_ptr<TimeProxy> timeProxy;
std::unique_ptr<DiscoveryProxy> discoveryProxy;

volatile int activeState = STATE_INITIALIZING;

//----------------------------------------------------------------------
// Static Variable Definitions 
//----------------------------------------------------------------------

// None.

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
onWiFiEvent()

DESCRIPTION:
Callback handler for various wifi events

RETURN VALUE:
none.

SIDE EFFECTS:
Changes the value of the activeState varaible

======================================================================*/
static void onWiFiEvent( WiFiEvent_t event )
{
    Serial.printf( "**Wifi event %d\n", event );

    switch ( event )
    {
        case WIFI_EVENT_STAMODE_CONNECTED:

            break;
           
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            
            activeState = STATE_WIFI_STA_DISCONNECTED;
            break;

        
        case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:

            break;
            
        case WIFI_EVENT_STAMODE_GOT_IP:
            // When we have this event, we should be fully 
            // good-to-go on the network.
            break;

        case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
        case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
        case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
        case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
        case WIFI_EVENT_MAX:
        case WIFI_EVENT_MODE_CHANGE:
            break;
    }
}

/*======================================================================
FUNCTION:
connectWifi()

DESCRIPTION:
Attempts to connect to the wireless AP

RETURN VALUE:
True if connected

SIDE EFFECTS:
none.

======================================================================*/
bool connectWifi()
{
    bool connected = false;

    // Set our callback handler
    WiFi.onEvent( onWiFiEvent, WIFI_EVENT_ANY );

    WiFi.begin( WLAN_SSID, WLAN_PASS );

    WiFi.persistent( false );
    WiFi.mode( WIFI_OFF );
    WiFi.mode( WIFI_STA );

    WiFi.begin( WLAN_SSID, WLAN_PASS );

    while ( WiFi.status() != WL_CONNECTED )
    {
        // TODO... break out of  loop after some N amount of time
        delay( 500 );
        Serial.print( "." );
    }

    connected = true;

    Serial.printf( "\nConnected to SSID %s, ip address %s\n",
                   WLAN_SSID,
                   WiFi.localIP().toString().c_str() );

    return connected;
}

/*======================================================================
FUNCTION:
setup()

DESCRIPTION:
Put your one-time initialization/setup code here

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void setup()
{
    if ( ledAnimator == false )
    {
        ledAnimator.reset(
            new LedAnimator( GPIO_PIXEL_DATA_PIN, NEOPIXEL_COUNT )
        );
    }

    ledAnimator->TurnAllOff();
    
    // Start animating
    ledAnimator->Demo();
}

/*======================================================================
FUNCTION:
loop()

DESCRIPTION:
Arduino main entry point for a sketch

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void loop()
{
    activityLed.Flash(50);
    
    // No matter what state we're in, we always animate
    ledAnimator->Process();

    switch ( activeState )
    {
        case STATE_INITIALIZING:

            activeState = STATE_INITIALIZED;
            break;

        case STATE_INITIALIZED:

            activeState = STATE_CHECK_STORED_CONFIG;
            break;

        case STATE_CHECK_STORED_CONFIG:

            activeState = STATE_WIFI_STA_DISCONNECTED;

        case STATE_WIFI_AP_CONFIG_MODE:
        
            activeState = STATE_WIFI_STA_DISCONNECTED;
            break;

        case STATE_WIFI_STA_DISCONNECTED:
        {
            networkLed.TurnOff();
            bool connected = connectWifi();
            if ( connected == true )
            {
                Serial.printf( "Setting state to WIFI STA connected\n" );
                activeState = STATE_WIFI_STA_CONNECTED;
            }
            else
            {
                Serial.printf( "connectWifi() returned false\n" );
            }
        }

        break;

        case STATE_WIFI_STA_CONNECTED:

            networkLed.TurnOn();
            if ( firmwareUpdater == false )
            {
                Serial.println( "Starting firmware OTA support" );

                firmwareUpdater.reset( new FirmwareUpdater(
                    PROJECT_NAME,
                    "") );

                firmwareUpdater->Begin();
            }

            // Do we have a webserver yet?
            if ( webserverProxy == false )
            {
                Serial.println( "Starting webserver" );
                // Allocate and start up
                webserverProxy.reset( new WebserverProxy(ledAnimator) );

                webserverProxy->Begin();
            }

            if ( timeProxy == false )
            {
                timeProxy.reset( new TimeProxy( "pool.ntp.org" ) );

                timeProxy->Begin();
            }

            if ( discoveryProxy == false )
            {
                discoveryProxy.reset( new DiscoveryProxy( PROJECT_NAME ) );
                discoveryProxy->Begin();
                
                // Add our web server
                discoveryProxy->AddService( "http", "tcp", 80 );
            }
            
            activeState = STATE_READY;
            break;

        case STATE_READY:

            // Call of the handlers so they can do their thing
            webserverProxy->Process();
            firmwareUpdater->Process();

            break;
    }

    yield();
}

/*=====================================================================
// IMPLEMENTATION NOTES
//=====================================================================

None

=====================================================================*/
