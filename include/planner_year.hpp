#ifndef PLANNER_YEAR_HPP
#define PLANNER_YEAR_HPP
#include "planner_month.hpp"

/*!
 * @brief
 * Class representing the Year page
 */
class PlannerYear : public PlannerBase {
  date::year _year;
  std::vector<std::shared_ptr<PlannerBase>> _months;

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
              HPDF_REAL margin
              )
      : _year(year) {
    _page_title = format("%Y", _year);
    _grid_string = format("%Y", _year);
    _page_height = height;
    _page_width = width;
    _note_section_percentage = 0.25;
    _parent = parent_main;
    _margin = margin;
  }

  PlannerYear(short year) : _year((date::year)year) {}

  date::year GetYear() { return _year; }

  void AddMonthsSection(HPDF_Doc& doc) {
    CreateGrid(doc,
               _page,
               _page_width * _note_section_percentage + 15,
               95,
               _page_width - 15,
               _page_height - 45,
               3,
               4,
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
                       _margin
                       )));

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
    CreateNotesSection();
  }
};
#endif // PLANNER_YEAR_HPP
