// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

TBitField::TBitField(int len) : BitLen(len) // = BitLen=len
{
    if(len<=0)
        throw "Incorect Bitfield lenght";
    //MemLen = (len+31) >> 5;
   
    MemLen = (len + sizeof(TELEM) * BITSINBYTE - 1) / (sizeof(TELEM) * BITSINBYTE);
    
    pMem = new TELEM[MemLen];
    if (pMem!=NULL)
    {
        for(int i = 0; i < MemLen;i++)
        {
            pMem[i]=0;
        }
       // for(int i = 0; i < len; i++)
       // {
      //      ClrBit(i);
        //}
        //cout << *this << "\n";
    }
            
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    if (pMem != NULL)
        for (int i = 0; i < MemLen; i++) pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    //return n >> 5;// вместо деления n на 32
    return n / (sizeof(TELEM) * BITSINBYTE); //деление на 64 для long long
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    //return 1 << (n & 31);    //TELEM();
    //n & 31 заменяет n % 32 и вычисляет позицию нужного нам бита в соответствующем элементе массива intov
    int mask = (sizeof(TELEM) * BITSINBYTE - 1);
    int sdvig = (n & mask);
    TELEM result = (TELEM)1 << sdvig;
    return result;
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
  return BitLen;
}

void TBitField::SetBit(const int n) // установить бит с номером n
{
    if( (n>=0) && (n<BitLen))
    {
        int i = GetMemIndex(n);
        TELEM mask = GetMemMask(n);
        TELEM newelem = pMem[i] | mask;
        pMem[GetMemIndex(n)] = newelem; // формирует битовую маску в которой позиция с номером n установлена в единицу, а остальные позиции нулевые
    }
    else throw "Incorect Bitfield Index";
}

void TBitField::ClrBit(const int n) // очистить бит с номером n
{
    if ( (n > -1) && (n < BitLen) )
        pMem[GetMemIndex(n)] = pMem[GetMemIndex(n)] & ~GetMemMask(n);// ~ выполняет инверсию нашей маски: вместо 0->1 и наоборот
    else throw "Incorect Bitfield Index";
}

TELEM TBitField::GetBit(const int n) const // получить значение бита
{
    if ( (n > -1) && (n < BitLen) )
    {
        int i = GetMemIndex(n);
        TELEM mask = GetMemMask(n);
        TELEM result = pMem[i];
        result = result & mask;
        return result;
    }
    else throw "Incorect Bitfield Index";
    return 0;
    //возвращаемый результат операции & над нужным элементом массива и битовой маской, где установлен 1 нужны бит
    //если в массиве нужный бит был нулевым -> возвращается 0(false), иначе ненулевое значение(true)
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    BitLen = bf.BitLen;
    if (MemLen != bf.MemLen)
    {
        MemLen = bf.MemLen;
        if (pMem != NULL) delete pMem;
        pMem = new TELEM[MemLen];
    }
    if (pMem != NULL)
        for(int i = 0; i < MemLen; i++) pMem[i] = bf.pMem[i];
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    int res = 1;
    if(BitLen != bf.BitLen) res = 0;
    else
        //for (int i =0; i < MemLen; i++)
            //if(pMem[i] != bf.pMem[i]) { res = 0; break; }
        for(int i = 0; i < BitLen; i++)
            if(GetBit(i)!=bf.GetBit(i)) {res = 0; break;}
  return res;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(this->operator==(bf));
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    int i, len = BitLen;
    if (bf.BitLen > len) len = bf.BitLen;
    TBitField temp(len);
    for (i=0; i < MemLen; i++) temp.pMem[i] = pMem[i];
    for (i=0; i < bf.MemLen; i++) temp.pMem[i] |= bf.pMem[i];
    return temp;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    int i, len = BitLen;
    if (bf.BitLen > len) len = bf.BitLen;
    TBitField temp(len);
    //for (i=0; i < MemLen; i++) temp.pMem[i] = pMem[i];
    for(i = 0; i < BitLen; i++) if(GetBit(i))temp.SetBit(i);
    //for (i=0; i < bf.MemLen; i++) temp.pMem[i] &= bf.pMem[i];
    for(i = 0; i < bf.BitLen; i++)
        if(temp.GetBit(i) && bf.GetBit(i)) temp.SetBit(i);
        else temp.ClrBit(i);
    for(i = bf.BitLen; i < temp.BitLen; i++)
        temp.ClrBit(i);
    return temp;
}

TBitField TBitField::operator~(void) // отрицание
{
    int len = BitLen;
    TBitField temp(len);
   // cout << temp << "\n";
    //for( int i = 0; i < MemLen; i++) temp.pMem[i] = ~pMem[i];
    for( int i =0; i < len; i++)
        if(GetBit(i) == 0)
            temp.SetBit(i);
   // cout << temp << "\n";
    return temp;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    int i = 0; char ch;
    do {istr >> ch;} while (ch != ' ');
    while (1) { istr >> ch;
        if(ch == '0')bf.ClrBit(i++);
        else if (ch == '1')bf.SetBit(i++); else break;
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    int len = bf.GetLength();
    for (int i = 0; i < len; i++)
        if(bf.GetBit(i)) ostr << 'l'; else ostr << '0';
    return ostr;
}
