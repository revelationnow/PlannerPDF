
if [ ! -d build ]; then
  mkdir build
fi

cd build

cmake -DNUM_YEARS=1 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=1 -DPlanner_PDF_Left_Handed=0 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=5 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=1 -DPlanner_PDF_Left_Handed=0 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=1 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=1 -DPlanner_PDF_Left_Handed=1 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=5 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=1 -DPlanner_PDF_Left_Handed=1 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=5 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=0 -DPlanner_PDF_Left_Handed=1 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=1 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=0 -DPlanner_PDF_Left_Handed=1 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=1 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=0 -DPlanner_PDF_Left_Handed=0 ..
make update_samples
make update_compressed_samples

cmake -DNUM_YEARS=5 -DPDF_FILENAME=calendar -DSTART_YEAR=2021 -DCOMPRESSED_FILE=calendar_small -DPlanner_PDF_Start_Day=0 -DPlanner_PDF_Left_Handed=0 ..
make update_samples
make update_compressed_samples
