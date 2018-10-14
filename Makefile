.PHONY: build
export ROOT_DIR=${PWD}
build:
	rm -rf build
	mkdir -p build/lib
	mkdir -p build/bin
	mkdir -p build/include
	find VEngine | grep "\.h$$" | grep --invert-match stdafx.h | grep --invert-match tinydir.h | xargs dirname | awk '{print "mkdir -p build/include/"$$1""}' | sh
	find VEngine | grep "\.h$$" | grep --invert-match stdafx.h | grep --invert-match tinydir.h | awk '{print "cp "$$1" build/include/"$$1""}' | sh
	cd VEngine; \
	g++ -std=c++14 \
	-I${VULKAN_SDK}/include/vulkan \
	-I${STB_INCLUDE_DIR} \
	-shared -o libvengine.so -fPIC  `find . | grep ".*.cpp"`
	mv VEngine/libvengine.so build/lib/libvengine.so
	cd Example_AdvancedTeapot; \
	g++ -std=c++14 \
	-I${VULKAN_SDK}/include/vulkan \
	-I${ROOT_DIR}/VEngine \
	-L${ROOT_DIR}/build/lib \
	`find . | grep ".*.cpp"` -ldl -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXcursor -lXinerama -lvengine -lglfw3 -lvulkan
	cd Example_SimpleFullScreenPass; \
	g++ -std=c++14 \
	-I${VULKAN_SDK}/include/vulkan \
	-I${ROOT_DIR}/VEngine \
	-L${ROOT_DIR}/build/lib \
	`find . | grep ".*.cpp"` -ldl -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXcursor -lXinerama -lvengine -lglfw3 -lvulkan
	cd Example_Compute; \
	g++ -std=c++14 \
	-I${VULKAN_SDK}/include/vulkan \
	-I${ROOT_DIR}/VEngine \
	-L${ROOT_DIR}/build/lib \
	`find . | grep ".*.cpp"` -ldl -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXcursor -lXinerama -lvengine -lglfw3 -lvulkan
	mv Example_AdvancedTeapot/a.out build/bin/Example_AdvancedTeapot
	mv Example_SimpleFullScreenPass/a.out build/bin/Example_SimpleFullScreenPass
	mv Example_Compute/a.out build/bin/Example_Compute