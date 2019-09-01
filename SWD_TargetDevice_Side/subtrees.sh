# to update subtrees please change 'add' to 'pull'

git subtree pull	--prefix=src/cmsis	http://gitblit.dep111.rtc.local/r/EmbeddedLibs/cmsis.git	external_lite	--squash
git subtree pull	--prefix=src/global_macros	http://gitblit.dep111.rtc.local/r/EmbeddedLibs/global_macros.git	external	--squash
git subtree pull	--prefix=src/callbacks	http://gitblit.dep111.rtc.local/r/EmbeddedLibs/callbacks.git	external	--squash
git subtree pull	--prefix=src/retarget	http://gitblit.dep111.rtc.local/r/EmbeddedLibs/retarget.git	external	--squash
git subtree pull	--prefix=src/common_functions	http://gitblit.dep111.rtc.local/r/EmbeddedLibs/common_functions.git	external	--squash
git subtree pull	--prefix=src/mcu_support_package	http://gitblit.dep111.rtc.local/r/EmbeddedLibs/mcu_support_package.git	stm32f1xx	--squash

# that's to reduce local repo size, because subtree can fetch a lot of unnecessary files from other branches
git gc --prune=now