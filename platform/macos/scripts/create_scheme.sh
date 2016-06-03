#!/usr/bin/env bash

set -u

CONTAINER=build/macos/mbgl.xcodeproj

# Required ENV vars:
# - SCHEME_TYPE: type of the scheme
# - SCHEME_NAME: name of the scheme

# Optional ENV vars:
# - NPM_ARGUMENT (defaults to "")
# - BUILDABLE_NAME (defaults ot SCHEME_NAME)
# - BLUEPRINT_NAME (defaults ot SCHEME_NAME)

NPM_ARGUMENT=${NPM_ARGUMENT:-}
BUILDABLE_NAME=${BUILDABLE_NAME:-${SCHEME_NAME}}
BLUEPRINT_NAME=${BLUEPRINT_NAME:-${SCHEME_NAME}}

DIR="build/macos/mbgl.xcodeproj/xcshareddata/xcschemes"
mkdir -p "${DIR}"

sed "\
s#{{BLUEPRINT_ID}}#$(hexdump -n 12 -v -e '/1 "%02X"' /dev/urandom)#;\
s#{{BLUEPRINT_NAME}}#${BLUEPRINT_NAME}#;\
s#{{BUILDABLE_NAME}}#${BUILDABLE_NAME}#;\
s#{{CONTAINER}}#${CONTAINER}#;\
s#{{WORKING_DIRECTORY}}#$(pwd)#;\
s#{{NODE_PATH}}#$(dirname `which node`)#;\
s#{{NPM_ARGUMENT}}#${NPM_ARGUMENT}#" \
    platform/macos/scripts/${SCHEME_TYPE}.xcscheme > "${DIR}/${SCHEME_NAME}.xcscheme"
