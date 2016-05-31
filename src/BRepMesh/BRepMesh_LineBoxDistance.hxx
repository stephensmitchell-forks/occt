// Created on: 2015-11-27
// Created by: Danila ULYANOV
// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepMesh_LineBoxDistance_Header
#define _BRepMesh_LineBoxDistance_Header

#include <BVH_Types.hxx>

template <class T, int N>
class LineToBoxDistance
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:
  struct Result
  {
    T distance, sqrDistance;
    T lineParameter;
    BVH_VecNt closestPoint[2];
  };

  Result operator() (const BVH_VecNt& theOrigin,
                     const BVH_VecNt& theDirect,
                     const BVH_VecNt& theMinPoint,
                     const BVH_VecNt& theMaxPoint);

protected:
  // Compute the distance and closest point between a line and an
  // axis-aligned box whose center is the origin.  On input, 'point' is the
  // line origin and 'direction' is the line direction.  On output, 'point'
  // is the point on the box closest to the line.  The 'direction' is
  // non-const to allow transforming the problem into the first octant.
  void DoQuery (BVH_VecNt& point, BVH_VecNt& direction,
                BVH_VecNt const& boxExtent, Result& result);

private:
  void Face (int i0, int i1, int i2, BVH_VecNt& pnt,
             BVH_VecNt const& dir, BVH_VecNt const& PmE,
             BVH_VecNt const& boxExtent, Result& result);

  void CaseNoZeros (BVH_VecNt& pnt, BVH_VecNt const& dir,
                    BVH_VecNt const& boxExtent, Result& result);

  void Case0 (int i0, int i1, int i2, BVH_VecNt& pnt,
              BVH_VecNt const& dir, BVH_VecNt const& boxExtent,
              Result& result);

  void Case00 (int i0, int i1, int i2, BVH_VecNt& pnt,
               BVH_VecNt const& dir, BVH_VecNt const& boxExtent,
               Result& result);

  void Case000 (BVH_VecNt& pnt, BVH_VecNt const& boxExtent,
                Result& result);
};

// =======================================================================
// function :
// purpose  :
// =======================================================================
template<class T, int N>
typename LineToBoxDistance<T, N>::Result
LineToBoxDistance<T, N>::operator() (const BVH_VecNt& theOrigin,
                                     const BVH_VecNt& theDirect,
                                     const BVH_VecNt& theMinPoint,
                                     const BVH_VecNt& theMaxPoint)
{
  // Translate the line and box so that the box has center at the origin.
  BVH_VecNt boxCenter, boxExtent;
  boxCenter = (theMaxPoint + theMinPoint) * (T) 0.5;
  boxExtent = (theMaxPoint - theMinPoint) * (T) 0.5;
  BVH_VecNt point = theOrigin - boxCenter;
  BVH_VecNt direction = theDirect;

  Result result;
  DoQuery (point, direction, boxExtent, result);

  // Compute the closest point on the line.
  result.closestPoint[0] =
    theOrigin + theDirect * result.lineParameter;

  // Compute the closest point on the box.
  result.closestPoint[1] = boxCenter + point;
  return result;
}

