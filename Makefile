Demo: Grab_ImageCallback.cpp
	$(CXX) -g -pthread -o Grab_ImageCallback Grab_ImageCallback.cpp -I./include -L./lib -lMvCameraControl
	

clean:
	rm Grab_ImageCallback -rf
