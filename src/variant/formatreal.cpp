/* formatreal.cpp: Formatting real numbers */
/*
    Copyright (C) 2008 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#include "variant/formatreal.hxx"

static bool _isZero(const QString& str)
{
  return str.size() == 1 && str.at(0) == '0';
}

static char _digitsz(char base)
{
  char result = 1;
  switch (base)
  {
    case 16: ++result;
    case 8:  result+=2;
    default: ;
  }
  return result;
}

const char* fmtLongReal = "longreal";

QStringList* RealFormat::propList;

static const char* fmtRealProps[] =
{
  "formatMode",
  "base",
  "significandBase",
  "scaleBase",
  "precision",
//  "dot",
  0
};

void RealFormat::initClass()
{
  FormatIntf::initClass(&propList, &fmtRealProps[0]);
}

RealFormat::RealFormat()
{
  setMode(Scientific);
}

bool RealFormat::isValidBase(char base)
{
  switch (base)
  {
    case 2:
    case 8:
    case 10:
    case 16: return true;
    default: return false;
  }
}

bool RealFormat::canHandle(VariantType vt) const
{
  return vt == VariantIntf::variantType(VariantIntf::nLongReal);
}

bool RealFormat::setValue(int idx, const Variant& val)
{
  bool result;
  int tmp;
  switch (idx)
  {
    case 0:
      result = setInt(tmp, val) && setMode((FmtMode)tmp);
      break;
    case 1:
      result = setInt(tmp, val) && setBase(tmp);
      break;
    case 2:
      result = setInt(tmp, val) && setSignificandBase(tmp);
      break;
    case 3:
      result = setInt(tmp, val) && setScaleBase(tmp);
      break;
    case 4:
      result = setInt(tmp, val) && setPrecision(tmp);
      break;
//      result = setChar(dot, val); break;
    default:
      result = false;
  }
  return result;
}

Variant RealFormat::getValue(int idx) const
{
  Variant result;
  switch (idx)
  {
    case 0: getChar(result, dot); break;
    case 1: getInt(result, (int)mode); break;
    default: ;
  }
  return result;
}

bool RealFormat::setMode(FmtMode fm)
{
  if (fm > FmtLastMode)
    return false;
  mode = fm;
  return true;
}

bool RealFormat::setBase(char aBase)
{
  if (!isValidBase(aBase))
    return false;
  base = aBase;
  return true;
}

char RealFormat::getBase() const
{
  char result = base;
  switch (getMode())
  {
    case Engineering:
      result = 10; break;
    case Complement2:
      if (result == 10) result = 16; break;
    default: ;
  }
  return result;
}

bool RealFormat::setSignificandBase(char aBase)
{
  if (!isValidBase(aBase))
    return false;
  significandbase = aBase;
  return true;
}

char RealFormat::getSignificandBase() const
{
  char result = significandbase;
  if (getBase() == 10)
    result = 10;
  else
    switch (getMode())
    {
      case Scientific:
      case IEEE754: break;
      default: result = getBase();
    }
  return result;
}

bool RealFormat::setScaleBase(char aBase)
{
  if (!isValidBase(aBase))
    return false;
  scalebase = aBase;
  return true;
}

char RealFormat::getScaleBase() const
{
  char result = scalebase;
  switch (getMode())
  {
    case FixSize:
    case FixPoint:
    case Complement2: result = 0; break;
    default: ;
  }
  return result;
}

bool RealFormat::setPrecision(int prec)
{
  if (prec < 0 && prec > BINPRECISION)
    return false;
  precision = prec;
  return true;
}

bool RealFormat::setDigits(int digits)
{
  if (digits < 0 || digits > BINPRECISION)
    return false;
  int prec;
  switch (base)
  {
    case 10:
      prec = 2136 * digits / 643; break;
    default:
      prec = digits * _digitsz(significandbase); break;
  }
  switch (getMode())
  {
    case Complement2:
      if (digits != 0) return false; break;
    case Scientific:
    case Engineering:
    case IEEE754:
      ++prec; break;
    default: ;
  }
  return setPrecision(prec);
}

int RealFormat::getDigits() const
{
  int prec = getPrecision();
  if (prec == 0)
    return 0;
  switch (getMode())
  {
    case Complement2: return 0;
    case Scientific:
    case Engineering:
    case IEEE754: --prec; break;
    default: ;
  }
  switch (base)
  {
    case 10:
      return 643 * precision / 2136 + 1;
    default:
      return (prec - 1) / _digitsz(base) + 1;
  }
}

QString RealFormat::getPrefix(Sign sign, char base,
                              bool isCompl) const
{
  QString result;
  const char* radix;
  switch (sign)
  {
    case None:
    case Plus:
      if (showPlus)
        result = '+';
      break;
    default:
      result = '-';
  }
  if (showRadix)
  {
    switch (base)
    {
      case 16: radix = "0x"; break;
      case  8: radix = "0o"; break;
      case  2: radix = "0b"; break;
      default: radix = "0d";
    }
    if (isCompl)
      result = radix + result;
    else
      result += radix;
  }
  if (lowerCaseHexDigit)
    result = result.toUpper();
  return result;
}

QString RealFormat::getSignificandPrefix(const RawFloatIO& io) const
{
  return getPrefix(io.signSignificand, significandbase,
                   mode == Complement2);
}

QString RealFormat::getSignificandSuffix(const RawFloatIO& io) const
{
  return QString();
}

QString RealFormat::getScalePrefix(const RawFloatIO& io) const
{
  QString result;
  QChar sc = getScaleChar();
  bool useParen = mode == IEEE754 || sc == '(';
  if (useShortScale())
    result = sc;
  else
  {
    result.setNum(base);
    result = 'x' + result + sc;
  }
  if (useParen)
    result += '(';
  return result
      + getPrefix(io.signScale, scalebase, false);
}

QString RealFormat::getScaleSuffix(const RawFloatIO& io) const
{
  if (mode == IEEE754)
    return "+bias)";
  if (useShortScale() && getScaleChar() != '(')
    return QString();
  return ")";
}

QString RealFormat::formatNaN() const
{
  return "NaN";
}

QString RealFormat::formatZero() const
{
  const char* result;
  if (showTrailingDot)
    result = "0.";
  else
    result = "0";
  return result;
}

QString RealFormat::formatInt(const RawFloatIO& io) const
{
  if (minIntLg <= 0 && !showLeadingZero && _isZero(io.intpart))
    return QString();
  QString intpart = intPart(io);
  if (minIntLg < intpart.size())
  {
    QChar pad = ' ';
    if (showLeadingZero)
      pad = '0';
    if (io.signSignificand == Compl2)
      switch (io.baseSignificand)
    {
      case 2 : pad = '1'; break;
      case 8 : pad = '7'; break;
      case 16: pad = 'F'; break;
    }
    intpart = QString(minIntLg - intpart.size(), pad) + intpart;
  }
  if (lowerCaseHexDigit)
    intpart = intpart.toLower();
  if (!useGrouping() || grouplg >= intpart.size())
    return intpart;
  QString result;
  int idx = intpart.size();
  while (idx > 0)
  {
    idx -= grouplg;
    QChar gchar = ' ';
    if (intpart.at(idx) != ' ')
      gchar = groupchar;
    if (idx <= 0)
      result = intpart.mid(0, idx + grouplg) + result;
    else
      result = intpart.mid(idx, grouplg) + gchar + result;
  }
  return result;
}

QString RealFormat::formatFrac(const RawFloatIO& io) const
{
  QString fracpart = fracPart(io);
  if (fracpart.size() < minFracLg)
  {
    QChar pad = ' ';
    if (showTrailingZero)
      pad = '0';
    fracpart += QString(minFracLg - fracpart.size());
  }
  else if (!showTrailingZero)
  {
    int i = fracpart.size();
    while (--i >= minFracLg && fracpart.at(i) == '0');
    fracpart = fracpart.mid(0, i);
  }
  if (fracpart.isEmpty())
  {
    if (showTrailingDot)
      return dot;
    else
      return QString();
  }
  if (lowerCaseHexDigit)
    fracpart = fracpart.toLower();
  if (!useGrouping() || fracpart.size() <= grouplg)
    return dot + fracpart;
  QString result = dot;
  result += fracpart.mid(0, grouplg);
  for (int idx = grouplg; idx < fracpart.size(); idx += grouplg)
  {
    QChar gchar = ' ';
    if (fracpart.at(idx) != ' ')
      gchar = groupchar;
    result += gchar + fracpart.mid(idx, grouplg);
  }
  return result;
}

QString RealFormat::formatScale(const RawFloatIO& io) const
{
  QString result = QString::number(io.scale, (int)scalebase);
  if (scalebase == 16 && !lowerCaseHexDigit)
    result = result.toUpper();
  if (result.size() < minScaleLg)
    result = QString(result.size() - minScaleLg,
                     showScaleLeadingZero? '0' : ' ') + result;
  return result;
}

static QString changeBase(int idigitsz, int odigitsz, int osz,
                          QString input, int bufofs, bool lc)
{
  QString result(osz, 0);
  char buf = 0;
  char mask = (1 << odigitsz) - 1;
  char hexofs = lc? 'a' - 10 : 'A' - 10;
  char c;
  for (int i = -1; ++i < osz;)
  {
    while (bufofs < odigitsz)
    {
      c = input.at(i).toAscii() - '0';
      if (c > '9')
        c = (c - ('A' - '0' - 10)) & 0xF;
      buf = (buf << idigitsz) + c;
      bufofs += idigitsz;
    }
    bufofs -= odigitsz;
    char c = (buf >> bufofs) & mask;
    result[i] = c <= 9? c + '0' : c + hexofs;
  }
  return result;
}

QString RealFormat::intPart(const RawFloatIO& io) const
{
  if (significandbase == base)
    return io.intpart;
  int idigitsz = _digitsz(base);
  int ibitlg = (io.intpart.size() << idigitsz);
  // reduce length by count of leading zero bits in the first digit
  const char* p = "1248";
  QChar c = io.intpart.at(0);
  while (*p)
    if (c < *(p++))
      --ibitlg;
  if (ibitlg == 0)
    return "0";
  int odigitsz = _digitsz(significandbase);
  int osz = ((ibitlg - 1) >> odigitsz) + 1;
  return changeBase(idigitsz, odigitsz, osz, io.intpart,
                    (osz << odigitsz) - ibitlg, lowerCaseHexDigit);
}

QString RealFormat::fracPart(const RawFloatIO& io) const
{
  if (significandbase == base || precision == 0)
    return io.fracpart;
  return changeBase(_digitsz(base), _digitsz(significandbase), getDigits(),
                    io.fracpart, 0, lowerCaseHexDigit);
}

QString RealFormat::format(const Variant& val)
{
  if (usesBase())
  {
    RealFormat* formatter = new RealFormat;
    formatter->cloneFrom(this);
    QString result = formatter->format(val);
    delete formatter;
    return result;
  }
  return doFormat(val);
}

QString RealFormat::doFormat(const Variant& val) const
{
  QString result;
  const LongReal* vr = dynamic_cast<const LongReal*>((const VariantData*)val);
  if (!vr)
    return result;
  if (vr->isNaN())
    return formatNaN();
  if (vr->isZero())
    return formatZero();
  int usedigits = significandbase == base || precision == 0?
                  getDigits() :
                  (precision - 1) / _digitsz(base) + 1;
  RawFloatIO RawFloatIO = vr->convert(usedigits, mode, base);
  if (RawFloatIO.error != Success)
    return result;
  result = getSignificandPrefix(RawFloatIO)
      + formatInt(RawFloatIO)
      + formatFrac(RawFloatIO)
      + getSignificandSuffix(RawFloatIO);
  if (useScale(RawFloatIO))
    result += getScalePrefix(RawFloatIO)
        + formatScale(RawFloatIO)
        + getScaleSuffix(RawFloatIO);
  return result;
}

bool RealFormat::useScale(const RawFloatIO& io) const
{
  return mode == IEEE754 || io.scale != 0 || showZeroScale;
}

bool RealFormat::useShortScale() const
{
  return shortScale && significandbase == base;
}

QChar RealFormat::getScaleChar() const
{
  if (scalechar == ' ')
  {
    if (lowerCaseHexDigit)
      return significandbase == 10? 'E' : 'P';
    else
      return significandbase == 10? 'e' : 'p';
  }
  return scalechar;
}
