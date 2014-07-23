#!/bin/bash

# This script finds sizes of all files in the git commit history
# following http://naleid.com/blog/2012/01/17/finding-and-purging-big-files-from-git-history
mkdir -p tmp

# What object SHA is associated with each file in the Repo?
git rev-list --objects --all | sort -k 2 > tmp/allfileshas.txt

# What Unique Files Exist Throughout The History of My Git Repo?
# git rev-list --objects --all | sort -k 2 | cut -f 2 -d\  | uniq

# How Big Are The Files In My Repo?
git gc && git verify-pack -v .git/objects/pack/pack-*.idx | egrep "^\w+ blob\W+[0-9]+ [0-9]+ [0-9]+$" | sort -k 3 -n -r > tmp/bigobjects.txt

for SHA in `cut -f 1 -d\  < tmp/bigobjects.txt`; do
echo $(grep $SHA tmp/bigobjects.txt) $(grep $SHA tmp/allfileshas.txt) | awk '{print $1,$3,$7}' >> tmp/bigtosmall.txt
done;

head tmp/bigtosmall.txt


