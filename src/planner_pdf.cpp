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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Our apologies.  When the previous paragraph was written, lowercase had not yet
// been invented (that would involve another several millennia of evolution).
// We did not mean to shout.


#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
#include "date.h"
#include "hpdf.h"

enum PlannerTypes
{
  PlannerTypes_Base,
  PlannerTypes_Day,
  PlannerTypes_Week,
  PlannerTypes_Month,
  PlannerTypes_Year,
  PlannerTypes_Main,
};
const std::int64_t Remarkable_width_px = 1872;
const std::int64_t Remarkable_height_px = 1404;

static HPDF_REAL GetCenteredTextYPosition(HPDF_Page &page, std::string text, HPDF_REAL y_start, HPDF_REAL y_end)
{
  HPDF_REAL height = HPDF_Page_GetCurrentFontSize(page);
  return y_start + ((y_end - y_start)/2) - height/2;
}

static HPDF_REAL GetCenteredTextXPosition(HPDF_Page &page, std::string text, HPDF_REAL x_start, HPDF_REAL x_end)
{
  HPDF_REAL length = HPDF_Page_TextWidth(page, text.c_str());
  return x_start + ((x_end - x_start)/2) - length/2;
}


class PlannerBase;
void CreateGrid(
    HPDF_Doc &doc,
    HPDF_Page &page,
    HPDF_REAL x_start,
    HPDF_REAL y_start,
    HPDF_REAL x_stop,
    HPDF_REAL y_stop,
    HPDF_REAL num_rows,
    HPDF_REAL num_cols,
    std::vector<std::shared_ptr<PlannerBase> > &objects,
    bool create_annotations,
    size_t first_entry_offset,
    bool create_thumbnail,
    PlannerTypes page_type,
    PlannerTypes object_type,
    HPDF_REAL page_height,
    HPDF_REAL padding
    );

void FillAreaWithDots(
    HPDF_Page &page,
    HPDF_REAL dot_spacing_x,
    HPDF_REAL dot_spacing_y,
    HPDF_REAL page_height,
    HPDF_REAL page_width,
    HPDF_REAL x_start,
    HPDF_REAL y_start,
    HPDF_REAL x_stop,
    HPDF_REAL y_stop
    );

/*!
 * @brief
 * Base class for the different Planner elements
 *
 */
class PlannerBase : public std::enable_shared_from_this<PlannerBase> {
  protected:
  std::uint64_t _id;

  /*! What section of the page is reserved for notes */
  std::double_t _note_section_percentage;

  /*! representing the PDF Page that this object is controlling */
  HPDF_Page _page;

  /*! The font used for the notes header */
  HPDF_Font _notes_font;

  /*! The height of the page */
  HPDF_REAL _page_height;

  /*! The width of the page */
  HPDF_REAL _page_width;

  /*! A string representing the title of the page */
  std::string _page_title;

  /*! A string represeneting the title that appears when this object is displayed in a grid of a parent page */
  std::string _grid_string;

  /*! The font size of the page title */
  HPDF_REAL _page_title_font_size;

  /*! The font size of the notes section title */
  HPDF_REAL _note_title_font_size;

  /*! A pointer to the parent page object */
  std::shared_ptr<PlannerBase> _parent;

  /*! A pointer to the left page object */
  std::shared_ptr<PlannerBase> _left;

  /*! A pointer to the right page object */
  std::shared_ptr<PlannerBase> _right;

  public:
  PlannerBase():_id(0), _note_section_percentage(0.5), _page_title("Base"), _page_title_font_size(45), _note_title_font_size(35), _grid_string("GridBase")
  {
  }

  PlannerBase(std::string grid_string):_id(0), _note_section_percentage(0.5), _page_title("Base"), _page_title_font_size(45), _note_title_font_size(35), _grid_string(grid_string)
  {
  }

  void SetGridString(std::string grid_string)
  {
    _grid_string = grid_string;
  }

  void CreateThumbnail(HPDF_Doc & doc, HPDF_Page &page, HPDF_REAL x_start, HPDF_REAL y_start, HPDF_REAL x_stop, HPDF_REAL y_stop)
  {
  }

