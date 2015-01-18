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

Requires computer to have SSL certificate authority to verify that the program is connecting to an actual Microsoft server. The other option is very insecure. 

oneuploadlogin is insecure. It stores the refreshtoken that can be used to get an accesstoken to your account in plaintext. There is no suitable fix that won't require entering a password to decrypt the login file, which completely misses the point of the login file in the first place. If you care about security /that/ much, do not use -l option. 

Instead of displaying 100% when finished uploading, displays 199.999...% because the 1 is a single digit and .99... is many digits.
