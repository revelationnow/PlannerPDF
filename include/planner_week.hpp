#ifndef PLANNER_WEEK_HPP
#define PLANNER_WEEK_HPP
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
              HPDF_REAL width)
      : _week_num(week),
        _month((date::year_month){(date::year)year, (date::month)month}),
        _parent_month(parent_month) {
    _page_height = height;
    _page_width = width;
  }

  void Build(HPDF_Doc& doc) {
    CreatePage(doc, _page_height, _page_width);
    CreateNotesSection();
  }
};
#endif // PLANNER_WEEK_HPP
