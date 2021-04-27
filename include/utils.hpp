// The MIT License (MIT)
//
// Copyright (c) 2015, 2016, 2017 Howard Hinnant
// Copyright (c) 2016 Adrian Colomitchi
// Copyright (c) 2017 Florian Dang
// Copyright (c) 2017 Paul Thompson
// Copyright (c) 2018, 2019 Tomasz Kamiński
// Copyright (c) 2019 Jiangang Zhuang
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
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Our apologies.  When the previous paragraph was written, lowercase had not
// yet been invented (that would involve another several millennia of
// evolution). We did not mean to shout.

#ifndef UTILS_HPP
#define UTILS_HPP
#include "hpdf.h"
#include <iostream>
#include <memory>

enum PlannerTypes {
  PlannerTypes_Base,
  PlannerTypes_Day,
  PlannerTypes_Week,
  PlannerTypes_Month,
  PlannerTypes_Year,
  PlannerTypes_Main,
};

const std::int64_t Remarkable_width_px = 1872;
const std::int64_t Remarkable_height_px = 1404;
// const std::int64_t Remarkable_width_px = 1404;
// const std::int64_t Remarkable_height_px = 1872;
const std::int64_t Remarkable_margin_width_px = 120;
HPDF_REAL GetCenteredTextYPosition(HPDF_Page& page,
                                   std::string text,
                                   HPDF_REAL y_start,
                                   HPDF_REAL y_end);
HPDF_REAL GetCenteredTextXPosition(HPDF_Page& page,
                                   std::string text,
                                   HPDF_REAL x_start,
                                   HPDF_REAL x_end);
/**
 * @brief
 * A helper function to call the instance specific create thumbnail function
 * This is a little hacky and error prone as it relies on the caller to provide
 * the instance type. This might be helped by transitioning to std::any in the
 * base class to store the child objects instead of shared_ptrs to PlannerBase.
 *
 */
class PlannerBase;
void CreateThumbnailCaller(HPDF_Doc& doc,
                           HPDF_Page& page,
                           HPDF_REAL x_start,
                           HPDF_REAL y_start,
                           HPDF_REAL x_stop,
                           HPDF_REAL y_stop,
                           PlannerTypes type,
                           PlannerTypes object_type,
                           std::shared_ptr<PlannerBase> object);

#endif // UTILS_HPP
