#!/bin/bash

echo "$*"
if [ "$1" == "" ] 
then
echo "Usage save.sh <commit message>"

else

git add -A
git commit -m "$*"
git push https://github.com/shank8/CS360-Project.git

fi
