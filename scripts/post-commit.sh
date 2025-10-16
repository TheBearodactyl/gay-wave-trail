#!/bin/sh
# Copyright (c) 2013 Martin Seeler
# SPDX-License-Identifier: MIT
# edited version of https://github.com/MartinSeeler/auto-changelog-hook/blob/master/post-commit

OUTPUT_FILE=CHANGELOG.md

TEMP_FILE=$(mktemp)

cat > $TEMP_FILE << 'EOF'
# Changelog

## [Unreleased]

EOF

git --no-pager log --no-merges --reverse --format="### %s%n%n**Date:** %aD  \n**Author:** %aN (%aE)  \n**Commit Hash:** %H  \n%n%b%n%n---%n" >> $TEMP_FILE

if [ -f "$OUTPUT_FILE" ]; then
	tail -n +2 "$OUTPUT_FILE" >> $TEMP_FILE 2>/dev/null || true
fi

mv $TEMP_FILE $OUTPUT_FILE

res=$(git status --porcelain | grep -c "$OUTPUT_FILE")
if [ "$res" -gt 0 ]; then
	git add $OUTPUT_FILE
	git commit --amend --no-edit

	echo "Populated Changelog in $OUTPUT_FILE"
else
	echo "No changes to $OUTPUT_FILE"
fi
