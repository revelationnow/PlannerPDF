#include "hpdf.h"
#include <iostream>
#include <memory>

#ifndef UTILS_HPP
#define UTILS_HPP
enum PlannerTypes {
  PlannerTypes_Base,
  PlannerTypes_Day,
  PlannerTypes_Week,
  PlannerTypes_Month,
  PlannerTypes_Year,
  PlannerTypes_Main,
};

const std::int64_t Remarkable_width_px = 1872;
const std::int64_t Remarkable_height_px = 1404;
HPDF_REAL GetCenteredTextYPosition(HPDF_Page& page, std::string text,
                                   HPDF_REAL y_start, HPDF_REAL y_end);
HPDF_REAL GetCenteredTextXPosition(HPDF_Page& page, std::string text,
                                   HPDF_REAL x_start, HPDF_REAL x_end);
/**
 * @brief
 * A helper function to call the instance specific create thumbnail function
 * This is a little hacky and error prone as it relies on the caller to provide
 * the instance type. This might be helped by transitioning to std::any in the
 * base class to store the child objects instead of shared_ptrs to PlannerBase.
 *
 */
class PlannerBase;
void CreateThumbnailCaller(HPDF_Doc& doc, HPDF_Page& page, HPDF_REAL x_start,
                           HPDF_REAL y_start, HPDF_REAL x_stop,
                           HPDF_REAL y_stop, PlannerTypes type,
                           PlannerTypes object_type,
                           std::shared_ptr<PlannerBase> object);

#endif // UTILS_HPP
