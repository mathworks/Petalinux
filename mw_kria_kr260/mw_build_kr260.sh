###Declare the directories variables
CURR_DIR="$(pwd)"
META_MW_DIR="$CURR_DIR/project-spec/meta-mathworks"
CONFIGFILE="$CURR_DIR/project-spec/configs/config"

if [[ "$CURR_DIR" == *"mw_kria_kr260"* ]]; then

	if [ ! -d "META_MW_DIR" ]; then

		echo "Adding meta-mathworks layer to the Petalinux project"
		
		### Add the meta-mathworks layer to the Petalinux repo
		git submodule add https://github.com/mathworks/meta-mathworks project-spec/meta-mathworks

		### Copy the patch file to the meta-mathworks directory
		cp -f mw_kr260.patch project-spec/meta-mathworks/
	
		### Navigate to the meta-mathworks directory and apply the patch
		cd project-spec/meta-mathworks/
		git apply mw_kr260.patch
	else

		echo "The meta-mathworks layer is already added to the Petalinux project. Continuing with the build process"	

	fi
else
	echo "ERROR: You are not in the Petalinux project directory"
	exit
fi

cd $CURR_DIR

cat <<EOF >> $CONFIGFILE
CONFIG_USER_LAYER_0="$META_MW_DIR"
EOF

### Build the Petalinux Image
petalinux-build

### Package the Petalinux binaries into a zip file
./mw_package_binaries.sh
