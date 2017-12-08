#ifndef _JAROFLIGHT_LEDANIMATOR_H_
#define _JAROFLIGHT_LEDANIMATOR_H_

/*======================================================================
FILE:
ledanimator.h

CREATOR:
Sean Foley

DESCRIPTION:
Provides various animations for neopixel LEDs.

PUBLIC CLASSES AND FUNCTIONS:
LedAnimator

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

#include <stdint.h>

#include <memory>

#include <Adafruit_NeoPixel.h>

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
LedAnimator

DESCRIPTION:
Provides animation routines for neopixels

HOW TO USE:
1. Construct the object
2. Call an explicit animation method
3. Call Process() which will use the animation method in #2 above and 
continue that animation

======================================================================*/
class LedAnimator
{
    public:

    //=================================================================
    // TYPE DECLARATIONS AND CONSTANTS    
    //=================================================================

    enum AnimationState 
    { 
        STATE_UNKNOWN = 0,
        STATE_OFF, 
        STATE_ON, 
        STATE_COLOR_WHEEL,
        STATE_PULSE, 
        STATE_STROBE,
        STATE_FLICKER,
        STATE_DEMO
    };

    //=================================================================
    // CLIENT INTERFACE
    //=================================================================

    LedAnimator( uint8_t gpioDataPin, uint32_t pixelCount );

    void SetPixelBrightness( uint8_t brightness );
    void SetColor( uint32_t color ) { _color = color; }

    void TurnAllOff();

    void TurnAllOn( uint32_t color );
    void TurnAllOn() { TurnAllOn( _color ); }

    void AllPulse( uint32_t color );
    void AllPulse() { AllPulse( _color ); }

    void AllStrobe( uint32_t color );
    void AllStrobe() { AllStrobe( _color ); }

    void Flicker( uint32_t color );
    void Flicker() { Flicker( _color ); }

    void CycleThruColorWheel();

    // This is the Wheel algorithm by Bill Earl
    //  https://learn.adafruit.com/multi-tasking-the-arduino-part-3?view=all
    uint32_t Wheel( byte WheelPos );

    void Demo();

    void Process();

    static uint32_t Color( uint8_t r, uint8_t g, uint8_t b );
    static uint32_t Color( uint8_t r, uint8_t g, uint8_t b, uint8_t w );

    protected:

    //=================================================================
    // SUBCLASS INTERFACE   
    //=================================================================

    // None.

    private:

    //=================================================================
    // CUSTOMIZATION INTERFACE    
    //=================================================================

    // None.

    //=================================================================
    // IMPLEMENTATION INTERFACE    
    //=================================================================

    // No copying - intentionally not defining the implementation
    // to throw a link error if someone somehow somewhat invokes a copy.
    LedAnimator( const LedAnimator &rhs );

    void init();

    //=================================================================
    // DATA MEMBERS    
    //=================================================================

    uint8_t _gpioDataPin;

    uint32_t _pixelCount;

    uint32_t _color;

    uint8_t _brightness = 255;

    byte _wheeloffset = 0;

    std::unique_ptr< Adafruit_NeoPixel> _pixels;

    AnimationState _lastAnimationState;

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

#endif	// #ifendif _JAROFLIGHT_LEDANIMATOR_H_
