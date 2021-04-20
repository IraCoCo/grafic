#pragma once
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

/**
 * Threshold for zero.
 */
#define EPSILON 1.0e-5
 /**
  * Test if real value is zero.
  */
#define IS_ZERO(v) (abs(v) < EPSILON)
  /**
   * Signum function.
   */
#define SIGN(v) (int)(((v) > EPSILON) - ((v) < -EPSILON))
   /**
	* Amount of lines representing each Bezier segment.
	*/
#define RESOLUTION 32
	/**
	 * Paramenet affecting curvature, should be in [2; +inf).
	 */
#define C 2.0

class Point2D
{
public:
    /**
     * Point coordinates.
     */
    double x, y;

    /**
     * Point2D constructor.
     */
    Point2D();
    /**
     * Point2D constructor.
     *
     * @param x - x coordinate of the point.
     * @param y - y coordinate of the point.
     */
    Point2D(double _x, double _y);

    /**
     * Add other point to the current one.
     *
     * @param p - point to add.
     * @return summ of the current point and the given one.
     */
    Point2D operator +(const Point2D& p) const;
    /**
     * Subtract other point from the current one.
     *
     * @param p - point to subtract.
     * @return difference of the current point and the given one.
     */
    Point2D operator -(const Point2D& p) const;
    /**
     * Multiply current point by the real value.
     *
     * @param v - value to multiply by.
     * @return current point multiplied by the given value.
     */
    Point2D operator *(double v) const;

    /**
     * Safely normalize current point.
     */
    void normalize();

    /**
     * Get the absolute minimum of two given points.
     *
     * @param p1 - first point.
     * @param p2 - second point.
     * @return absolute minimum of the given points' coordinates.
     */
    static Point2D absMin(const Point2D& p1, const Point2D& p2)
    {
        return Point2D(abs(p1.x) < abs(p2.x) ? p1.x : p2.x, abs(p1.y) < abs(p2.y) ? p1.y : p2.y);
    };
};

/**
 * The Segment class provides methods to store and calculate Bezier-based cubic curve segment.
 */
class Segment
{
public:
    /**
     * Bezier control points.
     */
    Point2D points[4];

    /**
     * Calculate the intermediate curve points.
     *
     * @param t - parameter of the curve, should be in [0; 1].
     * @return intermediate Bezier curve point that corresponds the given parameter.
     */
    Point2D calc(double t) const;
};

/**
 * Build an interpolation curve with smoothness order 1 based on cubic Bezier according to given point set.
 *
 * @param values - input array of points to interpolate.
 * @param curve - output array of curve segments.
 * @return true if interpolation successful, false if not.
 */
bool tbezierSO1(const vector<Point2D>& values, vector<Segment>& curve);

/**
 * Build an interpolation curve with smoothness order 0 based on cubic Bezier according to given point set.
 *
 * @param values - input array of points to interpolate.
 * @param curve - output array of curve segments.
 * @return true if interpolation successful, false if not.
 */
bool tbezierSO0(const vector<Point2D>& values, vector<Segment>& curve);