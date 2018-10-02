#!/bin/bash

# based on https://github.com/k3rn31p4nic/travis-ci-discord-webhook

# Only run for commits/builds on master, not for PRs for now
if [[ "$TRAVIS_BRANCH" != "nightly" || $TRAVIS_PULL_REQUEST != "false" ]]; then
  exit
fi

# Webhook URL missing, do nothing
if [ -z "$2" ]; then
  exit
fi

echo -e "[Webhook]: Sending webhook to Discord...\\n";

AUTHOR_NAME="$(git log -1 "$TRAVIS_COMMIT" --pretty="%aN")"
COMMITTER_NAME="$(git log -1 "$TRAVIS_COMMIT" --pretty="%cN")"
COMMIT_SUBJECT="$(git log -1 "$TRAVIS_COMMIT" --pretty="%s")"
COMMIT_MESSAGE="$(git log -1 "$TRAVIS_COMMIT" --pretty="%b")"

if [ "$AUTHOR_NAME" == "$COMMITTER_NAME" ]; then
  CREDITS="$AUTHOR_NAME authored & committed"
else
  CREDITS="$AUTHOR_NAME authored & $COMMITTER_NAME committed"
fi

TIMESTAMP=$(date --utc +%FT%TZ)
WEBHOOK_DATA='{
  "avatar_url": "https://travis-ci.com/images/logos/TravisCI-Mascot-1.png",
  "embeds": [ {
    "author": {
      "name": "Job #'"$TRAVIS_JOB_NUMBER"' (Build #'"$TRAVIS_BUILD_NUMBER"')",
      "url": "https://travis-ci.com/'"$TRAVIS_REPO_SLUG"'/builds/'"$TRAVIS_BUILD_ID"'",
      "icon_url": "https://travis-ci.com/images/logos/TravisCI-Mascot-1.png"
    },
    "title": "['"\`${TRAVIS_COMMIT:0:7}\`"'] '"$COMMIT_SUBJECT"'",
    "url": "'"https://github.com/$TRAVIS_REPO_SLUG/commit/$TRAVIS_COMMIT"'",
    "description": "'"$CREDITS"'",
    "fields": [
      {
        "name": "Binary accuracy",
        "value": "'"$1"'",
        "inline": true
      }
    ],
    "timestamp": "'"$TIMESTAMP"'"
  } ]
}'

(curl --fail --progress-bar -A "TravisCI-Webhook" -H Content-Type:application/json -H X-Author:diasurgical -d "$WEBHOOK_DATA" "$2" \
  && echo -e "\\n[Webhook]: Successfully sent the webhook.") || echo -e "\\n[Webhook]: Unable to send webhook."
