/* This file is part of the SpeedCrunch project
   Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>

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

#include "evaluator.h"
#include "functions.h"
#include "hmath.h"

#include <QApplication>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include <float.h>
#include <math.h>

HNumber deg2rad( HNumber x )
{
  return x * HMath::pi() / HNumber(180);
}

HNumber rad2deg( HNumber x )
{
  return HNumber(180) * x / HMath::pi();
}

HNumber function_abs( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::abs( num );
}

HNumber function_int( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::integer( num );
}

HNumber function_trunc( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  int nArgs = args.count();

  if( nArgs != 1 && nArgs != 2 )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function requires 1 or 2 parameters" ) );
    return HNumber::nan();
  }

  HNumber num = args[0];
  HNumber prec;
  HNumber zero(0);
  if( nArgs == 2)
    prec = args[1];
  else prec = zero;

  if( !prec.isInteger() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function requires integer second parameter" ) );
    return HNumber::nan();
  }

  HNumber limit(150);
  if( prec > limit )
    prec = limit;
  else if( prec < zero )
    prec = zero;

  if( nArgs == 1 )
    return HMath::trunc( num );
  else // nArgs == 2
    return HMath::trunc( num, prec.toInt() );
}

HNumber function_frac( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::frac( num );
}

HNumber function_floor( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::floor( num );
}

HNumber function_ceil( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::ceil( num );
}

HNumber function_gcd( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  int nArgs = args.count();

  if ( nArgs < 2 )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function requires at least 2 parameters" ) );
    return HNumber::nan();
  }

  for ( int i = 0; i < args.count(); i++ )
    if ( !args[i].isInteger() )
    {
      fn->setError( fn->name(), QApplication::translate( "functions",
        "function requires integer parameters" ) );
      return HNumber::nan();
    }

  HNumber result = HMath::gcd( args[0], args[1] );
  for ( int i = 2; i < nArgs; i++ )
  {
    result = HMath::gcd( result, args[i] );
  }
  return result;
}

HNumber function_round( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  int nArgs = args.count();

  if( nArgs != 1 && nArgs != 2 )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function requires 1 or 2 parameters" ) );
    return HNumber::nan();
  }

  HNumber num = args[0];
  HNumber prec;
  HNumber zero(0);
  if( nArgs == 2)
    prec = args[1];
  else
    prec = zero;

  if( !prec.isInteger() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function requires integer second parameter" ) );
    return HNumber::nan();
  }

  HNumber limit(150);
  if( prec > limit )
    prec = limit;
  else if( prec < zero )
    prec = zero;

  if( nArgs == 1 )
    return HMath::round( num );
  else // nArgs == 2
    return HMath::round( num, prec.toInt() );
}

HNumber function_sqrt( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber::nan();
  }

  return HMath::sqrt( num );
}

HNumber function_cbrt( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::cbrt( num );
}

HNumber function_exp( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::exp( num );
}

HNumber function_ln( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber( 0 );
  }

  return HMath::ln( num );
}

HNumber function_log( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber( 0 );
  }

  return HMath::log( num );
}

HNumber function_lg( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber( 0 );
  }

  return HMath::lg( num );
}

HNumber function_sin( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  return HMath::sin( angle );
}

HNumber function_cos( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  return HMath::cos( angle );
}

HNumber function_tan( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::tan( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_cot( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::cot( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_sec( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::sec( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_csc( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::csc( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified parameter" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_asin( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::asin( num );
  if( eval->angleMode() == Evaluator::Degree )
    angle = rad2deg( angle );

  return angle;
}

HNumber function_acos( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::acos( num );
  if( eval->angleMode() == Evaluator::Degree )
    angle = rad2deg( angle );

  return angle;
}

HNumber function_atan( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::atan( num );
  if( eval->angleMode() == Evaluator::Degree )
    angle = rad2deg( angle );

  return angle;
}

HNumber function_sinh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );
  return HMath::sinh( angle );
}

HNumber function_cosh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );
  return HMath::cosh( angle );
}

HNumber function_tanh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );
  return HMath::tanh( angle );
}

HNumber function_sign( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();
  return HMath::sign( args[0] );
}

HNumber function_nCr( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 2 )
    return HNumber::nan();
  return HMath::nCr( args[0], args[1] );
}

HNumber function_nPr( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 2 )
    return HNumber::nan();
  return HMath::nPr( args[0], args[1] );
}

HNumber function_degrees( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();


  HNumber angle = args[0];
  return angle * HNumber(180) / HMath::pi();
}

HNumber function_radians( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();


  HNumber angle = args[0];
  return angle * HMath::pi() / HNumber(180);
}


HNumber function_max( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber::nan();

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    if( HMath::compare( result, args[c] ) == -1 )
      result = args[c];

  return result;
}

HNumber function_min( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber::nan();

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    if( HMath::compare( result, args[c] ) == 1 )
      result = args[c];

  return result;
}

HNumber function_sum( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result + args[c];

  return result;
}

HNumber function_product( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  return result;
}

HNumber function_average( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result + args[c];

  result = result / HNumber(args.count());

  return result;
}

HNumber function_geomean( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  result = result / HNumber(args.count());

  if( result <= HNumber(0))
    return HNumber("NaN");

  return result;
}

HNumber function_dec( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('g');
  return result;
}

HNumber function_hex( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('h');
  return result;
}

HNumber function_oct( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('o');
  return result;
}

HNumber function_bin( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('b');
  return result;
}


class FunctionPrivate
{
public:
  QString name;
  int argc;
  QString desc;
  QString error;
  FunctionPtr ptr;

  FunctionPrivate(): name(), argc(0), desc(), error(), ptr(0) {}
};

class FunctionRepositoryPrivate
{
public:
  QHash<QString, Function*> functions;
};

Function::Function( const QString& name, int argc, FunctionPtr ptr, const QString& desc ):
d( new FunctionPrivate )
{
  d->name = name;
  d->argc = argc;
  d->desc = QApplication::translate( "FunctionRepository", desc.toLatin1() );
  d->ptr = ptr;
}

Function::Function( const QString& name, FunctionPtr ptr, const QString& desc ):
d( new FunctionPrivate )
{
  d->name = name;
  d->argc = -1;
  d->desc = QApplication::translate( "FunctionRepository", desc.toLatin1() );
  d->ptr = ptr;
}

Function::~Function()
{
  delete d;
}

QString Function::name() const
{
  return d->name;
}

QString Function::description() const
{
  return d->desc;
}

QString Function::error() const
{
  return d->error;
}

void Function::setError( const QString& context, const QString& error )
{
  d->error = context + ": " + error;
}

HNumber Function::exec( const Evaluator* eval, const FunctionArguments& args )
{
  d->error = QString();
  if( !d->ptr )
  {
    setError( QString("error"), QString( QApplication::translate( "functions",
      "cannot execute function %1") ).arg( name() ) );
    return HNumber(0);
  }

  if( d->argc >= 0 )
  if( args.count() != d->argc )
  {
    if ( d->argc == 1 )
      setError( d->name, QString( QApplication::translate( "functions",
        "function accepts 1 argument" ) ) );
    else
      setError( d->name, QString( QApplication::translate( "functions",
        "function accepts %1 arguments" ) ).arg( d->argc ) );
    return HNumber(0);
  }

  return (*d->ptr)( eval, this, args );
}

FunctionRepository* FunctionRepository::s_self = 0;

FunctionRepository* FunctionRepository::self()
{
  if( !s_self )
    s_self = new FunctionRepository();
  return s_self;
}

FunctionRepository::FunctionRepository()
{
  d = new FunctionRepositoryPrivate;

  // functions are sorted by description
  add( new Function( "abs",     1, function_abs,     QT_TR_NOOP("Absolute")                            ) );
  add( new Function( "acos",    1, function_acos,    QT_TR_NOOP("Arc Cosine")                          ) );
  add( new Function( "asin",    1, function_asin,    QT_TR_NOOP("Arc Sine")                            ) );
  add( new Function( "atan",    1, function_atan,    QT_TR_NOOP("Arc Tangent")                         ) );
  add( new Function( "average",    function_average, QT_TR_NOOP("Average (Arithmetic Mean)")           ) );
  add( new Function( "log",     1, function_log,     QT_TR_NOOP("Base-10 Logarithm")                   ) );
  add( new Function( "lg",      1, function_lg,      QT_TR_NOOP("Base-2 Logarithm")                    ) );
  add( new Function( "bin",        function_bin,     QT_TR_NOOP("Binary Representation")               ) );
  add( new Function( "ncr",     2, function_nCr,     QT_TR_NOOP("Combination (Binomial Coefficient)")  ) );
  add( new Function( "ceil",    1, function_ceil,    QT_TR_NOOP("Ceiling")                             ) );
  add( new Function( "csc",     1, function_csc,     QT_TR_NOOP("Cosecant")                            ) );
  add( new Function( "cos",     1, function_cos,     QT_TR_NOOP("Cosine")                              ) );
  add( new Function( "cot",     1, function_cot,     QT_TR_NOOP("Cotangent")                           ) );
  add( new Function( "cbrt",    1, function_cbrt,    QT_TR_NOOP("Cube Root")                           ) );
  add( new Function( "dec",        function_dec,     QT_TR_NOOP("Decimal Representation")              ) );
  add( new Function( "degrees", 1, function_degrees, QT_TR_NOOP("Degrees")                             ) );
  add( new Function( "exp",     1, function_exp,     QT_TR_NOOP("Exponent")                            ) );
  add( new Function( "floor",   1, function_floor,   QT_TR_NOOP("Floor")                               ) );
  add( new Function( "frac",    1, function_frac,    QT_TR_NOOP("Fraction")                            ) );
  add( new Function( "geomean",    function_geomean, QT_TR_NOOP("Geometric Mean")                      ) );
  add( new Function( "gcd",        function_gcd,     QT_TR_NOOP("Greatest Common Divisor")             ) );
  add( new Function( "hex",        function_hex,     QT_TR_NOOP("Hexadecimal Representation")          ) );
  add( new Function( "cosh",    1, function_cosh,    QT_TR_NOOP("Hyperbolic Cosine")                   ) );
  add( new Function( "sinh",    1, function_sinh,    QT_TR_NOOP("Hyperbolic Sine")                     ) );
  add( new Function( "tanh",    1, function_tanh,    QT_TR_NOOP("Hyperbolic Tangent")                  ) );
  add( new Function( "int",     1, function_int,     QT_TR_NOOP("Integer")                             ) );
  add( new Function( "max",        function_max,     QT_TR_NOOP("Maximum")                             ) );
  add( new Function( "min",        function_min,     QT_TR_NOOP("Minimum")                             ) );
  add( new Function( "ln",      1, function_ln,      QT_TR_NOOP("Natural Logarithm")                   ) );
  add( new Function( "oct",        function_oct,     QT_TR_NOOP("Octal Representation")                ) );
  add( new Function( "npr",     2, function_nPr,     QT_TR_NOOP("Permutation (Arrangement)")           ) );
  add( new Function( "product",    function_product, QT_TR_NOOP("Product")                             ) );
  add( new Function( "radians", 1, function_radians, QT_TR_NOOP("Radians")                             ) );
  add( new Function( "round",      function_round,   QT_TR_NOOP("Rounding")                            ) );
  add( new Function( "sec",     1, function_sec,     QT_TR_NOOP("Secant")                              ) );
  add( new Function( "sign",    1, function_sign,    QT_TR_NOOP("Signum")                              ) );
  add( new Function( "sin",     1, function_sin,     QT_TR_NOOP("Sine")                                ) );
  add( new Function( "sqrt",    1, function_sqrt,    QT_TR_NOOP("Square Root")                         ) );
  add( new Function( "sum",        function_sum,     QT_TR_NOOP("Sum")                                 ) );
  add( new Function( "tan",     1, function_tan,     QT_TR_NOOP("Tangent")                             ) );
  add( new Function( "trunc",      function_trunc,   QT_TR_NOOP("Truncation")                          ) );
}

FunctionRepository::~FunctionRepository()
{
  while( d->functions.size() > 0 )
  {
    delete d->functions[ 0 ];
    d->functions.remove( 0 );
  }

  delete d;
}

void FunctionRepository::add( Function* function )
{
  if( !function ) return;
  d->functions.insert( function->name().toUpper(), function );
}

Function* FunctionRepository::function( const QString& name )
{
  return d->functions.value( name.toUpper() );
}

QStringList FunctionRepository::functionNames() const
{
  QStringList result;
  QHashIterator<QString, Function*> it( d->functions );
  while( it.hasNext() )
  {
    it.next();
    result.append( it.key().toLower() );
  }
  return result;
}
