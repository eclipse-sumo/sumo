/************************************************************************
 *                                                                      *
 *       Copyright 2004, Brown University, Providence, RI               *
 *                                                                      *
 *  Permission to use and modify this software and its documentation    *
 *  for any purpose other than its incorporation into a commercial      *
 *  product is hereby granted without fee. Recipient agrees not to      *
 *  re-distribute this software or any modifications of this            *
 *  software without the permission of Brown University. Brown          *
 *  University makes no representations or warrantees about the         *
 *  suitability of this software for any purpose.  It is provided       *
 *  "as is" without express or implied warranty. Brown University       *
 *  requests notification of any modifications to this software or      *
 *  its documentation. Notice should be sent to:                        *
 *                                                                      *
 *  To:                                                                 *
 *        Software Librarian                                            *
 *        Laboratory for Engineering Man/Machine Systems,               *
 *        Division of Engineering, Box D,                               *
 *        Brown University                                              *
 *        Providence, RI 02912                                          *
 *        Software_Librarian@lems.brown.edu                             *
 *                                                                      *
 *  We will acknowledge all electronic notifications.                   *
 *                                                                      *
 ************************************************************************/

#ifndef  POINTS_H
#define  POINTS_H

//#include <vcl_iostream.h>

template <class coord_type>
class Point2D
  {
    protected: 
        coord_type _x, _y;
    public:
        Point2D()
          {
            _x=_y=-1;
           }

                inline Point2D(coord_type x0, coord_type y0)
                  {
                    _x  = x0;
                    _y  = y0;
                   }

        template <class point_type>
                inline Point2D<coord_type>(const Point2D<point_type> &old)
                  {
                    _x  = old.getX();
                    _y  = old.getY();
                   }

        template <class point_type>
               inline Point2D<coord_type>& operator=(const Point2D<point_type> &old)
                  {
                    _x  = old.getX();
                    _y  = old.getY();

                    return *this;
                   }

        //TODO : The following function is a "specialization" of the previous
        //function. Basically this is supposed to kick in when the two points
        //have the same type. This is here to improve speed, vcl_since
        //the values of x and y can be accessed directly without the need 
        //for the get. functions. 
        //
        //Check if this thing works.

        inline Point2D<coord_type>(const Point2D<coord_type> &old)
          {
            _x  = old._x;
            _y  = old._y;
           }
        
        inline Point2D<coord_type>& operator=(const Point2D<coord_type> &old)
          {
            _x = old._x;
            _y = old._y;
            return *this;
           }
        
        ~Point2D()
          {
           }

        coord_type getX() const { return _x; };
        coord_type getY() const { return _y; };
        coord_type x() const    { return _x; };
        coord_type y() const    { return _y; };
        
        void setX(coord_type nx) { _x = nx; }
        void setY(coord_type ny) { _y = ny; }
        void set(coord_type nx, coord_type ny) { setX(nx); setY(ny); }
        
        coord_type operator[](int i) const {
          switch(i) {
            case 0: return _x;
            case 1: return _y;
            case 2: return 1;
            default: assert(NULL == "Point2D::operator[] index out of range");
                     return 0;
          }
        }

        template <class point_type>
                bool operator==(const Point2D<point_type> &old) const
                  {
                    if ((_x == old._x)&&(_y==old._y))
                        return true;
                    else
                        return false;
                   }

        template <class point_type>
                bool operator!=(const Point2D<point_type> &old) const
                  {
                    if ((_x == old._x)&&(_y==old._y))
                        return false;
                    else
                        return true;
                   }

        
        template <class point_type>
                Point2D<coord_type>& operator+=(const Point2D<point_type> &old)
                  {
                    _x += old._x;
                    _y += old._y;

                    return *this;
                   }

        template <class point_type>
                Point2D<coord_type>& operator-=(const Point2D<point_type> &old)
                  {
                    _x -= old._x;
                    _y -= old._y;

                    return *this;
                   }

        /**
         * NOTE: The angle is assumed to be in RADIANS.
         * */
        void rotate(double angle)
          {
            coord_type rot_x=0;
            coord_type rot_y=0;

            rot_x  = ( _x*vcl_cos(angle)+_y*vcl_sin(angle));
            rot_y  = (-_x*vcl_sin(angle)+_y*vcl_cos(angle));

            _x = rot_x;
            _y = rot_y;
           }

        void swap()
          {
            std::swap(_x,_y);
           }

        double magnitude() const
          {
            return (vcl_sqrt(_x*_x+_y*_y));
           }
   };


/*--------------------- Multiplication --------------------------------*/

    template <class mul_type, class point_type> 
inline Point2D<point_type> operator*(mul_type val, const Point2D<point_type> pt)
  {
    return Point2D<point_type>(val*pt.getX(), val*pt.getY());
   }


    template <class point_type, class mul_type> 
inline Point2D<point_type> operator*(const Point2D<point_type> pt, mul_type val)
  {
    return val*pt;
   }




/*--------------------- Division --------------------------------*/

    template <class point_type, class div_type>
inline Point2D<point_type> operator/(const Point2D<point_type> pt, div_type val)
  {
    if (val ==0)
        vcl_cout<<" Error: <Point2D operator/> Division by 0"<<vcl_endl;
    return Point2D<point_type>(pt.getX()/val, pt.getY()/val);
   }



/*-------------------------------------------------------------*/

/* 
 * Note: Return type is same as the first point. Not the smartest thing to do.
 * But no other way out. Can try typelists.
 * */

    template <class point1_type, class point2_type>
inline Point2D<double> operator+(const Point2D<point1_type> &pt1, const Point2D<point2_type> &pt2)
  {
    return Point2D<double>(pt1.getX()+pt2.getX(), pt1.getY()+pt2.getY());
   }

/*
template<class coord_type>
inline Point2D<coord_type> operator+(const Point2D<coord_type> &pt1,const Point2D<coord_type> &pt2)
  {
    return Point2D<coord_type>(pt1.getX()+pt2.getX(), pt1.getY()+pt2.getY());
   }
*/
    template <class point1_type, class point2_type>
inline Point2D<point1_type> operator-(Point2D<point1_type> pt1, Point2D<point2_type> pt2)
  {
    return Point2D<point1_type>(pt1.getX()-pt2.getX(), pt1.getY()-pt2.getY());
   }
    
/*------------------------------------------------------------*/
    /*
    template <class point_type>    
inline vcl_ostream & operator<< (vcl_ostream & os, const Point2D<point_type> pt)
  {
    os<<" ["<<pt.getX()<<", "<<pt.getY()<<"] ";
    return os;
   }
*/
/*------------------------------------------------------------*/

    template <class point_type1, class point_type2>
double euc_distance(Point2D<point_type1> pt1, Point2D<point_type2> pt2)
  {
    double x_dist, y_dist,dist=0;

    x_dist = (pt1.getX()-pt2.getX())*(pt1.getX()-pt2.getX());
    y_dist = (pt1.getY()-pt2.getY())*(pt1.getY()-pt2.getY());

    dist = sqrt(x_dist+y_dist);
    return dist;
   }

#endif /*POINTS_H*/
