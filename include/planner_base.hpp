#ifndef PLANNER_BASE_HPP
#define PLANNER_BASE_HPP
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
#include "date.h"
#include "hpdf.h"
#include "utils.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>

#define FILL_BLACK 0.0
#define FILL_TITLE 0.8
#define FILL_LIGHT 0.9
#define FILL_DARK 0.5

/*!
 * @brief
 * Base class for the different Planner elements
 *
 */
class PlannerBase : public std::enable_shared_from_this<PlannerBase> {
protected:
  std::uint64_t _id;

  /*! What section of the page is reserved for notes */
  double _note_section_percentage;

  /*! representing the PDF Page that this object is controlling */
  HPDF_Page _page;

  /*! The font used for the notes header */
  HPDF_Font _notes_font;

  /*! The height of the page */
  HPDF_REAL _page_height;

  /*! The width of the page */
  HPDF_REAL _page_width;

  /*! The left margin of the page */
  HPDF_REAL _margin_width;

  /*! The left margin of the page */
  HPDF_REAL _margin_left;

  /*! The left margin of the page */
  HPDF_REAL _margin_right;

  /*! A string representing the title of the page */
  std::string _page_title;

  /*! A string represeneting the title that appears when this object is
   * displayed in a grid of a parent page */
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

  /*! Whether it shouold be left handed orientation */
  bool _is_left_handed;

  /*! Whether it should be portrait / landscape orientation */
  bool _is_portrait;

public:
  PlannerBase()
      : _id(0), _note_section_percentage(0.5), _page_title("Base"),
        _page_title_font_size(45), _note_title_font_size(35),
        _grid_string("GridBase"), _margin_width(Remarkable_margin_width_px),
        _is_left_handed(false), _page_width(Remarkable_width_px),
        _page_height(Remarkable_height_px), _is_portrait(false) {
    _margin_left = _margin_width;
    _margin_right = _page_width - _margin_width;
  }

  PlannerBase(std::string grid_string, bool is_left_handed)
      : _id(0), _note_section_percentage(0.5), _page_title("Base"),
        _page_title_font_size(45), _note_title_font_size(35),
        _grid_string(grid_string), _margin_width((Remarkable_margin_width_px)),
        _is_left_handed(is_left_handed), _page_width(Remarkable_width_px),
        _page_height(Remarkable_height_px), _is_portrait(false) {
    _margin_left = _margin_width;
    _margin_right = _page_width - _margin_width;
  }

  void SetGridString(std::string grid_string) { _grid_string = grid_string; }

  void CreateThumbnail(HPDF_Doc& doc,
                       HPDF_Page& page,
                       HPDF_REAL x_start,
                       HPDF_REAL y_start,
                       HPDF_REAL x_stop,
                       HPDF_REAL y_stop) {}

  void FillAreaWithDots(HPDF_Page& page,
                        HPDF_REAL dot_spacing_x,
                        HPDF_REAL dot_spacing_y,
                        HPDF_REAL page_height,
                        HPDF_REAL page_width,
                        HPDF_REAL x_start,
                        HPDF_REAL y_start,
                        HPDF_REAL x_stop,
                        HPDF_REAL y_stop) {

    HPDF_REAL x = x_start;
    const HPDF_UINT16 DASH_MODE1[] = {2, HPDF_UINT16(dot_spacing_x)};

    for (HPDF_REAL y = y_start; y < y_stop; y = y + dot_spacing_y) {
      HPDF_Page_SetLineWidth(page, 2);
      HPDF_Page_SetDash(page, DASH_MODE1, 2, 0);
      HPDF_Page_MoveTo(page, x, page_height - y);
      HPDF_Page_LineTo(page, x_stop, page_height - y);
      HPDF_Page_Stroke(page);
    }
    HPDF_Page_SetDash(page, NULL, 0, 0);
  }

