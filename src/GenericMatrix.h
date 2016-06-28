//////////////////////////////////////////////////////////////////
//
//  Generic Matrix class with derived classes for 3x3, 3x1 and 1x3 classes.
//  
//////////////////////////////////////////////////////////////////

/* 
Copyright 2016 Simon Weis and Philipp Schoenhoefer

This file is part of Pomelo.

Pomelo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pomelo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pomelo.  If not, see <http://www.gnu.org/licenses/>.

The development of Pomelo took place at the Friedrich-Alexander University of Erlangen and was funded by the German Research Foundation (DFG) Forschergruppe FOR1548 "Geometry and Physics of Spatial Random Systems" (GPSRS). 
*/


#ifndef GENERIC_MATRIX_H_GUARD_123456
#define GENERIC_MATRIX_H_GUARD_123456

#include <iostream>
#include <cmath>


// Generic Matrix Class to hold all types of Types and a (by compile time) given number of Rows and Columns
template <typename T, int TRows, int TColumns>
class GenericMatrix
{
public:

   GenericMatrix()
   {
      for (int x = 1; x <= TRows; ++x )
      {
         for (int y = 1; y <= TColumns; ++y )
         {
            this->Set(x,y, 0);
         }
      }
   };

   //virtual destructor to make sure that derived classes always reach their own dtor
   virtual ~GenericMatrix()
   {};

   // Simple Getter with RangeCheck
   T Get(int x, int y ) const
   {
      if ( x >=1 && x <= TRows )
      {
         if ( y >=1 && y <= TColumns )
         {
            return m_Elements[x-1][y-1];
         }
      }
      throw (std::string("epic fail in Get"));
   };



   // Simple Setter with RangeCheck
   void Set(int x, int y, T Value )
   {
      int R = TRows;
      int C = TColumns;
      if ( x >=1 && x <= R )
      {
         if ( y >=1 && y <= C )
         {
            //std::cout<< "Set   " << x-1 << "  " << y-1 << "    " << Value <<std::endl;
            m_Elements[x-1][y-1] = Value;
            return;
         }
      }
      throw (std::string("epic fail in Set"));
   };


   inline int GetRowDim ()
   {
      return TRows;
   };

   inline int GetColumnDim ()
   {
      return TColumns;
   };

   inline bool IsSquare()
   {
      if (TRows == TColumns)
      {
         return true;
      }
      return false;
   };


   // Output Operator
   virtual std::ostream& operator << (std::ostream &stream)
   {
      for (int x = 1; x <= TRows; ++x )
      {
         for (int y = 1; y <= TColumns; ++y )
         {
            stream << this->Get(x,y) << "   ";
         }
         stream << std::endl;
      }
      return stream;
   };

   // Addition of two Matrices of same size and Type


   // Assignment Operator
   GenericMatrix operator= ( const GenericMatrix<T, TRows, TColumns>& A)
   {
      for (int x = 1; x <= TRows; ++x )
      {
         for (int y = 1; y <= TColumns; ++y )
         {
            this->Set( x,y, ( A.Get(x,y)) );
         }
      }
      return *this;
   };


    // Method to get a specific Row out of the Matrix
   GenericMatrix<T,1,TColumns> GetRowNr ( const unsigned int& i )
   {
       GenericMatrix<T,1,TColumns> M;
        for (int y = 1; y <= TColumns; ++y )
         {
            M.Set( 1,y, this->Get(i,y));
         }
       return M;
   };

    // Method to get a specific Column out of the Matrix
    GenericMatrix<T,TRows,1> GetColumnNr (const unsigned int& i )
   {
       GenericMatrix<T,TRows,1> M;
        for (int x = 1; x <= TRows; ++x )
         {
            M.Set( x,1, this->Get(x,i));
         }
       return M;
   };

   T Trace ()
   {
       T t_ret = 0;
       int R = TRows;
       int C = TColumns;


       int MaxV = R;
       if (R>C)
       {
           MaxV = C;
       }
        for (int i = 1; i <= MaxV; ++i )
         {
            t_ret += this->Get(i,i);
         }
         return t_ret;
   };

   GenericMatrix<T, TColumns, TRows> Transpose ()
   {
       GenericMatrix<T, TColumns, TRows> t_ret;

       for (int x = 1; x <= TRows; ++x )
      {
         for (int y = 1; y <= TColumns; ++y )
         {
            t_ret.Set( y,x, ( this->Get(x,y) ) );

         }
      }
      return t_ret;
   };


private:
   // And this is the Magic. As simple (and fast) as it could be.
      T m_Elements[TRows][TColumns];
};



// Multiplication T * GenericMatrix
template <typename T, int TRows, int TColumns>
GenericMatrix<T,TRows,TColumns> operator*(const T &b, const GenericMatrix<T,TRows,TColumns>& M )
{
   GenericMatrix<T,TRows, TColumns> t_ret;
   for (int x = 1; x <= TRows; ++x )
   {
      for (int y = 1; y <= TColumns; ++y )
      {
         t_ret.Set(x,y, (b *  M.Get(x,y) ) );
      }
   }
  return t_ret;
};


// Multiplication GenericMatrix * T (including implicit casts)
template <typename T, int TRows, int TColumns>
GenericMatrix<T,TRows,TColumns> operator*(const GenericMatrix<T,TRows,TColumns>& M, const T &b)
{
   GenericMatrix<T,TRows, TColumns> t_ret;
   for (int x = 1; x <= TRows; ++x )
   {
      for (int y = 1; y <= TColumns; ++y )
      {
         t_ret.Set(x,y, (b *  M.Get(x,y) ) );
      }
   }
  return t_ret;
};



