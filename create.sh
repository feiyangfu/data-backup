mkdir -p manual_source/empty_dir
mkdir -p manual_source/subdir
echo "This is file one." > manual_source/file1.txt
echo "This is a file in a subdirectory." > manual_source/subdir/file2.txt
ln manual_source/file1.txt manual_source/hardlink_to_file1.txt
ln -s file1.txt manual_source/symlink_to_file1.txt
ln -s ../subdir manual_source/symlink_to_subdir