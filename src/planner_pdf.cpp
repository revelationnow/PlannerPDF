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
#include "planner_main.hpp"
#include "utils.hpp"


HPDF_REAL GetCenteredTextYPosition(HPDF_Page &page, std::string text, HPDF_REAL y_start, HPDF_REAL y_end)
{
  HPDF_REAL height = HPDF_Page_GetCurrentFontSize(page);
  return y_start + ((y_end - y_start)/2) - height/2;
}

HPDF_REAL GetCenteredTextXPosition(HPDF_Page &page, std::string text, HPDF_REAL x_start, HPDF_REAL x_end)
{
  HPDF_REAL length = HPDF_Page_TextWidth(page, text.c_str());
  return x_start + ((x_end - x_start)/2) - length/2;
}





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

  /* Draw grid lines in the middle of the padding */
  for(size_t row_num = 0; row_num < num_rows; row_num++)
  {
    HPDF_REAL x_line_start = x_start;
    HPDF_REAL x_line_stop  = x_stop;

    HPDF_REAL y_line_start = y_start + row_num * y_step_size;
    HPDF_REAL y_line_stop  = y_start + row_num * y_step_size;

    HPDF_Page_SetLineWidth (page, 2);
    HPDF_Page_MoveTo (page, x_line_start, page_height - y_line_start);
    HPDF_Page_LineTo (page, x_line_stop, page_height - y_line_stop);
    HPDF_Page_Stroke (page);

  }

  for(size_t col_num = 1; col_num < num_cols; col_num++)
  {
    HPDF_REAL x_line_start = x_start + x_step_size * col_num;
    HPDF_REAL x_line_stop  = x_start + x_step_size * col_num;

    HPDF_REAL y_line_start = y_start;
    HPDF_REAL y_line_stop  = y_stop;

    HPDF_Page_SetLineWidth (page, 1);
    HPDF_Page_MoveTo (page, x_line_start, page_height - y_line_start);
    HPDF_Page_LineTo (page, x_line_stop, page_height - y_line_stop);
    HPDF_Page_Stroke (page);

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