  /*!
   * @brief
   * Create the page for this object with the given height and width
   */
  void CreatePage(HPDF_Doc doc, std::uint64_t height, std::uint64_t width)
  {
    _page = HPDF_AddPage(doc);
    _notes_font = HPDF_GetFont(doc, "Helvetica", NULL);
    HPDF_Page_SetHeight(_page, height);
    _page_height = height;
    HPDF_Page_SetWidth(_page, width);
    _page_width = width;
  }

  /*!
   * Set the navigation pointer for left sibling
   */
  void SetLeft(std::shared_ptr<PlannerBase> & left)
  {
    _left = left;
  }

  /*!
   * Set the navigation pointer for the right sibling
   */
  void SetRight(std::shared_ptr<PlannerBase> & right)
  {
    _right = right;
  }

  /*!
   * Base function for the build operation to create the page
   */
  void Build()
  {
  }

  std::string GetGridString()
  {
    return _grid_string;
  }

  HPDF_Page & GetPage()
  {
    return _page;
  }


  void SetNotesSectionPercentage(double notes_section_percentage)
  {
    _note_section_percentage = notes_section_percentage;
  }

  /*!
   * Function to generate the title of a page. The title of the page when clicked on will navigate to the parent page.
   */
  void CreateTitle()
  {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _page_title_font_size);
    HPDF_Page_SetLineWidth(_page, 1);
    HPDF_REAL page_title_text_x = GetCenteredTextXPosition(_page, _page_title, 0, _page_width);
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page, page_title_text_x, _page_height - _page_title_font_size - 10);
    if(NULL != _parent)
    {
      HPDF_Destination dest = HPDF_Page_CreateDestination(_parent->GetPage());
      HPDF_REAL length = HPDF_Page_TextWidth(_page, _page_title.c_str());
      HPDF_Rect rect = {page_title_text_x , _page_height, page_title_text_x + length, _page_height - (_page_title_font_size * 2) };
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest );
    }
    HPDF_Page_ShowText(_page, _page_title.c_str());
    HPDF_Page_EndText(_page);

  }

  /*!
   * Function to setup the left and right navigation elements of the page.
   */
  void AddNavigation()
  {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _page_title_font_size);
    HPDF_Page_SetLineWidth(_page, 1);
    /* Add navigation to left and right */
    std::string left_string = "<";
    std::string right_string = ">";
    HPDF_REAL page_title_text_x = GetCenteredTextXPosition(_page, _page_title, 0, _page_width);

    /* Add left navigation */
    if(NULL != _left)
    {
      HPDF_Page_BeginText(_page);
      HPDF_Page_MoveTextPos(_page, page_title_text_x - 100, _page_height - _page_title_font_size - 10);
      HPDF_Destination dest = HPDF_Page_CreateDestination(_left->GetPage());
      HPDF_REAL length = HPDF_Page_TextWidth(_page, left_string.c_str());
      HPDF_Rect rect = {page_title_text_x - 100 , _page_height, page_title_text_x - 100 + length, _page_height - (_page_title_font_size * 2) };
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest );
      HPDF_Page_ShowText(_page, left_string.c_str());
      HPDF_Page_EndText(_page);
    }

    /* Add right navigation */
    if(NULL != _right)
    {
      HPDF_REAL title_length = HPDF_Page_TextWidth(_page, _page_title.c_str());
      HPDF_Page_BeginText(_page);
      HPDF_Page_MoveTextPos(_page, page_title_text_x + title_length + 100, _page_height - _page_title_font_size - 10);
      HPDF_Destination dest = HPDF_Page_CreateDestination(_right->GetPage());
      HPDF_REAL length = HPDF_Page_TextWidth(_page, right_string.c_str());
      HPDF_Rect rect = {page_title_text_x + title_length + 100 , _page_height, page_title_text_x + title_length + 100 + length, _page_height - (_page_title_font_size * 2) };
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest );
      HPDF_Page_ShowText(_page, right_string.c_str());
      HPDF_Page_EndText(_page);
    }

  }

  /*!
   * Functin to generate the notes section
   */
  void CreateNotesSection()
  {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _note_title_font_size);
    HPDF_Page_SetLineWidth(_page, 1);

    HPDF_REAL notes_divider_x = _page_width * _note_section_percentage;
    HPDF_Page_MoveTo(_page, notes_divider_x, 0);
    HPDF_Page_LineTo(_page, notes_divider_x, _page_height - _page_title_font_size - 20);
    HPDF_Page_Stroke(_page);

    std::string notes_string = "Notes";
    HPDF_REAL notes_section_text_x = GetCenteredTextXPosition(_page, notes_string, 0, notes_divider_x);
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page, notes_section_text_x, _page_height - _page_title_font_size - _note_title_font_size - 10);
    HPDF_Page_ShowText(_page, notes_string.c_str());
    HPDF_Page_EndText(_page);

    HPDF_Page_SetLineWidth (_page, 1);
    HPDF_Page_MoveTo (_page, 120, 0);
    HPDF_Page_LineTo (_page, 120, _page_height);
    HPDF_Page_Stroke (_page);

    HPDF_REAL dot_spacing = 20;
    /*
     * @TODO : This increases file size a lot, try replacing with a pattern fill
    FillAreaWithDots(
        _page,
        dot_spacing,
        dot_spacing,
        _page_height,
        _page_width,
        95,
        0,
        notes_divider_x,
        _page_height
        );
        */


  }

};

