/*
 * spectrum_analyzer.cpp - spectrum analyzer plugin
 *
 * Copyright (c) 2008-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "spectrum_analyzer.h"

#include "embed.cpp"


extern "C"
{

plugin::descriptor PLUGIN_EXPORT spectrumanalyzer_plugin_descriptor =
{
	STRINGIFY( PLUGIN_NAME ),
	"Spectrum Analyzer",
	QT_TRANSLATE_NOOP( "pluginBrowser",
				"plugin for using arbitrary VST-effects "
				"inside LMMS." ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	plugin::Effect,
	new pluginPixmapLoader( "logo" ),
	NULL,
	NULL
} ;

}



spectrumAnalyzer::spectrumAnalyzer( model * _parent,
			const descriptor::subPluginFeatures::key * _key ) :
	effect( &spectrumanalyzer_plugin_descriptor, _parent, _key ),
	m_saControls( this ),
	m_framesFilledUp( 0 ),
	m_energy( 0 )
{
	m_specBuf = (fftwf_complex *) fftwf_malloc( ( FFT_BUFFER_SIZE + 1 ) *
						sizeof( fftwf_complex ) );
	m_fftPlan = fftwf_plan_dft_r2c_1d( FFT_BUFFER_SIZE*2, m_buffer,
						m_specBuf, FFTW_MEASURE );
}




spectrumAnalyzer::~spectrumAnalyzer()
{
	fftwf_destroy_plan( m_fftPlan );
	fftwf_free( m_specBuf );
}




bool spectrumAnalyzer::processAudioBuffer( sampleFrame * _buf,
							const fpp_t _frames )
{
	if( !isEnabled() || !isRunning () )
	{
		return( FALSE );
	}

	fpp_t f = 0;
	if( _frames > FFT_BUFFER_SIZE )
	{
		m_framesFilledUp = 0;
		f = _frames - FFT_BUFFER_SIZE;
	}

	const int cm = m_saControls.m_channelMode.value();

	switch( cm )
	{
		case MergeChannels:
			for( ; f < _frames; ++f )
			{
				m_buffer[m_framesFilledUp] =
					( _buf[f][0] + _buf[f][1] ) * 0.5;
				++m_framesFilledUp;
			}
			break;
		case LeftChannel:
			for( ; f < _frames; ++f )
			{
				m_buffer[m_framesFilledUp] = _buf[f][0];
				++m_framesFilledUp;
			}
			break;
		case RightChannel:
			for( ; f < _frames; ++f )
			{
				m_buffer[m_framesFilledUp] = _buf[f][1];
				++m_framesFilledUp;
			}
			break;
	}

	if( m_framesFilledUp < FFT_BUFFER_SIZE )
	{
		return( isRunning() );
	}


//	hanming( m_buffer, FFT_BUFFER_SIZE, HAMMING );

	const sample_rate_t sr = engine::getMixer()->processingSampleRate();
	const int LOWEST_FREQ = 0;
	const int HIGHEST_FREQ = sr / 2;

	fftwf_execute( m_fftPlan );
	absspec( m_specBuf, m_absSpecBuf, FFT_BUFFER_SIZE+1 );
	if( m_saControls.m_linearSpec.value() )
	{
		compressbands( m_absSpecBuf, m_bands, FFT_BUFFER_SIZE+1,
			MAX_BANDS,
			(int)(LOWEST_FREQ*(FFT_BUFFER_SIZE+1)/(float)(sr/2)),
			(int)(HIGHEST_FREQ*(FFT_BUFFER_SIZE+1)/(float)(sr/2)));
		m_energy = maximum( m_bands, MAX_BANDS ) /
					maximum( m_buffer, FFT_BUFFER_SIZE );
	}
	else
	{
		calc13octaveband31( m_absSpecBuf, m_bands,
						FFT_BUFFER_SIZE+1, sr/2.0 );
		m_energy = signalpower( m_buffer, FFT_BUFFER_SIZE ) /
					maximum( m_buffer, FFT_BUFFER_SIZE );
	}


	m_framesFilledUp = 0;

	checkGate( 0 );

	return( isRunning() );
}





extern "C"
{

// neccessary for getting instance out of shared lib
plugin * PLUGIN_EXPORT lmms_plugin_main( model * _parent, void * _data )
{
	return( new spectrumAnalyzer( _parent,
		static_cast<const plugin::descriptor::subPluginFeatures::key *>(
								_data ) ) );
}

}

