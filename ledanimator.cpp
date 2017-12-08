/*======================================================================
FILE:
ledanimator.cpp

CREATOR:
Sean Foley

GENERAL DESCRIPTION:
Provides various animations for neopixel LEDs.

PUBLIC CLASSES AND FUNCTIONS:
LedAnimator

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

// None.

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------

#include "ledanimator.h"

// Hack to deal with a collision between the Arduino.h definitions and
// what is in the std c++ definitions
// https://github.com/platformio/platformio-core/issues/646
#undef min
#undef max

#include <random>

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
C-tor()

DESCRIPTION:
Constructs the object with the GPIO data pin and # of pixels
we are going to animate.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
LedAnimator::LedAnimator( uint8_t gpioDataPin, uint32_t pixelCount )
    : _gpioDataPin( gpioDataPin ), _pixelCount( pixelCount )
{
    init();
}

/*======================================================================
FUNCTION:
init()

DESCRIPTION:
Do any object initialization here, that way if the object is extended
with more C-tors(), we can have a common initialization.

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void LedAnimator::init()
{
    _lastAnimationState = AnimationState::STATE_UNKNOWN;

    if ( _pixels == false )
    {
        _pixels.reset( 
            new Adafruit_NeoPixel( _pixelCount, _gpioDataPin, NEO_RGBW + NEO_KHZ800 )
        );
    }

    // Initalize
    _pixels->begin();
}

/*======================================================================
FUNCTION:
TurnAllOff()

DESCRIPTION:
Turns all of the pixels off

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void LedAnimator::TurnAllOff()
{
    _lastAnimationState = AnimationState::STATE_OFF;

    for ( int i = 0; i < _pixelCount; i++ )
    {
        _pixels->setPixelColor( i, _pixels->Color( 0, 0, 0, 0 ) );
    }

    _pixels->show();
}

/*======================================================================
FUNCTION:
TurnAllOn()

DESCRIPTION:
Turns all of the pixels on with the color provided

RETURN VALUE:
none.

SIDE EFFECTS:
none

======================================================================*/
void LedAnimator::TurnAllOn( uint32_t color )
{
    _lastAnimationState = AnimationState::STATE_ON;

    for ( int i = 0; i < _pixelCount; i++ )
    {
        _pixels->setPixelColor( i, color ); 
    }

    _pixels->setBrightness( _brightness );

    _pixels->show();
}