// =======================================================================
// function : DoQuery
// purpose  :
// =======================================================================
template<class T, int N>
void LineToBoxDistance<T, N>::DoQuery (
  BVH_VecNt& point, BVH_VecNt& direction,
  BVH_VecNt const& boxExtent, Result& result)
{
  result.sqrDistance = (T)0;
  result.lineParameter = (T)0;

  // Apply reflections so that direction vector has nonnegative components.
  bool reflect[3];

  for (int i = 0; i < 3; ++i)
  {
    if (direction[i] < (T)0)
    {
      point[i] = -point[i];
      direction[i] = -direction[i];
      reflect[i] = true;
    }
    else
    {
      reflect[i] = false;
    }
  }

  if (direction[0] > (T)0)
  {
    if (direction[1] > (T)0)
    {
      if (direction[2] > (T)0)  // (+,+,+)
      {
        CaseNoZeros (point, direction, boxExtent, result);
      }
      else  // (+,+,0)
      {
        Case0 (0, 1, 2, point, direction, boxExtent, result);
      }
    }
    else
    {
      if (direction[2] > (T)0)  // (+,0,+)
      {
        Case0 (0, 2, 1, point, direction, boxExtent, result);
      }
      else  // (+,0,0)
      {
        Case00 (0, 1, 2, point, direction, boxExtent, result);
      }
    }
  }
  else
  {
    if (direction[1] > (T)0)
    {
      if (direction[2] > (T)0)  // (0,+,+)
      {
        Case0 (1, 2, 0, point, direction, boxExtent, result);
      }
      else  // (0,+,0)
      {
        Case00 (1, 0, 2, point, direction, boxExtent, result);
      }
    }
    else
    {
      if (direction[2] > (T)0)  // (0,0,+)
      {
        Case00 (2, 0, 1, point, direction, boxExtent, result);
      }
      else  // (0,0,0)
      {
        Case000 (point, boxExtent, result);
      }
    }
  }

  // Undo the reflections applied previously.
  for (int i = 0; i < 3; ++i)
  {
    if (reflect[i])
    {
      point[i] = -point[i];
    }
  }

  result.distance = sqrt (result.sqrDistance);
}

