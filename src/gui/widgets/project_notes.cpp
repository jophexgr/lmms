/*
 * project_notes.cpp - implementation of project-notes-editor
 *
 * Copyright (c) 2005-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#include "project_notes.h"

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QColorDialog>
#include <QtGui/QComboBox>
#include <QtGui/QFontDatabase>
#include <QtGui/QLineEdit>
#include <QtGui/QMdiArea>
#include <QtGui/QTextCursor>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtXml/QDomCDATASection>

#include "embed.h"
#include "engine.h"
#include "main_window.h"
#include "song.h"



projectNotes::projectNotes( void ) :
	QMainWindow( engine::getMainWindow()->workspace() )
{
	m_edit = new QTextEdit( this );
	m_edit->setAutoFillBackground( TRUE );
	QPalette pal;
	pal.setColor( m_edit->backgroundRole(), QColor( 64, 64, 64 ) );
	m_edit->setPalette( pal );
	m_edit->show();

	clear();

	connect( m_edit,
		SIGNAL( currentCharFormatChanged( const QTextCharFormat & ) ),
		this, SLOT( formatChanged( const QTextCharFormat & ) ) );
//	connect( m_edit, SIGNAL( currentAlignmentChanged( int ) ),
//			this, SLOT( alignmentChanged( int ) ) );
	connect( m_edit, SIGNAL( textChanged() ),
			engine::getSong(), SLOT( setModified() ) );

	setupActions();

	setCentralWidget( m_edit );
	setWindowTitle( tr( "Project notes" ) );
	setWindowIcon( embed::getIconPixmap( "project_notes" ) );

	engine::getMainWindow()->workspace()->addSubWindow( this );
	parentWidget()->setAttribute( Qt::WA_DeleteOnClose, false );
	parentWidget()->move( 700, 10 );
	parentWidget()->resize( 400, 300 );
	parentWidget()->hide();
}




projectNotes::~projectNotes()
{
}




void projectNotes::clear( void )
{
	m_edit->setHtml( tr( "Put down your project notes here." ) );
	m_edit->selectAll();
	m_edit->setTextColor( QColor( 224, 224, 224 ) );
	QTextCursor cursor = m_edit->textCursor();
	cursor.clearSelection();
	m_edit->setTextCursor( cursor );
}




void projectNotes::setText( const QString & _text )
{
	m_edit->setHtml( _text );
}




void projectNotes::setupActions()
{
	QToolBar * tb = addToolBar( tr( "Edit Actions" ) );
	QAction * a;

	a = new QAction( embed::getIconPixmap( "edit_undo" ), tr( "&Undo" ),
									this );
	a->setShortcut( tr( "Ctrl+Z" ) );
	connect( a, SIGNAL( triggered() ), m_edit, SLOT( undo() ) );
	tb->addAction( a );

	a = new QAction( embed::getIconPixmap( "edit_redo" ), tr( "&Redo" ),
									this );
	a->setShortcut( tr( "Ctrl+Y" ) );
	connect( a, SIGNAL( triggered() ), m_edit, SLOT( redo() ) );
	tb->addAction( a );

	a = new QAction( embed::getIconPixmap( "edit_copy" ), tr( "&Copy" ),
									this );
	a->setShortcut( tr( "Ctrl+C" ) );
	connect( a, SIGNAL( triggered() ), m_edit, SLOT( copy() ) );
	tb->addAction( a );

	a = new QAction( embed::getIconPixmap( "edit_cut" ), tr( "Cu&t" ),
									this );
	a->setShortcut( tr( "Ctrl+X" ) );
	connect( a, SIGNAL( triggered() ), m_edit, SLOT( cut() ) );
	tb->addAction( a );

	a = new QAction( embed::getIconPixmap( "edit_paste" ), tr( "&Paste" ),
									this );
	a->setShortcut( tr( "Ctrl+V" ) );
	connect( a, SIGNAL( triggered() ), m_edit, SLOT( paste() ) );
	tb->addAction( a );


	tb = addToolBar( tr( "Format Actions" ) );

	m_comboFont = new QComboBox( tb );
	m_comboFont->setEditable( TRUE );
	QFontDatabase db;
	m_comboFont->addItems( db.families() );
	connect( m_comboFont, SIGNAL( activated( const QString & ) ),
			m_edit, SLOT( setFontFamily( const QString & ) ) );
	m_comboFont->lineEdit()->setText( QApplication::font().family() );

	m_comboSize = new QComboBox( tb );
	m_comboSize->setEditable( TRUE );
	QList<int> sizes = db.standardSizes();
	QList<int>::Iterator it = sizes.begin();
	for ( ; it != sizes.end(); ++it )
	{
		m_comboSize->addItem( QString::number( *it ) );
	}
	connect( m_comboSize, SIGNAL( activated( const QString & ) ),
		     this, SLOT( textSize( const QString & ) ) );
	m_comboSize->lineEdit()->setText( QString::number(
					QApplication::font().pointSize() ) );

	m_actionTextBold = new QAction( embed::getIconPixmap( "text_bold" ),
							tr( "&Bold" ), this );
	m_actionTextBold->setShortcut( tr( "Ctrl+B" ) );
	m_actionTextBold->setCheckable( TRUE );
	connect( m_actionTextBold, SIGNAL( triggered() ), this,
							SLOT( textBold() ) );

	m_actionTextItalic = new QAction( embed::getIconPixmap( "text_italic" ),
							tr( "&Italic" ), this );
	m_actionTextItalic->setShortcut( tr( "Ctrl+I" ) );
	m_actionTextItalic->setCheckable( TRUE );
	connect( m_actionTextItalic, SIGNAL( triggered() ), this,
							SLOT( textItalic() ) );

	m_actionTextUnderline = new QAction( embed::getIconPixmap(
								"text_under" ),
						tr( "&Underline" ), this );
	m_actionTextUnderline->setShortcut( tr( "Ctrl+U" ) );
	m_actionTextUnderline->setCheckable( TRUE );
	connect( m_actionTextUnderline, SIGNAL( triggered() ), this,
						SLOT( textUnderline() ) );


	QActionGroup * grp = new QActionGroup( tb );
	connect( grp, SIGNAL( triggered( QAction * ) ), this,
					SLOT( textAlign( QAction * ) ) );

	m_actionAlignLeft = new QAction( embed::getIconPixmap( "text_left" ),
						tr( "&Left" ), m_edit );
	m_actionAlignLeft->setShortcut( tr( "Ctrl+L" ) );
	m_actionAlignLeft->setCheckable( TRUE );
	grp->addAction( m_actionAlignLeft );

	m_actionAlignCenter = new QAction( embed::getIconPixmap(
								"text_center" ),
						tr( "C&enter" ), m_edit );
	m_actionAlignCenter->setShortcutContext( Qt::WidgetShortcut );
	m_actionAlignCenter->setShortcut( tr( "Ctrl+E" ) );
	m_actionAlignCenter->setCheckable( TRUE );
	grp->addAction( m_actionAlignCenter );

	m_actionAlignRight = new QAction( embed::getIconPixmap( "text_right" ),
						tr( "&Right" ), m_edit );
	m_actionAlignRight->setShortcutContext( Qt::WidgetShortcut );
	m_actionAlignRight->setShortcut( tr( "Ctrl+R" ) );
	m_actionAlignRight->setCheckable( TRUE );
	grp->addAction( m_actionAlignRight );

	m_actionAlignJustify = new QAction( embed::getIconPixmap(
								"text_block" ),
						tr( "&Justify" ), m_edit );
	m_actionAlignJustify->setShortcut( tr( "Ctrl+J" ) );
	m_actionAlignJustify->setCheckable( TRUE );
	grp->addAction( m_actionAlignJustify );


	QPixmap pix( 16, 16 );
	pix.fill( Qt::black );
	m_actionTextColor = new QAction( pix, tr( "&Color..." ), this );
	connect( m_actionTextColor, SIGNAL( triggered() ), this,
							SLOT( textColor() ) );

	tb->addWidget( m_comboFont );
	tb->addWidget( m_comboSize );
	tb->addAction( m_actionTextBold );
	tb->addAction( m_actionTextItalic );
	tb->addAction( m_actionTextUnderline );

	tb->addAction( m_actionAlignLeft );
	tb->addAction( m_actionAlignCenter );
	tb->addAction( m_actionAlignRight );
	tb->addAction( m_actionAlignJustify );

	tb->addAction( m_actionTextColor );
}




void projectNotes::textBold()
{
	m_edit->setFontWeight( m_actionTextBold->isChecked() ? QFont::Bold :
								QFont::Normal );
	engine::getSong()->setModified();
}




void projectNotes::textUnderline()
{
	m_edit->setFontUnderline( m_actionTextUnderline->isChecked() );
	engine::getSong()->setModified();
}




void projectNotes::textItalic()
{
	m_edit->setFontItalic( m_actionTextItalic->isChecked() );
	engine::getSong()->setModified();
}




void projectNotes::textFamily( const QString & _f )
{
	m_edit->setFontFamily( _f );
	m_edit->viewport()->setFocus();
	engine::getSong()->setModified();
}




void projectNotes::textSize( const QString & _p )
{
	m_edit->setFontPointSize( _p.toInt() );
	m_edit->viewport()->setFocus();
	engine::getSong()->setModified();
}




void projectNotes::textColor()
{
	QColor col = QColorDialog::getColor( m_edit->textColor(), this );
	if ( !col.isValid() )
	{
		return;
	}
	m_edit->setTextColor( col );
	QPixmap pix( 16, 16 );
	pix.fill( Qt::black );
	m_actionTextColor->setIcon( pix );
}




void projectNotes::textAlign( QAction * _a )
{
	if( _a == m_actionAlignLeft )
	{
		m_edit->setAlignment( Qt::AlignLeft );
	}
	else if( _a == m_actionAlignCenter )
	{
		m_edit->setAlignment( Qt::AlignHCenter );
	}
	else if( _a == m_actionAlignRight )
	{
		m_edit->setAlignment( Qt::AlignRight );
	}
	else if( _a == m_actionAlignJustify )
	{
		m_edit->setAlignment( Qt::AlignJustify );
	}
}




void projectNotes::formatChanged( const QTextCharFormat & _f )
{
	QFont font = _f.font();
	m_comboFont->lineEdit()->setText( font.family() );
	m_comboSize->lineEdit()->setText( QString::number( font.pointSize() ) );
	m_actionTextBold->setChecked( font.bold() );
	m_actionTextItalic->setChecked( font.italic() );
	m_actionTextUnderline->setChecked( font.underline() );

	QPixmap pix( 16, 16 );
	pix.fill( _f.foreground().color() );
	m_actionTextColor->setIcon( pix );

	engine::getSong()->setModified();
}




void projectNotes::alignmentChanged( int _a )
{
	if ( _a & Qt::AlignLeft )
	{
		m_actionAlignLeft->setChecked( TRUE );
	}
	else if ( ( _a & Qt::AlignHCenter ) )
	{
		m_actionAlignCenter->setChecked( TRUE );
	}
	else if ( ( _a & Qt::AlignRight ) )
	{
		m_actionAlignRight->setChecked( TRUE );
	}
	else if ( ( _a & Qt::AlignJustify ) )
	{
		m_actionAlignJustify->setChecked( TRUE );
	}
	engine::getSong()->setModified();
}




void projectNotes::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	mainWindow::saveWidgetState( this, _this );

	QDomCDATASection ds = _doc.createCDATASection( m_edit->toHtml() );
	_this.appendChild( ds );
}




void projectNotes::loadSettings( const QDomElement & _this )
{
	mainWindow::restoreWidgetState( this, _this );
	m_edit->setHtml( _this.text() );
}


#include "moc_project_notes.cxx"


