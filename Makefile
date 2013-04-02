
CC=g++
NVCC=nvcc
NVFLAGS=-O3 --profile -arch=compute_20 -code=sm_20 -Xptxas -dlcm=ca 
PROJECT_ROOT=${PWD}
OPENCV_SRC_PATH=${PROJECT_ROOT}/opencv
OPENCV_LIB_PATH=${OPENCV_SRC_PATH}/release
OPENCV_INCLUDE_PATH=${OPENCV_SRC_PATH}/include
OPENCV_PKG_CONFIG_PATH=${OPENCV_SRC_PATH}/lib/pkgconfig
CFLAGS= `pkg-config --cflags --libs opencv` -g 
SRC_FILES=src/main.cpp
LIBS=


EXECUTABLE=-o stereotouch



all: 
	PKG_CONFIG_PATH=${OPENCV_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH};\
	export PKG_CONFIG_PATH;\
	${CC} ${EXECUTABLE} ${SRC_FILES} ${LIBS} ${CFLAGS} ;

cuda:
	${NVCC} ${NVFLAGS} -c -o ransac.o src/ransac.cu


calibration:
	PKG_CONFIG_PATH=${OPENCV_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH};\
	export PKG_CONFIG_PATH;\
	${CC} -o stereo_calib ${OPENCV_SRC_PATH}/samples/cpp/stereo_calib.cpp ${LIBS} ${CFLAGS} ;\
	${CC} -o calibration ${OPENCV_SRC_PATH}/samples/cpp/calibration.cpp ${LIBS} ${CFLAGS} ;\
	${CC} -o imagelist_creator ${OPENCV_SRC_PATH}/samples/cpp/imagelist_creator.cpp ${LIBS} ${CFLAGS} ;\
	${CC} -o image_splitter src/main.cpp src/utils.cpp ${LIBS} ${CFLAGS} ;\

opencv:
	cd ${OPENCV_LIB_PATH}; \
	pwd; \
	cmake -D WITH_CUDA=OFF -D WITH_JPEG=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON .. > ${PROJECT_ROOT}/cmake_output.txt ;\
	#make; \
	sudo make install; \

opencv_GPU:
	cd ${OPENCV_LIB_PATH}; \
	pwd; \
	cmake -D CUDA_ARCH_BIN = "2.1" -D CUDA_ARCH_PTX = "empty" -D WITH_JPEG=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON .. > ${PROJECT_ROOT}/cmake_output.txt ;\
	sudo make install; \


