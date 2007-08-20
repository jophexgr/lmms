/*
 * bit_invader.h - declaration of class bitInvader and bSynth which
 *                         are a wavetable synthesizer
 *
 * Copyright (c) 2006 Andreas Brandmaier <andy/at/brandmaier/dot/de>
 * 
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#ifndef _BIT_INVADER_H
#define _BIT_INVADER_H


#include "instrument.h"
#include "spc_bg_hndl_widget.h"
#include "graph.h"
#include "led_checkbox.h"
#include "oscillator.h"

class QPixmap;

class knob;
class notePlayHandle;
class pixmapButton;

class bSynth
{
public:
	bSynth(float* sample, int length, float _pitch, bool _interpolation, float factor, const sample_rate_t _sample_rate);
	virtual ~bSynth();
	
	sample_t nextStringSample();	


private:

	int sample_index;
	float sample_realindex;
	int sample_length;
	float* sample_shape;
	float sample_step;	

	bool interpolation;
	
} ;

class bitInvader : public instrument, public specialBgHandlingWidget
{
	Q_OBJECT
public:
	bitInvader(instrumentTrack * _channel_track );
	virtual ~bitInvader();

	virtual void FASTCALL playNote( notePlayHandle * _n,
						bool _try_parallelizing );
	virtual void FASTCALL deleteNotePluginData( notePlayHandle * _n );


	virtual void FASTCALL saveSettings( QDomDocument & _doc,
							QDomElement & _parent );
	virtual void FASTCALL loadSettings( const QDomElement & _this );

	virtual QString nodeName( void ) const;

	virtual f_cnt_t desiredReleaseFrames( void ) const
	{
		return( 64 );
	}


public slots:
        void sampleSizeChanged( float _new_sample_length );
        void sampleChanged( void );

        void interpolationToggle( bool value );
        void normalizeToggle( bool value );
        void smoothClicked( void  );

        void sinWaveClicked( void );
        void triangleWaveClicked( void );
        void sqrWaveClicked( void );
        void sawWaveClicked( void );
        void noiseWaveClicked( void );
        void usrWaveClicked( void );
/*
protected:
	virtual void paintEvent( QPaintEvent * );
*/

private:
	knob * m_sampleLengthKnob;

	pixmapButton * sinWaveBtn;
	pixmapButton * triangleWaveBtn;
	pixmapButton * sqrWaveBtn;
	pixmapButton * sawWaveBtn;
	pixmapButton * whiteNoiseWaveBtn;
	pixmapButton * smoothBtn;
	pixmapButton * usrWaveBtn;

	static QPixmap * s_artwork;

	graph * m_graph;
	ledCheckBox * m_interpolationToggle;
	ledCheckBox * m_normalizeToggle;

	int sample_length;
        float* sample_shape;

	bool interpolation;	                
	bool normalize;
	float normalizeFactor;
	
	oscillator * m_osc;
} ;


#include "bit_invader.moc"


#endif