/*======================================================================
FUNCTION:
SetPixelBrightness()

DESCRIPTION:
Helper method to set the brightness.
Brightness can be 0..255, with 0 = off, 255 = high

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void inline LedAnimator::SetPixelBrightness( uint8_t brightness )
{
    // Setting the brightness causes "bit rot" (loss) which
    // can skew the colors over time. The library guidance is
    // to call this once.  However, since we are re-initializing
    // the pixel buffer, bit rot shouldn't be a concern.
    // You can also set the brightness by scaling the RGB/W values 
    // in the setPixelColor() call.  For example, a full red would be r=255, 
    // 50% would be r=127. So to control brightness, just scale the rbg 
    // values by the same amount.
    _brightness = brightness;
    _pixels->setBrightness( _brightness );
}

/*======================================================================
FUNCTION:
AllPulse()

DESCRIPTION:
This function will pulse all the pixels by the specified color.
It will flip/flop by ramping the brightness level up/down each
time it is called.  If we did a loop with a delay internal to this
method, it would delay the ability of the caller to responsive
to distance changes.

Just call it multiple times with the color you want to pulse.  You
need about 10-50ms between calls to allow for decent pulse effect.

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void LedAnimator::AllPulse( uint32_t color )
{
    _lastAnimationState = AnimationState::STATE_PULSE;

    // The min and max brightness levels
    const uint8_t MIN = 10;
    const uint8_t MAX = 255;

    // The larger the step, the more dramatic the changes
    // in brightness.  Smaller values give more subtle
    // changes.
    const uint8_t STEP = 10;

    // We need to use something bigger than a byte
    // We step (up/down) the brightness value, then
    // bounds check it.  Since we bounds check after-the-fact
    // it is possible to exceed a byte range (0..255).
    static uint16_t brightness = MAX;

    // Note that this is a signed 8 bit int
    // We need the sign because we are going to
    // flip flop from +1 to -1
    static int8_t countdirection = 1;

    const int8_t UP = 1;
    const int8_t DOWN = -1;

    // What we want is to ramp down from MAX to MIN, then
    // ramp up from MIN to MAX.  This will give a nice
    // pulsing effect.
    brightness = brightness + ( STEP * countdirection );

    // Bounds checking
    if ( brightness <= MIN )
    {
        // count up
        countdirection = UP;

        // bounds check to minimum
        brightness = MIN;
    }

    // Bounds checking the other extreme
    if ( brightness >= MAX )
    {
        // count down
        countdirection = DOWN;

        brightness = MAX;
    }

    SetPixelBrightness( brightness );

    for ( int i = 0; i < _pixelCount; i++ )
    {
        _pixels->setPixelColor( i, color );
    }

    // We've set the entire array (i.e. the buffer), so
    // now call show to display it (i.e. make it active)
    _pixels->show();
}

/*======================================================================
FUNCTION:
pixelsAllStrobe()

DESCRIPTION:
This function will strobe (alternate) all of the pixels between a minimum
brightness to a maximum brightness.  If we handled the delay internally
to this method, it would delay the ability of the caller to responsive
to distance changes.

Just call it multiple times with the color you want to strobe.  You
need about 10-50ms between calls to allow for decent strobe effect.

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void LedAnimator::AllStrobe( uint32_t color )
{
    _lastAnimationState = AnimationState::STATE_STROBE;

    // Min is 25% bright, max is 100%
    const uint8_t MIN = 64;
    const uint8_t MAX = 255;

    static uint8_t brightness = MIN;

    // Flip/flop between the extremes
    switch ( brightness )
    {
        case MIN:
            brightness = MAX;
            break;

        case MAX:
            brightness = MIN;
            break;

        default:
            brightness = MAX;
            break;
    }

    SetPixelBrightness( brightness );

    for ( int i = 0; i < _pixelCount; i++ )
    {
        _pixels->setPixelColor( i, color );
    }

    // We've set the entire array (i.e. the buffer), so
    // now call show to display it (i.e. make it active)
    _pixels->show();
}

/*======================================================================
FUNCTION:
Wheel()

DESCRIPTION:
Implements a color wheel and returns a color based on the wheel position.
This code is by Bill Earl from the Adafruit site.

https://learn.adafruit.com/multi-tasking-the-arduino-part-3?view=all

RETURN VALUE:
Color value of the wheel based on the wheel position

SIDE EFFECTS:
none

======================================================================*/
uint32_t LedAnimator::Wheel( byte WheelPos )
{
    _lastAnimationState = STATE_COLOR_WHEEL;

    WheelPos = 255 - WheelPos;
    if ( WheelPos < 85 )
    {
        return Color( 255 - WheelPos * 3, 0, WheelPos * 3 );
    }
    else if ( WheelPos < 170 )
    {
        WheelPos -= 85;
        return Color( 0, WheelPos * 3, 255 - WheelPos * 3 );
    }
    else
    {
        WheelPos -= 170;
        return Color( WheelPos * 3, 255 - WheelPos * 3, 0 );
    }
}