// =======================================================================
// function : Face
// purpose  :
// =======================================================================
template<class T, int N>
void LineToBoxDistance<T, N>::Face (int i0, int i1,
                                    int i2, BVH_VecNt& pnt, BVH_VecNt const& dir,
                                    BVH_VecNt const& PmE, BVH_VecNt const& boxExtent, Result& result)
{
  BVH_VecNt PpE;
  T lenSqr, inv, tmp, param, t, delta;

  PpE[i1] = pnt[i1] + boxExtent[i1];
  PpE[i2] = pnt[i2] + boxExtent[i2];

  if (dir[i0] * PpE[i1] >= dir[i1] * PmE[i0])
  {
    if (dir[i0] * PpE[i2] >= dir[i2] * PmE[i0])
    {
      // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
      pnt[i0] = boxExtent[i0];
      inv = ((T)1) / dir[i0];
      pnt[i1] -= dir[i1] * PmE[i0] * inv;
      pnt[i2] -= dir[i2] * PmE[i0] * inv;
      result.lineParameter = -PmE[i0] * inv;
    }
    else
    {
      // v[i1] >= -e[i1], v[i2] < -e[i2]
      lenSqr = dir[i0] * dir[i0] + dir[i2] * dir[i2];
      tmp = lenSqr * PpE[i1] - dir[i1] * (dir[i0] * PmE[i0] +
                                          dir[i2] * PpE[i2]);

      if (tmp <= ((T)2)*lenSqr * boxExtent[i1])
      {
        t = tmp / lenSqr;
        lenSqr += dir[i1] * dir[i1];
        tmp = PpE[i1] - t;
        delta = dir[i0] * PmE[i0] + dir[i1] * tmp + dir[i2] * PpE[i2];
        param = -delta / lenSqr;
        result.sqrDistance += PmE[i0] * PmE[i0] + tmp * tmp +
                              PpE[i2] * PpE[i2] + delta * param;

        result.lineParameter = param;
        pnt[i0] = boxExtent[i0];
        pnt[i1] = t - boxExtent[i1];
        pnt[i2] = -boxExtent[i2];
      }
      else
      {
        lenSqr += dir[i1] * dir[i1];
        delta = dir[i0] * PmE[i0] + dir[i1] * PmE[i1] + dir[i2] * PpE[i2];
        param = -delta / lenSqr;
        result.sqrDistance += PmE[i0] * PmE[i0] + PmE[i1] * PmE[i1]
                              + PpE[i2] * PpE[i2] + delta * param;

        result.lineParameter = param;
        pnt[i0] = boxExtent[i0];
        pnt[i1] = boxExtent[i1];
        pnt[i2] = -boxExtent[i2];
      }
    }
  }
  else
  {
    if (dir[i0] * PpE[i2] >= dir[i2] * PmE[i0])
    {
      // v[i1] < -e[i1], v[i2] >= -e[i2]
      lenSqr = dir[i0] * dir[i0] + dir[i1] * dir[i1];
      tmp = lenSqr * PpE[i2] - dir[i2] * (dir[i0] * PmE[i0] +
                                          dir[i1] * PpE[i1]);

      if (tmp <= ((T)2)*lenSqr * boxExtent[i2])
      {
        t = tmp / lenSqr;
        lenSqr += dir[i2] * dir[i2];
        tmp = PpE[i2] - t;
        delta = dir[i0] * PmE[i0] + dir[i1] * PpE[i1] + dir[i2] * tmp;
        param = -delta / lenSqr;
        result.sqrDistance += PmE[i0] * PmE[i0] + PpE[i1] * PpE[i1] +
                              tmp * tmp + delta * param;

        result.lineParameter = param;
        pnt[i0] = boxExtent[i0];
        pnt[i1] = -boxExtent[i1];
        pnt[i2] = t - boxExtent[i2];
      }
      else
      {
        lenSqr += dir[i2] * dir[i2];
        delta = dir[i0] * PmE[i0] + dir[i1] * PpE[i1] + dir[i2] * PmE[i2];
        param = -delta / lenSqr;
        result.sqrDistance += PmE[i0] * PmE[i0] + PpE[i1] * PpE[i1] +
                              PmE[i2] * PmE[i2] + delta * param;

        result.lineParameter = param;
        pnt[i0] = boxExtent[i0];
        pnt[i1] = -boxExtent[i1];
        pnt[i2] = boxExtent[i2];
      }
    }
    else
    {
      // v[i1] < -e[i1], v[i2] < -e[i2]
      lenSqr = dir[i0] * dir[i0] + dir[i2] * dir[i2];
      tmp = lenSqr * PpE[i1] - dir[i1] * (dir[i0] * PmE[i0] +
                                          dir[i2] * PpE[i2]);

      if (tmp >= (T)0)
      {
        // v[i1]-edge is closest
        if (tmp <= ((T)2)*lenSqr * boxExtent[i1])
        {
          t = tmp / lenSqr;
          lenSqr += dir[i1] * dir[i1];
          tmp = PpE[i1] - t;
          delta = dir[i0] * PmE[i0] + dir[i1] * tmp + dir[i2] * PpE[i2];
          param = -delta / lenSqr;
          result.sqrDistance += PmE[i0] * PmE[i0] + tmp * tmp +
                                PpE[i2] * PpE[i2] + delta * param;

          result.lineParameter = param;
          pnt[i0] = boxExtent[i0];
          pnt[i1] = t - boxExtent[i1];
          pnt[i2] = -boxExtent[i2];
        }
        else
        {
          lenSqr += dir[i1] * dir[i1];
          delta = dir[i0] * PmE[i0] + dir[i1] * PmE[i1]
                  + dir[i2] * PpE[i2];
          param = -delta / lenSqr;
          result.sqrDistance += PmE[i0] * PmE[i0] + PmE[i1] * PmE[i1]
                                + PpE[i2] * PpE[i2] + delta * param;

          result.lineParameter = param;
          pnt[i0] = boxExtent[i0];
          pnt[i1] = boxExtent[i1];
          pnt[i2] = -boxExtent[i2];
        }

        return;
      }

      lenSqr = dir[i0] * dir[i0] + dir[i1] * dir[i1];
      tmp = lenSqr * PpE[i2] - dir[i2] * (dir[i0] * PmE[i0] +
                                          dir[i1] * PpE[i1]);

      if (tmp >= (T)0)
      {
        // v[i2]-edge is closest
        if (tmp <= ((T)2)*lenSqr * boxExtent[i2])
        {
          t = tmp / lenSqr;
          lenSqr += dir[i2] * dir[i2];
          tmp = PpE[i2] - t;
          delta = dir[i0] * PmE[i0] + dir[i1] * PpE[i1] + dir[i2] * tmp;
          param = -delta / lenSqr;
          result.sqrDistance += PmE[i0] * PmE[i0] + PpE[i1] * PpE[i1] +
                                tmp * tmp + delta * param;

          result.lineParameter = param;
          pnt[i0] = boxExtent[i0];
          pnt[i1] = -boxExtent[i1];
          pnt[i2] = t - boxExtent[i2];
        }
        else
        {
          lenSqr += dir[i2] * dir[i2];
          delta = dir[i0] * PmE[i0] + dir[i1] * PpE[i1]
                  + dir[i2] * PmE[i2];
          param = -delta / lenSqr;
          result.sqrDistance += PmE[i0] * PmE[i0] + PpE[i1] * PpE[i1]
                                + PmE[i2] * PmE[i2] + delta * param;

          result.lineParameter = param;
          pnt[i0] = boxExtent[i0];
          pnt[i1] = -boxExtent[i1];
          pnt[i2] = boxExtent[i2];
        }

        return;
      }

      // (v[i1],v[i2])-corner is closest
      lenSqr += dir[i2] * dir[i2];
      delta = dir[i0] * PmE[i0] + dir[i1] * PpE[i1] + dir[i2] * PpE[i2];
      param = -delta / lenSqr;
      result.sqrDistance += PmE[i0] * PmE[i0] + PpE[i1] * PpE[i1]
                            + PpE[i2] * PpE[i2] + delta * param;

      result.lineParameter = param;
      pnt[i0] = boxExtent[i0];
      pnt[i1] = -boxExtent[i1];
      pnt[i2] = -boxExtent[i2];
    }
  }
}

