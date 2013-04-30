
CC=g++
NVCC=nvcc
NVFLAGS=-O3 --profile -arch=compute_20 -code=sm_20 -Xptxas -dlcm=ca 
PROJECT_ROOT=${PWD}
OPENCV_SRC_PATH=${PROJECT_ROOT}/opencv
OPENCV_LIB_PATH=${OPENCV_SRC_PATH}/release
OPENCV_INCLUDE_PATH=${OPENCV_SRC_PATH}/include
OPENCV_PKG_CONFIG_PATH=${OPENCV_SRC_PATH}/lib/pkgconfig
CFLAGS= `pkg-config --cflags --libs opencv` -g 
SRC_FILES=src/main.cpp src/StereoCapture.cpp src/FrameProcessor.cpp
LIBS=
LDFLAGS=-lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy


EXECUTABLE=-o stereotouch



all: 
	PKG_CONFIG_PATH=${OPENCV_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH};\
	export PKG_CONFIG_PATH;\
	${CC} ${EXECUTABLE} ${SRC_FILES} ${LIBS} ${CFLAGS} ;

libs:
	$(CC) $(EXECUTABLE) $(SRC_FILES) $(LDFLAGS)

record: 
	PKG_CONFIG_PATH=${OPENCV_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH};\
	export PKG_CONFIG_PATH;\
	${CC} -o stereo_record src/record.cpp ${LIBS} ${CFLAGS} ;\


calibration:
	PKG_CONFIG_PATH=${OPENCV_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH};\
	export PKG_CONFIG_PATH;\
	${CC} -o stereo_calib ${OPENCV_SRC_PATH}/samples/cpp/stereo_calib.cpp ${LIBS} ${CFLAGS} ;\
	${CC} -o calibration ${OPENCV_SRC_PATH}/samples/cpp/calibration.cpp ${LIBS} ${CFLAGS} ;\
	${CC} -o imagelist_creator ${OPENCV_SRC_PATH}/samples/cpp/imagelist_creator.cpp ${LIBS} ${CFLAGS} ;\
	

opencv:
	cd ${OPENCV_LIB_PATH}; \
	pwd; \
	cmake -D WITH_CUDA=OFF -D WITH_JPEG=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON .. > ${PROJECT_ROOT}/cmake_output.txt ;\
	make; \
	sudo make install; \

opencv_GPU:
	cd ${OPENCV_LIB_PATH}; \
	pwd; \
	cmake -D WITH_CUDA=ON  -D CUDA_ARCH_BIN="2.0" -D CUDA_ARCH_PTX="empty" -D WITH_JPEG=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON .. > ${PROJECT_ROOT}/cmake_output.txt ;\
	make; \
	sudo make install; \


