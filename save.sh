#!/bin/bash

echo "$*"
if [ "$1" == "" ] 
then
echo "Usage save.sh <commit message>"

else

git add .
git commit -m "$*"
git push final master

fi