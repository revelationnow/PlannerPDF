#ifndef PLANNER_YEAR_HPP
#define PLANNER_YEAR_HPP
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
#include "planner_month.hpp"

/*!
 * @brief
 * Class representing the Year page
 */
class PlannerYear : public PlannerBase {
  date::year _year;
  std::vector<std::shared_ptr<PlannerBase>> _months;
  short _first_day_of_week;

public:
  PlannerYear() : _year((date::year)2021) {
    _page_title = format("%Y", _year);
    _grid_string = format("%Y", _year);
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
    _note_section_percentage = 0.25;
  }

  PlannerYear(date::year year,
              std::shared_ptr<PlannerBase> parent_main,
              HPDF_REAL height,
              HPDF_REAL width,
              HPDF_REAL margin,
              short first_day_of_week,
              bool is_left_handed,
              bool is_portrait)
      : _year(year) {
    _page_title = format("%Y", _year);
    _grid_string = format("%Y", _year);
    _page_height = height;
    _page_width = width;
    _note_section_percentage = is_portrait ? 0.085 : 0.25;
    _parent = parent_main;
    _margin_width = margin;
    _margin_left = _margin_width;
    _margin_right = _page_width - _margin_width;
    _first_day_of_week = first_day_of_week;
    _is_left_handed = is_left_handed;
    _is_portrait = is_portrait;
  }

  PlannerYear(short year) : _year((date::year)year) {}

  date::year GetYear() { return _year; }

  void AddMonthsSection(HPDF_Doc& doc) {

    HPDF_REAL notes_divider_x = _page_width * _note_section_percentage;
    HPDF_REAL section_x_start;
    HPDF_REAL section_y_start;
    HPDF_REAL section_x_stop;
    HPDF_REAL section_y_stop;

    if (true == _is_left_handed) {
      section_x_start = 0;
      section_y_start = _page_title_font_size * 2;
      section_x_stop = _page_width - notes_divider_x;
      section_y_stop = _page_height;
    } else {
      section_x_start = notes_divider_x;
      section_y_start = _page_title_font_size * 2;
      section_x_stop = _page_width;
      section_y_stop = _page_height;
    }

    CreateGrid(doc,
               _page,
               section_x_start + 15,
               section_y_start + 5,
               section_x_stop - 15,
               section_y_stop - 45,
               _is_portrait ? 4 : 3,
               _is_portrait ? 3 : 4,
               _months,
               true,
               0,
               true,
               PlannerTypes_Year,
               PlannerTypes_Month,
               _page_height,
               10,
               false);
  }

  void BuildMonths(HPDF_Doc& doc) {
    for (auto month : _months) {
      std::static_pointer_cast<PlannerMonth>(month)->Build(doc);
    }
  }

  std::vector<std::shared_ptr<PlannerBase>>& GetMonths() { return _months; }

  void CreateNavigation(HPDF_Doc& doc) {
    AddNavigation();
    for (auto month : _months) {
      std::static_pointer_cast<PlannerMonth>(month)->CreateNavigation(doc);
    }
  }

  void Build(HPDF_Doc& doc) {
    CreatePage(doc, _page_height, _page_width);
    /* Add months to _months and call build on each of them */
    for (size_t month_id = 1; month_id <= 12; month_id++) {
      _months.push_back(std::make_shared<PlannerMonth>(
          PlannerMonth((date::year_month){_year, (date::month)month_id},
                       shared_from_this(),
                       _page_height,
                       _page_width,
                       _margin_width,
                       _first_day_of_week,
                       _is_left_handed,
                       _is_portrait)));

      std::shared_ptr<PlannerBase> prev_month;

      if (month_id > 1) {
        prev_month = _months[month_id - 2];
      } else {
        if (NULL != _left) {
          prev_month =
              std::static_pointer_cast<PlannerYear>(_left)->GetMonths().back();
        }
      }

      if (NULL != prev_month) {
        _months.back()->SetLeft(prev_month);
        prev_month->SetRight(_months.back());
      }
    }
    BuildMonths(doc);
    AddMonthsSection(doc);
    CreateTitle();
    if (false == _is_portrait) {
      CreateNotesSection();
    }
  }
};
#endif // PLANNER_YEAR_HPP
