# OneUpload
Uploads a single file or recursively uploads a directory to Microsoft OneDrive

This was written mainly because I wanted to play around with cURLpp. It's not actually substantially better than just using libcurl, though it does look more fancy and objecty, the underlying workings of libcurl show through pretty strongly. 

#Building:

run "make all"

#Using:

Creating a login: ./oneupload -l

This prevents you from having to log in each time you use the application. To log out, delete the file oneuploadlogin

Uploading a single file: ./oneupload -f local cloud

Example, uploading test.txt to hello.txt on OneDrive: ./oneupload -f "./test.txt" "hello.txt"


Uploading a single file, keeping name: ./oneupload -f local

Example, uploading testImage.jpg to OneDrive: ./oneupload -f "./testImage.jpg"


Uploading a directory: ./oneupload -d dir

Example, uploading the testfolder directory to OneDrive: ./oneupload -d "./testfolder/"

#Issues

Instead of displaying 100% when finished uploading, displays 199.999...% because the 1 is a single digit and .99... is many digits.