// =======================================================================
// function : CaseNoZeros
// purpose  :
// =======================================================================
template<class T, int N>
void LineToBoxDistance<T, N>::CaseNoZeros (
  BVH_VecNt& pnt, BVH_VecNt const& dir,
  BVH_VecNt const& boxExtent, Result& result)
{
  BVH_VecNt PmE = pnt - boxExtent;
  T prodDxPy = dir[0] * PmE[1];
  T prodDyPx = dir[1] * PmE[0];
  T prodDzPx, prodDxPz, prodDzPy, prodDyPz;

  if (prodDyPx >= prodDxPy)
  {
    prodDzPx = dir[2] * PmE[0];
    prodDxPz = dir[0] * PmE[2];

    if (prodDzPx >= prodDxPz)
    {
      // line intersects x = e0
      Face (0, 1, 2, pnt, dir, PmE, boxExtent, result);
    }
    else
    {
      // line intersects z = e2
      Face (2, 0, 1, pnt, dir, PmE, boxExtent, result);
    }
  }
  else
  {
    prodDzPy = dir[2] * PmE[1];
    prodDyPz = dir[1] * PmE[2];

    if (prodDzPy >= prodDyPz)
    {
      // line intersects y = e1
      Face (1, 2, 0, pnt, dir, PmE, boxExtent, result);
    }
    else
    {
      // line intersects z = e2
      Face (2, 0, 1, pnt, dir, PmE, boxExtent, result);
    }
  }
}

// =======================================================================
// function : Case0
// purpose  :
// =======================================================================
template<class T, int N>
void LineToBoxDistance<T, N>::Case0 (int i0, int i1,
                                     int i2, BVH_VecNt& pnt, BVH_VecNt const& dir,
                                     BVH_VecNt const& boxExtent, Result& result)
{
  T PmE0 = pnt[i0] - boxExtent[i0];
  T PmE1 = pnt[i1] - boxExtent[i1];
  T prod0 = dir[i1] * PmE0;
  T prod1 = dir[i0] * PmE1;
  T delta, invLSqr, inv;

  if (prod0 >= prod1)
  {
    // line intersects P[i0] = e[i0]
    pnt[i0] = boxExtent[i0];

    T PpE1 = pnt[i1] + boxExtent[i1];
    delta = prod0 - dir[i0] * PpE1;

    if (delta >= (T)0)
    {
      invLSqr = ((T)1) / (dir[i0] * dir[i0] + dir[i1] * dir[i1]);
      result.sqrDistance += delta * delta * invLSqr;
      pnt[i1] = -boxExtent[i1];
      result.lineParameter = - (dir[i0] * PmE0 + dir[i1] * PpE1) * invLSqr;
    }
    else
    {
      inv = ((T)1) / dir[i0];
      pnt[i1] -= prod0 * inv;
      result.lineParameter = -PmE0 * inv;
    }
  }
  else
  {
    // line intersects P[i1] = e[i1]
    pnt[i1] = boxExtent[i1];

    T PpE0 = pnt[i0] + boxExtent[i0];
    delta = prod1 - dir[i1] * PpE0;

    if (delta >= (T)0)
    {
      invLSqr = ((T)1) / (dir[i0] * dir[i0] + dir[i1] * dir[i1]);
      result.sqrDistance += delta * delta * invLSqr;
      pnt[i0] = -boxExtent[i0];
      result.lineParameter = - (dir[i0] * PpE0 + dir[i1] * PmE1) * invLSqr;
    }
    else
    {
      inv = ((T)1) / dir[i1];
      pnt[i0] -= prod1 * inv;
      result.lineParameter = -PmE1 * inv;
    }
  }

  if (pnt[i2] < -boxExtent[i2])
  {
    delta = pnt[i2] + boxExtent[i2];
    result.sqrDistance += delta * delta;
    pnt[i2] = -boxExtent[i2];
  }
  else if (pnt[i2] > boxExtent[i2])
  {
    delta = pnt[i2] - boxExtent[i2];
    result.sqrDistance += delta * delta;
    pnt[i2] = boxExtent[i2];
  }
}