// define matrix product
template<typename T, int TR1, int TC1,int TC2>
GenericMatrix<T, TR1, TC2> operator* ( const GenericMatrix<T,TR1,TC1>& A, const GenericMatrix<T,TC1,TC2>& B )
{
    GenericMatrix<T, TR1, TC2> M;
    for ( int i = 1; i <=TR1; ++i)
    {
        for ( int k = 1; k <=TC2; ++k)
        {
            T V = 0;
            for ( int j = 1; j <= TC1; ++j )
            {
                //std::cout << i << "    " << j << "  " << k << std::endl;
                T t_TAdd = A.Get(i,j) * B.Get(j,k);
                //std::cout << t_TAdd << std::endl;
                V += t_TAdd;
            };

            M.Set(i,k, V);
        }
    }
    return M;
};

// vector-vector product (i.e. inner product)
template<typename T, int TC1>
T operator* ( const GenericMatrix<T,1,TC1>& A, const GenericMatrix<T,TC1,1>& B )
{
    T V = 0;
    for ( int j = 1; j <= TC1; ++j )
    {
        V += A.Get(1,j) * B.Get(j,1);
    }

    return V;
}

template <typename T, int TRows, int TColumns>
 GenericMatrix<T, TRows, TColumns> operator+ (const GenericMatrix<T, TRows, TColumns>& A, const GenericMatrix<T, TRows, TColumns>& B )
   {
      GenericMatrix<T, TRows, TColumns> t_ret;

      for (int x = 1; x <= TRows; ++x )
      {
         for (int y = 1; y <= TColumns; ++y )
         {
            t_ret.Set(x,y, (A.Get(x,y) + B.Get(x,y)));
         }
      }
      return t_ret;
   };


template <typename T, int TRows, int TColumns>
 GenericMatrix<T, TRows, TColumns> operator- (const GenericMatrix < T, TRows, TColumns>& A, const GenericMatrix<T, TRows, TColumns>& B )
{
   GenericMatrix<T, TRows, TColumns> t_ret;

   for (int x = 1; x <= TRows; ++x )
   {
      for (int y = 1; y <= TColumns; ++y )
      {
         t_ret.Set( x,y, ( A.Get(x,y) - B.Get(x,y)) );
      }
   }
   return t_ret;
};



/// Template Specification zVec
class zvec : public GenericMatrix<double, 1, 3>
{
   public:
   zvec ()
   {
      Set(1,1,0);
      Set(1,2,0);
      Set(1,3,0);
   };
   zvec (double x, double y, double z)
   {
      Set(1,1,x);
      Set(1,2,y);
      Set(1,3,z);
   };

   zvec ( GenericMatrix<double, 1, 3> M )
   {
      Set(1,1, M.Get(1,1) );
      Set (1,2 , M.Get(1,2) );
      Set (1,3 , M.Get(1,3) );
   };

   double x() const {return Get(1,1);};
   double y() const {return Get(1,2);};
   double z() const {return Get(1,3);};
   
   double abs() const
   {
      return sqrt( x()*x() + y()*y() + z()*z() );
   }


};

///Template Specification sVec
class svec : public GenericMatrix<double, 3, 1>
{  public:

   svec ()
   {
      Set(1,1,0);
      Set(2,1,0);
      Set(3,1,0);
   };
   svec ( double x, double y, double z )
   {
      Set(1,1, x);
      Set(2,1, y);
      Set(3,1, z);
   };

   svec ( GenericMatrix<double, 3, 1> M )
   {
      Set (1,1, M.Get(1,1) );
      Set (2,1, M.Get(2,1) );
      Set (3,1, M.Get(3,1) );
   };


   double x() const {return Get(1,1);};
   double y() const {return Get(2,1);};
   double z() const {return Get(3,1);};
   double abs() const
   {
      return sqrt( x()*x() + y()*y() + z()*z() );
   }
};


///Template Specification matrix
class matrix : public GenericMatrix<double, 3, 3>
{
   public:
   matrix()
   {
      Set(1,1, 0);
      Set(1,2, 0);
      Set(1,3, 0);
      Set(2,1, 0);
      Set(2,2, 0);
      Set(2,3, 0);
      Set(3,1, 0);
      Set(3,2, 0);
      Set(3,3, 0);
   };

   matrix( double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3 )
   {
      Set(1,1, x1);
      Set(1,2, x2);
      Set(1,3, x3);
      Set(2,1, y1);
      Set(2,2, y2);
      Set(2,3, y3);
      Set(3,1, z1);
      Set(3,2, z2);
      Set(3,3, z3);
   };

   //specified ctor to implicitly convert GenericMatrices<double,2,2> to matrices (!not the other way round!)
   matrix ( GenericMatrix<double,3,3> M )
   {
      Set(1,1, M.Get(1,1));
      Set(1,2, M.Get(1,2));
      Set(1,3, M.Get(1,3));
      Set(2,1, M.Get(2,1));
      Set(2,2, M.Get(2,2));
      Set(2,3, M.Get(2,3));
      Set(3,1, M.Get(3,1));
      Set(3,2, M.Get(3,2));
      Set(3,3, M.Get(3,3));
   };
};


////////////////////////////////////////////////////////////////////////////
// Operator Definitions for z,s and matrix
// copied mostly from GST code
////////////////////////////////////////////////////////////////////////////



#endif // MATRIX_H_INCLUDED
