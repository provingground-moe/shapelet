// -*- LSST-C++ -*-
/*
 * LSST Data Management System
 * Copyright 2008-2013 LSST Corporation.
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */

#ifndef LSST_AFW_MATH_SHAPELETS_ModelBuilder_h_INCLUDED
#define LSST_AFW_MATH_SHAPELETS_ModelBuilder_h_INCLUDED

#include "ndarray/eigen.h"

#include "lsst/shapelet/BasisEvaluator.h"
#include "lsst/afw/detection/Footprint.h"
#include "lsst/afw/geom/ellipses.h"

namespace lsst { namespace shapelet {

/**
 *  @brief A class that evaluates a Gauss-Hermite (shapelet with HERMITE basis type) basis over
 *         multiple pixels in a flattened array.
 *
 *  Unlike virtually everything else in the shapelets library, ModelBuilder does not rely on the
 *  GaussHermiteEvaluator class to compute basis functions.  Instead of making the iteration over the
 *  pixels the outer loop, it uses Eigen array objects that are the size of an entire image.  This
 *  uses more memory for temporaries, but it takes advantage of Eigen's vectorized arithmetic operators.
 *
 *  The ellipse-handling and coefficient conventions of ModelBuilder are the same as those of
 *  ShapeletFunction and MultiShapeletFunction; the basis functions are divided by a*b, and hence
 *  coefficients have units of flux, not surface brightness.
 */
template <typename T>
class ModelBuilder {
public:

    /**
     *  @brief Construct a ModelBuilder that can be used to fit data from an Image.
     *
     *  @param[in] x           Array of center-subtracted X coordinates (same shape as y).
     *  @param[in] y           Array of center-subtracted Y coordinates (same shape as x).
     *  @param[in] useApproximateExp  Use utils::PowFast to compute approximate fast exponentials.
     */
    ModelBuilder(
        ndarray::Array<T const,1,1> const & x,
        ndarray::Array<T const,1,1> const & y,
        bool useAppoximateExp = false
    );

    /**
     *  @brief Update the basis ellipse and recompute the model matrix.
     */
    void update(afw::geom::ellipses::BaseCore const & ellipse);

    /**
     *  @brief Update the basis ellipse (including center) and recompute the model matrix.
     */
    void update(afw::geom::ellipses::Ellipse const & ellipse);

    /// @brief Fill a model design matrix (basis functions in columns, flattened pixels in rows).
    void addModelMatrix(int order, ndarray::Array<T,2,-1> const & output);

    /// @brief Fill a model design matrix (basis functions in columns, flattened pixels in rows).
    void addModelVector(
        int order,
        ndarray::Array<T const,1,1> const & coefficients,
        ndarray::Array<T,1,1> const & output
    );

private:
    int _wsOrder;
    bool _useApproximateExp;
    double _ellipseFactor;
    ndarray::EigenView<T const,1,1,Eigen::ArrayXpr> _x;
    ndarray::EigenView<T const,1,1,Eigen::ArrayXpr> _y;
    Eigen::Array<T,Eigen::Dynamic,1> _xt;
    Eigen::Array<T,Eigen::Dynamic,1> _yt;
    Eigen::Array<T,Eigen::Dynamic,1> _expWorkspace;
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> _xWorkspace;
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> _yWorkspace;
};

}} // namespace lsst::shapelet

#endif // !defined(LSST_AFW_MATH_SHAPELETS_ModelBuilder_h_INCLUDED)
