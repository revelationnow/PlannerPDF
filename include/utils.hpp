#include <iostream>
#include "hpdf.h"

#ifndef UTILS_HPP
#define UTILS_HPP
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
HPDF_REAL GetCenteredTextYPosition(HPDF_Page &page, std::string text, HPDF_REAL y_start, HPDF_REAL y_end);
HPDF_REAL GetCenteredTextXPosition(HPDF_Page &page, std::string text, HPDF_REAL x_start, HPDF_REAL x_end);
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
#endif //UTILS_HPP
