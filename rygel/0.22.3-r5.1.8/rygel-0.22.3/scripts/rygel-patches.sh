#!/bin/sh

function die() {
	echo
	echo $1
	exit -1
}

function reconfigure() {
	FLAGS="--disable-debug --disable-coverage \
		--disable-playbin-plugin --disable-tracker-plugin \
		--disable-media-export-plugin --with-media-engine=simple --enable-vala"
	./autogen.sh $FLAGS
}

function generate_sources() {
	dir=$1
	stamp=$2

	pushd . >/dev/null
	cd $dir || die "Cannot change directory to $dir"
	make $MAKEOPTS $stamp || die "Compilation failed!"
	popd >/dev/null
}


HISTORY_START=RYGEL_0_22_3
TMP=/tmp/rygel-tmp
VALAC_VERSION="0.24"
VALAC=`which valac-$VALAC_VERSION`
OLD_VALAC=`which valac-0.22`
PATCHDIR=`readlink -f .`/patches
VERBOSE=0



# Set up variables
if [ $VERBOSE -ne 0 ]; then
	MAKEOPTS=-s
fi

# Basic sanity checks:

if [ ! -x $VALAC ]; then
		die "No working valac of requested version $VALAC_VERSION has been found"
fi


rm -rf $TMP
mkdir -p $TMP

echo "Generating a patchset starting at $HISTORY_START on the current branch"

# Copy .git directory, so we won't pollute it with extraneous commits

git clone . $TMP

export VALAC

# Check out initial commit

cd $TMP
COMMITS=$(git log --oneline --reverse $HISTORY_START~1.. |cut -d" " -f1)
FIRST_COMMIT=$(echo $COMMITS | cut -f1 -d" ")
git checkout $FIRST_COMMIT -B generated

# Configure the tree
reconfigure

for commit in $COMMITS; do
	# Dirty hacks to keep the script working
	git rm --ignore-unmatch data/xml/*.xml # Some commits contain generated .xml files
	# END Dirty hacks
	if [ $commit != $FIRST_COMMIT ]; then git cherry-pick $commit || die "Cherry-pick failed!"; fi

	# Clean up the .c and .h files
	find src -iname \*.c -o -iname \*.h |xargs -- git rm -q

	# Restrict code generation only to few directories, others might be
	# incompatible by our changes. This also speeds up compilation.
	generate_sources src/librygel-core librygel_core_2_2_la_vala.stamp
	generate_sources src/librygel-renderer librygel_renderer_2_2_la_vala.stamp


	git add -f `find src -iname \*.c -o -iname \*.h` || die "Adding files failed"
	git commit --amend --no-edit --allow-empty || die "Amending failed!"

	if [ $commit == $FIRST_COMMIT ]; then NEW_START=$(git rev-parse HEAD); fi
	find src -iname \*.stamp -delete
done;

echo "Storing patches to $PATCHDIR, starting from $NEW_START"
rm -rf $PATCHDIR
mkdir -p $PATCHDIR
git format-patch -N -o $PATCHDIR $NEW_START
echo
echo "All done! Result in $PATCHDIR"

rm -rf $TMP