// =======================================================================
// function : Case00
// purpose  :
// =======================================================================
template<class T, int N>
void LineToBoxDistance<T, N>::Case00 (int i0, int i1,
                                      int i2, BVH_VecNt& pnt, BVH_VecNt const& dir,
                                      BVH_VecNt const& boxExtent, Result& result)
{
  T delta;

  result.lineParameter = (boxExtent[i0] - pnt[i0]) / dir[i0];

  pnt[i0] = boxExtent[i0];

  if (pnt[i1] < -boxExtent[i1])
  {
    delta = pnt[i1] + boxExtent[i1];
    result.sqrDistance += delta * delta;
    pnt[i1] = -boxExtent[i1];
  }
  else if (pnt[i1] > boxExtent[i1])
  {
    delta = pnt[i1] - boxExtent[i1];
    result.sqrDistance += delta * delta;
    pnt[i1] = boxExtent[i1];
  }

  if (pnt[i2] < -boxExtent[i2])
  {
    delta = pnt[i2] + boxExtent[i2];
    result.sqrDistance += delta * delta;
    pnt[i2] = -boxExtent[i2];
  }
  else if (pnt[i2] > boxExtent[i2])
  {
    delta = pnt[i2] - boxExtent[i2];
    result.sqrDistance += delta * delta;
    pnt[i2] = boxExtent[i2];
  }
}

// =======================================================================
// function : Case000
// purpose  :
// =======================================================================
template<class T, int N>
void LineToBoxDistance<T, N>::Case000 (
  BVH_VecNt& pnt, BVH_VecNt const& boxExtent, Result& result)
{
  T delta;

  if (pnt[0] < -boxExtent[0])
  {
    delta = pnt[0] + boxExtent[0];
    result.sqrDistance += delta * delta;
    pnt[0] = -boxExtent[0];
  }
  else if (pnt[0] > boxExtent[0])
  {
    delta = pnt[0] - boxExtent[0];
    result.sqrDistance += delta * delta;
    pnt[0] = boxExtent[0];
  }

  if (pnt[1] < -boxExtent[1])
  {
    delta = pnt[1] + boxExtent[1];
    result.sqrDistance += delta * delta;
    pnt[1] = -boxExtent[1];
  }
  else if (pnt[1] > boxExtent[1])
  {
    delta = pnt[1] - boxExtent[1];
    result.sqrDistance += delta * delta;
    pnt[1] = boxExtent[1];
  }

  if (pnt[2] < -boxExtent[2])
  {
    delta = pnt[2] + boxExtent[2];
    result.sqrDistance += delta * delta;
    pnt[2] = -boxExtent[2];
  }
  else if (pnt[2] > boxExtent[2])
  {
    delta = pnt[2] - boxExtent[2];
    result.sqrDistance += delta * delta;
    pnt[2] = boxExtent[2];
  }
}

#endif // _BRepMesh_LineBoxDistance_Header
