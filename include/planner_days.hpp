#ifndef PLANNER_DAYS_HPP
#define PLANNER_DAYS_HPP
#include "planner_base.hpp"

/*!
 * @brief
 * Class representing a day page
 */
class PlannerDay : public PlannerBase {
  date::year_month_day _day;
  std::shared_ptr<PlannerBase> _parent_week;

public:
  PlannerDay()
      : _day((date::year_month_day){
            (date::year)2021, (date::month)1, (date::day)1}) {
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
  }

  PlannerDay(date::day day,
             date::month month,
             date::year year,
             PlannerBase* parent_week,
             std::shared_ptr<PlannerBase> parent_month,
             HPDF_REAL height,
             HPDF_REAL width,
             std::string page_title,
             std::string grid_string,
             HPDF_REAL margin,
             bool is_left_handed
             )
      : _day((date::year_month_day){
            (date::year)year, (date::month)month, (date::day)day}) {
    _page_height = height;
    _page_width = width;
    _page_title = page_title;
    _grid_string = grid_string;
    _parent = parent_month;
    _margin_width = margin;
    _is_left_handed = is_left_handed;
  }

  void CreateTasksSection(HPDF_Doc& doc) {
    std::string year_title_string = "Tasks";
    HPDF_REAL notes_divider_x = _page_width * _note_section_percentage;
    HPDF_REAL section_x_start;
    HPDF_REAL section_y_start;
    HPDF_REAL section_x_stop;
    HPDF_REAL section_y_stop;
    HPDF_REAL years_section_text_x;

    if(true == _is_left_handed)
    {
      section_x_start = 0;
      section_y_start = _page_title_font_size * 2;
      section_x_stop  = _page_width - notes_divider_x;
      section_y_stop  = _page_height;
      years_section_text_x = GetCenteredTextXPosition(
        _page, year_title_string, section_x_start, section_x_stop);
    }
    else
    {
      section_x_start = notes_divider_x;
      section_y_start = _page_title_font_size * 2;
      section_x_stop  = _page_width;
      section_y_stop  = _page_height;
      years_section_text_x = GetCenteredTextXPosition(
        _page, year_title_string, notes_divider_x, _page_width);
    }
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page,
                          years_section_text_x,
                          _page_height - (section_y_start + _note_title_font_size + 10) );
    HPDF_Page_ShowText(_page, year_title_string.c_str());
    HPDF_Page_EndText(_page);

    /* @TODO : This increases filesize a lot, try to replace with pattern fill
    or background png etc */
    FillAreaWithDots(_page,
                     40,
                     40,
                     _page_height,
                     _page_width,
                     section_x_start + 30,
                     section_y_start + (2 * _note_title_font_size),
                     section_x_stop - 10,
                     section_y_stop - 30);
  }

  void CreateNavigation(HPDF_Doc& doc) { AddNavigation(); }

  void Build(HPDF_Doc& doc) {
    CreatePage(doc, _page_height, _page_width);
    CreateTitle();
    CreateNotesSection();
    CreateTasksSection(doc);
  }
};
#endif // PLANNER_DAYS_HPP
