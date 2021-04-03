#ifndef PLANNER_MAIN_HPP
#define PLANNER_MAIN_HPP
#include "planner_year.hpp"

/*!
 * @brief
 * The Main Planner page class
 */
class PlannerMain : public PlannerBase {
  date::year_month_day _base_date;
  std::string _filename;
  std::uint64_t _num_years;
  std::vector<std::shared_ptr<PlannerBase>> _years;
  HPDF_Doc _pdf;

public:
  PlannerMain()
      : _base_date((date::year)2021, (date::month)1, (date::day)1),
        _num_years(10), _filename("test.pdf") {
    _page_title = "Planner";
  }

  PlannerMain(short year,
              std::string filename,
              short num_years,
              HPDF_REAL height,
              HPDF_REAL width)
      : _base_date((date::year)year, (date::month)1, (date::day)1),
        _filename(filename), _num_years(num_years) {
    _page_title = "Planner";
    _page_height = height;
    _page_width = width;
  }

  static void
  err_cb(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    std::cout << "[ERR] : PDF operation error : Received error : " << std::hex
              << error_no << " with detail : " << detail_no << std::endl;

    throw std::exception();
  }

  void CreateDocument() {
    _pdf = HPDF_New(this->err_cb, NULL);
    if (NULL == _pdf) {
      std::cout << "[ERR] Failed to create PDF object" << std::endl;
      throw std::exception();
    }
  }

  void CreateYearsSection(HPDF_Doc& doc) {
    std::string year_title_string = "Years";
    HPDF_REAL notes_divider_x = _page_width * _note_section_percentage;
    HPDF_REAL years_section_text_x = GetCenteredTextXPosition(
        _page, year_title_string, notes_divider_x, _page_width);
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page,
                          years_section_text_x,
                          _page_height - (_page_title_font_size * 2) -
                              _note_title_font_size - 10);
    HPDF_Page_ShowText(_page, year_title_string.c_str());
    HPDF_Page_EndText(_page);

    CreateGrid(doc,
               _page,
               notes_divider_x + 20,
               _page_title_font_size * 2 + _note_title_font_size + 20,
               _page_width - 20,
               _page_height,
               _years.size(),
               1,
               _years,
               true,
               0,
               false,
               PlannerTypes_Main,
               PlannerTypes_Year,
               _page_height,
               10,
               true
               );
  }

  void BuildYears() {
    for (auto year : _years) {
      std::static_pointer_cast<PlannerYear>(year)->Build(_pdf);
    }
  }

  void CreateNavigation() {
    for (auto year : _years) {
      std::static_pointer_cast<PlannerYear>(year)->CreateNavigation(_pdf);
    }
  }

  void Build() {
    CreatePage(_pdf, _page_height, _page_width);
    /* Add _num_years of year objects and call their build functions */
    for (size_t loop_index = 0; loop_index < _num_years; loop_index++) {
      date::year next_year = _base_date.year() + (date::years)loop_index;
      _years.push_back(std::make_shared<PlannerYear>(PlannerYear(
          next_year, shared_from_this(), _page_height, _page_width)));
      if (loop_index != 0) {
        _years.back()->SetLeft(_years[loop_index - 1]);
        _years[loop_index - 1]->SetRight(_years.back());
      }
    }
    CreateTitle();
    BuildYears();
    CreateNavigation();
    CreateNotesSection();
    CreateYearsSection(_pdf);
  }

  void FinishDocument() {
    HPDF_SaveToFile(_pdf, _filename.c_str());
    HPDF_Free(_pdf);
  }
};
#endif // PLANNER_MAIN_HPP
