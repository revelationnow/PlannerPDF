#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
#include "hpdf.h"
#include "date.h"
#include "utils.hpp"


#ifndef PLANNER_BASE_HPP
#define PLANNER_BASE_HPP

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
#endif //PLANNER_BASE_HPP
