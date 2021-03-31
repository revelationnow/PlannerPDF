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
#include "date.h"
#include "hpdf.h"

const std::int64_t Remarkable_width_px = 1872;
const std::int64_t Remarkable_height_px = 1404;

class PlannerBase : public std::enable_shared_from_this<PlannerBase> {
  protected:
  std::uint64_t _id;
  std::double_t _note_section_percentage;
  HPDF_Page _page;
  HPDF_Font _notes_font;
  HPDF_REAL _page_height;
  HPDF_REAL _page_width;
  std::string _page_title;
  std::string _grid_string;
  HPDF_REAL _page_title_font_size;
  HPDF_REAL _note_title_font_size;
  std::shared_ptr<PlannerBase> _parent;
  std::shared_ptr<PlannerBase> _left;
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

  void CreatePage(HPDF_Doc doc, std::uint64_t height, std::uint64_t width)
  {
    _page = HPDF_AddPage(doc);
    _notes_font = HPDF_GetFont(doc, "Helvetica", NULL);
    HPDF_Page_SetHeight(_page, height);
    _page_height = height;
    HPDF_Page_SetWidth(_page, width);
    _page_width = width;
  }

  void SetLeft(std::shared_ptr<PlannerBase> & left)
  {
    _left = left;
  }

  void SetRight(std::shared_ptr<PlannerBase> & right)
  {
    _right = right;
  }

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

