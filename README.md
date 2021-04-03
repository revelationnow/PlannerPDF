# Planner PDF
This will generate a PDF File that can be used as a planner on the remarkable platform.<br>
It uses the libharu library to generate the PDF and the Date library to perform calendar math.

# How to compile
Make sure you have libharu development library installed. Either compiled from source or from your package manager.

    sudo apt-get install libhpdf-dev

The libharu github page is at:

  https://github.com/libharu/libharu

The date library is part of this repository. It has its own license and is obtained from:

  https://github.com/HowardHinnant/date


# Compilation
Clone the repository then create a build directory and run cmake followed by make.

    git clone https://github.com/revelationnow/PlannerPDF.git
    cd PlannerPDF
    mkdir build
    cd build
    cmake ..
    make
    ./Planner_PDF

# Usage
To use the PDF copy it to the remarkable device. <br>
The navigation from child page to parent page is done by clicking on the title page.<br>
Clicking on the left or right arrows will allow navigation to previous and next pages for days, months or years.