  void FillAreaWithLines(HPDF_Page& page,
                         bool is_vertical_line,
                         HPDF_REAL area_x_start,
                         HPDF_REAL area_y_start,
                         HPDF_REAL area_x_stop,
                         HPDF_REAL area_y_stop,
                         HPDF_REAL line_gap,
                         HPDF_REAL page_height) {
    HPDF_REAL dim_start;
    HPDF_REAL dim_stop;

    if (is_vertical_line) {
      dim_start = area_x_start;
      dim_stop = area_x_stop;
    } else {
      dim_start = area_y_start;
      dim_stop = area_y_stop;
    }

    HPDF_Page_SetLineWidth(page, 0.5);
    HPDF_Page_SetGrayStroke(page, 0.5);
    for (HPDF_REAL dim = dim_start; dim < dim_stop; dim = dim + line_gap) {
      if (is_vertical_line) {
        HPDF_Page_MoveTo(page, dim, page_height - area_y_start);
        HPDF_Page_LineTo(page, dim, page_height - area_y_stop);
      } else {
        HPDF_Page_MoveTo(page, area_x_start, page_height - dim);
        HPDF_Page_LineTo(page, area_x_stop, page_height - dim);
      }
      HPDF_Page_Stroke(page);
    }
  }
  /*!
   * @brief
   * Create the page for this object with the given height and width
   */
  void CreatePage(HPDF_Doc doc, std::uint64_t height, std::uint64_t width) {
    _page = HPDF_AddPage(doc);

    _notes_font = HPDF_GetFont(doc, "Helvetica", NULL);
    HPDF_Page_SetHeight(_page, height);
    _page_height = height;
    HPDF_Page_SetWidth(_page, width);
    _page_width = width;
    /* Draw the margin */
    HPDF_REAL margin_x;
    if (_is_left_handed) {
      margin_x = _margin_right;
    } else {
      margin_x = _margin_left;
    }
    HPDF_Page_SetLineWidth(_page, 1);
    HPDF_Page_MoveTo(_page, margin_x, 0);
    HPDF_Page_LineTo(_page, margin_x, _page_height);
    HPDF_Page_Stroke(_page);
  }

  /*!
   * Set the navigation pointer for left sibling
   */
  void SetLeft(std::shared_ptr<PlannerBase>& left) { _left = left; }

  /*!
   * Set the navigation pointer for the right sibling
   */
  void SetRight(std::shared_ptr<PlannerBase>& right) { _right = right; }

  /*!
   * Base function for the build operation to create the page
   */
  void Build() {}

  std::string GetGridString() { return _grid_string; }

  HPDF_Page& GetPage() { return _page; }

  void SetNotesSectionPercentage(double notes_section_percentage) {
    _note_section_percentage = notes_section_percentage;
  }

  /*!
   * Function to paint the background of a link/anchor.
   */
  void PaintRect(HPDF_Page& page,
                 HPDF_REAL page_height,
                 HPDF_REAL rect_x_start,
                 HPDF_REAL rect_y_start,
                 HPDF_REAL rect_x_stop,
                 HPDF_REAL rect_y_stop,
                 HPDF_REAL padding_x,
                 HPDF_REAL padding_y,
                 HPDF_REAL gray) {
    HPDF_REAL grayfill = HPDF_Page_GetGrayFill(page);
    HPDF_Page_SetGrayFill(page, gray);
    HPDF_STATUS status = HPDF_Page_Rectangle(page,
                                             rect_x_start - padding_x,
                                             page_height - (rect_y_stop + padding_y),
                                             rect_x_stop - rect_x_start + (2 * padding_x),
                                             rect_y_stop - rect_y_start + (2 * padding_y));
    HPDF_Page_Fill(page);
    HPDF_Page_SetGrayFill(page, grayfill);
  }

  /*!
   * Function to generate the title of a page. The title of the page when
   * clicked on will navigate to the parent page.
   */
  void CreateTitle() {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _page_title_font_size);
    HPDF_Page_SetLineWidth(_page, 1);
    HPDF_REAL page_title_text_x =
        GetCenteredTextXPosition(_page, _page_title, 0, _page_width);
    HPDF_REAL length = HPDF_Page_TextWidth(_page, _page_title.c_str());
    HPDF_REAL x_padding = 20;
    HPDF_REAL y_padding = 0;

