#ifndef f4e33db519474903b9495a155c5e9e85
#define f4e33db519474903b9495a155c5e9e85

/*
 * one curve per slot
 * each curve in the slot can be multiplicative (scaling) or additive (offset)
 * by default the curve is as long as the note, but it can be shorter or longer
 *     (in proportion to the note length or fixed), repeat with its own "repeat
 *     param", etc.(?)
 * when a curve finishes, the last value of the curve is folded into the fixed
 *     scalar and the slot's value is reset to 0; in this sense, curves are
 *     "sticky"
 *
 * -repeat note event
 * -    - curve for repeat rate
 * :    - curves for any parameter over time from start to end of the
 * -      repeat
 * -repeat phrase start / repeat phrase end
 * -    - # of times to repeat
 * -    - curve for repeat rate
 * :    - curves for any parameter over time from start to end of the
 * -      whole repeat
 * :    - curves for any parameter over time that recur with each
 * -      repeat
 *
 * ------------------------------------------------------------------------------------------------------------
 * | param name  | curves    |                    ````````````````````| scale    | 0.7                         
 * |=============|           |                                        | offset   | -0.2                        
 * | 0.8         | sine      |                                        | op       | multiply                    
 * |-------------| square   *|                                        | length   | 1.25 * note                 
 * | 1 sine     *| saw       |                                        | repeat   | 0.25 [param ->]             
 * | 2 sine      | triangle  |                                        |                                        
 * | 3 square    |           |                                        |                                        
 * | 4 saw       |           |,,,,,,,,,,,,,,,,,,,,                    |                                        
 * ------------------------------------------------------------------------------------------------------------
 *
 * everything to the right of the "param name" widget is part of a curve slot
 *
 * length:
 *     1.2
 *     1.2 * event
 *     1.2 + event
 *     event
 *
 * op can be add, multiply, or expon (scalar**curve)
 *
 * param should be scaled only after all the curves are added up
 *
 * length/repeat isn't quite right
 *     what i really want is like
 *         * a length, which can be "indefinite"/"until changed"
 *         * a curve that moves the "read head", which itself has a length
 *         - if the length of the "read head" curve is shorter than the total
 *           length, it repeats to fill the total length
 *         - maybe the way to do varying repeat lengths is to have a repeat
 *           length param and sample it every time the current repeat finishes
 */

#include "curve.hpp"

#include <memory>

class Param;

class CurveSlot {
public:
    enum operation {
        add,
        multiply,
        expon,
    };

    enum length_style {
        fixed,
        event_mul,
        event_add,
        event_only,
        indef,
    };

    CurveSlot(std::shared_ptr<Curve> ncrve)
        : crve {ncrve}
    {}

    std::shared_ptr<Curve> curve() const { return crve; }
    std::shared_ptr<Param> repeat() const { return rpt; }
    double scale() const { return scl; }
    double offset() const { return offs; }
    double fixed_len() const { return flen; }
    enum operation oper() const { return op; }
    enum length_style length_stl() const { return len_st; }

    void curve(std::shared_ptr<Curve> ncrve) { crve = ncrve; }
    void repeat(std::shared_ptr<Param> nrpt) { rpt = nrpt; }
    void scale(double nscl) { nscl = scl; }
    void offset(double noffs) { offs = noffs; }
    void fixed_len(double nlen) { flen = nlen; }
    void oper(enum operation nop) { op = nop; }
    void length_stl(enum length_style nlen_st) { len_st = nlen_st; }

    void repeat_off() { rpt = nullptr; }

    /* advance the "read head" dt amount
     *
     * if repeat is not null, 
     */
    void incr(double dt);
    double samp() const;

private:
    std::shared_ptr<Curve> crve;

private:
    std::shared_ptr<Param> rpt = nullptr;

private:
    double scl;
    double offs;
    double len;
    double flen;
    enum operation op;
    enum length_style len_st;
};

#endif
