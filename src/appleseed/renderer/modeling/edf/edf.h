
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014-2016 Francois Beaune, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef APPLESEED_RENDERER_MODELING_EDF_EDF_H
#define APPLESEED_RENDERER_MODELING_EDF_EDF_H

// appleseed.renderer headers.
#include "renderer/global/globaltypes.h"
#include "renderer/modeling/entity/connectableentity.h"

// appleseed.foundation headers.
#include "foundation/math/basis.h"
#include "foundation/math/vector.h"
#include "foundation/utility/uid.h"

// appleseed.main headers.
#include "main/dllsymbol.h"

// Forward declarations.
namespace foundation    { class IAbortSwitch; }
namespace renderer      { class Assembly; }
namespace renderer      { class InputEvaluator; }
namespace renderer      { class ParamArray; }
namespace renderer      { class Project; }
namespace renderer      { class ShadingPoint; }

namespace renderer
{

//
// Emittance Distribution Function (EDF).
//
// Conventions:
//
//   * All direction vectors are expressed in world space.
//
//   * All direction vectors are unit-length and pointing outward.
//
//   * All probability densities are measured with respect to solid angle.
//

class APPLESEED_DLLSYMBOL EDF
  : public ConnectableEntity
{
  public:
    // Return the unique ID of this class of entities.
    static foundation::UniqueID get_class_uid();

    // Constructor.
    EDF(
        const char*                 name,
        const ParamArray&           params);

    // Return a string identifying the model of this entity.
    virtual const char* get_model() const = 0;

    enum Flags
    {
        CastIndirectLight = 1 << 0                              // does this light generate indirect lighting?
    };

    // Retrieve the flags.
    int get_flags() const;

    // Retrieve the importance multiplier.
    double get_uncached_importance_multiplier() const;

    // Get the cached light near start value.
    double get_light_near_start() const;

    // Retrieve the light near start value.
    double get_uncached_light_near_start() const;

    // This method is called once before rendering each frame.
    // Returns true on success, false otherwise.
    virtual bool on_frame_begin(
        const Project&              project,
        const Assembly&             assembly,
        foundation::IAbortSwitch*   abort_switch = 0);

    // This method is called once after rendering each frame.
    virtual void on_frame_end(
        const Project&              project,
        const Assembly&             assembly);

    // Evaluate the inputs of this EDF.
    // Input values are stored in the input evaluator.
    virtual void evaluate_inputs(
        InputEvaluator&             input_evaluator,
        const ShadingPoint&         shading_point) const;       // shading point on the light source

    // Sample the EDF and compute the emission direction, its probability
    // density and the value of the EDF for this direction.
    virtual void sample(
        SamplingContext&            sampling_context,
        const void*                 data,                       // input values
        const foundation::Vector3d& geometric_normal,           // world space geometric normal, unit-length
        const foundation::Basis3d&  shading_basis,              // world space orthonormal basis around shading normal
        const foundation::Vector2d& s,                          // sample in [0,1)^2
        foundation::Vector3d&       outgoing,                   // world space emission direction, unit-length
        Spectrum&                   value,                      // EDF value for this direction
        double&                     probability) const = 0;     // PDF value

    // Evaluate the EDF for a given emission direction.
    virtual void evaluate(
        const void*                 data,                       // input values
        const foundation::Vector3d& geometric_normal,           // world space geometric normal, unit-length
        const foundation::Basis3d&  shading_basis,              // world space orthonormal basis around shading normal
        const foundation::Vector3d& outgoing,                   // world space emission direction, unit-length
        Spectrum&                   value) const = 0;           // EDF value for this direction
    virtual void evaluate(
        const void*                 data,                       // input values
        const foundation::Vector3d& geometric_normal,           // world space geometric normal, unit-length
        const foundation::Basis3d&  shading_basis,              // world space orthonormal basis around shading normal
        const foundation::Vector3d& outgoing,                   // world space emission direction, unit-length
        Spectrum&                   value,                      // EDF value for this direction
        double&                     probability) const = 0;     // PDF value

    // Evaluate the PDF for a given emission direction.
    virtual double evaluate_pdf(
        const void*                 data,                       // input values
        const foundation::Vector3d& geometric_normal,           // world space geometric normal, unit-length
        const foundation::Basis3d&  shading_basis,              // world space orthonormal basis around shading normal
        const foundation::Vector3d& outgoing) const = 0;        // world space emission direction, unit-length

  private:
    int    m_flags;
    double m_light_near_start;
};


//
// EDF class implementation.
//

inline int EDF::get_flags() const
{
    return m_flags;
}

inline double EDF::get_light_near_start() const
{
    return m_light_near_start;
}

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_MODELING_EDF_EDF_H
