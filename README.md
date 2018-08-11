# First part of concurrency course.

Slides are availible at [Google Drive](https://drive.google.com/file/d/1UA6YsGyvwy9VuBAzAq9drmwaBanJy-td/view?usp=sharing)

## How to build with Visual Studio 2017 or later

* Open `concurrencycourse.sln`
* Build solution
* Enjoy

## How to build with CMake

* Ensure that CMake 3.11 or higher installed
    * or change `cmake_minimum_required` in CMakeLists.txt
* Create directory  in repository folder: `mkdir ~output && cd ~output`
* Configure with `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`
* Build with `cmake --build . -- -j4`
    * or just `--cmake --build .`