void FillAreaWithDots(
    HPDF_Page &page,
    HPDF_REAL dot_spacing_x,
    HPDF_REAL dot_spacing_y,
    HPDF_REAL page_height,
    HPDF_REAL page_width,
    HPDF_REAL x_start,
    HPDF_REAL y_start,
    HPDF_REAL x_stop,
    HPDF_REAL y_stop
    )
{

  for(HPDF_REAL x = x_start; x < x_stop; x = x + dot_spacing_x)
  {
    for(HPDF_REAL y = y_start; y < y_stop; y = y + dot_spacing_y)
    {
      HPDF_Page_SetLineWidth (page, 0.7);
      HPDF_Page_MoveTo (page, x, page_height - y);
      HPDF_Page_LineTo (page, x + 1, page_height - y + 1);
      HPDF_Page_Stroke (page);
    }
  }
}

/*!
 * @brief
 * Class representing a day page
 */
class PlannerDay:public PlannerBase {
  date::year_month_day _day;
  std::shared_ptr<PlannerBase> _parent_week;
  public:
  PlannerDay(): _day((date::year_month_day) { (date::year)2021, (date::month)1, (date::day)1})
  {
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
  }

  PlannerDay(date::day day, date::month month, date::year year, PlannerBase *parent_week, std::shared_ptr<PlannerBase> parent_month, HPDF_REAL height, HPDF_REAL width, std::string page_title, std::string grid_string) : _day((date::year_month_day) { (date::year)year, (date::month)month, (date::day)day})
  {
    _page_height = height;
    _page_width = width;
    _page_title = page_title;
    _grid_string = grid_string;
    _parent = parent_month;
  }

  void CreateTasksSection(HPDF_Doc &doc)
  {
    std::string year_title_string = "Tasks";
    HPDF_REAL notes_divider_x = _page_width * _note_section_percentage;
    HPDF_REAL years_section_text_x = GetCenteredTextXPosition(_page, year_title_string, notes_divider_x, _page_width );
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page, years_section_text_x, _page_height - _page_title_font_size - _note_title_font_size - 10);
    HPDF_Page_ShowText(_page, year_title_string.c_str());
    HPDF_Page_EndText(_page);

    /* @TODO : This increases filesize a lot, try to replace with pattern fill or background png etc 
    FillAreaWithDots(
        _page,
        40,
        40,
        _page_height,
        _page_width,
        notes_divider_x,
        0,
        _page_width,
        _page_height
        );
        */


  }

  void CreateNavigation(HPDF_Doc &doc)
  {
    AddNavigation();
  }

  void Build(HPDF_Doc &doc)
  {
    CreatePage(doc, _page_height, _page_width);
    CreateTitle();
    CreateNotesSection();
    CreateTasksSection(doc);
  }
};

/*!
 * @brief
 * Class representing a Week page
 */