/*======================================================================
FUNCTION:
CycleThruColorWheel()

DESCRIPTION:
This is a helper function that will drive thru a color wheel.  Essentially
this is a wrapper that makes it easier than the caller having to do all 
the work.

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void LedAnimator::CycleThruColorWheel()
{
    TurnAllOn( Wheel( _wheeloffset++ ) );

    // The call to TurnAllOn will set the last state to all on
    // so we need to reset it here.
    _lastAnimationState = AnimationState::STATE_COLOR_WHEEL;
}

/*======================================================================
FUNCTION:
Flicker()

DESCRIPTION:
This method will randomly select 1 pixel from total pixels and turn it
on.  Any previous pixel will turned off.  This will give the effect
of random pixels turning on.

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void LedAnimator::Flicker( uint32_t color )
{
    static int offset = 0;

    // If we weren't doing flickering before, we 
    // need to reset everything to a known state
    if ( _lastAnimationState != STATE_FLICKER )
    {
        for ( int i = 0; i < _pixelCount; i++ )
        {
            // Turn all the pixels off
            _pixels->setPixelColor( i, 0, 0, 0, 0 );
        }
    }

    static std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution( 0, _pixelCount );

    int previous = offset;

    // get a new pixel to use
    offset = distribution( generator );

    // Now turn that one on
    _pixels->setPixelColor( offset, _color );
    _pixels->show();

    // At this point, we may have turned the same pixel on as the 
    // previous one (i.e. we randomly selected the same pixel)
    // so don't turn it off if that's the case. 
    if ( previous != offset )
    {
        _pixels->setPixelColor( previous, 0, 0, 0, 0 );
        _pixels->show();
    }

    _lastAnimationState = STATE_FLICKER;
}

/*======================================================================
FUNCTION:
Demo()

DESCRIPTION:
This method will cycle thru all of the various animation effects
that this class implements.  The way it works is it calculates
an elapsed time, and if that timeframe has expired, then it advances
to the next animation step.

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void LedAnimator::Demo()
{
    // We are going to do our own internal state machine
    // in this method. We will use this to cycle thru
    // the various animation features
    static int lastDemoState = STATE_OFF;

    static unsigned long start = 0;

    if ( start == 0 )
    {
        start = millis();
    }
    
    unsigned long stop = millis();
    unsigned long elapsed = stop - start;

    static unsigned long timeToShow = 1000;

    // How long have we been doing this particular state
    // demo?
    if ( elapsed >= timeToShow )
    {
        // Slide to the next time period
        start = stop;

        // We want to exclude the demo state, which MUST
        // be the last enumerated CONST in the list for
        // this code to work.  If you add more enumerations,
        // make sure to keep STATE_DEMO at the end of the list
        const int TOTAL_STATES = 8 - 1;

        // Advance the state and wrap if we exceed
        // the bounds
        lastDemoState = ( lastDemoState + 1 ) % TOTAL_STATES;
    }

    switch ( lastDemoState )
    {
        case STATE_UNKNOWN:
            TurnAllOff();
            lastDemoState = STATE_OFF;
            break;

        case STATE_OFF:
            TurnAllOff();
            timeToShow = 1000;
            break;

        case STATE_ON:
            timeToShow = 5000;
            // Let's make it green
            _color = Color( 0, 255, 0, 0 );

            TurnAllOn( _color );
            break;

        case STATE_COLOR_WHEEL:
            CycleThruColorWheel();
            break;

        case STATE_PULSE:
            _color = Color( 255, 0, 0, 0 );
            AllPulse( _color );
            break;

        case STATE_STROBE:
            _color = Color( 255, 255, 255, 255 );
            AllStrobe( _color );
            break;

        case STATE_FLICKER:
            _color = Color( 0, 0, 255, 0 );
            Flicker( _color );
            break;
    }

    // Make sure to keep us in the DEMO mode
    // so next time Process() is called, we know
    // where to pick up
    _lastAnimationState = STATE_DEMO;
}

/*======================================================================
FUNCTION:
Process()

DESCRIPTION:
This method will use the last known animation and call it.  This is
needed for the "active" animations, such as pulsing/strobing/flashing
etc.

RETURN VALUE:
none.

SIDE EFFECTS:
none.

======================================================================*/
void LedAnimator::Process()
{
    switch ( _lastAnimationState )
    {
        case STATE_COLOR_WHEEL:
            CycleThruColorWheel();
            break;

        case STATE_STROBE:
            AllStrobe( _color );
            break;

        case STATE_PULSE:
            AllPulse( _color );
            break;

        case STATE_FLICKER:
            Flicker( _color );
            break;

        case STATE_DEMO:
            Demo();
            break;

        // We don't need to do anything for the following 
        // states
        case STATE_UNKNOWN:
        case STATE_ON:
        case STATE_OFF:
            break;
    }
}
 
/*======================================================================
FUNCTION:
Color()

DESCRIPTION:
Helper method that will pack the r/g/b values into a uint32_t

RETURN VALUE:
packed color into a uint32_t

SIDE EFFECTS:
none.

======================================================================*/
uint32_t LedAnimator::Color( uint8_t r, uint8_t g, uint8_t b )
{
    //const uint32_t COLOR_WHITE = Adafruit_NeoPixel::Color( 0, 0, 0, 255 );
    return Adafruit_NeoPixel::Color( r, g, b );
}

/*======================================================================
FUNCTION:
Color()

DESCRIPTION:
Helper method that will pack the r/g/b/w values into a uint32_t

RETURN VALUE:
packed color into a uint32_t

SIDE EFFECTS:
none.

======================================================================*/
uint32_t LedAnimator::Color( uint8_t r, uint8_t g, uint8_t b, uint8_t w )
{
    return Adafruit_NeoPixel::Color( r, g, b, w );
}

/*=====================================================================
// IMPLEMENTATION NOTES
//=====================================================================

None

=====================================================================*/

