#ifndef PLANNER_MONTH_HPP
#define PLANNER_MONTH_HPP
#include "planner_week.hpp"
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

/*!
 * @brief
 * Class representing a month page
 */
class PlannerMonth : public PlannerBase {
  date::year_month _month;
  short _first_day_of_week;

  std::vector<std::shared_ptr<PlannerBase>> _weeks;
  std::vector<std::shared_ptr<PlannerBase>> _days;

public:
  PlannerMonth()
      : _month((date::year_month){(date::year)2021, (date::month)1}) {
    _page_title = format("%b %Y", _month);
    _grid_string = format("%b", _month);
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
    _note_section_percentage = 0.25;
  }

  PlannerMonth(date::year_month month,
               std::shared_ptr<PlannerBase> parent_year,
               HPDF_REAL height,
               HPDF_REAL width,
               HPDF_REAL margin,
               short first_day_of_week,
               bool is_left_handed,
               bool is_portrait)
      : _month(month) {
    _page_title = format(" %b %Y ", _month);
    _grid_string = format("%b", _month);
    _page_height = height;
    _page_width = width;
    //    _note_section_percentage = 0.25;
    _note_section_percentage = is_portrait ? 0.085 : 0.25;
    _parent = parent_year;
    _first_day_of_week = first_day_of_week;
    _is_left_handed = is_left_handed;
    _is_portrait = is_portrait;
    _margin_width = margin;
    _margin_left = _margin_width;
    _margin_right = _page_width - _margin_width;
  }

  std::vector<std::shared_ptr<PlannerBase>>& GetDays() { return _days; }

  /*! Function to build the days */
  void AddDays() {
    date::year_month_day temp1 = date::year(_month.year()) / _month.month() / 1;
    date::year_month_day temp2 = temp1 + (date::months)1;
    size_t num_days = ((date::sys_days)temp2 - (date::sys_days)temp1).count();

    for (size_t i = 1; i <= num_days; i++) {
      std::string day_page_title = format(
          "%B %d %Y",
          (date::year_month_day)((date::sys_days)temp1 + (date::days)(i - 1)));
      std::string day_grid_title = format(
          "%d",
          (date::year_month_day)((date::sys_days)temp1 + (date::days)(i - 1)));

      _days.push_back(
          std::make_shared<PlannerDay>(PlannerDay(temp1.day(),
                                                  temp1.month(),
                                                  temp1.year(),
                                                  NULL,
                                                  shared_from_this(),
                                                  _page_height,
                                                  _page_width,
                                                  day_page_title,
                                                  day_grid_title,
                                                  _margin_width,
                                                  _is_left_handed,
                                                  _is_portrait)));

      std::shared_ptr<PlannerBase> prev_day;

      if (i > 1) {
        prev_day = _days[i - 2];
      } else {
        if (NULL != _left) {
          prev_day =
              std::static_pointer_cast<PlannerMonth>(_left)->GetDays().back();
        }
      }

      if (NULL != prev_day) {
        _days.back()->SetLeft(prev_day);
        prev_day->SetRight(_days.back());
      }
    }
  }

  void BuildDays(HPDF_Doc& doc) {
    for (auto day : _days) {
      std::static_pointer_cast<PlannerDay>(day)->Build(doc);
    }
  }
  /*!
   * Function to create the weekday name header
   */
  void CreateWeekdayHeader(HPDF_Doc& doc,
                           HPDF_Page& page,
                           HPDF_REAL x_start,
                           HPDF_REAL y_start,
                           HPDF_REAL x_stop,
                           HPDF_REAL y_stop,
                           bool create_thumbnail,
                           HPDF_REAL padding,
                           bool first_letter_only) {
    std::vector<std::shared_ptr<PlannerBase>> weekdays;
    date::weekday weekday;
    for (size_t i = 0; i < 7; i++) {
      weekday = (date::weekday)((i + _first_day_of_week) % 7);
      std::string weekday_name = format("%a", weekday);
      if (true == first_letter_only) {
        weekday_name = weekday_name.substr(0, 1);
      }
      weekdays.push_back(std::make_shared<PlannerBase>(
          PlannerBase(weekday_name, _is_left_handed)));
    }

    CreateGrid(doc,
               page,
               x_start,
               y_start,
               x_stop,
               y_stop,
               1,
               7,
               weekdays,
               false,
               0,
               create_thumbnail,
               PlannerTypes_Month,
               PlannerTypes_Day,
               _page_height,
               padding,
               true);
  }

  void AddDaysSection(HPDF_Doc& doc,
                      HPDF_Page& page,
                      HPDF_REAL x_start,
                      HPDF_REAL y_start,
                      HPDF_REAL x_stop,
                      HPDF_REAL y_stop,
                      bool create_thumbnail,
                      HPDF_REAL padding) {

    date::year_month_day first_day =
        date::year(_month.year()) / _month.month() / 1;

    CreateGrid(
        doc,
        page,
        x_start,
        y_start,
        x_stop,
        y_stop,
        6,
        7,
        _days,
        true,
        (date::weekday{first_day}.c_encoding() - _first_day_of_week + 7) % 7,
        create_thumbnail,
        PlannerTypes_Month,
        PlannerTypes_Day,
        _page_height,
        padding,
        true);
  }

  void CreateDaysSection(HPDF_Doc& doc) {
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
    CreateWeekdayHeader(doc,
                        _page,
                        section_x_start + 30,
                        section_y_start,
                        section_x_stop - 30,
                        section_y_start + (_note_title_font_size * 2),
                        false,
                        10,
                        false);
    AddDaysSection(doc,
                   _page,
                   section_x_start + 30,
                   section_y_start + (_note_title_font_size * 2),
                   section_x_stop - 30,
                   section_y_stop - 105,
                   false,
                   10);
  }

  void CreateThumbnail(HPDF_Doc& doc,
                       HPDF_Page& page,
                       HPDF_REAL x_start,
                       HPDF_REAL y_start,
                       HPDF_REAL x_stop,
                       HPDF_REAL y_stop) {
    CreateWeekdayHeader(doc,
                        page,
                        x_start,
                        y_start + 50,
                        x_stop,
                        y_start + 100,
                        false,
                        2,
                        true);
    AddDaysSection(doc, page, x_start, y_start + 100, x_stop, y_stop, false, 2);
  }

  void CreateNavigation(HPDF_Doc& doc) {
    AddNavigation();
    for (auto day : _days) {
      std::static_pointer_cast<PlannerDay>(day)->CreateNavigation(doc);
    }
  }

  void Build(HPDF_Doc& doc) {
    CreatePage(doc, _page_height, _page_width);
    CreateTitle();
    AddDays();
    BuildDays(doc);
    CreateDaysSection(doc);
    if (false == _is_portrait) {
      CreateNotesSection();
    }
  }
};
#endif // PLANNER_MONTH_HPP
