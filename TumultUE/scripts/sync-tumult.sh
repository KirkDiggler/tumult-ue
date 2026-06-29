#!/usr/bin/env bash
# Sync the vendored Tumult module to pinned release tags.
# Usage: ./scripts/sync-tumult.sh
set -euo pipefail

TUMULT_TAG="v0.1.0"
RPGKIT_TAG="v0.3.0"
DEST="Source/ThirdParty/Tumult"

echo "Syncing Tumult $TUMULT_TAG + rpgkit $RPGKIT_TAG into $DEST..."

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

git clone --depth 1 --branch "$TUMULT_TAG" https://github.com/KirkDiggler/tumult.git "$TMP/tumult"
git clone --depth 1 --branch "$RPGKIT_TAG" https://github.com/KirkDiggler/rpgkit.git "$TMP/rpgkit"

rm -rf "$DEST/include" "$DEST/src" "$DEST/rpgkit"
mkdir -p "$DEST"

cp -r "$TMP/tumult/include" "$DEST/include"
cp -r "$TMP/tumult/src" "$DEST/src"
cp "$TMP/tumult/LICENSE" "$DEST/LICENSE.tumult"

mkdir -p "$DEST/rpgkit/core"
cp -r "$TMP/rpgkit/core/include" "$DEST/rpgkit/core/include"
cp "$TMP/rpgkit/LICENSE" "$DEST/LICENSE.rpgkit"

# The vendored snapshot is compiled as a separate Unreal module. Export the
# non-inline Tumult classes that host modules instantiate across the DLL
# boundary; upstream Tumult stays host-agnostic and does not carry UE macros.
sed -i \
	-e 's/class Encounter {/class TUMULT_API Encounter {/' \
	"$DEST/include/tumult/encounter.hpp"
sed -i \
	-e 's/class VulnerableEffect/class TUMULT_API VulnerableEffect/' \
	"$DEST/include/tumult/effects/vulnerable.hpp"
sed -i \
	-e 's/class ToughSkinEffect/class TUMULT_API ToughSkinEffect/' \
	"$DEST/include/tumult/effects/tough_skin.hpp"
sed -i \
	-e 's/class BleedEffect/class TUMULT_API BleedEffect/' \
	"$DEST/include/tumult/effects/bleed.hpp"

printf 'tumult %s\nrpgkit %s\n' "$TUMULT_TAG" "$RPGKIT_TAG" > "$DEST/.pinned-tags"

echo "Done. Vendored Tumult $TUMULT_TAG + rpgkit $RPGKIT_TAG in $DEST."
