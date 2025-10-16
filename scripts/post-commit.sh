#!/bin/sh
# Copyright (c) 2013 Martin Seeler
# SPDX-License-Identifier: MIT
# taken from https://github.com/MartinSeeler/auto-changelog-hook/blob/master/post-commit

OUTPUT_FILE=CHANGELOG.md

git --no-pager log --no-merges --format="### %s%d%n>%aD%n%n>Author: %aN (%aE)%n%n>Commiter: %cN (%cE)%n%n%b%n%N%n" > $OUTPUT_FILE

res=$(git status --porcelain | grep -c ".\$OUTPUT_FILE$")
if [ "$res" -gt 0 ]; then
	git add $OUTPUT_FILE
	git commit --amend

	echo "Populated Changelog in $OUTPUT_FILE"
fi
