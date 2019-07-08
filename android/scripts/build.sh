#!/bin/bash - 

set -o errexit
set -o nounset

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "${SCRIPT_DIR}");
BUILD_DIR="$PROJECT_DIR/build";

PROJECT_NAME="ElastosSdkWallet";
PROJECT_BUILDTYPE="Release";
PROJECT_REVISION="$(git rev-list --count HEAD)";
PROJECT_VERSION="v0.1.$PROJECT_REVISION";

TARGET_PATH="$BUILD_DIR/${PROJECT_NAME}-${PROJECT_VERSION}.aar";

cd "$PROJECT_DIR";
./gradlew :lib:assembleDebug -P versionCode=${PROJECT_REVISION} -P versionName=${PROJECT_VERSION/v/}
rm -rf "$TARGET_PATH";
mkdir -p "$BUILD_DIR/";
cp "$PROJECT_DIR/lib/build/outputs/aar/lib-debug.aar" "$TARGET_PATH";
git tag --force ${PROJECT_VERSION}


TARBALL_PATH="$BUILD_DIR/${PROJECT_NAME}-${PROJECT_VERSION}.zip";
cd "$BUILD_DIR";
zip -r "$TARBALL_PATH" . -i $(basename "$TARGET_PATH");

echo "Done!!!";

