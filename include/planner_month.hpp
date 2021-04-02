#include "planner_week.hpp"

#ifndef PLANNER_MONTH_HPP
#define PLANNER_MONTH_HPP
/*!
 * @brief
 * Class representing a month page
 */
class PlannerMonth:public PlannerBase  {
  date::year_month _month;

  std::vector<std::shared_ptr<PlannerBase> > _weeks;
  std::vector<std::shared_ptr<PlannerBase> > _days;

  public:
  PlannerMonth(): _month((date::year_month){(date::year)2021, (date::month) 1})
  {
    _page_title = format("%b %Y", _month);
    _grid_string = format("%b", _month);
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
    _note_section_percentage = 0.25;
  }

  PlannerMonth(date::year_month month, std::shared_ptr<PlannerBase> parent_year, HPDF_REAL height, HPDF_REAL width) : _month(month)
  {
    _page_title = format("%b %Y", _month);
    _grid_string = format("%b", _month);
    _page_height = height;
    _page_width = width;
    _note_section_percentage = 0.25;
    _parent = parent_year;
  }



  std::vector<std::shared_ptr<PlannerBase> > & GetDays()
  {
    return _days;
  }

  /*! Function to build the days */
  void AddDays()
  {
    date::year_month_day temp1 = date::year(_month.year())/_month.month()/1;
    date::year_month_day temp2 = temp1 + (date::months)1;
    size_t num_days = ((date::sys_days)temp2 - (date::sys_days)temp1).count();

    for(size_t i = 1; i <= num_days; i++)
    {
      std::string day_page_title = format("%B %d %Y", (date::year_month_day) ((date::sys_days)temp1 + (date::days)(i-1)));
      std::string day_grid_title = format("%d", (date::year_month_day) ((date::sys_days)temp1 + (date::days)(i-1)));

      _days.push_back(std::make_shared<PlannerDay>(PlannerDay(temp1.day(),temp1.month(), temp1.year(), NULL, shared_from_this(), _page_height, _page_width, day_page_title, day_grid_title)));

      std::shared_ptr<PlannerBase> prev_day;

      if(i > 1)
      {
        prev_day = _days[i - 2];
      }
      else
      {
        if(NULL != _left)
        {
          prev_day = std::static_pointer_cast<PlannerMonth>(_left)->GetDays().back();
        }
      }

      if(NULL != prev_day)
      {
        _days.back()->SetLeft(prev_day);
        prev_day->SetRight(_days.back());
      }
    }
  }

  void BuildDays(HPDF_Doc &doc)
  {
    for(auto day:_days)
    {
      std::static_pointer_cast<PlannerDay>(day)->Build(doc);
    }
  }
/*!
   * Function to create the weekday name header
   */
  void CreateWeekdayHeader(HPDF_Doc &doc, HPDF_Page & page, HPDF_REAL x_start, HPDF_REAL y_start, HPDF_REAL x_stop, HPDF_REAL y_stop, bool create_thumbnail, HPDF_REAL padding, bool first_letter_only)
  {
    std::vector<std::shared_ptr<PlannerBase> > weekdays;
    date::weekday weekday;
    for(size_t i = 0; i < 7; i++)
    {
      weekday = (date::weekday)i;
      std::string weekday_name = format("%a", weekday);
      if(true == first_letter_only)
      {
        weekday_name = weekday_name.substr(0,1);
      }
      weekdays.push_back(std::make_shared<PlannerBase>(PlannerBase(weekday_name)));
    }

    std::cout<<"Creating weekday header"<<std::endl;
    CreateGrid
      (
       doc,
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
       padding
      );
  }

  void AddDaysSection(HPDF_Doc &doc, HPDF_Page &page, HPDF_REAL x_start, HPDF_REAL y_start, HPDF_REAL x_stop, HPDF_REAL y_stop, bool create_thumbnail, HPDF_REAL padding)
  {

    date::year_month_day first_day = date::year(_month.year())/_month.month()/1;
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
        date::weekday{first_day}.c_encoding(),
        create_thumbnail,
        PlannerTypes_Month,
        PlannerTypes_Day,
        _page_height,
        padding
        );

  }


  void CreateDaysSection(HPDF_Doc & doc )
  {
    CreateWeekdayHeader(doc, _page, _page_width * _note_section_percentage + 30, 95, _page_width - 30, 130, false, 10, false);
    AddDaysSection(doc, _page, _page_width * _note_section_percentage + 30, 150, _page_width - 30, _page_height - 105, false, 10);
  }

  void CreateThumbnail(HPDF_Doc & doc, HPDF_Page &page, HPDF_REAL x_start, HPDF_REAL y_start, HPDF_REAL x_stop, HPDF_REAL y_stop)
  {
    std::cout<<"Creating thumbnail : "
             <<x_start
             <<", "
             <<y_start
             <<", "
             <<x_stop
             <<", "
             <<y_stop
             <<std::endl;
    CreateWeekdayHeader(doc, page, x_start, y_start + 50, x_stop, y_start + 100, false, 2, true);
    AddDaysSection(doc, page, x_start, y_start + 100, x_stop, y_stop, false, 2);
  }

  void CreateNavigation(HPDF_Doc &doc)
  {
    AddNavigation();
    for(auto day:_days)
    {
      std::static_pointer_cast<PlannerDay>(day)->CreateNavigation(doc);
    }
  }

  void Build(HPDF_Doc &doc)
  {
    CreatePage(doc, _page_height, _page_width);
    CreateTitle();
    AddDays();
    BuildDays(doc);
    CreateDaysSection(doc);
    CreateNotesSection();
  }
};
#endif //PLANNER_MONTH_HPP
