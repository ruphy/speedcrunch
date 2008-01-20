/* This file is part of the SpeedCrunch project
   Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
   Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include <gui/aboutbox.hxx>

#include <QtGui>

#define MAKE_STRING(a) DO_MAKE_STRING(a)
#define DO_MAKE_STRING(a) # a

class MarqueeTextPrivate
{
  public:
    QTextEdit     * editor;
    QTextDocument * doc;
    QPixmap       * buffer;
    unsigned        pos;
    unsigned        scrollStep;
    unsigned        scrollTick;
    unsigned        sideMargin;
};

MarqueeText::MarqueeText( QWidget * parent, const char * name ):
  QFrame( parent )
{
  d = new MarqueeTextPrivate;
  d->scrollStep = 1;
  d->scrollTick = 40;
  d->sideMargin = 20;

  d->buffer = new QPixmap;
  d->editor = new QTextEdit( this );
  d->editor->hide();
  d->doc = d->editor->document();
  d->pos = 50;

  setFrameStyle( Panel | Sunken );
  setLineWidth( 1 );

  QTimer * scrollTimer = new QTimer( this );
  connect( scrollTimer, SIGNAL(timeout()), this, SLOT(scroll()) );
  scrollTimer->start( d->scrollTick );
}

MarqueeText::~MarqueeText()
{
  delete d->buffer;
  delete d;
}

QSizePolicy MarqueeText::sizePolicy() const
{
  return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void MarqueeText::setText( const QString & text )
{
  d->doc->clear();
  d->doc->setHtml( text );
  layout();
  d->pos = height();
  update();
}

void MarqueeText::paintEvent( QPaintEvent * e )
{
  QPainter painter( this );
  //painter.fillRect( this->rect(), painter.background().color() );
  painter.fillRect( this->rect(), Qt::black );
  painter.end();

  QFrame::paintEvent( e );

  if( d->buffer )
  {
    QPainter p( this );
    QPixmap pix = d->buffer->copy();
    QRegion clip( 2, 2, (width() - 4), (height() - 4) );
    p.setClipRegion( clip );
    p.drawPixmap( 0, d->pos, pix );
  }
}

void MarqueeText::resizeEvent( QResizeEvent * e )
{
  layout();
  e->accept();
  update();
}

void MarqueeText::scroll()
{
  d->pos -= d->scrollStep;
  int offset = d->pos + d->buffer->height();

  if ( offset < 0 )
    d->pos = height() + 2 * d->scrollStep;

  update();
}

void MarqueeText::layout()
{
  QList< QTextLayout * > layouts;

  int leading = fontMetrics().leading();
  double lineWidth = width() - d->sideMargin;
  double hh = 10;
  int c = 0;

  // layouting lines, also to find total height
  QTextBlock block = d->doc->begin();
  while ( block.isValid() )
  {
    QTextLayout * textLayout = new QTextLayout( block );
    layouts.append( textLayout );

    textLayout->beginLayout();
    while ( true )
    {
      QTextLine line = textLayout->createLine();
      if( !line.isValid() )
        break;

      line.setLineWidth( lineWidth );
      double x = (lineWidth - line.naturalTextWidth()) / 2;
      line.setPosition( QPointF( x - 3, hh ) );
      hh += line.height() / 2;
      hh += leading;
      c++;
    }
    textLayout->endLayout();

    block = block.next();
    hh += 10; // paragraph-spacing
  }

  // recreate off-screen buffer
  QPixmap * oldbuf = d->buffer;
  d->buffer = 0;
  delete oldbuf;

  d->buffer = new QPixmap( QSize( int(lineWidth + 5), int(2 * hh + 50) ) );
  d->buffer->fill( Qt::black );

  // paint the lines
  QPainter painter( d->buffer );
  painter.setPen( Qt::white );
  double ypos = 0;
  for ( int k = 0; k < layouts.count(); k++ )
  {
    QTextLayout * textLayout = layouts[k];
    if ( ! textLayout )
      continue;
    for ( int i = 0; i < textLayout->lineCount(); ++i )
    {
      QTextLine line = textLayout->lineAt( i );
      QPointF pos = QPointF( (d->sideMargin / 2), ypos );
      line.draw( & painter, pos, 0 );
      ypos += line.height() / 2;
      ypos += leading;
    }
    ypos += 10; // paragraph-spacing
  }
}

AboutBox::AboutBox( QWidget * parent ) : QDialog( parent )
{
  setObjectName( "AboutBox" );
  setWindowTitle( tr("About SpeedCrunch") );

  QString website = "http://www.speedcrunch.org";

  QLabel * infoLabel = new QLabel( this );
  QString info = "<b>";
  info += tr("SpeedCrunch version %1").arg( MAKE_STRING(SPEEDCRUNCH_VERSION) );
  info += "</b><br>";
  info += QString( "<a href=\"%1\">%2</a>" ).arg( website ).arg( website );
  infoLabel->setText( info );
  infoLabel->setOpenExternalLinks( true );

  QLabel * iconLabel = new QLabel( this );
  iconLabel->setPixmap( QPixmap( ":/crunch.png" ) );
  iconLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  MarqueeText * marqueeText = new MarqueeText( this );
  QString msg = "<br><br><br><br><br><center>";
  msg += "<p><b><i>";
  msg += tr( "SpeedCrunch version %1").arg( MAKE_STRING(SPEEDCRUNCH_VERSION) );
  msg += "</i></b><br></p>";

  const char * mainFmt = "<p><b>%1</b><br>%2";
  msg += QString( mainFmt ).arg( tr("Based on original idea from") ).arg( "Roberto Alsina" );
  msg += QString( mainFmt ).arg( tr("Original author") ).arg( "Ariya Hidayat" );
  msg += QString( mainFmt ).arg( tr("Current maintainer") ).arg( "Helder Correia" );
  msg += QString( mainFmt ).arg( tr("Developers") ).arg( "Ariya Hidayat<br>"
                                                         "Helder Correia<br>"
                                                         "Johan Thelin<br>"
                                                         "Wolf Lammen" );

  msg += QString( mainFmt ).arg( tr("Special thanks to") ).arg( "Marco Wegner<br>"
                                                                "Michael Pyne<br>"
                                                                "Jonathan Avraham<br>"
                                                                "Witold Wysota<br>" );

  msg += QString( mainFmt ).arg( tr("Artworks") ).arg( "Kuswanto (Zeus)<br>"
                                                       "Helder Correia<br>"
                                                       "Oxygen Project" );

  msg += "<p>";
  msg += QString( "<b>%1</b><br><br>" ).arg( tr("Thanks to") );
  msg += "Alejandro Villarreal<br>"
         "Alexey Kouznetsov<br>"
         "Anders Lund<br>"
	 "Bart Martens<br>"
         "Bernhard Schiffner<br>"
         "Damir Perisa<br>"
         "Derek O'Connor<br>"
         "Francesco di Cugno<br>"
         "Gary Cramblitt<br>"
         "Henrique Pinto<br>"
         "Jean-Remy Falleri<br>"
         "Lars Ivar Igesund<br>"
         "Maciek Borowka<br>"
	 "Matthew J Smith<br>"
         "Melchior Franz<br>"
         "Mohamed Eldesoky<br>"
         "Philippe Fremy<br>"
         "Pieter Pareit<br>"
         "Roland \"liquidat\" Wolters<br>"
	 "Sarah Hobbs<br>"
	 "Stephan Binner<br>"
         "Thomas Luebking<br>"
         "Thomas Nagy<br>"
	 "Vibet Alexis<br>"
         "Vladimir Pouzanov<br>"
	 "Yolla Indria<br>"
         ;
  msg += QString( "<i>%1</i><br>" ).arg( tr("...and many others...") );
  msg += "</p>";

  msg += "<p>";
  msg += QString( "<b>%1</b><br><br>"  ).arg( tr("Translations") );
  const char * i18nFmt = "<b>%1</b><br>%2<br><br>";
  msg += QString( i18nFmt ).arg( tr("Argentinian Spanish")  ).arg( "Lucas Sallovitz"        );
  msg += QString( i18nFmt ).arg( tr("Brazilian Portuguese") ).arg( "F&aacute;bio Margarido" );
  msg += QString( i18nFmt ).arg( tr("Czech")                ).arg( "Blond'&aacute;k"        );
  msg += QString( i18nFmt ).arg( tr("Dutch")                ).arg( "Maarten Fonville"       );
  msg += QString( i18nFmt ).arg( tr("French")               ).arg( "Gr&eacute;gory Treiber" );
  msg += QString( i18nFmt ).arg( tr("Finnish")              ).arg( "J-P Nurmi"              );
  msg += QString( i18nFmt ).arg( tr("German")               ).arg( "Marco Wegner"           );
  msg += QString( i18nFmt ).arg( tr("Hebrew")               ).arg( "Jonathan Avraham"       );
  msg += QString( i18nFmt ).arg( tr("Indonesian")           ).arg( "Ariya Hidayat"          );
  msg += QString( i18nFmt ).arg( tr("Italian")              ).arg( "Alessio Sclocco"        );
  msg += QString( i18nFmt ).arg( tr("Norwegian")            ).arg( "Thomas Finneid"         );
  msg += QString( i18nFmt ).arg( tr("Portuguese")           ).arg( "Helder Correia"         );
  msg += QString( i18nFmt ).arg( tr("Romanian")             ).arg( "Eugeniu Plamadeala"     );
  msg += QString( i18nFmt ).arg( tr("Russian")              ).arg( "Irek Khasyanov"         );
  msg += QString( i18nFmt ).arg( tr("Spanish")              ).arg( "F Chris Carrera"        );
  msg += QString( i18nFmt ).arg( tr("Swedish")              ).arg( "Johan Thelin"           );
  msg += "</p>";

  msg += "<p>";
  const char * copyFmt = "<b>%1 %2 %3</b><br><i>%4</i><br><br>";
  msg += QString( copyFmt ).arg( tr("Copyright (C)") ).arg( "2004-2007" ).arg( "Ariya Hidayat"  ).arg( "ariya@kde.org"                    );
  msg += QString( copyFmt ).arg( tr("Copyright (C)") ).arg( "2005-2006" ).arg( "Johan Thelin"   ).arg( "e8johan@gmail.com"                );
  msg += QString( copyFmt ).arg( tr("Copyright (C)") ).arg( "2007-2008" ).arg( "Helder Correia" ).arg( "helder.pereira.correia@gmail.com" );
  msg += QString( copyFmt ).arg( tr("Copyright (C)") ).arg( "2007-2008" ).arg( "Wolf Lammen"    ).arg( "ookami1@gmx.de" );
  msg += "</p>";

  msg += "<p>";
  msg += tr("This program is free software; you can redistribute it and/or "
            "modify it under the terms of the GNU General Public License "
            "as published by the Free Software Foundation; either version 2 "
            "of the License, or (at your option) any later version" );
  msg += "</p>";
  msg += "<p>";
  msg += tr("This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
            "GNU General Public License for more details.");
  msg += "</p>";
  msg +=  "<p>&nbsp;</p>";
  msg += QString( "<p>%1</p>" ).arg( tr("Visit <b>http://www.speedcrunch.org</b> for more information!") );
  marqueeText->setText( msg );

  QPushButton * okButton = new QPushButton( this );
  okButton->setObjectName( "OKButton" );
  okButton->setText( tr("OK") );
  QObject::connect( okButton, SIGNAL(clicked()), this, SLOT(accept()) );

  QSpacerItem * buttonSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

  // --- layout handling ---

  QVBoxLayout * mainLayout   = new QVBoxLayout( this );
  QHBoxLayout * topLayout    = new QHBoxLayout();
  QHBoxLayout * buttonLayout = new QHBoxLayout();

  mainLayout->addLayout( topLayout );
  mainLayout->addWidget( marqueeText );
  mainLayout->addLayout( buttonLayout );
  mainLayout->setStretchFactor( marqueeText, 1 );

  topLayout->addWidget( infoLabel );
  topLayout->addWidget( iconLabel );

  buttonLayout->addItem( buttonSpacer );
  buttonLayout->addWidget( okButton );

  setWindowTitle( tr("About SpeedCrunch") );
  setMinimumSize( 350, 400 );
  setMaximumSize( 350, 400 );
}
