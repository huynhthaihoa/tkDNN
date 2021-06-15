#!/bin/bash

#based on https://devtalk.nvidia.com/default/topic/1042035/installing-opencv4-on-xavier/ & https://github.com/markste-in/OpenCV4XAVIER/blob/master/buildOpenCV4.sh

# Compute Capabilities can be found here https://developer.nvidia.com/cuda-gpus#compute
ARCH_BIN=7.2 # AGX Xavier
#ARCH_BIN=6.2 # Tx2

cd ~/Downloads
sudo apt-get install -y unzip \
    pkg-config \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libgtk-3-dev \
    libatlas-base-dev \
    gfortran \
    python3-dev \
    python3-venv \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libdc1394-22-dev \
    libavresample-dev \
	libcurl4-gnutls-dev \
	libpq-dev \
	libavresample-dev \
	libva-dev \
	gtk+-3.0 \
	libpng-dev \
	libtbb2 \
	libtbb-dev \
	libtiff5-dev \
	libavdevice-dev \
	v4l-utils \
	libxvidcore-dev \
	libxine2-dev \
	libatlas-base-dev \
	gfortran \
	libeigen3-dev \
	mesa-utils \
	libgl1-mesa-dri \
	libgtkgl2.0-dev \
	libgtkglext1-dev \
	libqt4-dev \
	qt5-default \
	libgtk2.0-dev \
	cmake \
	cmake-gui \
	pkg-config \ 
	build-essential

# git clone https://github.com/opencv/opencv.git
# git clone https://github.com/opencv/opencv_contrib.git
wget https://github.com/opencv/opencv/archive/refs/tags/4.5.2.zip
wget https://github.com/opencv/opencv_contrib/archive/refs/tags/4.5.2.zip

python3 -m venv opencv4
source opencv4/bin/activate
pip install wheel
pip install numpy

cd opencv-4.5.2 && mkdir build && cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    -D OPENCV_EXTRA_MODULES_PATH='../../opencv_contrib-4.5.2/modules' \
    -D PYTHON_EXECUTABLE='~/Downloads/opencv4/bin/python' \
    -D BUILD_EXAMPLES=ON \
	-D WITH_TBB=ON \
	-D BUILD_TBB=ON \
	-D WITH_1394=ON \
	-D WITH_OPENMP=ON \
	-D BUILD_WITH_DEBUG_INFO=OFF \
	-D BUILD_DOCS=OFF \
	-D INSTALL_C_EXAMPLES=OFF \
	-D INSTALL_PYTHON_EXAMPLES=OFF \
	-D BUILD_EXAMPLES=OFF \
	-D BUILD_TESTS=OFF \
	-D BUILD_PERF_TESTS=OFF \
	-D WITH_QT=ON \
	-D WITH_GTK=OFF \
	-D WITH_OPENGL=ON \
    -D WITH_CUDA=ON \
    -D CUDA_ARCH_BIN=${ARCH_BIN} \
    -D CUDA_ARCH_PTX="" \
    -D ENABLE_FAST_MATH=ON \
    -D CUDA_FAST_MATH=ON \
    -D WITH_CUBLAS=ON \
    -D WITH_LIBV4L=ON \
    -D WITH_GSTREAMER=ON \
    -D WITH_GSTREAMER_0_10=OFF \
	-D WITH_V4L=ON \
	-D WITH_FFMPEG=ON \
	-D WITH_XINE=OFF \
	-D BUILD_NEW_PYTHON_SUPPORT=OFF \
	-D BUILD_opencv_python_tests=OFF \
	-D OPENCV_GENERATE_PKGCONFIG=ON \
	-D WITH_CUFFT=ON \
	-D BUILD_opencv_cudacodec=ON \
	-D WITH_NVCUVID=ON \
	-D ENABLE_FAST_MATH=ON \
	-D CUDA_FAST_MATH=ON \
	-D WITH_CUDNN=ON \
	-D OPENCV_DNN_CUDA=ON \
	-D CUDNN_LIBRARY=/usr/lib/aarch64-linux-gnu/libcudnn.so \
	-D OPENCV_ENABLE_NONFREE=ON \
	-D WITH_OPENCLAMDBLAS=OFF \
	-D BUILD_TIFF=ON \
	-D BUILD_JASPER=ON \
	-D HAVE_opencv_gapi=ON \
	-D BUILD_opencv_ts=ON \
	-D WITH_TESSERACT=OFF \
	-D WITH_VTK=OFF \
    ../

make -j4
sudo make install
sudo ldconfig

cd ~/Downloads/opencv4/lib/python3.6/site-packages
ln -s /usr/local/lib/python3.6/site-packages/cv2.cpython-36m-aarch64-linux-gnu.so cv2.so
