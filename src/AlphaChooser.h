/*
 *  AlphaChooser.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2011 Bloom Studio, Inc. All rights reserved.
 *
 */

#pragma once
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "Orientation.h"
#include "OrientationEvent.h"
#include "WheelOverlay.h"
#include "UiLayer.h"
#include "cinder/Function.h"
#include "BloomNode.h"

class AlphaChooser : public BloomNode {

public:
    
	AlphaChooser() {};
	~AlphaChooser() {};
		
	void	setup( const ci::Font &font, UiLayerRef uiLayer, WheelOverlayRef wheelOverlay );    
	void	update();
	void	draw();
    
	bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchMoved( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );
    
    void    setNumberAlphaPerChar( float *numAlphaPerChar );
        	
    void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }
	
	template<typename T>
	ci::CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( char ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
    bool hitTest( ci::Vec2f globalPos ) { return mVisible && mFullRect.contains( globalToLocal( globalPos ) ); }

private:
    
	void	setRects();    
    
    WheelOverlayRef mWheelOverlay;
	UiLayerRef mUiLayer;
    
    float           *mNumberAlphaPerChar;

	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar;
	
    ci::Vec2f       mInterfaceSize;
    
	std::vector<ci::gl::Texture> mAlphaTextures;
	std::vector<ci::Rectf>       mAlphaRects;
	std::vector<ci::Rectf>       mAlphaHitRects;
    ci::Rectf mFullRect;
    
	ci::CallbackMgr<bool(char)> mCallbacksAlphaCharSelected;    
};
