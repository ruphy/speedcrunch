// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.


#include <base/functions.hxx>
#include <base/settings.hxx> //refdp
#include <gui/result.hxx>
#include <math/hmath.hxx>

#include <QApplication>
#include <QClipboard>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>


class ResultPrivate
{
  public:
    QStringList contents;
    int         count;
    bool        customAppearance;
    QColor      customBackgroundColor1;
    QColor      customBackgroundColor2;
    QColor      customErrorColor;
    QColor      customTextColor;
    int         decimalDigits;
    char        format;
};


class BaseItem : public QListWidgetItem
{
  public:
    BaseItem( Result * r ) : QListWidgetItem( r ) {}
    virtual void updateItem() {}
};


class ExprItem : public BaseItem
{
  public:
    ExprItem( Result * r, int i, const QString & e )
      : BaseItem( r )
    {
      expr = e;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignLeft );
      setText( e );
      //printf( "%s\n", qPrintable(e) );
      updateItem();
    }

    void updateItem()
    {
      if ( result->customAppearance() )
      {
        setForeground( result->customTextColor() );
        setBackground( (index & 1) ?  result->customBackgroundColor1() : result->customBackgroundColor2() );
      }
      else
      {
        const QPalette & pal = result->palette();
        setForeground( pal.text() );
        setBackground( (index & 1) ? pal.base() : pal.alternateBase() );
      }
    }

  private:
    QString  expr;
    int      index;
    Result * result;
};


class ResultItem : public BaseItem
{
  public:
    ResultItem( Result * r, int i,  const HNumber & v )
      : BaseItem( r )
    {
      value = v;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignRight );
      setText( result->formatNumber( value ) );
      //printf( "%s\n", qPrintable(text()) );
      updateItem();
    }

    void updateItem()
    {
      setText( result->formatNumber( value ) );

      if ( result->customAppearance() )
      {
        setForeground( result->customTextColor() );
        setBackground( (index & 1) ? result->customBackgroundColor1() :
        result->customBackgroundColor2() );
      }
      else
      {
        const QPalette & pal = result->palette();
        setForeground( pal.text() );
        setBackground( (index & 1) ? pal.base() : pal.alternateBase() );
      }

    }

  private:
    int      index;
    Result * result;
    HNumber  value;
};


class ErrorItem: public BaseItem
{
  public:
    ErrorItem( Result * r, int i, const QString& m )
      : BaseItem( r )
    {
      msg = m;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignRight );
      setText( m );
      //printf( "%s\n", qPrintable(msg) );
      updateItem();
    }

    void updateItem()
    {
      if( result->customAppearance() )
      {
        setForeground( result->customErrorColor() );
        setBackground( (index & 1) ? result->customBackgroundColor1() :
          result->customBackgroundColor2() );
      }
      else
      {
        const QPalette & pal = result->palette();
        setForeground( Qt::red );
        setBackground( (index & 1) ? pal.base() : pal.alternateBase() );
      }
    }

  private:
    int      index;
    QString  msg;
    Result * result;
};


// public

Result::Result( QWidget * parent, const char * name )
  : QListWidget( parent )
{
  d = new ResultPrivate;
  d->format = 'g';
  d->decimalDigits = -1;
  d->customAppearance = false;
  d->count = 0;

  d->customTextColor        = QColor( 255, 255, 255 );
  d->customBackgroundColor1 = QColor(   0,   0,   0 );
  d->customBackgroundColor2 = QColor(  21,  21,  21 );
  d->customErrorColor       = QColor( 255,   0,   0 );

  setObjectName( name );
  setBackgroundRole(QPalette::Base);

  setSelectionMode( NoSelection );
  setMinimumWidth( 150 );

  connect( this, SIGNAL(itemClicked( QListWidgetItem * )), this, SLOT(copyToClipboard( QListWidgetItem * )) );

  setFocusPolicy( Qt::NoFocus );

  setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  setVerticalScrollMode(   QAbstractItemView::ScrollPerPixel );
}


QString Result::asText() const
{
  return d->contents.join( "\n" );
}


void Result::append( const QString & expr, const HNumber & value )
{
  d->count++;

  new ExprItem(   this, d->count, expr );
  new ResultItem( this, d->count, value );

  d->contents.append( expr );
  d->contents.append( HMath::format( value ) );

  QTimer::singleShot( 0, this, SLOT(scrollEnd()) );
}


void Result::appendError( const QString & expr, const QString & msg )
{
  d->count++;

  new ExprItem(  this, d->count, expr );
  new ErrorItem( this, d->count, msg );

  d->contents.append( expr );
  d->contents.append( msg );

  QTimer::singleShot( 0, this, SLOT(scrollEnd()) );
}


int Result::count() const
{
  return d->count;
}


bool Result::customAppearance() const
{
  return d->customAppearance;
}


QColor Result::customBackgroundColor1() const
{
  return d->customBackgroundColor1;
}


QColor Result::customBackgroundColor2() const
{
  return d->customBackgroundColor2;
}


QColor Result::customErrorColor() const
{
  return d->customErrorColor;
}


QColor Result::customTextColor() const
{
  return d->customTextColor;
}


int Result::decimalDigits() const
{
  return d->decimalDigits;
}


char Result::format() const
{
  return d->format;
}


QString Result::formatNumber( const HNumber & value ) const
{
  char * str = HMath::format( value, value.format() ? value.format() : d->format, d->decimalDigits );
  QString s = QString::fromLatin1( str );
  free( str );
  QChar dot = Settings::decimalPoint();
  if ( dot != '.' )
    s.replace( '.', dot);
  return s;
}


void Result::setCustomAppearance( bool custom )
{
  d->customAppearance = custom;
  QColor bgcolor = QApplication::palette().base().color();
  if ( custom )
    bgcolor = customBackgroundColor1();

  QPalette pal = palette();
  pal.setColor( QPalette::Background, bgcolor);
  setPalette( pal );

  triggerUpdate();
}


void Result::setCustomBackgroundColor( const QColor & bg1, const QColor & bg2 )
{
  d->customBackgroundColor1 = bg1;
  d->customBackgroundColor2 = bg2;
  if( d->customAppearance )
  {
    QPalette pal = palette();
    pal.setColor( QPalette::Base, bg1 );
    setPalette( pal );
  }
  triggerUpdate();
}


void Result::setCustomErrorColor( const QColor & e )
{
  d->customErrorColor = e;
  triggerUpdate();
};


void Result::setCustomTextColor( const QColor & c )
{
  d->customTextColor = c;
  triggerUpdate();
}


void Result::setDecimalDigits( int digits )
{
  d->decimalDigits = digits;
  triggerUpdate();
}


void Result::setFormat( char format )
{
  d->format = format;
  triggerUpdate();
}


Result::~Result()
{
  delete d;
}


// public slots

void Result::clear()
{
  d->count = 0;
  d->contents.clear();
  QListWidget::clear();
}


// protected

void Result::resizeEvent( QResizeEvent * )
{
   triggerUpdate();
}


// private slots

void Result::copyToClipboard( QListWidgetItem* item )
{
  if ( ! item )
    return;

  QClipboard * cb = QApplication::clipboard();
  cb->setText( item->text(), QClipboard::Clipboard );
  emit textCopied( item->text() );
}


void Result::scrollEnd()
{
  QScrollBar * hBar = horizontalScrollBar();
  hBar->setValue( hBar->maximum() );
  scrollToBottom();
}


void Result::triggerUpdate()
{
  for ( int c = 0; c < count(); c++ )
    dynamic_cast<BaseItem *>( item(c) )->updateItem();
}