class PlannerWeek:public PlannerBase  {
  unsigned char _week_num;
  date::year_month _month;
  std::vector<std::shared_ptr<PlannerBase> > _days;
  PlannerBase *_parent_month;
  public:
  PlannerWeek(): _week_num((unsigned char)0), _month((date::year_month){(date::year)2021, (date::month)1})
  {
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
  }

  PlannerWeek(
      unsigned char week,
      date::month month,
      date::year year,
      PlannerBase *parent_month,
      HPDF_REAL height,
      HPDF_REAL width) :
    _week_num(week), _month((date::year_month){(date::year)year, (date::month)month}), _parent_month(parent_month)
  {
    _page_height = height;
    _page_width = width;
  }

  void Build(HPDF_Doc &doc)
  {
    CreatePage(doc, _page_height, _page_width);
    CreateNotesSection();
  }

};

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

/*!
 * @brief
 * Class representing the Year page
 */
class PlannerYear:public PlannerBase  {
  date::year _year;
  std::vector<std::shared_ptr<PlannerBase> > _months;
  public:
  PlannerYear():_year((date::year)2021)
  {
    _page_title = format("%Y", _year);
    _grid_string = format("%Y", _year);
    _page_height = Remarkable_height_px;
    _page_width = Remarkable_width_px;
    _note_section_percentage = 0.25;
  }

  PlannerYear(date::year year, std::shared_ptr<PlannerBase> parent_main,HPDF_REAL height, HPDF_REAL width) : _year( year)
  {
    _page_title = format("%Y", _year);
    _grid_string = format("%Y", _year);
    _page_height = height;
    _page_width = width;
    _note_section_percentage = 0.25;
    _parent = parent_main;
  }

  PlannerYear(short year) : _year((date::year) year) { }

  date::year GetYear()
  {
    return _year;
  }

  void AddMonthsSection(HPDF_Doc &doc)
  {
    std::cout<<"Adding Months Section"<<std::endl;
    CreateGrid(
        doc,
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
        10
        );
  }

  void BuildMonths(HPDF_Doc &doc)
  {
    for(auto month:_months)
    {
      std::static_pointer_cast<PlannerMonth>(month)->Build(doc);
    }
  }

  std::vector<std::shared_ptr<PlannerBase> > & GetMonths()
  {
    return _months;
  }

  void CreateNavigation(HPDF_Doc &doc)
  {
    AddNavigation();
    for(auto month:_months)
    {
      std::static_pointer_cast<PlannerMonth>(month)->CreateNavigation(doc);
    }

  }

