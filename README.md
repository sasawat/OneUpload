# OneUpload
Uploads a single file or recursively uploads a directory to Microsoft OneDrive

This was written mainly because I wanted to play around with cURLpp. It's not actually substantially better than just using libcurl, though it does look more fancy and objecty. 

#Building:

run "make all"

#Using:

Uploading a single file: ./oneupload -f local cloud

Example, uploading test.txt to hello.txt on OneDrive: ./oneupload -f "./test.txt" "hello.txt"


Uploading a single file, keeping name: ./oneupload -f local

Example, uploading testImage.jpg to OneDrive: ./oneupload -f "./testImage.jpg"


Uploading a directory: ./oneupload -d dir

Example, uploading the testfolder directory to OneDrive: ./oneupload -d "./testfolder/"

