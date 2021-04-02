#include "planner_base.hpp"
#ifndef MAIN_HPP
#define MAIN_HPP
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

#endif //MAIN_HPP