    PaintRect(_page,
              _page_height,
              page_title_text_x,
              0,
              page_title_text_x + length,
              _page_title_font_size * 2,
              x_padding,
              y_padding,
              FILL_TITLE);

    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(
        _page, page_title_text_x, _page_height - _page_title_font_size - 10);
    if (NULL != _parent) {
      HPDF_Destination dest = HPDF_Page_CreateDestination(_parent->GetPage());
      HPDF_Rect rect = {page_title_text_x - x_padding,
                        _page_height - y_padding,
                        page_title_text_x + length + x_padding,
                        _page_height - ((_page_title_font_size * 2) + y_padding)};
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest);
    }
    HPDF_Page_ShowText(_page, _page_title.c_str());
    HPDF_Page_EndText(_page);

    DrawTitleSeparator();

  }

  void DrawTitleSeparator() {
    HPDF_Page_SetLineWidth(_page, 2);
    HPDF_Page_MoveTo(_page, 0, _page_height - (_page_title_font_size * 2));
    HPDF_Page_LineTo(
        _page, _page_width, _page_height - (_page_title_font_size * 2));
    HPDF_Page_Stroke(_page);
  }

  /*!
   * Function to setup the left and right navigation elements of the page.
   */
  void AddNavigation() {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _page_title_font_size);
    HPDF_Page_SetLineWidth(_page, 1);
    /* Add navigation to left and right */
    std::string left_string = "<";
    std::string right_string = ">";
    HPDF_REAL page_title_text_x =
        GetCenteredTextXPosition(_page, _page_title, 0, _page_width);
    HPDF_REAL x_padding = 20;
    HPDF_REAL y_padding = 0;

    /* Add left navigation */
    if (NULL != _left) {
      HPDF_REAL length = HPDF_Page_TextWidth(_page, left_string.c_str());

      PaintRect(_page,
                _page_height,
                page_title_text_x - 100,
                0,
                page_title_text_x - 100 + length,
                (_page_title_font_size * 2),
                x_padding,
                y_padding,
                FILL_TITLE);

      HPDF_Page_BeginText(_page);
      HPDF_Page_MoveTextPos(_page,
                            page_title_text_x - 100,
                            _page_height - _page_title_font_size - 10);
      HPDF_Destination dest = HPDF_Page_CreateDestination(_left->GetPage());
      HPDF_Rect rect = {page_title_text_x - 100 - x_padding,
                        _page_height - y_padding,
                        page_title_text_x - 100 + length + x_padding,
                        _page_height - ((_page_title_font_size * 2) + y_padding)};
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest);
      HPDF_Page_ShowText(_page, left_string.c_str());
      HPDF_Page_EndText(_page);
    }

    /* Add right navigation */
    if (NULL != _right) {
      HPDF_REAL title_length = HPDF_Page_TextWidth(_page, _page_title.c_str());
      HPDF_REAL length = HPDF_Page_TextWidth(_page, right_string.c_str());

      PaintRect(_page,
                _page_height,
                page_title_text_x + title_length + 100 - length,
                0,
                page_title_text_x + title_length + 100,
                (_page_title_font_size * 2),
                x_padding,
                y_padding,
                FILL_TITLE);
      HPDF_Page_BeginText(_page);
      HPDF_Page_MoveTextPos(_page,
                            page_title_text_x + title_length + 100 - length,
                            _page_height - _page_title_font_size - 10);
      HPDF_Destination dest = HPDF_Page_CreateDestination(_right->GetPage());
      HPDF_Rect rect = {page_title_text_x + title_length + 100 - length - x_padding,
                        _page_height - y_padding,
                        page_title_text_x + title_length + 100 + x_padding,
                        _page_height - ((_page_title_font_size * 2) + y_padding)};
      HPDF_Annotation annotation = HPDF_Page_CreateLinkAnnot(_page, rect, dest);
      HPDF_Page_ShowText(_page, right_string.c_str());
      HPDF_Page_EndText(_page);
    }
    DrawTitleSeparator();
  }

  /*!
   * Functin to generate the notes section
   */
  void CreateNotesSection() {
    HPDF_Page_SetFontAndSize(_page, _notes_font, _note_title_font_size);
    HPDF_Page_SetLineWidth(_page, 2);

    HPDF_REAL notes_divider_x_width;
    HPDF_REAL notes_section_text_x;
    HPDF_REAL notes_x_start;
    HPDF_REAL notes_y_start;
    HPDF_REAL notes_x_stop;
    HPDF_REAL notes_y_stop;
    HPDF_REAL margin_x;
    HPDF_REAL divider_location_x;
    std::string notes_string = "Notes";

    notes_divider_x_width = _page_width * _note_section_percentage;

    if (_is_left_handed) {
      notes_x_start = _page_width - notes_divider_x_width;
      notes_y_start = 2 * _page_title_font_size;
      notes_x_stop = _page_width;
      notes_y_stop = _page_height;
      margin_x = _margin_right;
      divider_location_x = notes_x_start;
      notes_section_text_x = GetCenteredTextXPosition(
          _page, notes_string, notes_x_start, _margin_right);
    } else {
      notes_x_start = 0;
      notes_y_start = 2 * _page_title_font_size;
      notes_x_stop = notes_divider_x_width;
      notes_y_stop = _page_height;
      margin_x = _margin_left;
      divider_location_x = notes_x_stop;
      notes_section_text_x = GetCenteredTextXPosition(
          _page, notes_string, _margin_left, notes_divider_x_width);
    }

    /* Draw dividing line between notes section and the rest of the page */
    HPDF_Page_MoveTo(_page, divider_location_x, 0);
    HPDF_Page_LineTo(_page, divider_location_x, _page_height - notes_y_start);
    HPDF_Page_Stroke(_page);

    /* Print Notes section title */
    HPDF_Page_BeginText(_page);
    HPDF_Page_MoveTextPos(_page,
                          notes_section_text_x,
                          _page_height - notes_y_start - _note_title_font_size -
                              10);
    HPDF_Page_ShowText(_page, notes_string.c_str());
    HPDF_Page_EndText(_page);

    HPDF_REAL dot_spacing = 40;

    FillAreaWithLines(_page,
                      false,
                      notes_x_start,
                      notes_y_start + (2 * _note_title_font_size),
                      notes_x_stop,
                      notes_y_stop - 30,
                      dot_spacing,
                      _page_height);
  }

  /*!
   * Function to create a grid of child elements to be able to navigate to them
   */
  void CreateGrid(HPDF_Doc& doc,
                  HPDF_Page& page,
                  HPDF_REAL x_start,
                  HPDF_REAL y_start,
                  HPDF_REAL x_stop,
                  HPDF_REAL y_stop,
                  HPDF_REAL num_rows,
                  HPDF_REAL num_cols,
                  std::vector<std::shared_ptr<PlannerBase>>& objects,
                  bool create_annotations,
                  size_t first_entry_offset,
                  bool create_thumbnail,
                  PlannerTypes page_type,
                  PlannerTypes object_type,
                  HPDF_REAL page_height,
                  HPDF_REAL padding,
                  bool grid_string_in_middle) {
    if ((first_entry_offset + objects.size()) > (num_rows * num_cols)) {
      std::cout << "[ERR] : Too many objects to fit in given grid : num_rows: "
                << num_rows << ", num_cols : " << num_cols
                << ", first_entry_offset : " << first_entry_offset
                << ", num objects : " << objects.size() << std::endl;
      return;
    }
    HPDF_REAL x_step_size = (x_stop - x_start) / num_cols;
    HPDF_REAL y_step_size = (y_stop - y_start) / num_rows;
    HPDF_Font font = HPDF_GetFont(doc, "Helvetica", NULL);

    HPDF_Page_SetFontAndSize(page, font, 25);
    size_t object_index = 0;

    size_t row_num = 0;
    for (HPDF_REAL y = y_start; y < y_stop && row_num < num_rows;
         y = y + y_step_size, row_num++) {
      size_t col_num = 0;
      for (HPDF_REAL x = x_start;
           x < x_stop && object_index < objects.size() && col_num < num_cols;
           x = x + x_step_size, col_num++) {
        HPDF_REAL x_pad_start = x + padding;
        HPDF_REAL y_pad_start = y + padding;
        HPDF_REAL x_pad_end = x + x_step_size - padding;
        HPDF_REAL y_pad_end = y + y_step_size - padding;
        if (first_entry_offset == 0) {

          if (true == create_annotations) {
            HPDF_REAL paint_rect_y_end = y_pad_end;
            if (true == create_thumbnail) {
              paint_rect_y_end = y_pad_start + 50;
            }
            PaintRect(page,
                      page_height,
                      x_pad_start,
                      y_pad_start,
                      x_pad_end,
                      paint_rect_y_end,
                      0,
                      0,
                      FILL_LIGHT);
          }

          HPDF_Page_BeginText(page);
          HPDF_REAL grid_x_start =
              GetCenteredTextXPosition(page,
                                       objects[object_index]->GetGridString(),
                                       x_pad_start,
                                       x_pad_end);
          HPDF_REAL grid_y_start = y_pad_start + 30;
          if (true == grid_string_in_middle) {
            grid_y_start = GetCenteredTextYPosition(
                page, GetGridString(), grid_y_start, y_pad_end);
          }

          HPDF_Page_MoveTextPos(
              page, grid_x_start, _page_height - grid_y_start);

          if (true == create_annotations) {
            HPDF_Destination dest =
                HPDF_Page_CreateDestination(objects[object_index]->GetPage());
            HPDF_REAL rect_y_end = page_height - y_pad_end;
            if (true == create_thumbnail) {
              // To avoid the link covering the entire thumbnail, limit
              // annotation rect height to 50 pixels
              // TODO: Replace 50 with actual height of title of thumbnail
              rect_y_end = page_height - (y_pad_start + 50);
            }
            HPDF_Rect rect = {
                x_pad_start, rect_y_end, x_pad_end, page_height - y_pad_start};
            HPDF_Annotation annotation =
                HPDF_Page_CreateLinkAnnot(page, rect, dest);
          }

          HPDF_Page_ShowText(page,
                             objects[object_index]->GetGridString().c_str());
          HPDF_Page_EndText(page);

          if (true == create_thumbnail) {
            CreateThumbnailCaller(doc,
                                  page,
                                  x_pad_start,
                                  y_pad_start,
                                  x_pad_end,
                                  y_pad_end,
                                  page_type,
                                  object_type,
                                  objects[object_index]);
          }

          object_index++;
        } else {
          first_entry_offset--;
        }
      }
    }

    /* Draw grid lines in the middle of the padding */
    for (size_t row_num = 0; row_num < num_rows; row_num++) {
      HPDF_REAL x_line_start = x_start;
      HPDF_REAL x_line_stop = x_stop;

      HPDF_REAL y_line_start = y_start + row_num * y_step_size;
      HPDF_REAL y_line_stop = y_start + row_num * y_step_size;

      HPDF_Page_SetLineWidth(page, 2);
      HPDF_Page_MoveTo(page, x_line_start, page_height - y_line_start);
      HPDF_Page_LineTo(page, x_line_stop, page_height - y_line_stop);
      HPDF_Page_Stroke(page);
    }

    for (size_t col_num = 1; col_num < num_cols; col_num++) {
      HPDF_REAL x_line_start = x_start + x_step_size * col_num;
      HPDF_REAL x_line_stop = x_start + x_step_size * col_num;

      HPDF_REAL y_line_start = y_start;
      HPDF_REAL y_line_stop = y_stop;

      HPDF_Page_SetLineWidth(page, 1);
      HPDF_Page_MoveTo(page, x_line_start, page_height - y_line_start);
      HPDF_Page_LineTo(page, x_line_stop, page_height - y_line_stop);
      HPDF_Page_Stroke(page);
    }
  }
};
#endif // PLANNER_BASE_HPP
