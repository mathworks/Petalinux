###Declare the directories variables
CURR_DIR="$(pwd)"
META_MW_DIR="$CURR_DIR/project-spec/meta-mathworks"
CONFIGFILE="$CURR_DIR/project-spec/configs/config"
GITMODULE_FILE="$(git rev-parse --show-toplevel)/.gitmodules"
SUBMODULE_STRING="mw_kria_kv260/project-spec/meta-mathworks"

deinit_sub_module()
{
	git submodule deinit -f "$META_MW_DIR"
	git rm -rf "$META_MW_DIR" >/dev/null
	rm -rf -q ../.git/modules/mw_kria_kv260
}

if [[ "$CURR_DIR" == *"mw_kria_kv260"* ]]; then

	if  ! grep -q "$SUBMODULE_STRING" "$GITMODULE_FILE"; then

		echo "Adding meta-mathworks layer to the Petalinux project"
		
		### Add the meta-mathworks layer to the Petalinux repo
		git submodule add https://github.com/mathworks/meta-mathworks project-spec/meta-mathworks
		
		if  grep -q "$SUBMODULE_STRING" "$GITMODULE_FILE"; then
			echo "Added meta-mathworks layer successfully"
		else
			echo "Failed to add meta-mathworks layer"
			deinit_sub_module
			exit 1
		fi

			
		### Copy the patch file to the meta-mathworks directory
		cp -f mw_kv260.patch project-spec/meta-mathworks/
	
		### Navigate to the meta-mathworks directory and apply the patch
		cd project-spec/meta-mathworks/
		

		if git apply mw_kv260.patch;then
			echo "Patch applied successfully"
		else
			echo "Failed to apply patch"
			deinit_sub_module
			exit 1
		fi		
		
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

BUILD_STATUS=$?

if [ "$BUILD_STATUS" -ne 0 ]; then
	exit 1
fi

### Package the Petalinux binaries into a zip file
chmod +x mw_package_binaries.sh
./mw_package_binaries.sh
