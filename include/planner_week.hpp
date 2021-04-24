#ifndef PLANNER_WEEK_HPP
#define PLANNER_WEEK_HPP
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
#include "planner_days.hpp"

/*!
 * @brief
 * Class representing a Week page
 */
class PlannerWeek : public PlannerBase {
  unsigned char _week_num;
  date::year_month _month;
  std::vector<std::shared_ptr<PlannerBase>> _days;
  PlannerBase* _parent_month;

public:
  PlannerWeek()
      : _week_num((unsigned char)0),
        _month((date::year_month){(date::year)2021, (date::month)1}) {
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
  }

  PlannerWeek(unsigned char week,
              date::month month,
              date::year year,
              PlannerBase* parent_month,
              HPDF_REAL height,
              HPDF_REAL width,
              HPDF_REAL margin)
      : _week_num(week),
        _month((date::year_month){(date::year)year, (date::month)month}),
        _parent_month(parent_month) {
    _page_height = height;
    _page_width = width;
    _margin_width = margin;
    _margin_left = _margin_width;
    _margin_right = _page_width - _margin_width;
  }

  void Build(HPDF_Doc& doc) {
    CreatePage(doc, _page_height, _page_width);
    CreateNotesSection();
  }
};
#endif // PLANNER_WEEK_HPP
