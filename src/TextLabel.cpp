//
//  TextLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TextLabel.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"

using namespace std;
using namespace ci;
using namespace ci::app;

void TextLabel::setText(string text)
{ 
    if (mText != text) { 
        mText = text; 
        updateTexture(); 
    }
}

void TextLabel::updateTexture()
{
    TextLayout layout;
    layout.setFont( mFont );
    layout.setColor( mColor );
    layout.addLine( mText );
    mTexture = layout.render( true, false );
}

void TextLabel::draw()
{
    if (mTexture) {
        gl::draw( mTexture, mRect.getUpperLeft() );
    }
}

bool TextLabel::touchBegan( TouchEvent::Touch touch )
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}

bool TextLabel::touchEnded( TouchEvent::Touch touch )
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