  void CreateGrid(
      HPDF_Doc &doc,
      HPDF_REAL x_start,
      HPDF_REAL y_start,
      HPDF_REAL x_stop,
      HPDF_REAL y_stop,
      HPDF_REAL num_rows,
      HPDF_REAL num_cols,
      std::vector<std::shared_ptr<PlannerBase> > &objects,
      bool create_annotations,
      size_t first_entry_offset
      )
  {
    if((first_entry_offset + objects.size()) > (num_rows * num_cols))
    {
      std::cout<<"[ERR] : Too many objects to fit in given grid"<<std::endl;
      return;
    }
    HPDF_REAL x_step_size = (x_stop - x_start) / num_cols;
    HPDF_REAL y_step_size = (y_stop - y_start) / num_rows;
    HPDF_Font font = HPDF_GetFont (doc, "Helvetica", NULL);

    HPDF_Page_SetFontAndSize (_page, font, 25);
    size_t object_index = 0;

    for(HPDF_REAL y = y_start; y < y_stop ; y = y + y_step_size)
    {
      for(HPDF_REAL x = x_start; x < x_stop && object_index < objects.size(); x = x + x_step_size)
      {
        if(first_entry_offset == 0)
        {
          HPDF_Page_BeginText(_page);
          HPDF_REAL grid_x_start = GetCenteredTextXPosition(_page, GetGridString(), x, x + x_step_size);
          HPDF_REAL grid_y_start = y_stop - y - 30;//GetCenteredTextYPosition(_page, GetGridString(), y_stop - y, y_stop - y - y_step_size);
          HPDF_Page_MoveTextPos(_page, grid_x_start, grid_y_start);

          if(true == create_annotations)
          {
            HPDF_Destination dest = HPDF_Page_CreateDestination(objects[object_index]->GetPage());
            HPDF_Rect rect = {x, y_stop - y - y_step_size, x + x_step_size, y_stop - y };
            HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest );
          }

          HPDF_Page_ShowText(_page, objects[object_index]->GetGridString().c_str());
          HPDF_Page_EndText(_page);

          object_index++;
        }
        else
        {
          first_entry_offset--;
        }
      }
    }

  }

  void SetNotesSectionPercentage(double notes_section_percentage)
  {
    _note_section_percentage = notes_section_percentage;
  }

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

  void AddNavigation()
  {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _page_title_font_size);
    HPDF_Page_SetLineWidth(_page, 1);
    /* Add navigation to left and right */
    std::string left_string = "<-";
    std::string right_string = "->";
    HPDF_Page_BeginText(_page);
    HPDF_REAL page_title_text_x = GetCenteredTextXPosition(_page, _page_title, 0, _page_width);
    HPDF_Page_MoveTextPos(_page, page_title_text_x - 100, _page_height - _page_title_font_size - 10);
    if(NULL != _left)
    {
      HPDF_Destination dest = HPDF_Page_CreateDestination(_left->GetPage());
      HPDF_REAL length = HPDF_Page_TextWidth(_page, left_string.c_str());
      HPDF_Rect rect = {page_title_text_x - 100 , _page_height, page_title_text_x - 100 + length, _page_height - (_page_title_font_size * 2) };
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest );
    }
    HPDF_Page_ShowText(_page, left_string.c_str());
    HPDF_Page_EndText(_page);

    HPDF_REAL title_length = HPDF_Page_TextWidth(_page, _page_title.c_str());
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page, page_title_text_x + title_length + 100, _page_height - _page_title_font_size - 10);
    if(NULL != _right)
    {
      HPDF_Destination dest = HPDF_Page_CreateDestination(_right->GetPage());
      HPDF_REAL length = HPDF_Page_TextWidth(_page, right_string.c_str());
      HPDF_Rect rect = {page_title_text_x + title_length + 100 , _page_height, page_title_text_x + title_length + 100 + length, _page_height - (_page_title_font_size * 2) };
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest );
    }
    HPDF_Page_ShowText(_page, right_string.c_str());
    HPDF_Page_EndText(_page);

  }
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

  }

};

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

  void CreateWeekdayHeader(HPDF_Doc &doc)
  {
    std::vector<std::shared_ptr<PlannerBase> > weekdays;
    date::weekday weekday;
    for(size_t i = 0; i < 7; i++)
    {
      weekday = (date::weekday)i;
      std::string weekday_name = format("%a", weekday);
      weekdays.push_back(std::make_shared<PlannerBase>(PlannerBase(weekday_name)));
    }

    CreateGrid
      (
       doc,
       _page_width * _note_section_percentage + 30,
       45,
       _page_width - 30,
       _page_height - 65,
       1,
       7,
       weekdays,
       false,
       0
      );
  }

  std::vector<std::shared_ptr<PlannerBase> > & GetDays()
  {
    return _days;
  }

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


  void AddDaysSection(HPDF_Doc &doc)
  {

    date::year_month_day first_day = date::year(_month.year())/_month.month()/1;
    CreateGrid(
        doc,
       _page_width * _note_section_percentage + 30,
       45,
       _page_width - 30,
       _page_height - 105,
        6,
        7,
        _days,
        true,
        date::weekday{first_day}.c_encoding()
        );

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
    CreateWeekdayHeader(doc);
    AddDays();
    BuildDays(doc);
    AddDaysSection(doc);
    CreateNotesSection();
  }
};

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
    CreateGrid(
        doc,
        _page_width * _note_section_percentage + 15,
        45,
        _page_width - 15,
        _page_height - 45,
        3,
        4,
        _months,
        true,
        0
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

    CreateGrid(
        doc,
        notes_divider_x + 20,
        35,
        _page_width - 20,
        _page_height - _page_title_font_size - _note_title_font_size - 50,
        _years.size(),
        1,
        _years,
        true,
        0
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

int main(int argc, char *argv[])
{
  HPDF_Doc  pdf;
  HPDF_Page page;

  pdf = HPDF_New(NULL, NULL);
  if (NULL == pdf)
  {
    std::cout<<"Failed to create PDF object"<<std::endl;
    return 1;
  }

  page = HPDF_AddPage(pdf);
  HPDF_Page_SetHeight(page, Remarkable_height_px);
  HPDF_Page_SetWidth(page, Remarkable_width_px);

  HPDF_SaveToFile(pdf, "test.pdf");
  HPDF_Free(pdf);

  auto Test = std::make_shared<PlannerMain>(PlannerMain(2021, "planner.pdf", 5, Remarkable_height_px, Remarkable_width_px));
  Test->CreateDocument();
  Test->Build();
  Test->FinishDocument();



  return 0;

}
