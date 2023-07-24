#ifndef J78d5f7805f443ae8e34105a9a59d3d4
#define J78d5f7805f443ae8e34105a9a59d3d4

/*
 * can toggle individually
 *     center - scalar (or vector...?)
 *     scale  - [-1.0,1.0] or [0.0,1.0] fn (curve) (set by instr)
 *     offset - [-1.0,1.0] fn (curve)
 *
 *  if repeat is on:
 *      ロ scale length to the repeated phrase
 *
 * each param has a fixed scalar "start setting" and multiple "curve slots"
 * one curve per slot
 * each curve in the slot can be multiplicative (scaling) or additive (offset)
 * by default the curve is as long as the note, but it can be shorter or longer
 *     (in proportion to the note length or fixed), repeat with its own "repeat
 *     param", etc.(?)
 * when a curve finishes, the last value of the curve is folded into the fixed
 *     scalar and the slot's value is reset to 0; in this sense, curves are
 *     "sticky"
 *
 * a param can be a toggle, in which case (0–1] is "on" and [-1–0] is "off"

 * commmands.txt

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
 * -

 * -
 * -copy other instrument
 * :    - then overwrite its params piecemeal as desired
 * -    - every time the source instrument gets a new event, this
 * -      instrument does as well, with whatever its characteristic
 * -      modifications are
 * :    - params should/could be relative to the source instrument's
 * :      params
 * -    - maybe via shadow tracks
 * -

 * instruments.txt

 * have parameters, similar to note commands/settings
 *     parameters are determined by the synthesis method(s)

 * each parameter can have its own "fixed" curves that are behind
 *     the scenes

 * can also present "symboled" params to score
 *     from the score's perspective these are like other params
 *         (curve)
 * -    from the instr.'s perspective:
 * :        apply to any "behind the scene" param
 * -            - multiplicatively
 * -            - additively
 * -        can also swap out curves for others (esp. in terms of
 * -            toggles)
 * :            maybe even present different params w/ some toggles?
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
 */

#include "curve_slot.hpp"
#include "curve.hpp"

#include <vector>
#include <string>

class Param {
public:
    Param();

    ~Param() noexcept;

    double scalar() const { return sclr; }

    void scalar(double nsclr) { sclr = nsclr; }

    std::size_t slots_cnt() const { return slots.size(); }

private:
    std::vector<CurveSlot> slots;

private:
    std::string nme;

private:
    double sclr;
};

#endif
