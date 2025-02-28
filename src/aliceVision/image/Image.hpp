// This file is part of the AliceVision project.
// Copyright (c) 2016 AliceVision contributors.
// Copyright (c) 2012 openMVG contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <aliceVision/numeric/numeric.hpp>
#include <aliceVision/image/pixelTypes.hpp>

//---------------------------------
//  Universal Image Processing Algorithm
//-- Container for a 2D image
//-- This class ensure that the image have a width and a height
//-- and a 2D array of T data.
//-
//-- Data is saved in row major format
//-- Pixel access is done with operator(y,x)
//  [2/3/2011 pierre MOULON]
//---------------------------
namespace aliceVision
{
  namespace image
  {
    template <typename T>
    class Image : public Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
    {

    public:
      typedef T Tpixel; //-- Pixel data type
      typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Base;


      /**
       * @brief Default constructor
       * @note This create an empty image
       */
      inline Image()
      {
        Base::resize( 0, 0 );
      }

      /**
      * @brief Full constructor
      * @param width Width of the image (ie number of column)
      * @param height Height of the image (ie number of row)
      * @param fInit Tell if the image should be initialized
      * @param val If fInit is true, set all pixel to the specified value
      */
      inline Image( int width, int height, bool fInit = false, const T val = T{} )
      {
        Base::resize( height, width );
        if ( fInit )
        {
          Base::fill( val );
        }
      };

      /**
      * @brief Copy constructor
      * @param I Source image
      */
      inline Image( const Base& I )
        : Base( I )
      {

      }

      /**
      * @brief Move constructor
      * @param src Source image
      */
      inline Image( Base && src )
        : Base( std::move( src ) )
      {

      }

      /**
      * @brief Assignment operator
      * @param I Source image
      * @return Image after assignment
      */
      inline Image& operator=( const Base& I )
      {
        Base::operator=( I );
        return *this;
      }

      /**
      * @brief destructor
      */
      virtual ~Image() = default;
      //-- Image construction method
      //------------------------------


      /**
      * @brief Change geometry of image
      * @param width New width of image
      * @param height New height of image
      * @param fInit Indicate if new image should be initialized
      * @param val if fInit is true all pixel in the new image are set to this value
      */
      inline void resize( int width, int height, bool fInit = true, const T val = T{} )
      {
        Base::resize( height, width );
        if ( fInit )
        {
          Base::fill( val );
        }
      }

      //------------------------------
      //-- accessors/getters methods
      /**
       * @brief Retrieve the width of the image
       * @return Width of image
       */
      inline int Width()  const
      {
        return static_cast<int>( Base::cols() );
      }

      /**
       * @brief Retrieve the height of the image
       * @return Height of the image
       */
      inline int Height() const
      {
        return static_cast<int>( Base::rows() );
      }

      /**
      * @brief Return the depth in byte of the pixel
      * @return depth of the pixel (in byte)
      * @note (T=unsigned char will return 1)
      */
      inline int Depth() const
      {
        return sizeof( Tpixel );
      }

      /**
       * @brief Retrieve the size in byte of the image 
       * @return size of the image (in byte)
       * @note We use unsigned long long integers to avoid issues with large images, which can exceed several GB.
       */
      inline unsigned long long int MemorySize() const
      {
        return static_cast<unsigned long long int>(Width()) *
               static_cast<unsigned long long int>(Height()) *
               static_cast<unsigned long long int>(Depth());
      }


      /**
      * @brief Return the number of channels
      * @return number of channels
      */
      inline int Channels() const
      {
        return NbChannels<Tpixel>::size;
      }

      /**
      * @brief constant random pixel access
      * @param y Index of the row
      * @param x Index of the column
      * @return Constant pixel reference at position (y,x)
      */
      inline const T& operator()( int y, int x ) const
      {
        return Base::operator()( y, x );
      }

      /**
       * @brief random pixel access
       * @param y Index of the row
       * @param x Index of the column
       * @return Pixel reference at position (y,x)
       */
      inline T& operator()( int y, int x )
      {
        return Base::operator()( y, x );
      }

      inline const T& operator()( int i ) const
      {
        return Base::operator()( i );
      }
      inline T& operator()( int i )
      {
        return Base::operator()( i );
      }

      /**
      * @brief Get low level access to the internal pixel data
      * @return const reference to internal matrix data
      */
      inline const Base& GetMat() const
      {
        return ( *this );
      }
      inline Base& GetMat()
      {
          return (*this);
      }

      //-- accessors/getters methods
      //------------------------------

      /**
      * @brief Tell if a point is inside the image.
      * @param y Index of the row
      * @param x Index of the column
      * @retval true If pixel (y,x) is inside the image
      * @retval false If pixel (y,x) is outside the image
      */
      inline bool Contains( int y, int x ) const
      {
        return 0 <= x && x < Base::cols()
               && 0 <= y && y < Base::rows();
      }

      /**
      * @brief Pixelwise addition of two images
      * @param imgA First image
      * @param imgB Second image
      * @return pixelwise imgA + imgB
      * @note Images must have the same size
      */
      template< typename T1>
      friend Image<T1> operator+( const Image<T1> & imgA , const Image<T1> & imgB ) ;

      /**
      * @brief Pixelwise subtraction of two images
      * @param imgA First image
      * @param imgB Second image
      * @return pixelwise imgA - imgB
      * @note Images must have the same size
      */
      template< typename T1>
      friend Image<T1> operator-( const Image<T1> & imgA , const Image<T1> & imgB ) ;
      
      template <class UnaryFunction>
      bool perPixelOperation(UnaryFunction f)
      {
          for(auto row : this->rowwise())
          {
              std::transform(row.begin(), row.end(), row.begin(), f);
          }

          return true;
      }

    protected :
      //-- Image data are stored by inheritance of a matrix
    };

    /**
    * @brief Pixelwise addition of two images
    * @param imgA First image
    * @param imgB Second image
    * @return pixelwise imgA + imgB
    * @note Images must have the same size
    */
    template< typename T1 >
    Image<T1> operator+( const Image<T1> & imgA , const Image<T1> & imgB )
    {
      return Image<T1>( imgA.Image<T1>::operator+( imgB ) ) ;
    }

    /**
    * @brief Pixelwise subtraction of two images
    * @param imgA First image
    * @param imgB Second image
    * @return pixelwise imgA - imgB
    * @note Images must have the same size
    */
    template< typename T1>
    Image<T1> operator-( const Image<T1> & imgA , const Image<T1> & imgB )
    {
      return Image<T1>( imgA.Image<T1>::operator-( imgB ) ) ;
    }

    template< typename T >
    T getInterpolateColor(const Image<T>& img, double y, double x)
    {
        const int xp = std::min(static_cast<int>(x), img.Width() - 2);
        const int yp = std::min(static_cast<int>(y), img.Height() - 2);

        // precision to 4 decimal places
        const float ui = x - static_cast<float>(xp);
        const float vi = y - static_cast<float>(yp);

        const T lu = img(yp, xp);
        const T ru = img(yp, (xp + 1));
        const T rd = img((yp + 1), (xp + 1));
        const T ld = img((yp + 1), xp);

        // bilinear interpolation of the pixel intensity value
        const T u = lu + (ru - lu) * ui;
        const T d = ld + (rd - ld) * ui;
        const T out = u + (d - u) * vi;
        return out;
    }
  } // namespace image
} // namespace aliceVision