  void Build(HPDF_Doc &doc)
  {
    CreatePage(doc, _page_height, _page_width);
    /* Add months to _months and call build on each of them */
    for(size_t month_id = 1; month_id <= 12; month_id++)
    {
      _months.push_back(std::make_shared<PlannerMonth>(PlannerMonth((date::year_month){_year, (date::month) month_id}, shared_from_this(), _page_height, _page_width)));

      std::shared_ptr<PlannerBase> prev_month;

      if(month_id > 1)
      {
        prev_month = _months[month_id - 2];
      }
      else
      {
        if(NULL != _left)
        {
          prev_month = std::static_pointer_cast<PlannerYear>(_left)->GetMonths().back();
        }
      }

      if(NULL != prev_month)
      {
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

/*!
 * @brief
 * The Main Planner page class
 */
class PlannerMain:public PlannerBase   {
  date::year_month_day _base_date;
  std::string _filename;
  std::uint64_t _num_years;
  std::vector<std::shared_ptr<PlannerBase> > _years;
  HPDF_Doc _pdf;
  public:
  PlannerMain():_base_date((date::year)2021, (date::month)1, (date::day)1),_num_years(10), _filename("test.pdf")
  {
    _page_title = "Planner";
  }

  PlannerMain(short year, std::string filename, short num_years, HPDF_REAL height, HPDF_REAL width) :
    _base_date((date::year)year, (date::month)1, (date::day)1), _filename(filename), _num_years(num_years)
  {
    _page_title = "Planner";
    _page_height = height;
    _page_width = width;
  }

  static void err_cb(HPDF_STATUS   error_no,
              HPDF_STATUS   detail_no,
              void         *user_data)
  {
    std::cout<<"[ERR] : PDF operation error : Received error : "<<
                std::hex<<error_no <<
                " with detail : "<<
                detail_no <<
                std::endl;

    throw std::exception();
  }

  void CreateDocument()
  {
    _pdf = HPDF_New(this->err_cb, NULL);
    if(NULL == _pdf)
    {
      std::cout<<"[ERR] Failed to create PDF object"<<std::endl;
      throw std::exception();
    }
  }

  void CreateYearsSection(HPDF_Doc &doc)
  {
    std::string year_title_string = "Years";
    HPDF_REAL notes_divider_x = _page_width * _note_section_percentage;
    HPDF_REAL years_section_text_x = GetCenteredTextXPosition(_page, year_title_string, notes_divider_x, _page_width );
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page, years_section_text_x, _page_height - _page_title_font_size - _note_title_font_size - 10);
    HPDF_Page_ShowText(_page, year_title_string.c_str());
    HPDF_Page_EndText(_page);

    std::cout<<"Adding Years Section"<<std::endl;
    CreateGrid(
        doc,
        _page,
        notes_divider_x + 20,
        115,
        _page_width - 20,
        _page_height - _page_title_font_size - _note_title_font_size - 50,
        _years.size(),
        1,
        _years,
        true,
        0,
        false,
        PlannerTypes_Main,
        PlannerTypes_Year,
        _page_height,
        10
        );

  }

  void BuildYears()
  {
    for(auto year:_years)
    {
      std::static_pointer_cast<PlannerYear>(year)->Build(_pdf);
    }
  }

  void CreateNavigation()
  {
    for(auto year:_years)
    {
      std::static_pointer_cast<PlannerYear>(year)->CreateNavigation(_pdf);
    }
  }

  void Build()
  {
    CreatePage(_pdf, _page_height, _page_width);
    /* Add _num_years of year objects and call their build functions */
    for(size_t loop_index = 0; loop_index < _num_years; loop_index++)
    {
      date::year next_year = _base_date.year() + (date::years)loop_index;
      _years.push_back(std::make_shared<PlannerYear>(PlannerYear(next_year, shared_from_this(), _page_height, _page_width)));
      if(loop_index != 0)
      {
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

  void FinishDocument()
  {
    HPDF_SaveToFile(_pdf, _filename.c_str());
    HPDF_Free(_pdf);
  }
};


/**
 * @brief
 * A helper function to call the instance specific create thumbnail function
 * This is a little hacky and error prone as it relies on the caller to provide the instance type.
 * This might be helped by transitioning to std::any in the base class to store the child objects
 * instead of shared_ptrs to PlannerBase.
 *
 */
void CreateThumbnailCaller(
    HPDF_Doc &doc,
    HPDF_Page &page,
    HPDF_REAL x_start,
    HPDF_REAL y_start,
    HPDF_REAL x_stop,
    HPDF_REAL y_stop,
    PlannerTypes type,
    PlannerTypes object_type,
    std::shared_ptr<PlannerBase> object
    )
{
  switch(object_type)
  {
    case PlannerTypes_Month:
      std::static_pointer_cast<PlannerMonth>(object)->CreateThumbnail(doc, page, x_start, y_start, x_stop, y_stop);
      break;
    default:
      break;
  }
}

/*!
 * Function to create a grid of child elements to be able to navigate to them
 */
void CreateGrid(
    HPDF_Doc &doc,
    HPDF_Page &page,
    HPDF_REAL x_start,
    HPDF_REAL y_start,
    HPDF_REAL x_stop,
    HPDF_REAL y_stop,
    HPDF_REAL num_rows,
    HPDF_REAL num_cols,
    std::vector<std::shared_ptr<PlannerBase> > &objects,
    bool create_annotations,
    size_t first_entry_offset,
    bool create_thumbnail,
    PlannerTypes page_type,
    PlannerTypes object_type,
    HPDF_REAL page_height,
    HPDF_REAL padding
    )
{
    std::cout<< "num_rows: "
             << num_rows
             << ", num_cols : "
             << num_cols
             << ", first_entry_offset : "
             << first_entry_offset
             << ", num objects : "
             << objects.size()
             << std::endl;
  if((first_entry_offset + objects.size()) > (num_rows * num_cols))
  {
    std::cout<< "[ERR] : Too many objects to fit in given grid : num_rows: "
             << num_rows
             << ", num_cols : "
             << num_cols
             << ", first_entry_offset : "
             << first_entry_offset
             << ", num objects : "
             << objects.size()
             << std::endl;
    return;
  }
  HPDF_REAL x_step_size = (x_stop - x_start) / num_cols;
  HPDF_REAL y_step_size = (y_stop - y_start) / num_rows;
  HPDF_Font font = HPDF_GetFont (doc, "Helvetica", NULL);

  HPDF_Page_SetFontAndSize (page, font, 25);
  size_t object_index = 0;

  std::cout<<"Box dimensions : "
           << x_start
           << ", "
           << y_start
           << ", "
           << x_stop
           << ", "
           << y_stop
           <<std::endl;

  size_t row_num = 0;
  for(HPDF_REAL y = y_start; y < y_stop && row_num < num_rows ; y = y + y_step_size, row_num++)
  {
    size_t col_num = 0;
    for(HPDF_REAL x = x_start; x < x_stop && object_index < objects.size() && col_num < num_cols; x = x + x_step_size, col_num++)
    {
      HPDF_REAL x_pad_start = x + padding;
      HPDF_REAL y_pad_start = y + padding;
      HPDF_REAL x_pad_end   = x + x_step_size - padding;
      HPDF_REAL y_pad_end   = y + y_step_size - padding;
      if(first_entry_offset == 0)
      {
        HPDF_Page_BeginText(page);
        HPDF_REAL grid_x_start = GetCenteredTextXPosition(page, objects[object_index]->GetGridString(), x_pad_start, x_pad_end);
        HPDF_REAL grid_y_start = page_height - y_pad_start - 30;//GetCenteredTextYPosition(page, GetGridString(), y_stop - y, y_stop - y - y_step_size);
        HPDF_Page_MoveTextPos(page, grid_x_start, grid_y_start);

        if(true == create_annotations)
        {
          HPDF_Destination dest = HPDF_Page_CreateDestination(objects[object_index]->GetPage());
          HPDF_REAL rect_y_end = page_height - y_pad_end;
          if(true == create_thumbnail)
          {
            rect_y_end = page_height - y_pad_start - 50;
          }
          HPDF_Rect rect = {x_pad_start, rect_y_end, x_pad_end, page_height - y_pad_start };
          HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(page, rect, dest );
        }

        HPDF_Page_ShowText(page, objects[object_index]->GetGridString().c_str());
        HPDF_Page_EndText(page);

        if(true == create_thumbnail)
        {
          std::cout<<objects[object_index]->GetGridString()<<std::endl;
          std::cout<<"Calling thumbnail caller"<<std::endl;
          CreateThumbnailCaller(doc, page, x_pad_start, y_pad_start, x_pad_end, y_pad_end, page_type, object_type, objects[object_index] );
        }

        object_index++;
      }
      else
      {
        first_entry_offset--;
      }
    }
  }
}

/**!
 * Main function to generate the file.
 */
int main(int argc, char *argv[])
{
  short start_year = 2021;
  short num_years  = 5;
  std::string filename = "planner.pdf";

  if(argc > 1)
  {
    int start_year_cl = atoi(argv[1]);
    if( (start_year_cl != 0) && (start_year_cl < 3000))
    {
      start_year = start_year_cl;
    }
  }

  if(argc > 2)
  {
    int num_years_cl = atoi(argv[2]);
    if( (num_years_cl > 0) && (num_years_cl < 100) )
    {
      num_years = num_years_cl;
    }
  }

  if(argc > 3)
  {
    filename = argv[3];
  }

  auto Test = std::make_shared<PlannerMain>(PlannerMain(start_year, filename, num_years, Remarkable_height_px, Remarkable_width_px));
  Test->CreateDocument();
  Test->Build();
  Test->FinishDocument();
  return 0;
}
