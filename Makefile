all: onedrive.cpp onedrive.h main.cpp
	g++ main.cpp onedrive.cpp -lcurl -lcurlpp -o oneupload

clean:
	rm oneupload
