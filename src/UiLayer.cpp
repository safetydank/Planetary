/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "cinder/gl/gl.h"
#include "UiLayer.h"
#include "CinderFlurry.h"
#include "Globals.h"
#include "BloomGl.h"
#include "UIController.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::app;
using namespace std;

void UiLayer::setup( const gl::Texture &uiButtonsTex, const bool &showSettings, const Vec2f interfaceSize )
{
    mUiButtonsTex = uiButtonsTex;
    
	mPanelOpenHeight		= 63.0f;
	mPanelSettingsHeight	= 108.0f;    
    
    mIsPanelOpen			= false;
	mIsPanelTabTouched		= false;
	mHasPanelBeenDragged	= false;

    // set now, needed in setShowSettings and updateLayout
    mInterfaceSize = interfaceSize;

    // these rectangles are essentially constant (except for width), movement is handled by setTransform
    mPanelRect      = Rectf(0, 0, interfaceSize.x, interfaceSize.y + mPanelSettingsHeight);
	mPanelUpperRect = Rectf(0, 0, interfaceSize.x, interfaceSize.y + mPanelSettingsHeight);
	mPanelLowerRect = Rectf(0, 0, interfaceSize.x, interfaceSize.y + mPanelSettingsHeight);
    mPanelTabRect   = Rectf( interfaceSize.x - 180.0f, -42.0f, interfaceSize.x, 2.0f );
    
    // make sure we're showing enough, then update layout
    setShowSettings(showSettings);    
    
    updateLayout( interfaceSize );
}

void UiLayer::setShowSettings( bool visible ) 
{
	if( visible ){
		mPanelHeight = mPanelSettingsHeight;
	} else {
		mPanelHeight = mPanelOpenHeight;
	}    
    mPanelOpenY	= mInterfaceSize.y - mPanelHeight;        
}

void UiLayer::updateLayout( Vec2f interfaceSize )
{    
    mPanelRect.x2 = interfaceSize.x;
    mPanelTabRect.x1 = interfaceSize.x - 200.0f;
    mPanelTabRect.x2 = interfaceSize.x;    
	mPanelUpperRect.x2 = interfaceSize.x;
	mPanelLowerRect.x2 = interfaceSize.x;
    
    mPanelOpenY		= interfaceSize.y - mPanelHeight;
    mPanelClosedY	= interfaceSize.y;
    
    // cancel interactions
    mIsPanelTabTouched   = false;
    mHasPanelBeenDragged = false;
    
    // jump to end of animation
    if ( mIsPanelOpen ) {
        mPanelY = mPanelOpenY;        
    }
    else {
        mPanelY = mPanelClosedY;        
    }       
    
    mInterfaceSize = interfaceSize;
}

bool UiLayer::touchBegan( TouchEvent::Touch touch )
{
	mHasPanelBeenDragged = false;

	Vec2f touchPos = globalToLocal( touch.getPos() );

    mIsPanelTabTouched = mPanelTabRect.contains( touchPos );
    
	if( mIsPanelTabTouched ){
        // remember touch offset for accurate dragging
		mPanelTabTouchOffset = mPanelTabRect.getUpperLeft() - touchPos;
	}
		
	return mIsPanelTabTouched;
}

bool UiLayer::touchMoved( TouchEvent::Touch touch )
{
	Vec2f touchPos = globalToLocal( touch.getPos() );
    
	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;

        // apply the touch pos and offset
        Vec2f newPos = touchPos + mPanelTabTouchOffset;
        mPanelY += newPos.y - mPanelTabRect.y1;
	}

	return mIsPanelTabTouched;
}

bool UiLayer::touchEnded( TouchEvent::Touch touch )
{
    // decide if the open state should change:
	if( mIsPanelTabTouched ){
		if( mHasPanelBeenDragged ){
            mIsPanelOpen = (mPanelY - mPanelOpenY) < mPanelHeight/2.0f;
		} 
        else {
            mIsPanelOpen = !mIsPanelOpen;
            if (mIsPanelOpen) {
                Flurry::getInstrumentation()->logEvent("UIPanel Opened");
            } else {
                Flurry::getInstrumentation()->logEvent("UIPanel Closed");
            }
		}
		G_HELP = false;
	}

    // reset for next time
    mIsPanelTabTouched = false;
    mHasPanelBeenDragged = false;
    
	return false;
}

void UiLayer::update()
{
    Vec2f interfaceSize = mRoot->getInterfaceSize();
    // check for orientation change
    if( interfaceSize != mInterfaceSize ){
        updateLayout( interfaceSize );
    }
    
    if ( !mHasPanelBeenDragged ) {
        // if we're not dragging, animate to current state
        if( mIsPanelOpen ){
            mPanelY += (mPanelOpenY - mPanelY) * 0.25f;
        }
        else {
            mPanelY += (mPanelClosedY - mPanelY) * 0.25f;
        }
    }
    
    // make sure the drag/ease hasn't messed anything up
    // always use the tallest size for maxPanelY so we'll ease when closing settings...
    const float maxPanelY = mInterfaceSize.y - mPanelSettingsHeight;
    mPanelY = constrain( mPanelY, maxPanelY, mPanelClosedY );
    
    Matrix44f transform;
    transform.translate( Vec3f( 0, ceil( mPanelY ), 0 ) );
    setTransform( transform );
}

void UiLayer::draw()
{	
    bloom::gl::beginBatch();
    bloom::gl::batchRect(mUiButtonsTex, Rectf(0.01f, 0.51f, 0.09f, 0.99f), mPanelUpperRect);
    bloom::gl::batchRect(mUiButtonsTex, Rectf(0.01f, 0.51f, 0.09f, 0.99f), mPanelLowerRect);
    bloom::gl::batchRect(mUiButtonsTex, Rectf(0.1f, 0.0f, 1.0f, 1.0f), mPanelTabRect);
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );    
    bloom::gl::endBatch();

    gl::color( ColorA( BRIGHT_BLUE, 0.2f ) );
    // FIXME: can we "round" mPanelY so that this doesn't jitter?
	gl::drawLine( Vec2f( mPanelRect.x1, 0.0f ), Vec2f( mPanelTabRect.x1 + 5, 0.0f ) );
	
	gl::color( ColorA( BRIGHT_BLUE, 0.1f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, mPanelOpenHeight + 1.0f ), Vec2f( mPanelRect.x2, mPanelOpenHeight + 1.0f ) ); 
    
    // apply this alpha to all children
    // FIXME: is there a more reliable way to do this, does UINode need more inheritable properties?
    const float dragAlphaPer = pow( ( mInterfaceSize.y - mPanelY ) / 65.0f, 2.0f );    	
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, dragAlphaPer ) );
    
    // FIXME: make an mActive bool so we can skip interaction and drawing if the panel is hiding
    //mActive = (mInterfaceSize.y - y ) > 60.0f;
    
}
